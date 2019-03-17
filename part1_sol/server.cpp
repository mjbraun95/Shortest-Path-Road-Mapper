#include <iostream>
#include <list>
#include "wdigraph.h"
#include "dijkstra.h"
#include "server_util.h"

int main() {
  WDigraph graph;
  unordered_map<int, Point> points;

  // build the graph
  readGraph("edmonton-roads-2.0.1.txt", graph, points);

  // read a request
  char c;
  Point sPoint, ePoint;
  cin >> c >> sPoint.lat >> sPoint.lon >> ePoint.lat >> ePoint.lon;

  // c will be 'R' in part 1, no need to check until part 2

  // get the points closest to the two input points
  int start = findClosest(sPoint, points), end = findClosest(ePoint, points);

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
