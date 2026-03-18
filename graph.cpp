#include <cstddef>
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
};

namespace JsonFill {

    const validatorMap VALIDATORS = {
        {"ArticleName", [](const json& j){ return j.is_string(); }},
        {"Tags",        [](const json& j){ 
            if (!j.is_array()) return false;
            for (const auto& el : j) 
                if (!el.is_string()) return false;
            return true;
            }}
    };

    const validatorMap CRYTICAL_VALIDATORS = {
        {"Number",      [](const json& j){ return j.is_number_integer(); }}
    };

    void typesValidation(const json& article, Valid& results, size_t i){
        for (const auto& [key, value] : VALIDATORS) {
            if (!article.contains(key)) {
                results.crytical_errors.push_back(
                    "\tBrakuje pola: '" + key + "' w obiekcie nr." + to_string(i) + 
                    "\n");
                continue;
            }
            if (!value(article.at(key))) {
                results.errors.push_back("\tNiewłaściwa wartość pola: '" + key + "' w obiekcie nr. " + to_string(i) +
                " mogą wystąpić błędy!\n\tSprzwdź dokumentacje o tworzeniu pliku json do budowania grafów");
            }
        }
        for (const auto& [key, value] : CRYTICAL_VALIDATORS) {
            if (!article.contains(key)) {
                results.crytical_errors.push_back(
                    "\tBrakuje pola: '" + key + "' w obiekcie nr." + to_string(i) + 
                    "\n");
                continue;
            }
            if (!value(article.at(key))) {
                results.crytical_errors.push_back("\tNiewłaściwa wartość pola: '" + key + "' w obiekcie nr. " + to_string(i) +
                " błąd krytyczny!\n");
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

        if (results.build()) {
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
        }

        return results;
    } 

    bool checkErrors(const Valid& results){
        if (results.build()) {
            cout << "Crytical names - ok\n" << endl;
        } else {
            cout << "Found some crytical errors!\n\tBecause of this graph can't be build.\n\nCrytical Errors:\n";
            for (const auto& e : results.crytical_errors) {
                cout << e ;
            }
            cout << "\n";
        }
        if (results.ok()) {
            cout << "Configuration's values - ok\n" << endl;
        } else {
            cout << "Found some errors!\n\tBecause of this graph may doesn't work.\n\nErrors:\n";
            for (const auto& e : results.errors) {
                cout << e ;
            }
            cout << "\n";
        }
        if (results.build()) return false;
        return true; 
    }

    Graph buildGraph(const string& json_file){
        Valid results = validation(json_file);
        // validation of json file
        if (checkErrors(results)) throw runtime_error("Json File is corrupted!");
        // printing errors if there is any

        Graph article_graph;
        // variable to store information about graph
        ifstream file(json_file);
        // reading file
        json data;
        // creating container for our json file to parse
        file >> data;
        // automated parsing

        article_graph.pair_graph.resize(data.size());
        article_graph.info.resize(data.size());
        
        for (const auto& element : data) {
            int idx = element["Number"];
            article_graph.info[idx].article_name = element["ArticleName"];

            for (const auto& tag : element["Tags"])
                article_graph.info[idx].tags.push_back(tag);
        }

        for (size_t i = 0; i < article_graph.info.size(); i++) {
            // firt loop to check with other object for commmon tags

            unordered_set<string> tags_A(article_graph.info[i].tags.begin(), article_graph.info[i].tags.end());
            // fast comparison of tags with indexes made from tags

            for (size_t j = i + 1; j < article_graph.info.size(); j++){
                //second loop to compare tagc from first loop object
                int common = 0;
                // common is containing points form common tags and indicate if edge is created

                for (const auto& tag_B : article_graph.info[j].tags) {
                    // looping tags from second object
                    if (tags_A.count(tag_B)) common++;
                    // checking tags from 2 obcjets
                }

                    if (common > 0) {
                        // if there is any common tags we're creating two edges for each one
                        article_graph.pair_graph[i].push_back({j,common});
                        article_graph.pair_graph[j].push_back({i,common});
                    }
            }
        }

        return article_graph;
    }
}

namespace GraphTools {
    vector<pair<int,int>> recommend(int article, Graph& article_graph) {
        vector<pair<int,int>> neighbours = article_graph.pair_graph[article];
        sort(neighbours.begin(), neighbours.end(), [](const pair<int,int>& a, const pair<int,int>& b){
            return a.second > b.second; // malejąco po wadze
        });
        return neighbours;
    }
}

int main(int argc, char *argv[]){
    // command line arguments
    string file = argc > 1 ? argv[1] : "graph.json";
    // check if file path is given or not 
    int article_num = argc > 2 ? stoi(argv[2]) : 0;

    try {
        Graph graph = JsonFill::buildGraph(file);
        // building the graph 
        auto recommendations = GraphTools::recommend(article_num, graph);
        cout << "Recommendations for: " << graph.info[0].article_name << endl;
        for (auto& rec : recommendations)
            cout << graph.info[rec.first].article_name << " (similarity: " << rec.second << ")" << endl;

        for (size_t i = 0; i < graph.pair_graph.size(); i++) {
            cout << graph.info[i].article_name << " -> ";
            for (auto& edge : graph.pair_graph[i])
                cout << graph.info[edge.first].article_name << " (w:" << edge.second << ") ";
            cout << endl;
        }

    } catch (const runtime_error& e) {
        cout << e.what() << endl;
    }
    
    return 0;
}