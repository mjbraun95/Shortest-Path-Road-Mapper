#include <iostream>
#include "heap.h"
#include "wdigraph.h"
#include "dijkstra.h"

//////////////////////////////////////////////////////////
// CMPUT 275 Winter 2019
// Matthew Braun, 1497171
// Weekly Exercise 5: Graph Concepts
//////////////////////////////////////////////////////////

#include "digraph.h"
#include "digraph.cpp"

#include <iostream> 
#include <fstream>
#include <queue>

using namespace std;

//Found and modified from: https://www.geeksforgeeks.org/breadth-first-search-or-bfs-for-a-graph/
void BFS(Digraph &g, unordered_set<int> &graphVertices)
{ 
    unordered_set<int> output;
    queue<int> queue;
    queue.push(*graphVertices.begin()); 
    int currentNode;
    while(!queue.empty()) 
    { 
        // Dequeue a vertex from queue 
        currentNode = queue.front(); 
        output.insert(currentNode);
        queue.pop(); 
  
        // Get all adjacent vertices of the dequeued 
        // vertex s. If a adjacent has not been visited,  
        // then mark it visited and enqueue it 
        graphVertices.erase(currentNode);
        unordered_set<int>::const_iterator i;
        for (i = g.neighbours(currentNode); 
            i != g.endIterator(currentNode); ++i) 
        { 

            if (graphVertices.erase(*i))
            { 
                queue.push(*i); 
            } 
        } 
    } 
} 

//Counts # of node groups from a Digraph object
int count_components(Digraph g)
{
    //Initialize # of components to 0
    int totalComponents = 0; 

    // Initializing vectors for component counting
    unordered_set<int> neighbourList;
    vector<int> graphVerticesVector = g.vertices();
    unordered_set<int> graphVertices (graphVerticesVector.begin(), graphVerticesVector.end());
    // list<int> graphVertices;
    // copy( graphVerticesVector.begin(), graphVerticesVector.end(), back_inserter( graphVertices ) );
    
    unordered_set<int>::iterator graphVerticesIt = graphVertices.begin();
    unordered_set<int>::const_iterator neighbourListIt;

    // Initializing vectors for BFS 
    // 'i' will be used to get all adjacent 
    // vertices of a vertex 
    unordered_set<int>::const_iterator removeGraphVerticesIt;

    while (graphVertices.size() != 0) 
    {        
        BFS(g,graphVertices);
        totalComponents++;
    }
    return totalComponents;
}

// Reads a file with vertex + edge data and builds a Digraph from scratch
WDigraph read_city_graph_undirected(string filename) 
{
    WDigraph output_graph;
    ifstream txtfile;
    string line;
    txtfile.open(filename, ifstream::in);

    // Checks if the specified file exists
    if (!txtfile) 
    {
        cerr << "Unable to open file " << filename << endl;
        exit(1);   // call system to stop
    }
    size_t searchE;
    size_t searchV;
    size_t searchComma1;
    size_t searchComma2;
    size_t searchComma3;
    int edge1;
    int edge2;
    string alongTheEdge;
    int VID;
    while(getline(txtfile, line)) 
    {
        // breaks down each txt file data line and extracts key variables
        searchE = line.find("E",0);
        searchV = line.find("V",0);
        searchComma1 = line.find(",",searchE+1);
        searchComma2 = line.find(",",searchComma1+1);
        searchComma3 = line.find(",",searchComma2+1);

        // If the line of data is for an edge
        if (searchE == 0) 
        {
            // Second parameter edge case
            edge1 = stoi(line.substr(searchComma1+1,(searchComma2 - searchComma1 - 1))); 
            // Third parameter edge case
            edge2 = stoi(line.substr(searchComma2+1,(searchComma3 - searchComma2 - 1))); 
            // End parameter edge case
            alongTheEdge = line.substr(searchComma3+1,(line.length() - searchComma3 - 1));
            // Adds edge to WDigraph
            output_graph.addEdge(edge1, edge2);
            output_graph.addEdge(edge2, edge1);
            
        }

        // If the line of data is for a vertex
        else if (searchV == 0)
        {
            // Second parameter vertex case
            VID = stoi(line.substr(searchComma1+1,(searchComma2 - searchComma1 - 1))); 
            //Adds vertex to Digraph
            output_graph.addVertex(VID);   
        }
    }
    return output_graph;
}

int main() 
{
    //Loads txt file
    Digraph yeg_graph = read_city_graph_undirected("edmonton-roads-2.0.1.txt");
    int num_components = count_components(yeg_graph);
    cout << num_components << endl;
}

int main() {
    WDigraph graph;

    int startVertex, endVertex;

    // reading in the graph
    int n, m;
    cin >> n >> m;
    for (int i = 0; i < n; ++i) {
        int label;
        cin >> label;
        graph.addVertex(label);
    }
    for (int i = 0; i < m; ++i) {
        int u, v;
        long long c;
        cin >> u >> v >> c;
        graph.addEdge(u, v, c);
    }
    cin >> startVertex >> endVertex;


    // run the search
    // searchTree[v] will be the pair (u,d) of the node u prior to v
    // in a shortest path to v and d will be the shortest path distance to v
    unordered_map<int, PIL> searchTree;
    dijkstra(graph, startVertex, searchTree);

    // read off a path
    list<int> path;
    if (searchTree.find(endVertex) == searchTree.end()) {
      cout << "Vertex " << endVertex << " not reachable from " << startVertex << endl;
    }
    else {
      int stepping = endVertex;
      while (stepping != startVertex) {
        path.push_front(stepping);

        // crawl up the search tree one step
        stepping = searchTree[stepping].first;
      }
      path.push_front(startVertex);

      cout << "Cost of cheapest path from " << startVertex << " to "
           << endVertex << " is: " << searchTree[endVertex].second << endl;
      cout << "Path:";
      for (auto it : path) {
        cout << ' ' << it;
      }
      cout << endl;
    }

    return 0;
}
