#pragma once
#include <vector>
#include <map>
#include <memory>
#include "component.h"
#include "tag.h"


static std::map<std::string, Tag> tag_names {
    {"Player", Tag::Player},
    {"Enemies", Tag::Enemies},
    {"Bullets", Tag::Bullets},
    {"Pickups", Tag::Pickups},
    {"WorldBounds", Tag::WorldBounds},
    {"ScoreWindow", Tag::ScoreWindow}
};

static std::map<Tag, std::string> name_tags {
    {Tag::Player, "Player"},
    {Tag::Enemies, "Enemies"},
    {Tag::Bullets, "Bullets"},
    {Tag::Pickups, "Pickups"},
    {Tag::WorldBounds, "WorldBounds"},
    {Tag::ScoreWindow, "ScoreWindow"}
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
    std::shared_ptr<CInvincibility> invincibility;
    std::shared_ptr<CHealth> health;
    std::shared_ptr<CWeapon> weapon;
    std::shared_ptr<CSpecialWeapon> special_weapon;
    std::shared_ptr<CName> name;
    std::shared_ptr<CShape> shape;
    std::shared_ptr<CRect> rect;
    std::shared_ptr<CInput> input;
    std::shared_ptr<CPlayerStats> player;
    std::shared_ptr<CDeathSpawner> spawner;
    std::shared_ptr<CPickupSpawner> pickup_spawner;
    std::shared_ptr<CScoreReward> score_reward;
    std::shared_ptr<CWeaponPickup> weapon_pickup;
    std::shared_ptr<CText> text;
    
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