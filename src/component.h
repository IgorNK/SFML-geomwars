#pragma once
#include "vec2.h"
#include <iostream>
#include <map>

enum ComponentType {
    Transform,
    Velocity,
    Name
};

static std::map<ComponentType, std::string> component_names {
    {ComponentType::Transform, "Transform"},
    {ComponentType::Velocity, "Velocity"},
    {ComponentType::Name, "Name"}
};

static std::map<std::string, ComponentType> name_components {
    {"Transform", ComponentType::Transform},
    {"Velocity", ComponentType::Velocity},
    {"Name", ComponentType::Name}
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
    float velocity {0.f};
};

class CName : public Component {
public:
    std::string name {""};
};