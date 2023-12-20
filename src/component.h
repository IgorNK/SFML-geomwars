#pragma once
#include "vec2.h"
#include <iostream>
#include <map>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Text.hpp>
#include "tag.h"

enum ComponentType {
    Transform,
    Velocity,
    Collider,
    Shape,
    Text,
    Lifespan,
    Health,
    Weapon,
    SpecialWeapon,
    Name,
    Rect,
    Input,
    PlayerStats,
    DeathSpawner,
    PickupSpawner,
    ScoreReward,
    WeaponPickup
};

static std::map<ComponentType, std::string> component_names {
    {ComponentType::Transform, "Transform"},
    {ComponentType::Velocity, "Velocity"},
    {ComponentType::Collider, "Collider"},
    {ComponentType::Shape, "Shape"},
    {ComponentType::Text, "Text"},
    {ComponentType::Lifespan, "Lifespan"},
    {ComponentType::Health, "Health"},
    {ComponentType::Weapon, "Weapon"},
    {ComponentType::SpecialWeapon, "SpecialWeapon"},
    {ComponentType::Name, "Name"},
    {ComponentType::Rect, "Rect"},
    {ComponentType::Input, "Input"},
    {ComponentType::PlayerStats, "PlayerStats"},
    {ComponentType::DeathSpawner, "DeathSpawner"},
    {ComponentType::PickupSpawner, "PickupSpawner"},
    {ComponentType::ScoreReward, "ScoreReward"},
    {ComponentType::WeaponPickup, "WeaponPickup"}
};

static std::map<std::string, ComponentType> name_components {
    {"Transform", ComponentType::Transform},
    {"Velocity", ComponentType::Velocity},
    {"Collider", ComponentType::Collider},
    {"Shape", ComponentType::Shape},
    {"Text", ComponentType::Text},
    {"Lifespan", ComponentType::Lifespan},
    {"Health", ComponentType::Health},
    {"Weapon", ComponentType::Weapon},
    {"SpecialWeapon", ComponentType::SpecialWeapon},
    {"Name", ComponentType::Name},
    {"Rect", ComponentType::Rect},
    {"Input", ComponentType::Input},
    {"PlayerStats", ComponentType::PlayerStats},
    {"DeathSpawner", ComponentType::DeathSpawner},
    {"PickupSpawner", ComponentType::PickupSpawner},
    {"ScoreReward", ComponentType::ScoreReward},
    {"WeaponPickup", ComponentType::WeaponPickup}
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
    CTransform(
        const float pos_x
        , const float pos_y
        , const float in_rotation = 0.f
        , const float in_scale = 1.f
    ) 
        : position(Vec2(pos_x, pos_y))
        , rotation(in_rotation)
        , scale(in_scale) 
    { }
    CTransform(
        const Vec2 in_position
        , const float in_rotation = 0.f
        , const float in_scale = 1.f
    ) 
        : position(in_position)
        , rotation(in_rotation)
        , scale(in_scale) 
    { }
    ~CTransform() { }
};

class CVelocity : public Component {
public:
    Vec2 velocity {0.f, 0.f};
    CVelocity() { }
    CVelocity(const Vec2 vel) : velocity(vel) { }
    ~CVelocity() { }
};

class CCollider : public Component {
public:
    float radius {30.f};
    CCollider() { }
    CCollider(const float in_radius) : radius(in_radius) { }
    ~CCollider() { }
};

class CLifespan : public Component {
public:
    float duration {10.f};
    float countdown {10.f};
    CLifespan() { }
    CLifespan(const float span) : countdown(span), duration(span) { }
    ~CLifespan() { }
};

class CHealth : public Component {
public:
    int max_hp;
    int hp;
    int react_duration;
    int react_countdown;
    float expansion;
    CHealth(
        int in_hp = 1
        , int in_react_duration = 20.f
        , float expand_scale = 1.25f
    ) 
        : max_hp(in_hp)
        , hp(in_hp)
        , react_duration(in_react_duration)
        , react_countdown(in_react_duration)
        , expansion(expand_scale) 
    {};
    ~CHealth() { }
};

class CShape : public Component {
public:
    sf::CircleShape shape;
    CShape(
        const float radius = 30.f
        , const size_t point_count = 8
        , const sf::Color fill_color = sf::Color(0, 255, 0)
        , const sf::Color outline_color = sf::Color(255, 255, 255)
        , const float outline_thickness = 1.f
    )
        : shape(sf::CircleShape(radius, point_count)) 
    {
        shape.setFillColor(fill_color);
        shape.setOutlineColor(outline_color);
        shape.setOutlineThickness(outline_thickness);
        shape.setOrigin(radius, radius);
    }
    ~CShape() { }
};

class CText : public Component {
public:
    sf::Text text;
    sf::Color color;
    CText(
        const std::string & innerText = "default",
        const sf::Font & font = sf::Font(),
        const unsigned int size = 24,
        const sf::Color & color = sf::Color(255, 255, 255)
    )
        : text(sf::Text(innerText, font, size))
    {
        text.setColor(color);
    }
    ~CText() { }
};

class CWeapon: public Component {
public:
    enum FireMode {
        ShotSingle,
        ShotSpread
    };
    CWeapon::FireMode mode;
    float speed;
    float lifespan;
    CShape bullet;
    CWeapon(
        const float bullet_speed = 10.f
        , const float in_lifespan = 50.f
        , const CShape bullet_prototype = CShape(5.f, 12, sf::Color(255, 0, 0), sf::Color(0, 0, 0), 0.f)
        , const CWeapon::FireMode fire_mode = CWeapon::FireMode::ShotSingle
    ) 
        : speed(bullet_speed)
        , lifespan(in_lifespan)
        , bullet(bullet_prototype)
        , mode(fire_mode) 
    { }
    ~CWeapon() { }
};

class CSpecialWeapon: public Component {
public:
    enum FireMode {
        SpecialExplosion,
        SpecialFlamethower
    };
    CSpecialWeapon::FireMode mode;
    float speed;
    float lifespan;
    int recursion;
    int amount;
    CShape bullet;
    CSpecialWeapon(
        const float bullet_speed = 10.f
        , const float in_lifespan = 50.f
        , const int in_amount = 6
        , const int in_recursion = 2
        , const CShape bullet_prototype = CShape(5.f, 12, sf::Color(0, 0, 255), sf::Color(0, 0, 0), 0.f)
        , const CSpecialWeapon::FireMode fire_mode = CSpecialWeapon::FireMode::SpecialExplosion
    ) 
        : speed(bullet_speed)
        , lifespan(in_lifespan)
        , amount(in_amount)
        , recursion(in_recursion)
        , bullet(bullet_prototype)
        , mode(fire_mode)
    { }
    ~CSpecialWeapon() { }
};

class CName : public Component {
public:
    std::string name {"Default"};
    CName() { }
    CName(const std::string in_name) : name(in_name) { }
    ~CName() { }
};


class CRect : public Component {
public:
    sf::FloatRect rect {0, 0, 100.f, 100.f};
    float border {100.f};
    CRect() { }
    CRect(
        const float width
        , const float height
        , const float in_border = 100.f
    ) 
        : rect(sf::FloatRect(0, 0, width, height))
        , border(in_border) 
    { }
    CRect(
        const float left
        , const float top
        , const float width
        , const float height
        , const float in_border = 100.f
    ) 
        : rect(sf::FloatRect(left, top, width, height))
        , border(in_border) 
    { }
    CRect(const sf::FloatRect in_rect, const float in_border = 100.f) 
        : rect(in_rect)
        , border(in_border) { }
    ~CRect() { }
};

class CInput : public Component {
public:
    Vec2 axis {0, 0};
    Vec2 mousePosition {0, 0};
    bool fire {false};
    bool secondaryFire {false};
    CInput() { }
    ~CInput() { }
};

class CPlayerStats : public Component {
public:
    int max_lives;
    int lives;
    float speed;    
    int fire_delay;
    int special_delay;
    int invincibility_duration;
    int fire_countdown;
    int special_countdown;    
    int invincibility_countdown;
    int flicker_frequency;
    CPlayerStats(
        const int in_lives = 3
        , const float in_speed = 10.f
        , const int in_fire_delay = 10
        , const int in_special_delay = 180
        , const int in_invincibility_duration = 100
        , const int in_flicker_frequency = 3
    )
        : max_lives(in_lives)
        , lives(in_lives)
        , speed(in_speed)
        , fire_delay(in_fire_delay)
        , special_delay(in_special_delay)
        , invincibility_duration(in_invincibility_duration)
        , fire_countdown(0)
        , special_countdown(0) 
        , invincibility_countdown(in_invincibility_duration)
        , flicker_frequency(in_flicker_frequency)
    { }
    ~CPlayerStats() { }
};

class CDeathSpawner : public Component {
public:
    CShape prefab;
    Tag tag;
    int amount;
    int lifespan;
    float speed;
    int recursion;
    CDeathSpawner(
        const int in_amount
        , const CShape & in_prefab
        , const int in_lifespan
        , const float in_speed
        , const int in_recursion
        , const Tag in_tag
    ) 
        : amount(in_amount)
        , prefab(in_prefab)
        , lifespan(in_lifespan)
        , speed(in_speed)
        , recursion(in_recursion)
        , tag(in_tag) 
    { }
    ~CDeathSpawner() { }
};

class CWeaponPickup : public Component {
public:
    enum PickupType {
        ShotSingle,
        ShotSpread,
        SpecialExplosion,
        SpecialFlamethower
    };
    CWeaponPickup() {};
    ~CWeaponPickup() {};
};

class CPickupSpawner : public Component {
public:
    CWeaponPickup payload;
    CPickupSpawner() {};
    ~CPickupSpawner() {};
};

class CScoreReward : public Component {
public:
    int score {100};
    CScoreReward(int in_score) : score(in_score) {};
    ~CScoreReward() {};
};
