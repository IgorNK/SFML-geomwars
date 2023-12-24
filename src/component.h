#pragma once
#include "vec2.h"
#include <iostream>
#include <map>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Text.hpp>
#include "tag.h"

enum ComponentType {
	Transform,
	Velocity,
	Bounce,
	Orbit,
	Collider,
	Shape,
	Line,
	Text,
	Lifespan,
	Invincibility,
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
	WeaponPickup,
	Emitter,
};

static std::map<ComponentType, std::string> component_names {
	{ComponentType::Transform, "Transform"},
	{ComponentType::Velocity, "Velocity"},
	{ComponentType::Bounce, "Bounce"},
	{ComponentType::Orbit, "Orbit"},
	{ComponentType::Emitter, "Emitter"},
	{ComponentType::Collider, "Collider"},
	{ComponentType::Shape, "Shape"},
	{ComponentType::Line, "Line"},
	{ComponentType::Text, "Text"},
	{ComponentType::Lifespan, "Lifespan"},
	{ComponentType::Invincibility, "Invincibility"},
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
	{ComponentType::WeaponPickup, "WeaponPickup"},
	
};

static std::map<std::string, ComponentType> name_components {
	{"Transform", ComponentType::Transform},
	{"Velocity", ComponentType::Velocity},
	{"Bounce", ComponentType::Bounce},
	{"Orbit", ComponentType::Orbit},
	{"Emitter", ComponentType::Emitter},
	{"Collider", ComponentType::Collider},
	{"Shape", ComponentType::Shape},
	{"Line", ComponentType::Line},
	{"Text", ComponentType::Text},
	{"Lifespan", ComponentType::Lifespan},
	{"Invincibility", ComponentType::Invincibility},
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
	{"WeaponPickup", ComponentType::WeaponPickup},
	
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

class CBounce : public Component {
public:
	float amplitude {1.5f};
	bool direction {0};
	int frequency {70};
	int countdown {70};
	CBounce() {};
	~CBounce() {};
};

class COrbit : public Component {
public:
	const CTransform & parent;
	float angle;
	float radius;
	float speed;
	COrbit(
		const CTransform & in_parent, 
		const float in_radius, 
		const float in_speed
	) 
		: parent(in_parent)
		, angle(0.f)
		, radius(in_radius)
		, speed(in_speed) 
	{}
	~COrbit() {}
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

class CEmitter : public Component {
public:
	const CTransform & parent;
	const std::vector<CShape> particles;
	const Vec2 offset;
	const float angle;
	const float nozzleSpread;
	const float speed;
	const int freq;
	int countdown;
	const int quantity;
	const int lifespan;
	const float scale_mult;
	CEmitter(
		const CTransform & in_parent,
		const std::vector<CShape> & particle_prefabs,
		const Vec2 & in_offset,
		const int in_duration,
		const float spread_angle,
		const float in_nozzleSpread,
		const float in_speed,
		const int emit_freq,
		const int in_quantity,
		const int in_lifespan,
		const float random_scale_mult
	)
		: parent(in_parent)
		, particles(particle_prefabs)
		, offset(in_offset)
		, countdown(in_duration)
		, speed(in_speed)
		, angle(spread_angle)
		, nozzleSpread(in_nozzleSpread)
		, freq(emit_freq)
		, quantity(in_quantity)
		, lifespan(in_lifespan)
		, scale_mult(random_scale_mult)
	{ }
	~CEmitter() { };
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
	int duration {10};
	int countdown {10};
	CLifespan() { }
	CLifespan(const int span) : countdown(span), duration(span) { }
	~CLifespan() { }
};

class CInvincibility : public Component {
public:
	int countdown {10};
	int duration {10};
	CInvincibility() { }
	CInvincibility(const int in_duration, const int start = 0) : countdown(start), duration(in_duration) { }
	~CInvincibility() { }
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



class CLine: public Component {
public:
	const CTransform & parent;
	sf::RectangleShape shape;
	const Vec2 start;
	const Vec2 end;
	CLine(
		const CTransform & in_parent_transform
		, const Vec2 & in_start = Vec2(0, 0)
		, const Vec2 & in_end = Vec2(1, 0)
		, const float thickness = 1.f
		, const sf::Color & fill_color = sf::Color(0, 0, 255)
		, const sf::Color & outline_color = sf::Color(0, 0, 255)        
		, const float outline_thickness = 1.f
	)
		: parent(in_parent_transform)
		, start(in_start)
		, end(in_end)
		, shape(sf::RectangleShape(sf::Vector2f(in_start.distance_to(in_end), thickness)))
	{
		shape.setFillColor(fill_color);
		shape.setOutlineColor(outline_color);
		shape.setOutlineThickness(outline_thickness);
		shape.setOrigin(0, thickness / 2);
		shape.setRotation(start.angle_to_deg(end));
	}
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
		ShotSpread,
		ShotLaser
	};
	int fire_countdown {10};
	int power {0};
	CWeapon::FireMode mode {CWeapon::FireMode::ShotSingle};
	CWeapon(
		  const CWeapon::FireMode fire_mode
	)
		: mode(fire_mode) 
	{ }
	~CWeapon() { }
};

class CSpecialWeapon: public Component {
public:
	enum FireMode {
		SpecialExplosion,
		SpecialRotor,
		SpecialFlamethrower,
	};
	CSpecialWeapon::FireMode mode;
	int fire_countdown {10};
	int power {0};
	CSpecialWeapon(
		const CSpecialWeapon::FireMode fire_mode = CSpecialWeapon::FireMode::SpecialExplosion
	) 
		: mode(fire_mode)
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
	int flicker_frequency;
	CPlayerStats(
		const int in_lives = 3
		, const float in_speed = 10.f
		, const int in_flicker_frequency = 3
	)
		: max_lives(in_lives)
		, lives(in_lives)
		, speed(in_speed)
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
		ShotLaser
	};
	PickupType type {PickupType::ShotSingle};
	CWeaponPickup() {};
	CWeaponPickup(
		CWeaponPickup::PickupType in_type
	)
		: type(in_type)
	{};
	~CWeaponPickup() {};
};

class CPickupSpawner : public Component {
public:
	CWeaponPickup payload {CWeaponPickup()};    
	CShape shape {CShape()};
	int lifespan {500};
	int collision {32};
	CPickupSpawner(
		const CWeaponPickup & in_payload,
		const CShape & in_shape,
		int in_lifespan,
		int in_collision
	) 
		: payload(in_payload)
		, shape(in_shape)
		, lifespan(in_lifespan)
		, collision(in_collision)
	{};
	~CPickupSpawner() {};
};

class CScoreReward : public Component {
public:
	int score {100};
	CScoreReward(int in_score) : score(in_score) {};
	~CScoreReward() {};
};
