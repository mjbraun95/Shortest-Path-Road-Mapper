/*
  Names:  Ang Li  Matthew Braun
  IDs:    1550746 1497171
  CMPUT 275, Winter 2019

  Assignment 2: Directions Part 1

*/
#ifndef _DIJKSTRA_H_
#define _DIJKSTRA_H_
#include "wdigraph.h"

// PLI is an alias for "pair<long long, int>" type as discussed in class
typedef pair<long long, int> PLI;

/*
    Compute least cost paths that start from a given vertex
    Use a binary heap to efficiently retrieve an unexplored
    vertex that has the minimum distance from the start vertex
    at every iteration

    PLI is an alias for "pair<long long, int>" type as discussed in class

    PARAMETERS:
    WDigraph: an instance of the weighted directed graph (WDigraph) class
    startVertex: a vertex in this graph which serves as the root of the search tree
    tree: a search tree used to construct the least cost path to some vertex
*/
void dijkstra(const WDigraph& graph, int startVertex, unordered_map<int, PLI>& tree);

#endif
