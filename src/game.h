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
    std::string m_userconfig_file;
    sf::Font m_font;
    sf::RenderWindow m_window {sf::RenderWindow()};
    sf::Clock m_delta_clock {sf::Clock()};
    int m_enemy_spawn_interval {50};
    int m_enemy_spawn_countdown {50};
    int m_player_spawn_countdown {100};
    int m_player_spawn_interval {100};
    size_t m_score {0};
    size_t m_score_to_boss {1000};
    size_t m_score_to_boss_base {1000};
    float m_score_to_boss_mult {1.5f};
    size_t m_frameCount {0};
    float m_shape_rotation {5.f};
    void test_config(Config & config) const;
    Config parse_tokens(const std::vector<std::string> & tokenstream) const;
    const Config read_file(const std::string & configfile) const;
    void export_config(Config & config, const std::string & filename) const;
    const bool collides(const CTransform & transform_a, const CTransform & transform_b, const CCollider & collider_a, const CCollider & collider_b) const;
    const Vec2 bounce_movement(const CVelocity & velocity, const CRect & bounds, const CTransform & transform, const CCollider & collider) const;
    const Vec2 limit_movement(const CVelocity & velocity, const CRect & bounds, const CTransform & transform, const CCollider & collider) const;

    bool m_running {true};
    bool m_paused {false};
    bool m_sMovement {true};
    bool m_sCollision {true};
    bool m_sInputHandling {true};
    bool m_sRender {true};
    bool m_sGUI {true};
    bool m_sEnemySpawner {true};
public:
    Game(const std::string & userconfig, const std::string & default_config);
    ~Game() {};
    void init(const std::string & configfile);
    void shutdown();
    void run();
    void setPaused(const bool paused);    
    void on_game_over();

    // Spawning:
    void spawn_world();
    void spawn_player();
    void spawn_enemy();
    void spawn_boss();  
    void setup_random_enemy(Entity & enemy, const sf::FloatRect & spawn_bounds);
    void setup_boss(Entity & enemy, const sf::FloatRect & spawn_bounds);
    void setup_player(Entity & player, const Vec2 & position);
    void test_spawn();
    void shoot();
    void shootSpecialWeapon();
    void spawnSmallEntities(const Vec2 & position, const CDeathSpawner & spawner);
    void spawnPickup(const Vec2 & position, const CPickupSpawner & spawner);

    // Hits and death:
    void on_entity_hit(Entity & entity);
    void on_entity_death(Entity & entity);
    void on_pickup(const CWeaponPickup::PickupType type);

    // Systems:
    void sEnemySpawner(const sf::Time & deltaTime);
    void sPlayerSpawner(const sf::Time & deltaTime);
    void sMovement(const sf::Time & deltaTime);
    void sCollision();    
    void sLifespan(const sf::Time & deltaTime);
    void sDamageReact(const sf::Time & deltaTime);
    void sEffects(const sf::Time & deltaTime);
    void sTimers(const sf::Time & deltaTime);
    void sUserInput();
    void sInputHandling();
    void sGUI();
    void sRender(const sf::Time & deltaTime);
};