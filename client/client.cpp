/*
  Names:  Ang Li  Matthew Braun
  IDs:    1550746 1497171
  CMPUT 275, Winter 2019

  Assignment 2: Directions Part 2
  Modified from the part 1 solution given in E-class
*/
#include <Arduino.h>
#include <Adafruit_ILI9341.h>
#include <SD.h>
#include "consts_and_types.h"
#include "map_drawing.h"

// the variables to be shared across the project, they are declared here!
shared_vars shared;

// max size of buffer, including null terminator
const uint16_t buf_size = 256;
// current number of chars in buffer, not counting null terminator
uint16_t buf_len = 0;

// input buffer
char* buffer = (char *) malloc(buf_size);

Adafruit_ILI9341 tft = Adafruit_ILI9341(clientpins::tft_cs, clientpins::tft_dc);

void setup() {
  // initialize Arduino
  init();

  // initialize zoom pins
  pinMode(clientpins::zoom_in_pin, INPUT_PULLUP);
  pinMode(clientpins::zoom_out_pin, INPUT_PULLUP);

  // initialize joystick pins and calibrate center reading
  pinMode(clientpins::joy_button_pin, INPUT_PULLUP);
  // x and y are reverse because of how our joystick is oriented
  shared.joy_centre = xy_pos(analogRead(clientpins::joy_y_pin), analogRead(clientpins::joy_x_pin));

  // initialize serial port
  Serial.begin(9600);
  Serial.flush(); // get rid of any leftover bits

  // initially no path is stored
  shared.num_waypoints = 0;

  // initialize display
  shared.tft = &tft;
  shared.tft->begin();
  shared.tft->setRotation(3);
  shared.tft->fillScreen(ILI9341_BLUE); // so we know the map redraws properly

  // initialize SD card
  if (!SD.begin(clientpins::sd_cs)) {
      Serial.println("Initialization has failed. Things to check:");
      Serial.println("* Is a card inserted properly?");
      Serial.println("* Is your wiring correct?");
      Serial.println("* Is the chipSelect pin the one for your shield or module?");

      while (1) {} // nothing to do here, fix the card issue and retry
  }

  // initialize the shared variables, from map_drawing.h
  // doesn't actually draw anything, just initializes values
  initialize_display_values();

  // initial draw of the map, from map_drawing.h
  draw_map();
  draw_cursor();

  // initial status message
  status_message("FROM?");
}

// get line input form the serial port
int getLine(String &content, unsigned long timeOut) {
  // set up buffer as empty string
  buf_len = 0;
  buffer[buf_len] = 0;

  unsigned long currentTime = millis();

  while (millis() - currentTime < timeOut) {
    if (Serial.available()) {
      // read the incoming byte:
      char in_char = Serial.read();

      // if end of line is received, waiting for line is done:
      if (in_char == '\n' || in_char == '\r') {
        // now we process the buffer
        content = String(buffer);
        return 0;
      }
      else {
        // add character to buffer, provided that we don't overflow.
        // drop any excess characters.
        if (buf_len < buf_size - 1) {
          buffer[buf_len] = in_char;
          buf_len++;
          buffer[buf_len] = '\0';
        }
      }
    }
  }
  return -1; // time out
}

// communicate with the server to get the way points
void getWayPoints(lon_lat_32 start, lon_lat_32 end) {
  status_message("Receiving path...");

  // indicate the current statue when communicating with the server
  enum {sendQuery, numberOfWayPoints, storeWayPoints, finish} currentStatus = sendQuery;
  // message got from the server
  String content;

  while(true) {
    switch (currentStatus) {
      case sendQuery: {
        // send the query
        // e.g R 5365486 -11333915 5364728 -11335891
        String outMsg = ("R " + String(start.lat) + " " + String(start.lon) + " " +
          String(end.lat) + " " + String(end.lon) + "\n");
        Serial.print(outMsg);

        // get line input form the serial port
        // Arduino waits for the # for way points to be received from the server
        if (getLine(content, 10000) < 0) {
          break; // time out
        }

        // e.g. N 8<\n>
        if (content.charAt(0) == 'N') {
          // next statue
          currentStatus = numberOfWayPoints;
        }
        break;
      }

      case numberOfWayPoints: {
        shared.num_waypoints = content.substring(2).toInt();

        // no path
        if (shared.num_waypoints == 0) {
          // next statue
          status_message("No path, please reselect");
          delay(3000);
          return;
        }

        // send acknowledgment
        Serial.print("A\n");

        // next statue
        currentStatus = storeWayPoints;
        break;
      }
      
      case storeWayPoints: {
        // if the required condition fails in the half way
        // back to the start
        currentStatus = sendQuery;

        // get the way points & store them in the array shared.waypoints[]
        for (int i = 0; i < shared.num_waypoints; ++i) {
          // get line input form the serial port
          if (getLine(content, 1000) < 0) {
            break; // time out
          }

          // e.g. W 5365488 -11333914<\n>
          if (content.charAt(0) == 'W') {
            // send acknowledgment
            Serial.print("A\n");

            // find the 2nd occurrence of the blank space
            unsigned int pos = 2;
            for (pos; pos < content.length(); pos++) {
              if (content.charAt(pos) == ' ') {
                break;
              }
            }

            shared.waypoints[i].lat = content.substring(2, pos).toInt();
            shared.waypoints[i].lon = content.substring(pos + 1).toInt();
          }
          else {
            break; // invalid response
          }
          if (i == shared.num_waypoints - 1 || shared.num_waypoints == 1) {
            // next state
            currentStatus = finish;
            break;
          }
        }
      }

      case finish: {
        // if the required condition fails in the half way
        // back to the start
        currentStatus = sendQuery;

        // get line input form the serial port
        if (getLine(content, 1000) < 0) {
          break; // time out
        }

        if (content.charAt(0) == 'E') {
          return; // query finished
        }
        break;
      }
    } 
  }
}

void process_input() {
  // read the zoom in and out buttons
  shared.zoom_in_pushed = (digitalRead(clientpins::zoom_in_pin) == LOW);
  shared.zoom_out_pushed = (digitalRead(clientpins::zoom_out_pin) == LOW);

  // read the joystick button
  shared.joy_button_pushed = (digitalRead(clientpins::joy_button_pin) == LOW);

  // joystick speed, higher is faster
  const int16_t step = 64;

  // get the joystick movement, dividing by step discretizes it
  // currently a far joystick push will move the cursor about 5 pixels
  xy_pos delta(
    // the funny x/y swap is because of our joystick orientation
    (analogRead(clientpins::joy_y_pin)-shared.joy_centre.x)/step,
    (analogRead(clientpins::joy_x_pin)-shared.joy_centre.y)/step
  );
  delta.x = -delta.x; // horizontal axis is reversed in our orientation

  // check if there was enough movement to move the cursor
  if (delta.x != 0 || delta.y != 0) {
    // if we are here, there was noticeable movement

    // the next three functions are in map_drawing.h
    erase_cursor();       // erase the current cursor
    move_cursor(delta);   // move the cursor, and the map view if the edge was nudged
    if (shared.redraw_map == 0) {
      // it looks funny if we redraw the cursor before the map scrolls
      draw_cursor();      // draw the new cursor position
    }
  }
}

int main() {
  setup();

  // very simple finite state machine:
  // which endpoint are we waiting for?
  enum {WAIT_FOR_START, WAIT_FOR_STOP} curr_mode = WAIT_FOR_START;

  // the two points that are clicked
  lon_lat_32 start, end;

  while (true) {
    // clear entries for new state
    shared.zoom_in_pushed = 0;
    shared.zoom_out_pushed = 0;
    shared.joy_button_pushed = 0;
    shared.redraw_map = 0;

    // reads the three buttons and joystick movement
    // updates the cursor view, map display, and sets the
    // shared.redraw_map flag to 1 if we have to redraw the whole map
    // NOTE: this only updates the internal values representing
    // the cursor and map view, the redrawing occurs at the end of this loop
    process_input();

    // if a zoom button was pushed, update the map and cursor view values
    // for that button push (still need to redraw at the end of this loop)
    // function zoom_map() is from map_drawing.h
    if (shared.zoom_in_pushed) {
      zoom_map(1);
      shared.redraw_map = 1;
    }
    else if (shared.zoom_out_pushed) {
      zoom_map(-1);
      shared.redraw_map = 1;
    }

    // if the joystick button was clicked
    if (shared.joy_button_pushed) {

      if (curr_mode == WAIT_FOR_START) {
        // if we were waiting for the start point, record it
        // and indicate we are waiting for the end point
        start = get_cursor_lonlat();
        curr_mode = WAIT_FOR_STOP;
        status_message("TO?");

        // wait until the joystick button is no longer pushed
        while (digitalRead(clientpins::joy_button_pin) == LOW) {}
      }
      else {
        // if we were waiting for the end point, record it
        // and then communicate with the server to get the path
        end = get_cursor_lonlat();

        // TODO: communicate with the server to get the waypoints
        getWayPoints(start, end);
        shared.redraw_map = 1;

        // now we have stored the path length in
        // shared.num_waypoints and the waypoints themselves in
        // the shared.waypoints[] array, switch back to asking for the
        // start point of a new request
        curr_mode = WAIT_FOR_START;

        // wait until the joystick button is no longer pushed
        while (digitalRead(clientpins::joy_button_pin) == LOW) {}
      }
    }

    if (shared.redraw_map) {
      // redraw the status message
      if (curr_mode == WAIT_FOR_START) {
        status_message("FROM?");
      }
      else {
        status_message("TO?");
      }

      // redraw the map and cursor
      draw_map();
      draw_cursor();

      // TODO: draw the route if there is one
      if (shared.num_waypoints > 1) {
        for (int i = 0; i < shared.num_waypoints - 1; ++i) {
          int32_t heady= latitude_to_y(shared.map_number, shared.waypoints[i].lat);
          int32_t headx = longitude_to_x(shared.map_number, shared.waypoints[i].lon);
          int32_t taily = latitude_to_y(shared.map_number, shared.waypoints[i + 1].lat);
          int32_t tailx = longitude_to_x(shared.map_number, shared.waypoints[i + 1].lon);

          int32_t lineHeadx = headx - shared.map_coords.x;
          int32_t lineHeady = heady - shared.map_coords.y;
          int32_t lineTailx = tailx - shared.map_coords.x;
          int32_t lineTaily = taily - shared.map_coords.y;

          tft.drawLine(lineHeadx, lineHeady, lineTailx, lineTaily, ILI9341_BLUE);
        }
      }
      // only one way point
      else if (shared.num_waypoints == 1) {
        int32_t heady= latitude_to_y(shared.map_number, shared.waypoints[0].lat);
        int32_t headx = longitude_to_x(shared.map_number, shared.waypoints[0].lon);

        int32_t lineHeadx = headx - shared.map_coords.x;
        int32_t lineHeady = heady - shared.map_coords.y;

        tft.drawLine(lineHeadx, lineHeady, lineHeadx, lineHeady, ILI9341_BLUE);
      }
    }
  }

  Serial.flush();
  return 0;
}
