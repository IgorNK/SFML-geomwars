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
public:
    Game(std::string configfile) : m_entity_manager(EntityManager()) {

    }

    ~Game() {

    }
    void sEnemySpawner();
    void sMovement();
    void sCollision();
    void sUserInput();
    void sGUI();
    void sRender();    
    void run();
};