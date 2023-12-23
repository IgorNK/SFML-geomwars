#include "../game.h"

void Game::sCollision() {
  for (const std::shared_ptr<Entity> enemy :
       m_entity_manager.get_entities(Tag::Enemies)) {
    if (!enemy->is_alive() ||
        (enemy->invincibility && enemy->invincibility->countdown > 0)) {
      continue;
    }
    if (enemy->transform && enemy->collider) {
      for (const std::shared_ptr<Entity> bullet :
           m_entity_manager.get_entities(Tag::Bullets)) {
        if (!bullet->is_alive()) {
          continue;
        }
        if (bullet->transform && bullet->collider) {
          if (collides(*bullet->transform.get(), *enemy->transform.get(),
                       *bullet->collider.get(), *enemy->collider.get())) {
            on_entity_hit(*bullet.get());
            on_entity_hit(*enemy.get());
          }
        }
        if (bullet->transform && bullet->line) {
          if (line_intersect(*bullet->transform.get(), *enemy->transform.get(),
                             *bullet->line.get(), *enemy->collider.get())) {
            on_entity_hit(*bullet.get());
            on_entity_hit(*enemy.get());
          }
        }
      }
      for (const std::shared_ptr<Entity> player :
           m_entity_manager.get_entities(Tag::Player)) {
        if (player->transform && player->collider) {
          if (!player->is_alive() ||
              (player->invincibility && player->invincibility->countdown > 0)) {
            continue;
          }
          if (collides(*player->transform.get(), *enemy->transform.get(),
                       *player->collider.get(), *enemy->collider.get())) {
            on_entity_hit(*player.get());
            on_entity_hit(*enemy.get());
          }
        }
      }
    }
  }

  for (const std::shared_ptr<Entity> pickup :
       m_entity_manager.get_entities(Tag::Pickups)) {
    if (pickup->transform && pickup->collider) {
      for (const std::shared_ptr<Entity> player :
           m_entity_manager.get_entities(Tag::Player)) {
        if (player->transform && player->collider) {
          if (collides(*player->transform.get(), *pickup->transform.get(),
                       *player->collider.get(), *pickup->collider.get())) {
            if (pickup->weapon_pickup) {
              on_pickup(pickup->weapon_pickup->type);
            }
            pickup->destroy();
          }
        }
      }
    }
  }
}

const bool Game::collides(const CTransform &t_a, const CTransform &t_b,
                          const CCollider &c_a, const CCollider &c_b) const {
  const float dist = t_a.position.distance_to(t_b.position);
  const float radius_sum = c_a.radius + c_b.radius;
  if (dist * dist < radius_sum * radius_sum) {
    return true;
  }
  return false;
}

const bool Game::line_intersect(const CTransform &t_line,
                                const CTransform &t_obj, const CLine &line,
                                const CCollider &collider) const {
  const float dist_sq = Vec2::distance_to_line_sq(
      t_line.position + line.start.clone().rotate_rad(t_line.rotation),
      t_line.position + line.end.clone().rotate_rad(t_line.rotation),
      t_obj.position);
  if (dist_sq < collider.radius * collider.radius) {
    return true;
  }
  return false;
}
