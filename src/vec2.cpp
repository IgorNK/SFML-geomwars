#include "vec2.h"

Vec2 Vec2::clone() const {
    return Vec2(x, y);
}

float Vec2::length() const {
    return sqrtf(x * x + y * y);
};

float Vec2::distance_to(const Vec2 & other) const {
    return sqrtf((other.x - x) * (other.x - x) + (other.y - y) * (other.y - y));
};

float Vec2::angle_to_rad(const Vec2 & other) const {
//Angle to another Vec2 position, radian version
    const float dot = x * other.x + y * other.y;
    const float det = x * other.y - y * other.x;
    return atan2(det, dot);
};

float Vec2::angle_to_deg(const Vec2 & other) const {
//Angle to another Vec2 position, degrees version
    return (*this).angle_to_rad(other) * m_rad_to_deg;
}

Vec2 & Vec2::rotate_rad(const float radians) {
    float px = x * cos(radians) - y * sin(radians);
    float py = x * sin(radians) + y * cos(radians);
    x = px;
    y = py;
    return *this;
};

Vec2 & Vec2::rotate_deg(const float degrees) {
    return (*this).rotate_rad(degrees / m_rad_to_deg);
}

Vec2 & Vec2::normalize() {
    const float length = (*this).length();
    x = x / length;
    y = y / length;
    return *this;
};

Vec2 Vec2::operator + (const Vec2 & other) const {
    return Vec2(x + other.x, y + other.y);
};

// Vec2 Vec2::operator + (Vec2 other) {
//     return Vec2(x + other.x, y + other.y);
// }

Vec2 & Vec2::operator += (const Vec2 & other) {
    x += other.x;
    y += other.y;
    return *this;
}

// Vec2 & Vec2::operator += (Vec2 other) {
//     x += other.x;
//     y += other.y;
//     return *this;
// }

Vec2 Vec2::operator - (const Vec2 & other) const {
    return Vec2(x - other.x, y - other.y);
}

// Vec2 Vec2::operator - (Vec2 other) {
//     return Vec2(x - other.x, y - other.y);
// }

Vec2 & Vec2::operator -= (const Vec2 & other) {
    x -= other.x;
    y -= other.y;
    return *this;
}

// Vec2 & Vec2::operator -= (Vec2 other) {
//     x -= other.x;
//     y -= other.y;
//     return *this;
// }

Vec2 Vec2::operator * (const float multiplier) const {
    return Vec2(x * multiplier, y * multiplier);
};

Vec2 & Vec2::operator *= (const float multiplier) {
    x *= multiplier;
    y *= multiplier;
    return *this;
}

bool Vec2::operator == (const Vec2 & other) const {
    return (x == other.x && y == other.y);
};