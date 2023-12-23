#include "../game.h"

void Game::sTimers(const sf::Time &deltaTime) {
  for (const std::shared_ptr<Entity> entity : m_entity_manager.get_entities()) {
    if (entity->invincibility && entity->invincibility->countdown > 0) {
      --entity->invincibility->countdown;
    }
    if (entity->lifespan && entity->lifespan->countdown > 0) {
      --entity->lifespan->countdown;
    }
    if (entity->health && entity->health->react_countdown > 0) {
      --entity->health->react_countdown;
    }
    if (entity->weapon && entity->weapon->fire_countdown > 0) {
      --entity->weapon->fire_countdown;
    }
    if (entity->special_weapon && entity->special_weapon->fire_countdown > 0) {
      --entity->special_weapon->fire_countdown;
    }
    if (entity->emitter && entity->emitter->countdown) {
      --entity->emitter->countdown;
    }
  }
  if (m_game_close_countdown > 0) {
    --m_game_close_countdown;
  }
}
