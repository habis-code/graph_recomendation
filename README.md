# Graph Recommendation System

A graph data structure implemented in C++ that recommends related articles based on tag similarity.

## Features
- Automatic edge generation based on common tags
- Edge weight = number of shared tags between articles
- JSON-based article configuration with validation
- Recommendations sorted by similarity score

## How it works
Each article has a list of tags. The program automatically builds connections between articles that share at least one tag. The more tags in common, the stronger the connection. Given an article, the program returns a list of related articles sorted from most to least similar.

## JSON Structure
```json
[{
    "ArticleName": "Article title",
    "Number": 0,
    "Tags": ["tag1", "tag2"]
}]
```

## Technologies
- C++
- nlohmann/json

## How to run
1. Clone the repository
2. Install nlohmann/json library
3. Compile with `g++ graph.cpp -o graph`
4. Run `./graph <plik.json> <numer_artykulu>`
