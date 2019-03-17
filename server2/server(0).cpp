#include <iostream>
#include <list>

#include <cassert>
#include <string>
#include "serialport.h"

#include "wdigraph.h"
#include "dijkstra.h"
#include "server_util.h"

using namespace std;

int main() {
  SerialPort Serial("/dev/ttyACM0");
  WDigraph graph;
  string line;

  string startPhase = "Start: \n";
  string endPhase = "End: \n";
  unordered_map<int, Point> points;

  // build the graph
  readGraph("edmonton-roads-2.0.1.txt", graph, points);

  cout << "debug1" << endl;
  // read a request
  // char c;
  Point sPoint, ePoint;
  // cin >> c >> sPoint.lat >> sPoint.lon >> ePoint.lat >> ePoint.lon;
  cout << "debug2" << endl;
  // while (line != startPhase) 
  // {
  //   line = Serial.readline();
  // }
  line = Serial.readline();
  // c will be 'R' in part 1, no need to check until part 2

  cout << "Received: " << line; // note '\n' is in the string already
  // get the points closest to the two input points
  line = Serial.readline();
  sPoint.lat = stoll(line);
  line = Serial.readline();
  sPoint.lon = stoll(line);

  cout << "StartPoint: " << sPoint.lat << "," << sPoint.lon << endl;


  
  line = Serial.readline();
  
  // c will be 'R' in part 1, no need to check until part 2

  cout << "Received: " << line; // note '\n' is in the string already

  line = Serial.readline();
  ePoint.lat = stoll(line);
  line = Serial.readline();
  ePoint.lon = stoll(line);

  cout << "EndPoint: " << ePoint.lat << "," << ePoint.lon << endl;

  int start = findClosest(sPoint, points);
  cout << "debug3" << endl;
  int end = findClosest(ePoint, points);
  
  // run dijkstra's to compute a shortest path
  unordered_map<int, PLI> tree;
  dijkstra(graph, start, tree);

  if (tree.find(end) == tree.end()) {
      // no path
      cout << "N 0" << endl;
  }
  else {
    // read off the path by stepping back through the search tree
    list<int> path;
    while (end != start) {
      path.push_front(end);
      end = tree[end].second;
    }
    path.push_front(start);

    // output the path
    cout << "N " << path.size() << endl;
    for (auto v : path) {
      cout << "W " << points[v].lat << ' ' << points[v].lon << endl;
    }
    cout << "E" << endl;
  }
  

  return 0;
}
