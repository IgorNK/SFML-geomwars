#include "game.h"

void Game::sEffects(const sf::Time &deltaTime) {
  for (const std::shared_ptr<Entity> player :
       m_entity_manager.get_entities(Tag::Player)) {
    if (player->player && player->invincibility) {
      const int freq = player->player->flicker_frequency;
      const int countdown = player->invincibility->countdown;
      if (countdown > 0) {
        if (player->shape) {
          sf::CircleShape &shape = player->shape->shape;
          const sf::Color color = shape.getFillColor();
          const sf::Color outline = shape.getOutlineColor();
          int alpha = outline.a;
          if (countdown % freq == 0) {
            if (alpha > 0) {
              alpha = 0;
            } else {
              alpha = 255;
            }
          }
          if (countdown < freq) {
            alpha = 255;
          }
          shape.setFillColor(sf::Color(color.r, color.g, color.b, alpha));
          shape.setOutlineColor(
              sf::Color(outline.r, outline.g, outline.b, alpha));
        }
      }
    }
  }
}
