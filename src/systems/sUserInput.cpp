#include "../game.h"

void Game::sUserInput() {
  sf::Event event;
  std::shared_ptr<Entity> player;
  for (const std::shared_ptr<Entity> p :
       m_entity_manager.get_entities(Tag::Player)) {
    if (p->input) {
      player = std::move(p);
    }
  }

  if (player) {
    player->input->axis = Vec2(0, 0);
    player->input->fire = false;
    player->input->secondaryFire = false;
  }

  Vec2 move_axis{0, 0};
  bool fire_input = false;
  bool secondary_fire_input = false;

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
    move_axis.y = -1;
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
    move_axis.y = 1;
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
    move_axis.x = -1;
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
    move_axis.x = 1;
  }
  if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
    if (!ImGui::GetIO().WantCaptureMouse && player) {
      fire_input = true;
    }
  }
  if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
    if (!ImGui::GetIO().WantCaptureMouse && player) {
      secondary_fire_input = true;
    }
  }

  if (player) {
    player->input->axis = move_axis;
    player->input->fire = fire_input;
    player->input->secondaryFire = secondary_fire_input;
  }

  while (m_window.pollEvent(event)) {
    ImGui::SFML::ProcessEvent(event);
    if (event.type == sf::Event::Closed) {
      m_running = false;
    }

    if (event.type == sf::Event::KeyPressed) {
      switch (event.key.code) {
      case sf::Keyboard::F5: {
        m_sGUI = !m_sGUI;
        break;
      }
      case sf::Keyboard::F1: {
        m_paused = !m_paused;
        break;
      }
      case sf::Keyboard::Escape: {
        if (m_game_close_countdown > 0) {
          m_running = false;
        } else {
          m_game_close_countdown = m_game_close_timeout;
        }
        break;
      }
      }
    }

    if (event.type == sf::Event::MouseMoved) {
      const sf::Vector2i mouse_pos = sf::Mouse::getPosition(m_window);
      if (player) {
        player->input->mousePosition = Vec2(mouse_pos.x, mouse_pos.y);
      }
    }

    if (event.type == sf::Event::MouseButtonPressed) {
    }
  }
}
