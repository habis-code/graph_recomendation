#include <cstddef>
#include <queue>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream> 

using namespace std;
using json = nlohmann::json;
using validatorMap = std::map<std::string, std::function<bool(const json&)>>;

struct Valid {
    std::vector<std::string> errors;
    std::vector<std::string> crytical_errors;

    bool ok() const {
        return errors.empty();
    }

    bool build() const {
        return crytical_errors.empty();
    }
};

struct GraphInfo{
    vector<string> tags;
    string article_name;
};

struct Graph{
    vector<GraphInfo> info; 
    vector<vector<pair<int, int>>> pair_graph; 
    bool direct; 
};

namespace JsonFill {
    
    const validatorMap validators = {
        {"ArticleName", [](const json& j){ return j.is_string(); }},
        {"Number",      [](const json& j){ return j.is_number_integer(); }},
        {"Neighbours",  [](const json& j){ 
            if (!j.is_array()) return false;
            for (const auto& el : j)
                if (!el.is_number_integer()) return false;
            return true;
        }},
        {"Tags",        [](const json& j){ 
            if (!j.is_array()) return false;
            for (const auto& el : j) 
                if (!el.is_string()) return false;
            return true;
            }},
        {"Direct",      [](const json& j){ return j.is_boolean(); }}
    };

    void typesValidation(const json& article, Valid& results, size_t i){
        for (const auto& [key, value] : validators) {
            if (!article.contains(key)) {
                results.crytical_errors.push_back(
                    "Brakuje pola: " + key + "w obiekcie nr." + to_string(i) + 
                    "\nNie można zbudować grafu");
                continue;
            }
            if (!value(article.at(key))) {
                results.errors.push_back("Niewłaściwy typ pola: " + key + "w obiekcie nr. " + to_string(i) +
                " mogą wystąpić błędy!\n Sprzwdź dokumentacje o tworzeniu pliku json do budowania grafów");
            }
        }
    }

    Valid validation(const string& json_file){
        ifstream file(json_file);

        Valid results;
        string line;
        json data;

        if (!file) throw runtime_error("Nie można otworzyć pliku");
        
        file >> data;

        if (!data.is_array()) {
            results.crytical_errors.push_back("Plik główny musi być tablicą obiektów.");
            return results;
        }
        for (size_t i = 0; i < data.size(); ++i) {
            const auto& article = data[i];

            if (!article.is_object()) {
                results.crytical_errors.push_back(
                    "Element nr " + std::to_string(i) + " nie jest obiektem."
                );
                continue;
            }
            typesValidation(article, results, i);
        }

        unordered_set<int> numbers;
        for (const auto& element : data) 
            numbers.insert(element["Number"].get<int>());

        for (size_t i = 0; i < data.size(); i++) {
            if (!numbers.count(i)) {
                results.crytical_errors.push_back(
                    "Numery wierzchołków muszą być kolejne od 0! Brakuje wierzchołka nr. " + to_string(i)
                );
            }
        }

        return results;
    } 

    bool checkErrors(const Valid& results){
        if (results.ok()) {
            cout << "Configuration's values - ok\n" << endl;
        } else {
            cout << "Found some errors!\n Because of this graph may doesn't work.\n Errors:\n";
            for (const auto e : results.errors) {
                cout << e ;
            }
        }
        if (results.build()) return false;
        return true; 
    }

    Graph buildGraph(const string& json_file){
        Valid results = validation(json_file);
        if (checkErrors(results)) throw runtime_error("Json File is corrupted!");

        Graph graph;
        ifstream file(json_file);
        json data;
        file >> data;

        graph.pair_graph.resize(data.size());
        graph.info.resize(data.size());
        graph.direct = data[0]["Direct"];
        
        for (const auto& element : data) {
            int idx = element["Number"];
            graph.info[idx].article_name = element["ArticleName"];

            for (const auto& tag : element["Tags"]) {
                graph.info[idx].tags.push_back(tag);
            }

            for (const auto& num : element["Neighbours"]) {
                graph.pair_graph.at(idx).push_back({num, 0});
            }
        }
        return graph;
    }
}

namespace GraphTools {

    void dfs(int start, Graph& g, vector<int>& visited){
        visited[start] = 1;

        for (auto& pair_neighbor : g.pair_graph[start]) {
            if (visited[pair_neighbor.first] == -1) dfs(pair_neighbor.first, g, visited);
        }
    } 
    
    void bfs(int start, Graph& g, vector<int>& distances, queue<int>& graph_queue){ 
        graph_queue.push(start);
        distances[start] = 0;

        while (!graph_queue.empty()) {
            int v = graph_queue.front(); 
            graph_queue.pop();

            for (auto& vertex : g.pair_graph[v]) {
                
                if (distances[vertex.first] == -1) {
                    graph_queue.push(vertex.first);
                    distances[vertex.first] = distances[v] + 1;
                }
            }
        }
    }

    void dijkstra(){
        throw runtime_error("not implemented");
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
        for (size_t x = 0; x < g.pair_graph.size(); x++) {
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
    // Graph graph;
    // vector<int> visited(graph.pair_graph.size(), -1);
    // vector<int> distans(graph.pair_graph.size(), -1);
    // queue<int> graph_queue;
    // how to implement the graph

    try {
        Graph graph = JsonFill::buildGraph("graph_disconnected.json");
        GraphTools::fillWeight(graph);

        vector<int> visited(graph.pair_graph.size(), -1);
        GraphTools::dfs(0, graph, visited);

        vector<int> distances(graph.pair_graph.size(), -1);
        queue<int> graph_queue;
        GraphTools::bfs(0, graph, distances, graph_queue);

    } catch (const runtime_error& e) {
        cout << e.what() << endl;
    }

    
    
    return 0;
}