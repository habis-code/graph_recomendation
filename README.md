# Graph Recommendation System

A graph data structure implemented in C++ that models relationships between articles based on tag similarity.

## Features
- Directed and undirected graph support
- BFS and DFS traversal
- Automatic edge weight calculation based on article tag similarity
- JSON-based graph configuration with validation

## How it works
Each article has a list of tags. The weight of an edge between two articles is calculated based on how many tags they share. The more tags in common, the stronger the connection.

## JSON Structure
```json
[{
    "ArticleName": "Article title",
    "Number": 0,
    "Neighbours": [1, 2],
    "Tags": ["tag1", "tag2"],
    "Direct": false
}]
```

## Technologies
- C++
- nlohmann/json

## How to run
1. Clone the repository
2. Install nlohmann/json library
3. Compile with `g++ graph.cpp -o graph`
4. Edit JSON files to define your graph structure
5. Run `./graph`
