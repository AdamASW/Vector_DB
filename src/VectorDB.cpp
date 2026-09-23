#include "VectorDB.hpp"
#include <algorithm>
#include <queue>
#include <stdexcept>

VectorDB::VectorDB(std::size_t dimension, VectorDBVariant variant) {
    dimension_ = dimension;
    variant_ = variant;
}

void VectorDB::insert(int id, const std::vector<float>& vector) {
    if (vector.size() != dimension_) {
        throw std::invalid_argument(
        "Inserted vector must have the same dimension as the vector database."
        );
    }
    vectors_.insert_or_assign(id, Vector(vector));
}

std::size_t VectorDB::size() const {
    return vectors_.size();
}

std::vector<SearchResult> VectorDB::search(const std::vector<float>& query_vector, std::size_t top_k) const {
    // Edge case top_k = 0:
    if (top_k == 0) {
        throw std::invalid_argument("Cannot return a vector of 0 results. Please specify a positive, non-zero top_k");
    }
    Vector query(query_vector);
    if (query.dimension() != dimension_) {
        throw std::invalid_argument("Vector query_vector must have same dimension as the vector database.");
    }
    // Define min heap structure.
    std::priority_queue<SearchResult, std::vector<SearchResult>, std::greater<>> search_results;
    // Iterate through all vectors in the database, compute cosine similarity, and store top_k results in priority queue.
    for (const auto& [id, vector] : vectors_) {
        float score = vector.cosine_similarity(query_vector);
        SearchResult curr_result{id, score};
        if (search_results.size() >= top_k) { // should never be >
            float worst_score = search_results.top().score;
            if (score <= worst_score) { continue; }
            search_results.pop();
        }
        search_results.push(curr_result); 
    }
    // Turn priority queue into a vector to return:
    std::size_t results_size = std::min(top_k, search_results.size());
    std::vector<SearchResult> results(results_size);
    for (std::size_t i = results_size; i-- > 0;) {
        results[i] = search_results.top();
        search_results.pop();
    }
    return results;
}