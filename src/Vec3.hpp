#pragma once

#include <cmath>

struct Vec3 {
	Vec3() {};
	Vec3(float scalar): x(scalar), y(scalar), z(scalar) {};
	Vec3(float x, float y, float z): x(x), y(y), z(z) {};

	float length() const {
		return std::sqrt(x * x + y * y + z * z);
	};

	Vec3 operator-(const Vec3& other) const {
		return Vec3(
			x - other.x,
			y - other.y,
			z - other.z
		);
	}

	Vec3 operator+(const Vec3& vec) const {
		return Vec3(
			x + vec.x,
			y + vec.y,
			z + vec.z
		);
	};

	Vec3 operator*(float f) const {
		return Vec3(
			x * f,
			y * f,
			z * f
		);
	};

	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
};
