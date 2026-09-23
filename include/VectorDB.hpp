#pragma once

#include <cstddef>
#include <unordered_map>
#include <vector>

#include "Vector.hpp"

using namespace std;

struct SearchResult {
    int id;
    float score;

    bool operator>(const SearchResult& other) const {
        return score > other.score;
    }
};

enum class VectorDBVariant {
    Naive,
    OptimizedVectors,
    OptimizedSearch,
    OptimizedVectorsAndSearch
};

class VectorDB {
public:
    explicit VectorDB(std::size_t dimension,
        VectorDBVariant variant = VectorDBVariant::Naive);

    void insert(int id, const std::vector<float>& vector);

    std::vector<SearchResult> search(
        const std::vector<float>& query_vector, 
        std::size_t top_k)
        const;

    std::size_t size() const;
    std::size_t dimension() const;

private:

    VectorDBVariant variant_;
        
    std::size_t dimension_;

    std::unordered_map<int, Vector> vectors_;
};