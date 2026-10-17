#pragma once

#include <cstddef>
#include <vector>

class Vector {
    public:
        explicit Vector(std::vector<float> values);

        std::size_t dimension() const noexcept;

        float magnitude() const noexcept;

        float cosine_similarity(std::vector<float> values) const;

        const std::vector<float>& values() const noexcept {
            return values_;
        }

    private:
        std::vector<float> values_;

};