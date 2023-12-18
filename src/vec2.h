#pragma once
#include <cmath>

class Vec2 {
    static constexpr float m_rad_to_deg = 180.f / M_PI;
public:
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