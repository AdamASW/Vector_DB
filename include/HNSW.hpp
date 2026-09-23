#pragma once

#include <vector>
#include <VectorDB.hpp>
#include <Vector.hpp>
#include <random>

class HNSWIndex {
    public:
        HNSWIndex(std::size_t max_neighbors, std::size_t ef_construction);

        void insert(int id, const Vector& vector);

        std::vector<SearchResult> search(
            const Vector& query,
            std::size_t top_k,
            std::size_t ef_search) const;

    private:
        struct Node {
            int id;
            Vector vector;
            std::vector<std::vector<int>> neighbors;
        };

        std::unordered_map<int, Node> nodes_;

        // Tree Structure Definitions
        int entry_point_;

        int max_level_;

        // RNG
        std::mt19937 rng_;

        size_t random_level();

        // Search methods
        int greedy_search(
            const Vector& query,
            int entry_point,
            int layer
        ) const;

        std::vector<int> search_layer(
            const Vector& query,
            int entry_point,
            size_t ef,
            int layer
        ) const;

        std::vector<int> select_neighbors(
            const Vector& query,
            const std::vector<int>& candidates,
            size_t M
        ) const;
};