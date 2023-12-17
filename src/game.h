#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include "entity_manager.h"

class Game {
    EntityManager m_entity_manager;
    sf::RenderWindow m_window {sf::RenderWindow()};
    sf::Clock m_delta_clock {sf::Clock()};
    bool m_running {true};
    bool m_paused {false};
public:
    Game(std::string configfile);
    ~Game() {};
    void init(std::string configfile);
    void shutdown();
    void spawn_world();
    void spawn_player();
    void spawn_enemy();
    void test_spawn();
    void sEnemySpawner(sf::Time deltaTime);
    void sMovement(sf::Time deltaTime);
    void sCollision();
    void sUserInput();
    void sGUI();
    void sRender(sf::Time deltaTime);    
    void run();
    void setPaused(bool paused);
};