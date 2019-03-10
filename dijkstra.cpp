/*
  Names:  Ang Li  Matthew Braun
  IDs:    1550746 1497171
  CMPUT 275, Winter 2019

  Assignment 2: Directions Part 1

*/
#include <unordered_map>
#include <utility> // for pair()
#include "dijkstra.h"
#include "heap.h"
#include "wdigraph.h"

using namespace std;

// PLI is an alias for "pair<long long, int>" type as discussed in class
typedef pair<long long, int> PLI;

// Dijkstra's algorithm for finding the cheapest path 
void dijkstra(const WDigraph& graph, int startVertex, 
    unordered_map<int, PLI>& searchTree) {

    // all active fires stored as follows:
    // say an entry is (v, (d, u)), then there is a fire that started at u
    // and will burn the u -> v edge, reaching v at time d
    BinaryHeap<int, PLI> events;

    // at time 0, the startVertex burns
    events.insert(startVertex, PLI(0, startVertex));

    // while there is an active fire
    while (events.size() > 0) {
        // find the fire that reaches its endpoint v earliest
        pair<int, PLI> earliestFire = events.min();
        // to reduce notation in the code below, this u, v, d agrees with
        // the intuition presented in the comment
        int v = earliestFire.first; 
        int u = earliestFire.second.second;
        long long d = earliestFire.second.first;
        events.popMin();

        if (searchTree.find(v) != searchTree.end()) {
            continue;
        }

        // if v doesn't belong to searchTree then
        // declare that v is "burned" at time d with a fire that spawned from u
        searchTree[v] = PLI(d, u);

        // now start fires from all edges exiting vertex v
        for (auto iter = graph.neighbours(v); iter != graph.endIterator(v); iter++) {
            int nbr = *iter;

            // the fire starts at v at time d and will reach nbr
            // at time d + (length of v -> nbr edge)
            long long burn = d + graph.getCost(v, nbr);
            events.insert(nbr, PLI(burn, v));
        }
    }
}
