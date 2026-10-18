#include <HNSW.hpp>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <queue>

using namespace std;
class HNSWIndex {
public:
    HNSWIndex(std::size_t max_neighbors, std::size_t ef_construction)
        : max_neighbors(max_neighbors),
        ef_construction(ef_construction),
        entry_point_(-1),
        max_level_(-1),
        rng_(std::random_device{}()) {}

    void insert(int id, const Vector& vector) {
        // Initialize the new node in the graph:
        int level = random_level();
        this->max_level_ = max(level,this->max_level_);
        std::vector<std::vector<int>> new_neighbors(this->max_level_, std::vector<int>());
        Node new_node{id, vector, new_neighbors};

        // Deal with empty graph:
        if (graph_.empty()) { 
            entry_point_ = id;
            graph_.at(id) = new_node;
            return;
        }

        // If graph non-empty, start search for upper layers, if applicable:
        int k = 0;
        int entry_node = entry_point_;
        while ((level + k) < max_level_) {
            entry_node = greedy_search(vector, entry_node, (max_level_ - k));
            k++;
        }

        // Now process lower layers:
        for (int l = level; l >= 0; l--) {
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
                    ++it;
                }
            }
            if (!new_neighbors.empty()) entry_node = new_neighbors.front();
        }
    }

    std::vector<SearchResult> search(const Vector& query, std::size_t top_k,
            std::size_t ef_search) const {
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

private:
    std::size_t max_neighbors;
    std::size_t ef_construction;
    std::size_t rng_;
    int entry_point_;
    int max_level_;
    struct Node {
        int id;
        Vector vector;
        std::vector<std::vector<int>> neighbors;
    };

    unordered_map<int, Node> graph_;

    int greedy_search(const Vector& query, int entry_point, int layer) const {
        int current = entry_point;
        bool improved = true;
        int current_sim = query.cosine_similarity(graph_.at(current).vector.values());

        while (improved) {
            improved = false;

            for (int neighbor : graph_.at(current).neighbors[layer]) {
                // Compare distance between query,neighbor and query,current.
                int neighbor_sim = query.cosine_similarity(graph_.at(neighbor).vector.values());
                if (neighbor_sim > current_sim) {
                    improved = true;
                    current = neighbor;
                    current_sim = neighbor_sim;
                }
            }
        }
        return current;
    }

    vector<int> search_layer(const Vector& query, int entry_point, size_t ef,
        int layer) const {
        unordered_set<int> visited;
        priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> pq;
        queue<int> q;
        int vectors;
                
        int current_sim = query.cosine_similarity(graph_.at(entry_point).vector.values());
        pq.push({current_sim, entry_point});
        q.push(entry_point);
        visited.insert(entry_point);

        while (!q.empty()) {
            int curr_node = q.front();
            q.pop();

            for (int neighbor : graph_.at(curr_node).neighbors[layer]) {
                if (visited.count(neighbor)) { continue; }
                visited.insert(neighbor);
                q.push(neighbor);
                int neighbor_sim = query.cosine_similarity(graph_.at(neighbor).vector.values());
                if (neighbor_sim > pq.top().first) {
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

    vector<int> select_neighbors(const Vector& query, const std::vector<int>& candidates,
            size_t M) const {
        vector<int> selected;
        // Candidates explored in greatest -> least similarity to query, since they're returned in
        // least -> greatest order by search_layer().
        for (auto it = candidates.rbegin(); it != candidates.rend(); ++it) {
            int cq_distance = (1 - query.cosine_similarity(graph_.at(*it).vector.values()));

            bool diverse = true;
            for (int s : selected) {
                int cs_distance = (1 - query.cosine_similarity(graph_.at(s).vector.values()));
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

    size_t random_level() {
        std::size_t level = 0;
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        const std::size_t max_level = 16; // practical safeguard

        while (level < max_level &&
            dist(rng_) < 1.0f / max_neighbors) {
            ++level;
        }

        return level;
    }
};