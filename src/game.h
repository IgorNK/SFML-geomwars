#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include "entity_manager.h"
#include "configuration.cpp"

class Game {
    EntityManager m_entity_manager;
    Configuration::Config m_config;
    sf::RenderWindow m_window {sf::RenderWindow()};
    sf::Clock m_delta_clock {sf::Clock()};
    bool m_running {true};
    bool m_paused {false};
    float m_spawn_interval {0.1f};
public:
    Game(const std::string configfile);
    ~Game() {};
    void init(const std::string configfile);
    void shutdown();
    void spawn_world();
    void spawn_player();
    void spawn_enemy();
    void setup_random_enemy(const Entity & enemy, const sf::FloatRect spawn_bounds);
    void test_spawn();
    void sEnemySpawner(const sf::Time deltaTime);
    void sMovement(const sf::Time deltaTime);
    void sCollision();
    void sUserInput();
    void sGUI();
    void sRender(const sf::Time deltaTime);    
    void run();
    void setPaused(const bool paused);
    void shoot();
    void shootSpecialWeapon();
    void spawnSmallEnemies();
};