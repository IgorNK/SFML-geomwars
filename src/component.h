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
    Weapon,
    SpecialWeapon,
    Name,
    Rect,
    Input,
    PlayerStats
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
    {ComponentType::Input, "Input"},
    {ComponentType::PlayerStats, "PlayerStats"}
};

static std::map<std::string, ComponentType> name_components {
    {"Transform", ComponentType::Transform},
    {"Velocity", ComponentType::Velocity},
    {"Collider", ComponentType::Collider},
    {"Lifespan", ComponentType::Lifespan},
    {"Weapon", ComponentType::Weapon},
    {"SpecialWeapon", ComponentType::SpecialWeapon},
    {"Name", ComponentType::Name},
    {"Rect", ComponentType::Rect},
    {"Input", ComponentType::Input},
    {"PlayerStats", ComponentType::PlayerStats}
};

class Component {
public:
};

class CTransform : public Component {
public:
    Vec2 position {0.f, 0.f};
    float rotation {0.f};
    float scale {1.f};
    CTransform() {}
    CTransform(const float pos_x, const float pos_y, const float in_rotation = 0.f, const float in_scale = 1.f) : position(Vec2(pos_x, pos_y)), rotation(in_rotation), scale(in_scale) { }
    CTransform(const Vec2 in_position, const float in_rotation = 0.f, const float in_scale = 1.f) : position(in_position), rotation(in_rotation), scale(in_scale) { }
    ~CTransform() {}
};

class CVelocity : public Component {
public:
    Vec2 velocity {0.f, 0.f};
    CVelocity() {}
    CVelocity(const Vec2 vel) : velocity(vel) { }
    ~CVelocity() {}
};

class CCollider : public Component {
public:
    float radius {30.f};
    CCollider() {}
    CCollider(const float in_radius) : radius(in_radius) { }
    ~CCollider() {}
};

class CLifespan : public Component {
public:
    float duration {10.f};
    float countdown {10.f};
    CLifespan() {}
    CLifespan(const float span) : countdown(span), duration(span) { }
    ~CLifespan() {}
};

class CShape : public Component {
public:
    sf::CircleShape shape;
    CShape(const float radius = 30.f, const size_t point_count = 8, const sf::Color fill_color = sf::Color(0, 255, 0), const sf::Color outline_color = sf::Color(255, 255, 255), const float outline_thickness = 1.f)
    : shape(sf::CircleShape(radius, point_count)) {
        shape.setFillColor(fill_color);
        shape.setOutlineColor(outline_color);
        shape.setOutlineThickness(outline_thickness);
        shape.setOrigin(radius, radius);
    }
    ~CShape() {}
};

class CWeapon: public Component {
public:
    float speed;
    float lifespan;
    CShape bullet;
    CWeapon(const float bullet_speed = 10.f, const float in_lifespan = 50.f, const CShape bullet_prototype = CShape(5.f, 12, sf::Color(255, 0, 0), sf::Color(0, 0, 0), 0.f)) : speed(bullet_speed), lifespan(in_lifespan), bullet(bullet_prototype) { }
    ~CWeapon() {}
};

class CSpecialWeapon: public Component {
public:
    float speed;
    CShape bullet;
    CSpecialWeapon(const float bullet_speed = 10.f, const CShape bullet_prototype = CShape(5.f, 12, sf::Color(0, 0, 255), sf::Color(0, 0, 0), 0.f)) : speed(bullet_speed), bullet(bullet_prototype) { }
    ~CSpecialWeapon() {}
};

class CName : public Component {
public:
    std::string name {"Default"};
    CName() {}
    CName(const std::string in_name) : name(in_name) { }
    ~CName() {}
};


class CRect : public Component {
public:
    sf::FloatRect rect {0, 0, 100.f, 100.f};
    float border {100.f};
    CRect() {}
    CRect(const float width, const float height, const float in_border = 100.f) : rect(sf::FloatRect(0, 0, width, height)), border(in_border) { }
    CRect(const float left, const float top, const float width, const float height, const float in_border = 100.f) : rect(sf::FloatRect(left, top, width, height)), border(in_border) { }
    CRect(const sf::FloatRect in_rect, const float in_border = 100.f) : rect(in_rect), border(in_border) { }
    ~CRect() {}
};

class CInput : public Component {
public:
    Vec2 axis {0, 0};
    Vec2 mousePosition {0, 0};
    bool fire {false};
    bool secondaryFire {false};
    CInput() {}
    ~CInput() {}
};

class CPlayerStats : public Component {
public:
    int lives;
    float speed;
    int special_delay;
    int fire_delay;
    int fire_countdown;
    int special_countdown;
    CPlayerStats(const int in_lives = 3, const float in_speed = 10.f, const int in_special_delay = 180, const int in_fire_delay = 10, const int in_fire_countdown = 0, const int in_special_countdown = 0)
    : lives(in_lives), speed(in_speed), special_delay(in_special_delay), fire_delay(in_fire_delay), fire_countdown(in_fire_countdown), special_countdown(in_special_countdown) {};
    ~CPlayerStats() {};
};