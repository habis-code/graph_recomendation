#include <queue>
#include <string>
#include <unordered_set>
#include <vector>
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream> 
#include <iomanip>

using namespace std;
using json = nlohmann::json;

class GraphInfo{
    public:
    vector<string> tags;
    string article_name;
};

class Graph{ // class with everything needed for graphs
    public:
    vector<GraphInfo> info; // information about graph
    vector<vector<pair<int, int>>> pair_graph; // graphs with weight
    bool direct; // is the graph directed or not?
};

namespace GraphTools {

    void dfs(int start, Graph& g, vector<int>& visited){
        visited[start] = 1;

        for (auto& pair_neighbor : g.pair_graph[start]) {
            if (visited[pair_neighbor.first] == -1) dfs(pair_neighbor.first, g, visited);
        }
    } 
    
    void bfs(int start, Graph& g, vector<int>& distans, queue<int>& graph_queue){ 
        graph_queue.push(start);
        distans[start] = 0;

        while (!graph_queue.empty()) {
            int v = graph_queue.front(); 
            graph_queue.pop();

            for (auto& vertex : g.pair_graph[v]) {
                
                if (distans[vertex.first] == -1) {
                    graph_queue.push(vertex.first);
                    distans[vertex.first] = distans[v] + 1;
                }
            }
        }
    }

    void dijkstra(){
    }
    
    void weight(int first, int second, Graph& g, bool direct){

        // Algorithm assigning weight based on tags related to articles

        unordered_set<string> set_a(g.info[first].tags.begin(), g.info[first].tags.end());
        // Using 'set_a' to compare tags using mash instead of array (fastter)
        // I make set_a importing information from 'g.info[vertex_of_the_graph'
        // that instead of regular indexes uses mash which converts string 
        // to sha number used in index

        int common = 0, only_b = 0;

        for (auto& b : g.info[second].tags) {
            if (set_a.count(b)) {
                common++;
            } else {
                only_b++;
            }
        }
        // Determines if the tag is common or not and stores it in variables.

        int only_a = g.info[first].tags.size() - common;
        // Value of the uncommon tags from article A

        for (auto& edge : g.pair_graph[first]) {
            if (edge.first == second) {
                edge.second = common - only_a - only_b;
                break;
            }
        }
        
        if (!g.direct) {
            for (auto& edge : g.pair_graph[second]) {
                if (edge.first == first) {
                    edge.second = common - only_a - only_b;
                    break;
                }
            }
        }
    }

    void fillWeight(Graph& g){
        for (int x = 0; x < g.pair_graph.size(); x++) {
            // For each vertex of graph is doing 'for loop'
            for (auto& pair : g.pair_graph[x]) {
                // It gets the pair container to pair variable

                int y = pair.first;
                // Variable 'y' takes the neighbour number

                if (!g.direct && x > y) continue;   
                // pomiń duplikat

                weight(x, y, g, g.direct);
                // Execution of 'weight()' function to fill up the weight
            }
        }
    }
}

int main(){
    Graph graph;
    vector<int> visited(graph.pair_graph.size(), -1);
    vector<int> distans(graph.pair_graph.size(), -1);
    queue<int> graph_queue;


    json j;
    std::cin >> j;  // poprawne wczytanie z stdin
    std::cout << std::setw(4) << j << std::endl;
    
    return 0;
}