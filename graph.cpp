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

class GraphVertex{
    public:
    vector<GraphInfo> info;//info do grafów

    vector<vector<int>> graph;//grafy bez wag
    
    vector<vector<pair<int, int>>> pair_graph;//grafy z wagami
    vector<vector<int>> graph_weight;
    
    queue<int> graph_queue;//potrzebne do algorytmów
    vector<int> distans;
    vector<int> visited;
};

namespace GraphTools {
    void dfs(int start, GraphVertex& g){
        g.visited[start] = 1;

        for (int neighbor : g.graph[start]) {
            if (g.visited[neighbor] == -1) dfs(neighbor, g);
        }
    }

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
    
    void dijkstra(){
    }
    
    void weight(int first, int second, GraphVertex& g){

        unordered_set<string> set_a(g.info[first].tags.begin(), g.info[first].tags.end());

        int common = 0, only_b = 0;

        for (auto& b : g.info[second].tags) {
            if (set_a.count(b)) {
                common++;
            } else {
                only_b++;
            }
        }

        int only_a = g.info[first].tags.size() - common;

        g.graph_weight[first][second] = common - only_a - only_b;
    }

    void fillWeight(GraphVertex& g){
        for (int x = 0; x < g.pair_graph.size(); x++) {
            for (auto& pair : g.pair_graph[x]) {
                int y = pair.first;
                GraphTools::weight(x, y, g);
            }
        }
    }
}

int main(){
    
    
    return 0;
}