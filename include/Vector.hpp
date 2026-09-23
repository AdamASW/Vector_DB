#pragma once

#include <cstddef>
#include <vector>

class Vector {
    public:
        explicit Vector(std::vector<float> values);

        std::size_t dimension() const noexcept;

        float magnitude() const noexcept;

        float cosine_similarity(std::vector<float> values) const;

    private:
        std::vector<float> values_;

};