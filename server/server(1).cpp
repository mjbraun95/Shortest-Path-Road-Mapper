/*
  Names:  Ang Li  Matthew Braun
  IDs:    1550746 1497171
  CMPUT 275, Winter 2019

  Assignment 2: Directions Part 1

*/
#include <cstdlib> // for abs
#include <fstream>
#include <iostream>
#include <list>
#include <unordered_map>
#include <vector>
#include "dijkstra.h"
#include "heap.h"
#include "wdigraph.h"

using namespace std;

// PLI is an alias for "pair<long long, int>" type as discussed in class
typedef pair<long long, int> PLI;

struct Point {
    long long lat; // latitude of the point
    long long lon; // longitude of the point
};

// Return the Manhattan distance between the two given points
long long manhattan(const Point& pt1, const Point& pt2) {
    return (abs(pt1.lat - pt2.lat) + abs(pt1.lon - pt2.lon));
}

/*
    Read the Edmonton map data from the provided file and load it into the given WDigraph object.
    Store vertex coordinates in Point struct and map each vertex to its corresponding Point struct.

    PARAMETERS:
    filename: name of the file describing a road network
    graph: an instance of the weighted directed graph (WDigraph) class
    points: a mapping between vertex identifiers and their coordinates
*/
void readGraph(string filename, WDigraph& graph, unordered_map<int, Point>& points) {
    ifstream inputFile(filename);
    string content;

    // read the file line by line
    while (getline(inputFile, content)) {
        // find all occurrences of the comma
        vector<size_t> pos;
        size_t found = content.find(",", 0);
        while (found != string::npos) {
            pos.push_back(found);
            found = content.find(",", found + 1);
        }

        // get the info of vertices & edges according to the positions of the comma
        if (content.front() == 'V') {
            // e.g. V,29577354,53.430996,-113.491331
            // vertex ID
            int VID = stoi(content.substr(pos.begin()[0] + 1, pos.begin()[1] - pos.begin()[0] - 1)); 
            
            // latitude
            long double lat = stold(content.substr(pos.begin()[1] + 1, pos.begin()[2] - pos.begin()[1] - 1)); 
            long long LLlat = static_cast<long long>(100000 * lat);

            // longitude
            long double lon = stold(content.substr(pos.begin()[2] + 1));
            long long LLlon = static_cast<long long>(100000 * lon);

            Point newPoint;
            newPoint.lat = LLlat;
            newPoint.lon = LLlon;

            // Adds VID & its coordinates to the points
            points.insert({VID, newPoint});
        }
        else if (content.front() == 'E') {
            // e.g. E,277483195,277483200,109 Street NW
            // first edge
            int edge1 = stoi(content.substr(pos.begin()[0] + 1, pos.begin()[1] - pos.begin()[0] - 1)); 
            // second edge
            int edge2 = stoi(content.substr(pos.begin()[1] + 1, pos.begin()[2] - pos.begin()[1] - 1));
            // the name of the street
            // alongTheEdge = content.substr(pos.begin()[2] + 1);

            // Calculates Manhattan distance between 2 edges
            // Adds edges to WDigraph
            graph.addEdge(edge1, edge2, manhattan(points[edge1], points[edge2]));
        }
    }
}

// find the closest vertices in the road map of Edmonton to
// the start & end points according to the Manhattan distance
int findNearstVertex(Point currentPoint, unordered_map<int, Point>& points) {
    // comparison starts from the 1st vertex stored in the points
    int nearstV = points.begin()->first;
    long long lowestWeight = manhattan(currentPoint, points[nearstV]);

    for (auto iter: points) {
        if (manhattan(currentPoint, iter.second) < lowestWeight) {
            nearstV = iter.first;
            lowestWeight = manhattan(currentPoint, iter.second);
        }
    }

    return nearstV;
}

// compute a shortest path along Edmonton streets between the two vertices found,
// and finally output the found way-points from the first vertex to the last
// searchTree[v] will be the pair (d, u) of the node u prior to v in a shortest path to v
// and d will be the shortest path distance to v
void getShortestPath(int startV, int endV,
    unordered_map<int, PLI>& searchTree, unordered_map<int, Point>& points) {
    // read off a path
    list<int> path;
    string response;
    // no path from the start to the end vertex nearest to the start and end points sent to the server
    if (searchTree.find(endV) == searchTree.end()) {
      cout << "N" << " " << 0 << endl;
      return;
    }
    // send the coordinates of the way-points
    else {
        int stepping = endV;
        while (stepping != startV) {
            path.push_front(stepping);

            // crawl up the search tree one step
            stepping = searchTree[stepping].second;
        }
        path.push_front(startV);

        int steps = path.size();
        cout << "N" << " " << steps << endl;
        for (auto iter : path) {
            while (cin >> response) {
                if (response == "A") {
                    break;
                }
            }
            cout << "W" << " " << points[iter].lat << " " << points[iter].lon << endl;
        }
    }
    while (cin >> response) {
        if (response == "A") {
            break;
        }
    }
    cout << "E" << endl;
}

// receive and process requests by reading from and writing to stdin & stdout, respectively
void stdIO() {
    WDigraph graph;
    unordered_map<int, Point> points;
    // read the Edmonton map data from the provided file and load it into the given WDigraph object
    // store vertex coordinates in Point struct and map each vertex to its corresponding Point struct
    readGraph("edmonton-roads-2.0.1.txt", graph, points);

    // current position & destination
    Point startP;
    Point endP;

    // read from stdin
    string request;
    while (cin >> request) {
        if (request == "R") {
            cin >> startP.lat >> startP.lon >> endP.lat >> endP.lon;
            // find the closest vertices in the road map of Edmonton to
            // the start and end points according to the Manhattan distance
            int startV = findNearstVertex(startP, points);
            int endV = findNearstVertex(endP, points);

            // run the search
            // searchTree[v] will be the pair (d, u) of the node u prior to v in a shortest path to v
            // and d will be the shortest path distance to v
            unordered_map<int, PLI> searchTree;
            dijkstra(graph, startV, searchTree);
            // compute a shortest path along Edmonton streets between the two vertices found,
            // and finally output the found way-points from the first vertex to the last
            getShortestPath(startV, endV, searchTree, points);
            break;
        }
    }

    
} 

int main() {
    // receive and process requests by reading from
    // and writing to stdin & stdout, respectively
    stdIO();
    return 0;
}