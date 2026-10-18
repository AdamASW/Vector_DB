#include "HNSW.hpp"

#include <functional>
#include <unordered_set>
#include <algorithm>
#include <queue>
#include <stdexcept>
#include <utility>

using namespace std;
HNSWIndex::HNSWIndex(std::size_t max_neighbors, std::size_t ef_construction)
        : max_neighbors(max_neighbors),
            ef_construction(ef_construction),
      entry_point_(-1),
      max_level_(-1),
      rng_(std::random_device{}()) {
    if (max_neighbors == 0) {
        throw std::invalid_argument("max_neighbors must be positive.");
    }
    if (ef_construction == 0) {
        throw std::invalid_argument("ef_construction must be positive.");
    }
}

void HNSWIndex::insert(int id, const Vector& vector) {
        auto existing = graph_.find(id);
        if (existing != graph_.end()) {
            existing->second.vector = vector;
            return;
        }

        // Initialize the new node in the graph:
        int level = random_level();
        std::vector<std::vector<int>> new_neighbors(level + 1, std::vector<int>());
        Node new_node{id, vector, new_neighbors};

        // Deal with empty graph:
        if (graph_.empty()) { 
            entry_point_ = id;
            graph_.emplace(id, new_node);
            max_level_ = level;
            return;
        }

        const int previous_max_level = max_level_;
        graph_.emplace(id, std::move(new_node));

        // If graph non-empty, start search for upper layers, if applicable:
        int entry_node = entry_point_;
        for (int l = previous_max_level; l > level; --l) {
            entry_node = greedy_search(vector, entry_node, l);
        }

        // Now process lower layers:
        for (int l = min(level, previous_max_level); l >= 0; l--) {
            auto new_candidates = search_layer(vector, entry_node, ef_construction, l);
            auto new_neighbors = select_neighbors(vector, new_candidates, max_neighbors);

            int M = max_neighbors;
            for (auto it = new_neighbors.begin(); it != new_neighbors.end(); ) {
                if (l == 0) M *= 2;
                graph_.at(*it).neighbors[l].push_back(new_node.id);
                graph_.at(*it).neighbors[l] = select_neighbors(graph_.at(*it).vector,
                    graph_.at(*it).neighbors[l], M);
                auto neighbors_neighbors = graph_.at(*it).neighbors[l];
                if (std::find(neighbors_neighbors.begin(), neighbors_neighbors.end(),
                    new_node.id) == neighbors_neighbors.end()) {
                    it = new_neighbors.erase(it);
                } else {
                    graph_.at(id).neighbors[l].push_back(*it);
                    ++it;
                }
            }
            graph_.at(id).neighbors[l] = new_neighbors;
            if (!new_neighbors.empty()) entry_node = new_neighbors.front();
        }

        if (level > previous_max_level) {
            entry_point_ = id;
            max_level_ = level;
        }
    }

    std::vector<SearchResult> HNSWIndex::search(const Vector& query, std::size_t top_k,
        std::size_t ef_search) const {
        if (graph_.empty() || top_k == 0) {
            return {};
        }
        int entry_node = entry_point_;
        for (int l = max_level_; l > 0; l--) {
            entry_node = greedy_search(query, entry_node, l);
        }
        auto result = search_layer(query, entry_node, ef_search, 0);
        top_k = min(top_k, result.size());
        // search_layer() returns in least to greatest order.
        result.erase(result.begin(), result.end() - top_k);

        std::vector<SearchResult> topk_result;
        for (auto it = result.rbegin(); it != result.rend(); ++it) {
            int r = *it;
            SearchResult curr_r{r, query.cosine_similarity(graph_.at(r).vector.values())};
            topk_result.push_back(curr_r);
        }

        // Return a vector of SearchResult objects in greatest to least similarity score.
        return topk_result;
}

int HNSWIndex::greedy_search(const Vector& query, int entry_point, int layer) const {
        int current = entry_point;
        bool improved = true;
        float current_sim = query.cosine_similarity(graph_.at(current).vector.values());

        while (improved) {
            improved = false;

            if (layer < 0 || static_cast<std::size_t>(layer) >= graph_.at(current).neighbors.size()) {
                return current;
            }
            for (int neighbor : graph_.at(current).neighbors[layer]) {
                // Compare distance between query,neighbor and query,current.
                float neighbor_sim = query.cosine_similarity(graph_.at(neighbor).vector.values());
                if (neighbor_sim > current_sim) {
                    improved = true;
                    current = neighbor;
                    current_sim = neighbor_sim;
                }
            }
        }
        return current;
}

vector<int> HNSWIndex::search_layer(const Vector& query, int entry_point, size_t ef,
        int layer) const {
        unordered_set<int> visited;
    priority_queue<pair<float,int>, vector<pair<float,int>>, greater<pair<float,int>>> pq;
        queue<int> q;
    ef = max<std::size_t>(ef, 1);

    float current_sim = query.cosine_similarity(graph_.at(entry_point).vector.values());
        pq.push({current_sim, entry_point});
        q.push(entry_point);
        visited.insert(entry_point);

        while (!q.empty()) {
            int curr_node = q.front();
            q.pop();

            if (layer < 0 || static_cast<std::size_t>(layer) >= graph_.at(curr_node).neighbors.size()) {
                continue;
            }
            for (int neighbor : graph_.at(curr_node).neighbors[layer]) {
                if (visited.count(neighbor)) { continue; }
                visited.insert(neighbor);
                q.push(neighbor);
                float neighbor_sim = query.cosine_similarity(graph_.at(neighbor).vector.values());
                if (pq.size() < ef || neighbor_sim > pq.top().first) {
                    pq.push({neighbor_sim, neighbor});
                    if (pq.size() > ef) pq.pop();
                    q.push(neighbor);
                }
            }
        }
                
        vector<int> result_candidates;
        while (!pq.empty()) {
            auto [cand_sim, cand_idx] = pq.top();
            pq.pop();
            result_candidates.push_back(cand_idx);
        }
        return result_candidates;
    }

    vector<int> HNSWIndex::select_neighbors(const Vector& query, const std::vector<int>& candidates,
        size_t M) const {
        vector<int> selected;
        // Candidates explored in greatest -> least similarity to query, since they're returned in
        // least -> greatest order by search_layer().
        for (auto it = candidates.rbegin(); it != candidates.rend(); ++it) {
            int cq_distance = (1 - query.cosine_similarity(graph_.at(*it).vector.values()));

            bool diverse = true;
            for (int s : selected) {
                int cs_distance = (1 - graph_.at(*it).vector.cosine_similarity(
                    graph_.at(s).vector.values()));
                if (cq_distance >= cs_distance) {
                    diverse = false;
                    break;
                }
            }
            if (!diverse) continue;
            selected.push_back(*it);

            if (selected.size() >= M) break;
        }
        return selected;
}

size_t HNSWIndex::random_level() {
        std::size_t level = 0;
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        const std::size_t max_level = 16; // practical safeguard

        while (level < max_level &&
            dist(rng_) < 1.0f / max_neighbors) {
            ++level;
        }

        return level;
}