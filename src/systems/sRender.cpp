#include "../game.h"

void Game::sRender(const sf::Time &deltaTime) {
  m_window.clear();
  // Render stuff
  if (m_sRender) {
    for (const std::shared_ptr<Entity> entity :
         m_entity_manager.get_entities()) {
      if (entity->shape) {
        sf::CircleShape &shape = entity->shape->shape;
        if (entity->transform) {
          const Vec2 &pos = entity->transform->position;
          shape.setPosition(sf::Vector2f(pos.x, pos.y));
          shape.rotate(m_shape_rotation);
        }
        m_window.draw(shape);
      }
      if (entity->text && entity->transform) {
        sf::Text &text = entity->text->text;
        const Vec2 &pos = entity->transform->position;
        text.setPosition(sf::Vector2f(pos.x, pos.y));
        m_window.draw(text);
      }
      if (entity->line) {
        sf::RectangleShape &shape = entity->line->shape;
        if (entity->transform) {
          const float rot = entity->line->parent.rotation;
          const Vec2 &pos = entity->line->parent.position +
                            entity->line->start.clone().rotate_rad(rot);
          shape.setPosition(sf::Vector2f(pos.x, pos.y));
          shape.setRotation(rot * Vec2::rad_to_deg);
        }
        m_window.draw(shape);
      }
    }
  }
  ImGui::SFML::Render(m_window);
  m_window.display();
}
