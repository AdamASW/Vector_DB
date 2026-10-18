#pragma once

struct SearchResult {
    int id;
    float score;

    bool operator>(const SearchResult& other) const {
        return score > other.score;
    }
};