#pragma once
#define _USE_MATH_DEFINES
#include <math.h>

class Vec2 {
	
public:
	static constexpr float rad_to_deg = 180.f / M_PI;
	static Vec2 forward() {
		return Vec2(1, 0);
	};
	float x {0};
	float y {0};
	Vec2() {};
	Vec2(const float xin, const float yin) : x(xin), y(yin) {};
	Vec2 clone() const;
	float length() const;
	float distance_to(const Vec2 & other) const;
	static inline float distance_to_line_sq(const Vec2 & start, const Vec2 & end, const Vec2 & point) {
		// line:
		// A = y2 - y1
		// B = x1 - x2
		// C = y1 * (x2 - x1) - x1 * (y2 - y1)
		// Ax + By + C = 0

		// Squared Distance from point to line:
		// d^2 = (A*x + B*y + C)^2 / A^2 + B^2
		
		const float A = end.y - start.y;
		const float B = start.x - end.x;
		const float C = start.y * (end.x - start.x) - start.x * (end.y - start.y);
		const float top = (A * point.x + B * point.y + C);
		const float bottom = A * A + B * B;
		return (top * top) / bottom; 
	};
	static inline float lerp(float a, float b, float t) {
		return a + (b - a) * t;
	}
	static inline Vec2 lerp(Vec2 a, Vec2 b, float t) {
		return a + (b - a) * t;
	}
	float angle_to_rad(const Vec2 & other) const;
	float angle_to_deg(const Vec2 & other) const;
	Vec2 & rotate_rad(const float radians);
	Vec2 & rotate_deg(const float degrees);
	Vec2 & normalize();
	Vec2 operator + (const Vec2 & other) const;
	// Vec2 operator + (Vec2 other);
	Vec2 & operator += (const Vec2 & other);
	// Vec2 & operator += (Vec2 other);
	Vec2 operator - (const Vec2 & other) const;
	// Vec2 operator - (Vec2 other);
	Vec2 & operator -= (const Vec2 & other);
	// Vec2 & operator -= (Vec2 other);
	Vec2 operator * (const float multiplier) const;
	Vec2 & operator *= (const float multiplier);
	bool operator == (const Vec2 & other) const;
};