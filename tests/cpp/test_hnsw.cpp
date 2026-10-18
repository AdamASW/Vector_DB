#include "HNSW.hpp"

#include <cmath>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
int failures = 0;

void check(bool condition, const std::string& message) {
    if (!condition) {
        ++failures;
        std::cerr << "FAIL: " << message << '\n';
    }
}

template <typename Function>
void check_throws(Function function, const std::string& message) {
    try {
        function();
        check(false, message);
    } catch (const std::exception&) {
    }
}

void test_empty_index_and_top_k() {
    check_throws([] { HNSWIndex index(0, 32); },
        "max_neighbors zero is rejected");
    check_throws([] { HNSWIndex index(8, 0); },
        "ef_construction zero is rejected");

    HNSWIndex index(8, 32);
    check(index.search(Vector({1.0f, 0.0f}), 3, 8).empty(),
        "searching an empty index returns no matches");
    check(index.search(Vector({1.0f, 0.0f}), 0, 8).empty(),
        "top_k zero returns no matches");
}

void test_single_vector_and_query_edges() {
    HNSWIndex index(8, 32);
    index.insert(-7, Vector({1.0f, 0.0f, 0.0f}));

    auto results = index.search(Vector({1.0f, 0.0f, 0.0f}), 5, 8);
    check(results.size() == 1, "singleton search returns one result");
    if (!results.empty()) {
        check(results[0].id == -7, "singleton result retains its ID");
        check(std::abs(results[0].score - 1.0f) < 1e-5f,
            "identical vectors have cosine similarity one");
    }

    check_throws([&] { index.search(Vector({1.0f, 0.0f}), 1, 8); },
        "query dimensions must match indexed vectors");
    check_throws([&] { index.search(Vector({0.0f, 0.0f, 0.0f}), 1, 8); },
        "zero-magnitude query follows Vector cosine error behavior");
}

void test_insert_search_order_and_scores() {
    HNSWIndex index(8, 64);
    index.insert(10, Vector({0.9f, 0.1f, 0.0f}));
    index.insert(20, Vector({0.0f, 1.0f, 0.0f}));
    index.insert(30, Vector({1.0f, 0.0f, 0.0f}));

    const auto results = index.search(Vector({1.0f, 0.0f, 0.0f}), 3, 64);
    check(results.size() == 3, "search returns up to top_k results");
    if (results.size() == 3) {
        check(results[0].id == 30 && results[1].id == 10 && results[2].id == 20,
            "results are ordered by descending cosine similarity");
        check(std::abs(results[0].score - 1.0f) < 1e-5f,
            "exact match score is one");
        check(std::abs(results[1].score - 0.9f / std::sqrt(0.82f)) < 1e-5f,
            "non-integral cosine score retains floating-point precision");
        check(std::abs(results[2].score) < 1e-5f,
            "orthogonal vector score is zero");
    }
}

void test_large_top_k_and_small_ef() {
    HNSWIndex index(8, 64);
    for (int id = 1; id <= 20; ++id) {
        index.insert(id, Vector({static_cast<float>(id), 1.0f, 0.0f}));
    }

    const auto results = index.search(Vector({1.0f, 1.0f, 0.0f}), 100, 100);
    check(results.size() <= 20, "top_k above index size is bounded by index size");
    for (std::size_t i = 1; i < results.size(); ++i) {
        check(results[i - 1].score >= results[i].score,
            "larger index results remain sorted by descending score");
    }

    const auto low_ef_results = index.search(Vector({1.0f, 1.0f, 0.0f}), 5, 1);
    check(!low_ef_results.empty(), "ef_search of one still returns a candidate");
    check(low_ef_results.size() <= 5, "small ef search respects top_k");
}

void test_high_ef_reaches_all_inserted_nodes() {
    constexpr int vector_count = 64;
    constexpr float pi = 3.14159265358979323846f;
    HNSWIndex index(8, vector_count);
    for (int id = 0; id < vector_count; ++id) {
        const float angle = 2.0f * pi * id / vector_count;
        index.insert(id, Vector({std::cos(angle), std::sin(angle)}));
    }

    const auto results = index.search(Vector({1.0f, 0.0f}), vector_count, vector_count);
    check(results.size() == vector_count,
        "high ef search can reach every node in a connected inserted graph");
}
}

int main() {
    test_empty_index_and_top_k();
    test_single_vector_and_query_edges();
    test_insert_search_order_and_scores();
    test_large_top_k_and_small_ef();
    test_high_ef_reaches_all_inserted_nodes();
    return failures == 0 ? 0 : 1;
}