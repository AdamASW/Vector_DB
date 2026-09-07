#pragma once

#include <cstddef>
#include <iostream>
#include <string>
#include <cmath>
#include <fstream>
#include <ctime>
#include <unordered_map>
#include <vector>

using namespace std;

struct SearchResult {
    int id;
    float score;

    bool operator>(const SearchResult& other) const {
        return score > other.score;
    }
};

class VectorDB {
public:
    explicit VectorDB(std::size_t dimension);

    void insert(int id, const std::vector<float>& vector);

    std::vector<SearchResult> search(
        const std::vector<float>& query_vector, 
        std::size_t top_k)
        const;

    std::size_t size() const;
    std::size_t dimension() const;

private:

    std::size_t dimension_;

    std::unordered_map<int, std::vector<float>> vectors_;

    float cosine_similarity(
        const std::vector<float>& a, 
        const std::vector<float>& b) 
        const;
};