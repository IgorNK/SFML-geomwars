#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include "entity_manager.h"

typedef std::map<std::string, std::map<std::string, std::string>> Config;

class Game {
    EntityManager m_entity_manager;
    Config m_config;
    sf::RenderWindow m_window {sf::RenderWindow()};
    sf::Clock m_delta_clock {sf::Clock()};
    bool m_running {true};
    bool m_paused {false};
    int m_enemy_spawn_interval {50};
    int m_player_spawn_interval {100};
    size_t m_frameCount {0};
    float m_shape_rotation {5.f};
    void test_config(Config & config) const;
    Config parse_tokens(std::vector<std::string> tokenstream) const;
    const Config read_file(std::string configfile) const;
    const bool collides(const CTransform & transform_a, const CTransform & transform_b, const CCollider & collider_a, const CCollider & collider_b) const;
    const Vec2 bounce_movement(const CVelocity & velocity, const CRect & bounds, const CTransform & transform, const CCollider & collider) const;
    const Vec2 limit_movement(const CVelocity & velocity, const CRect & bounds, const CTransform & transform, const CCollider & collider) const;

public:
    Game(const std::string configfile);
    ~Game() {};
    void init(const std::string configfile);
    void shutdown();
    void spawn_world();
    void spawn_player();
    void spawn_enemy();
    void setup_random_enemy(Entity & enemy, const sf::FloatRect spawn_bounds);
    void setup_player(Entity & player, const Vec2 & position);
    void test_spawn();

    // Systems:
    void sEnemySpawner(const sf::Time deltaTime);
    void sPlayerSpawner(const sf::Time deltaTime);
    void sMovement(const sf::Time deltaTime);
    void sCollision();    
    void sLifespan(const sf::Time deltaTime);
    void sDamageReact(const sf::Time deltaTime);
    void sPlayerInvincibility(const sf::Time deltaTime);
    void sUserInput();
    void sInputHandling();
    void sGUI();
    void sRender(const sf::Time deltaTime);    

    void run();
    void setPaused(const bool paused);
    void shoot();
    void shootSpecialWeapon();
    void on_entity_hit(Entity & entity);
    void on_game_over();
    void spawnSmallEnemies(const Vec2 position, const CDeathSpawner & spawner);
};