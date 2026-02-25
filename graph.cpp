#include <queue>
#include <string>
#include <unordered_set>
#include <vector>

using namespace std;

class GraphInfo{
    public:
    vector<string> tags;
    string article_name;
};

class GraphVertex{ // class with everything needed for graphs
    public:
    vector<GraphInfo> info; // information about graph

    vector<vector<int>> graph; // graphs without weight
    
    vector<vector<pair<int, int>>> pair_graph; // graphs with weight
    vector<vector<int>> graph_weight; // weight in grapths
    
    queue<int> graph_queue; // needed for grapth_tools (GraphTools)
    vector<int> distans; // needed for algorithm
    vector<int> visited; // needed for algorithm
};

namespace GraphTools {
    void dfs(int start, GraphVertex& g){
        g.visited[start] = 1;

        for (int neighbor : g.graph[start]) {
            if (g.visited[neighbor] == -1) dfs(neighbor, g);
        }
    } 
    // function is okey
    // info: Variable 'visited' needs to be assigned with -1 first, and 
    // it doesn't save information about the way it goes

    void bfs(int start, GraphVertex& g){
        g.graph_queue.push(start);
        g.distans[start] = 0;

        while (!g.graph_queue.empty()) {
            int v = g.graph_queue.front(); 
            g.graph_queue.pop();

            for (int vertex : g.graph[v]) {
                
                if (g.distans[vertex] == -1) {
                    g.graph_queue.push(vertex);
                    g.distans[vertex] = g.distans[v] + 1;
                }
            }
        }
    }
    // I don't see anything worng with this function. It's alright!
    // info: As with the previous function, 
    // assigning this time distance before using the function is needed
    
    void dijkstra(){
    }
    
    void weight(int first, int second, GraphVertex& g){

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

        g.graph_weight[first][second] = common - only_a - only_b;
        // Value of how many points an article has
        // when tags are similar to each other
        // first - vertex of graph
        // second - neighbour of the vertex
        // question: to not waste the energy and time,
        // can't we assign the points to the neighbour
        // like 'g.graph_weight[second][first] = common - only_a - only_b;'
    }

    void fillWeight(GraphVertex& g){
        for (int x = 0; x < g.pair_graph.size(); x++) {
            // For each vertex of graph is doing 'for loop'
            for (auto& pair : g.pair_graph[x]) {
                // It gets the pair container to pair variable

                int y = pair.first;
                // Variable 'y' takes the first value from pair

                GraphTools::weight(x, y, g);
                // Execution of 'weight()' function to fill up the weight
            }
        }
    }
}

int main(){
    
    
    return 0;
}