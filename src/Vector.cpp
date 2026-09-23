#include "Vector.hpp"

#include <cmath>
#include <stdexcept>
#include <utility>

Vector::Vector(std::vector<float> values)
	: values_(std::move(values)) {}

std::size_t Vector::dimension() const noexcept {
	return values_.size();
}

float Vector::magnitude() const noexcept {
	float squared_magnitude = 0.0f;
	for (float value : values_) {
		squared_magnitude += value * value;
	}
	return std::sqrt(squared_magnitude);
}

float Vector::cosine_similarity(std::vector<float> values) const {
	if (values.size() != dimension()) {
		throw std::invalid_argument(
			"Cannot compute the cosine similarity of vectors with different sizes."
		);
	}

	float dot_product = 0.0f;
	for (std::size_t index = 0; index < dimension(); ++index) {
		dot_product += values_[index] * values[index];
	}

	float denominator = magnitude();
	float other_magnitude = 0.0f;
	for (float value : values) {
		other_magnitude += value * value;
	}
	denominator *= std::sqrt(other_magnitude);

	if (denominator == 0.0f) {
		throw std::domain_error(
			"Either of the vectors have a magnitude of 0, making cosine similarity invalid."
		);
	}

	return dot_product / denominator;
}
