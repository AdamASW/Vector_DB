#pragma once

#include <cstddef>
#include <random>
#include <unordered_map>
#include <vector>

#include <Vector.hpp>
#include <SearchResult.hpp>

class HNSWIndex {
public:
    HNSWIndex(std::size_t max_neighbors, std::size_t ef_construction);

    void insert(int id, const Vector& vector);

    std::vector<SearchResult> search(const Vector& query, std::size_t top_k,
        std::size_t ef_search) const;

private:
    struct Node {
        int id;
        Vector vector;
        std::vector<std::vector<int>> neighbors;
    };

    std::size_t max_neighbors;
    std::size_t ef_construction;
    std::unordered_map<int, Node> graph_;

    int entry_point_;
    int max_level_;
    std::mt19937 rng_;

    std::size_t random_level();

    int greedy_search(const Vector& query, int entry_point, int layer) const;

    std::vector<int> search_layer(const Vector& query, int entry_point,
        std::size_t ef, int layer) const;

    std::vector<int> select_neighbors(const Vector& query,
        const std::vector<int>& candidates, std::size_t M) const;
};