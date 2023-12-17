#pragma once
#include <cmath>

class Vec2 {
    static constexpr float m_rad_to_deg = 180.f / M_PI;
public:
    float x {0};
    float y {0};
    Vec2() {};
    Vec2(float xin, float yin) : x(xin), y(yin) {};
    Vec2 clone() const;
    float length() const;
    float distance_to(Vec2 & other) const;
    float angle_to_rad(Vec2 & other) const;
    float angle_to_deg(Vec2 & other) const;
    Vec2 & rotate_rad(float radians);
    Vec2 & rotate_deg(float degrees);
    Vec2 & normalize();
    Vec2 & operator + (Vec2 & other);
    Vec2 & operator - (Vec2 & other);
    Vec2 & operator * (float multiplier);
    bool operator == (Vec2 & other) const;
};