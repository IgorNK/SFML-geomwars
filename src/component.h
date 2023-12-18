#pragma once
#include "vec2.h"
#include <iostream>
#include <map>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Rect.hpp>

enum ComponentType {
    Transform,
    Velocity,
    Collider,
    Lifespan,
    Name,
    Rect
};

static std::map<ComponentType, std::string> component_names {
    {ComponentType::Transform, "Transform"},
    {ComponentType::Velocity, "Velocity"},
    {ComponentType::Collider, "Collider"},
    {ComponentType::Lifespan, "Lifespan"},
    {ComponentType::Weapon, "Weapon"},
    {ComponentType::SpecialWeapon, "SpecialWeapon"},
    {ComponentType::Name, "Name"},
    {ComponentType::Rect, "Rect"},
};

static std::map<std::string, ComponentType> name_components {
    {"Transform", ComponentType::Transform},
    {"Velocity", ComponentType::Velocity},
    {"Collider", ComponentType::Collider},
    {"Lifespan", ComponentType::Lifespan},
    {"Weapon", ComponentType::Weapon},
    {"SpecialWeapon", ComponentType::SpecialWeapon},
    {"Name", ComponentType::Name},
    {"Rect", ComponentType::Rect}
};

class Component {
public:
};

class CTransform : public Component {
public:
    CTransform();
    CTransform(const float pos_x, const float pos_y, const float in_rotation = 0.f, const float in_scale = 1.f) {
        position = Vec2(pos_x, pos_y);
        rotation = in_rotation;
        scale = in_scale;
    }
    CTransform(const Vec2 in_position, const float in_rotation = 0.f, const float in_scale = 1.f) {
        position = in_position;
        rotation = in_rotation;
        scale = in_scale;
    }
    ~CTransform();
    Vec2 position {0.f, 0.f};
    float rotation {0.f};
    float scale {1.f};
};

class CVelocity : public Component {
public:
    CVelocity();
    CVelocity(const Vec2 vel) {
        velocity = vel;
    }
    ~CVelocity();
    Vec2 velocity {0.f, 0.f};
};

class CCollider : public Component {
public:
    CCollider();
    CCollider(const float in_radius) {
        radius = in_radius;
    }
    ~CCollider();
    float radius {30.f};
}

class CLifespan : public Component {
public:
    CLifespan();
    CLifespan(const float span) {
        time = span;
    }
    ~CLifespan();
    float time {10.f};
}

class CWeapon: public Component {
public:
    CWeapon(const float in_delay = 0.1f, const float bullet_speed = 10.f, const CShape bullet_prototype = CShape(5.f, 12, sf::Color(255, 0, 0), sf::Color(0, 0, 0), 0.f)) {
        delay = in_delay;
        speed = bullet_speed;
        bullet = bullet_prototype;
    }
    ~CWeapon();
    float delay;
    float speed;
    CShape bullet;
}

class CSpecialWeapon: public Component {
public:
    CSpecialWeapon(const float in_delay = 0.1f, const float bullet_speed = 10.f, const CShape bullet_prototype = CShape(5.f, 12, sf::Color(0, 0, 255), sf::Color(0, 0, 0), 0.f)) {
        delay = in_delay;
        speed = bullet_speed;
        bullet = bullet_prototype;
    }
    ~CSpecialWeapon();
    float delay;
    float speed;
    CShape bullet;
}

class CName : public Component {
public:
    CName();
    CName(const std::string in_name) {
        name = in_name;
    }
    ~CName();
    std::string name {"Default"};
};

class CShape : public Component {
public:
    CShape(const float radius = 30.f, const size_t point_count = 8, const sf::Color fill_color = sf::Color(0, 255, 0), const sf::Color outline_color = sf::Color(255, 255, 255), const float outline_thickness = 1.f) {
        shape = sf::CircleShape(radius, point_count);
        shape.setFillColor(fill_color);
        shape.setOutlineColor(outline_color);
        shape.setOutlineThickness(outline_thickness);
        shape.setOrigin(radius, radius);
    }
    ~CShape();
    sf::CircleShape shape;
};

class CRect : public Component {
public:
    CRect();
    CRect(const float width, const float height, const float in_border = 100.f) {
        rect = sf::FloatRect(0, 0, width, height);
        border = in_border;
    }
    CRect(const float left, const float top, const float width, const float height, const float in_border = 100.f) {
        rect = sf::FloatRect(left, top, width, height);
        border = in_border;
    }
    CRect(const sf::FloatRect in_rect, const float in_border = 100.f) {
        rect = in_rect;
        border = in_border;
    }
    ~CRect();
    sf::FloatRect rect {0, 0, 100.f, 100.f};
    float border {100.f};
};