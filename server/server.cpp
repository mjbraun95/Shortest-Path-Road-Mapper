/*
  Names:  Ang Li  Matthew Braun
  IDs:    1550746 1497171
  CMPUT 275, Winter 2019

  Assignment 2: Directions Part 2
  Modified from the part 1 solution given in E-class
*/
#include <cassert>
#include <iostream>
#include <list>
#include <string>
#include <vector>
#include "dijkstra.h"
#include "serialport.h"
#include "server_util.h"
#include "wdigraph.h"

using namespace std;

// processes the communication
void sendWayPoints(WDigraph graph, unordered_map<int, Point> points) {
  SerialPort Serial("/dev/ttyACM0");

  // indicate the current statue when communicating with the Arduino
  enum {getQuery, numberOfWayPoints, sendWayPoints, finish} currentStatus = getQuery;
  // message got from the Arduino
  string content;
  Point sPoint, ePoint;
  list<int> path;

  while(true) {
    switch (currentStatus) {
      case getQuery: {
        cout << "Waiting for request..." << endl;
        // read and ignore lines until we get the request message
        content = Serial.readline();
        // e.g R 5365486 -11333915 5364728 -11335891
        cout << content << endl;
        if (content.front() == 'R') {
          // next statue
          currentStatus = numberOfWayPoints;
        }
        break;
      }

      case numberOfWayPoints: {
        // if the required condition fails in the half way
        // back to the start
        currentStatus = getQuery;

        // find all occurrences of the blank space
        vector<size_t> pos;
        size_t found = content.find(" ", 0);
        while (found != string::npos) {
          pos.push_back(found);
          found = content.find(" ", found + 1);
        }

        // read the start & end point coordinates
        // e.g R 5365486 -11333915 5364728 -11335891
        sPoint.lat = stoll(content.substr(pos.begin()[0] + 1, pos.begin()[1] - pos.begin()[0] - 1));
        sPoint.lon = stoll(content.substr(pos.begin()[1] + 1, pos.begin()[2] - pos.begin()[1] - 1));
        ePoint.lat = stoll(content.substr(pos.begin()[2] + 1, pos.begin()[3] - pos.begin()[2] - 1));
        ePoint.lon = stoll(content.substr(pos.begin()[3] + 1));

        // get the points closest to the two input points
        int start = findClosest(sPoint, points), end = findClosest(ePoint, points);
        cout << start << " " << end << endl;

        // run Dijkstra's to compute a shortest path
        unordered_map<int, PLI> tree;
        dijkstra(graph, start, tree);

        if (tree.find(end) == tree.end()) {
          // no path
          cout << "N 0" << endl;
          assert(Serial.writeline("N 0\n"));
          currentStatus = finish;
          break;
        }

        path.clear();
        // read off the path by stepping back through the search tree
        while (end != start) {
          path.push_front(end);
          end = tree[end].second;
        }
        path.push_front(start);

        // # of way points too large
        if (path.size() > 500) {
          cout << "N 0\n" << endl;
          assert(Serial.writeline("N 0\n"));
          currentStatus = finish;
          break;
        }

        cout << "N " << path.size() << endl;
        // write the # of way points into the serial port
        // e.g. N 8<\n>
        string outMsg ="N " + to_string(path.size()) + " " + "\n";
        assert(Serial.writeline(outMsg));

        cout << "Waiting for acknowledgment..." << endl;
        content = Serial.readline(1000);
        cout << content.front() << endl;
        if (content.front() == 'A') {
          // next statue
          currentStatus = sendWayPoints; // else time out
        }
        break;
      }

      case sendWayPoints: {
        // if the required condition fails in the half way
        // back to the start
        currentStatus = getQuery;

        // output the path
        for (auto v : path) {
          cout << "W " << points[v].lat << ' ' << points[v].lon << endl;
          // write the coordinates of way points into the serial port
          // e.g. W 5365488 -11333914<\n>
          string outMsg = "W " + to_string(points[v].lat) + " " + to_string(points[v].lon) + "\n";
          assert(Serial.writeline(outMsg));

          cout << "Waiting for acknowledgment..." << endl;
          content = Serial.readline(1000);
          cout << content.front() << endl;
          if (content.front() != 'A') {
            path.clear();
            break; // time out
          }
        }

        // next statue
        currentStatus = finish;
        break;
      }
        
      case finish: {
        cout << "E" << endl;
        assert(Serial.writeline("E\n"));
        // new query
        currentStatus = getQuery;
        break;
      }
    }
  }
}

int main() {
  WDigraph graph;
  unordered_map<int, Point> points;

  // build the graph
  readGraph("edmonton-roads-2.0.1.txt", graph, points);

  // processes the communication
  sendWayPoints(graph, points);

  return 0;
}
