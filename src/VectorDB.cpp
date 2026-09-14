#include "VectorDB.hpp"
#include <algorithm>
#include <stdexcept>
#include <queue>
#include <cmath>

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
    vectors_[id] = vector;
}

std::vector<SearchResult> VectorDB::search(const std::vector<float>& query_vector, std::size_t top_k) const {
    // Edge case top_k = 0:
    if (top_k == 0) {
        throw std::invalid_argument("Cannot return a vector of 0 results. Please specify a positive, non-zero top_k");
    }
    if (query_vector.size() != dimension_) {
        throw std::invalid_argument("Vector query_vector must have same dimension as the vector database.");
    }
    // Define min heap structure.
    std::priority_queue<SearchResult, std::vector<SearchResult>, std::greater<>> search_results;
    // Iterate through all vectors in the database, compute cosine similarity, and store top_k results in priority queue.
    for (const auto& [id, vector] : vectors_) {
        float score = cosine_similarity(vector, query_vector);
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

float VectorDB::cosine_similarity(const std::vector<float>& a, const std::vector<float>& b) const {
    // Verify same dimension:
    std::size_t n = a.size();
    if (n != b.size()) {
        throw std::invalid_argument("Cannot compute the cosine similarity of vectors with different sizes.");
    }
    // (A \dot B) / (||A|| * ||B||)
    float dot_prod = 0, mag_a = 0, mag_b = 0;
    for (std::size_t i = 0; i < n; i++) {
        float elem_a = a[i], elem_b = b[i];
        dot_prod += (elem_a * elem_b);
        mag_a += elem_a*elem_a;
        mag_b += elem_b*elem_b;
    }
    mag_a = std::sqrt(mag_a);
    mag_b = std::sqrt(mag_b);
    float denom = mag_a * mag_b;
    if (denom == 0) {
        throw std::domain_error("Either of the vectors have a magnitude of 0, making cosine similarity invalid.");
    }
    float cosine_sim = dot_prod/denom;
    return cosine_sim;
}