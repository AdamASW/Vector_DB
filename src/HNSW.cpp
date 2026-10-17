#include <HNSW.hpp>
#include <vector>

using namespace std;
class HNSWIndex {
public:
    HNSWIndex(std::size_t max_neighbors, std::size_t ef_construction);

private:
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

    std::vector<int> search_layer(const Vector& query, int entry_point, size_t ef,
            int layer) const {
                
            }
};