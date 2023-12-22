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
    float distance_to_line_sq(const Vec2 & start, const Vec2 & end, const Vec2 & point) const;
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