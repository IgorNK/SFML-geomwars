#include "../game.h"

void Game::sMovement(const sf::Time &deltaTime) {
  for (const std::shared_ptr<Entity> wb :
       m_entity_manager.get_entities(Tag::WorldBounds)) {
    if (wb->rect) {
      for (const std::shared_ptr<Entity> enemy :
           m_entity_manager.get_entities(Tag::Enemies)) {
        if (enemy->transform && enemy->collider && enemy->velocity) {
          enemy->velocity->velocity =
              bounce_movement(*enemy->velocity.get(), *wb->rect.get(),
                              *enemy->transform.get(), *enemy->collider.get());
        }
      }
    }
  }

  for (const std::shared_ptr<Entity> entity : m_entity_manager.get_entities()) {
    if (entity->transform && entity->velocity) {
      Vec2 &pos = entity->transform->position;
      Vec2 &vel = entity->velocity->velocity;

      if (entity->player && entity->collider) {
        for (const std::shared_ptr<Entity> wb :
             m_entity_manager.get_entities(Tag::WorldBounds)) {
          vel =
              limit_movement(*entity->velocity.get(), *wb->rect.get(),
                             *entity->transform.get(), *entity->collider.get());
        }
      }

      pos += vel;
    }
    if (entity->orbit && entity->transform) {
      const Vec2 parent_pos = entity->orbit->parent.position;
      const Vec2 pos =
          (parent_pos + Vec2::forward()
                                .rotate_rad(entity->transform->rotation)
                                .rotate_deg(entity->orbit->angle) *
                            entity->orbit->radius);
      entity->orbit->angle += entity->orbit->speed;
      if (entity->orbit->angle == 360) {
        entity->orbit->angle = 0;
      }
      entity->transform->position = pos;
    }
    if (entity->line && entity->transform) {
      const Vec2 parent_pos = entity->line->parent.position;
      const float parent_rot = entity->line->parent.rotation;
      entity->transform->position = parent_pos;
      entity->transform->rotation = parent_rot;
    }
  }
}

const Vec2 Game::bounce_movement(const CVelocity &velocity, const CRect &bounds,
                                 const CTransform &transform,
                                 const CCollider &collider) const {
  const sf::FloatRect &w_bounds = bounds.rect;
  const Vec2 &pos = transform.position;
  const float radius = collider.radius;
  Vec2 new_vel = velocity.velocity;

  if (pos.x - radius <= w_bounds.left ||
      pos.x + radius >= w_bounds.left + w_bounds.width) {
    new_vel.x = -new_vel.x;
  }
  if (pos.y - radius <= w_bounds.top ||
      pos.y + radius >= w_bounds.top + w_bounds.height) {
    new_vel.y = -new_vel.y;
  }
  return new_vel;
}

const Vec2 Game::limit_movement(const CVelocity &velocity, const CRect &bounds,
                                const CTransform &transform,
                                const CCollider &collider) const {
  const sf::FloatRect &w_bounds = bounds.rect;
  const Vec2 &pos = transform.position;
  const float radius = collider.radius;
  Vec2 new_vel = velocity.velocity;

  if (new_vel.x < 0 && pos.x - radius <= w_bounds.left) {
    new_vel.x = 0;
  }
  if (new_vel.x > 0 && pos.x + radius >= w_bounds.left + w_bounds.width) {
    new_vel.x = 0;
  }
  if (new_vel.y < 0 && pos.y - radius <= w_bounds.top) {
    new_vel.y = 0;
  }
  if (new_vel.y > 0 && pos.y + radius >= w_bounds.top + w_bounds.height) {
    new_vel.y = 0;
  }
  return new_vel;
}
