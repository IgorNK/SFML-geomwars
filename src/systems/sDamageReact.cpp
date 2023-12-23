#include "../game.h"

void Game::sDamageReact(const sf::Time &deltaTime) {
  for (std::shared_ptr<Entity> entity : m_entity_manager.get_entities()) {
    if (entity->health) {
      CHealth &health = *entity->health.get();
      if (entity->shape) {
        sf::CircleShape &shape = entity->shape->shape;
        const float scale =
            1.f + (health.expansion - 1.f) * ((float)health.react_countdown /
                                              (float)health.react_duration);
        shape.setScale(scale, scale);
      }
      if (entity->text) {
        sf::Text &shape = entity->text->text;
        const float scale =
            1.f + (health.expansion - 1.f) * ((float)health.react_countdown /
                                              (float)health.react_duration);
        shape.setScale(scale, scale);
      }
    }
  }
}
