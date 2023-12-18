#pragma once
#include <vector>
#include <map>
#include <memory>
#include "component.h"

enum Tag {
    Player,
    Enemies,
    Bullets,
    WorldBounds,
};

static std::map<std::string, Tag> tag_names {
    {"Player", Tag::Player},
    {"Enemies", Tag::Enemies},
    {"Bullets", Tag::Bullets},
    {"WorldBounds", Tag::WorldBounds}
};

static std::map<Tag, std::string> name_tags {
    {Tag::Player, "Player"},
    {Tag::Enemies, "Enemies"},
    {Tag::Bullets, "Bullets"},
    {Tag::WorldBounds, "WorldBounds"}
};

class Entity {
    const size_t m_id = 0;
    const Tag m_tag;
    bool m_is_alive = true;
public:
    std::shared_ptr<CTransform> transform;
    std::shared_ptr<CVelocity> velocity;
    std::shared_ptr<CCollider> collider;
    std::shared_ptr<CLifespan> lifespan;
    std::shared_ptr<CWeapon> weapon;
    std::shared_ptr<CSpecialWeapon> special_weapon;
    std::shared_ptr<CName> name;
    std::shared_ptr<CShape> shape;
    std::shared_ptr<CRect> rect;
    
    Entity(const Tag tag, const size_t id) : m_tag(tag), m_id(id) {}
    
    const size_t id() {
        return m_id;
    }

    Tag tag() {
        return m_tag;
    }

    void destroy() {
        m_is_alive = false;
    }

    bool is_alive() {
        return m_is_alive;
    }
};