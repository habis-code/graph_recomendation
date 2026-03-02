#include <queue>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream> 
#include <iomanip>

using namespace std;
using json = nlohmann::json;

struct valid {
    std::vector<std::string> errors;
    std::vector<std::string> cryt_err;

    bool ok() const {
        return errors.empty();
    }

    bool build() const {
        return cryt_err.empty();
    }
}; // var => errors | fun bool 'ok'


class GraphInfo{
    public:
    vector<string> tags;
    string article_name;
}; // var => tags, article_name

class Graph{
    public:
    vector<GraphInfo> info; 
    vector<vector<pair<int, int>>> pair_graph; 
    bool direct; 
}; // var => info, pair_graph, direct


namespace JsonFill {
    
    constexpr std::array<const char*, 5> names = {
        "ArticleName", "Number", "Neighbours", "Tags", "Direct"
    };
    
    void name_validation(const json& data, valid& results){
        for (const auto& name : names) {
            if (!data.contains(name)) 
            results.cryt_err.push_back(string("Brakuje pola: [") + name + "]");
        }
    }

    void value_validation(const json& data, valid& results){
        if (data.contains("ArticleName") && !data.at("ArticleName").is_string())
            results.errors.push_back("'ArticleName' musi byc stringiem");

        if (data.contains("Number") && !data.at("Number").is_number_integer()) 
            results.errors.push_back("'Number' musi byc intigerem");

        if (data.contains("Neighbours") && !data.at("Neighbours").is_array()) 
            results.errors.push_back("'Neighbours' musi byc tablica");

        if (data.contains("Tags") && !data.at("Tags").is_array()) 
            results.errors.push_back("'Tags' musi byc tablica");

        if (data.contains("Direct") && !data.at("Direct").is_boolean()) 
            results.errors.push_back("'Direct' musi byc boolean");
    }

    valid validation(const string json_file){
        ifstream file(json_file);

        valid results;
        string line;
        json data;

        if (!file) throw runtime_error("Nie można otworzyć pliku");
        
        file >> data;

        name_validation(data, results);
        if (!results.cryt_err.empty()) return results;
        value_validation(data, results);

        return results;
    }
}

// if (results.ok() == true) {
//     cout << "Walidacja zakończona POZYTYWNIE" << endl;
// } else {
//     cout << "UWAGA Walidacja zakończona NEGATYNIE \n\n Rodzaje błędów:\n" << endl;
//     for (auto err : results.errors) {
//         cout << err << endl;
//     }
// }
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
    
    return 0;
}