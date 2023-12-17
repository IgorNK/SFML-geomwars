#pragma once
#include "vec2.h"
#include <iostream>
#include <map>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Rect.hpp>

enum ComponentType {
    Transform,
    Velocity,
    Name,
    Rect
};

static std::map<ComponentType, std::string> component_names {
    {ComponentType::Transform, "Transform"},
    {ComponentType::Velocity, "Velocity"},
    {ComponentType::Name, "Name"},
    {ComponentType::Rect, "Rect"},
};

static std::map<std::string, ComponentType> name_components {
    {"Transform", ComponentType::Transform},
    {"Velocity", ComponentType::Velocity},
    {"Name", ComponentType::Name},
    {"Rect", ComponentType::Rect}
};

class Component {
public:
};

class CTransform : public Component {
public:
    Vec2 position {0.f, 0.f};
    float rotation {0.f};
    float scale {1.f};
};

class CVelocity : public Component {
public:
    Vec2 velocity {0.f, 0.f};
};

class CName : public Component {
public:
    std::string name {"Default"};
};

class CShape : public Component {
public:
    sf::CircleShape shape {sf::CircleShape(100.f)};
};

class CRect : public Component {
public:
    sf::FloatRect rect {0, 0, 100, 100};
};