#include "game.h"

void Game::sLifespan(const sf::Time &deltaTime) {
  for (const std::shared_ptr<Entity> entity : m_entity_manager.get_entities()) {
    if (entity->lifespan) {
      const int &countdown = entity->lifespan->countdown;
      const int duration = entity->lifespan->duration;
      if (countdown <= 0) {
        on_entity_death(*entity.get());
      }
      if (entity->shape) {
        const float rate = (float)countdown / (float)duration * 255;
        sf::CircleShape &shape = entity->shape->shape;
        const sf::Color color = shape.getFillColor();
        const sf::Color outline = shape.getOutlineColor();
        shape.setFillColor(sf::Color(color.r, color.g, color.b, rate));
        shape.setOutlineColor(sf::Color(outline.r, outline.g, outline.b, rate));
      }
      if (entity->line) {
        const float rate = (float)countdown / (float)duration * 255;
        sf::RectangleShape &shape = entity->line->shape;
        const sf::Color color = shape.getFillColor();
        const sf::Color outline = shape.getOutlineColor();
        shape.setFillColor(sf::Color(color.r, color.g, color.b, rate));
        shape.setOutlineColor(sf::Color(outline.r, outline.g, outline.b, rate));
      }
    }
  }
}
