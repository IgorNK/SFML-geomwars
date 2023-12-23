#include "game.h"

void Game::sEmitters(const sf::Time &deltaTime) {
  for (const std::shared_ptr<Entity> entity :
       m_entity_manager.get_entities(Tag::Emitters)) {
    if (entity->emitter && entity->transform) {
      const CEmitter &emitter = *entity->emitter.get();
      if (emitter.countdown > 0) {
        const Vec2 &parent_pos = emitter.parent.position;
        const float parent_rot = emitter.parent.rotation;

        if (emitter.countdown % emitter.freq == 0) {
          for (int i = 0; i < emitter.quantity; ++i) {
            const int rand_idx =
                std::round((float)(std::rand()) / (float)(RAND_MAX) *
                           (emitter.particles.size() - 1));
            const float rand_angle =
                (float)(std::rand()) / (float)(RAND_MAX)*emitter.angle -
                emitter.angle / 2;
            const int lifespan =
                emitter.lifespan +
                emitter.lifespan * ((emitter.scale_mult - 1) *
                                    ((float)std::rand() / (float)RAND_MAX));
            const float speed =
                emitter.speed +
                emitter.speed * ((emitter.scale_mult - 1) *
                                 ((float)std::rand() / (float)RAND_MAX));

            CShape shape = emitter.particles[rand_idx];
            const float collider_radius = shape.shape.getRadius();
            const Vec2 pos =
                parent_pos + emitter.offset.clone().rotate_rad(
                                 emitter.parent.rotation + rand_angle);
            const Vec2 vel =
                Vec2::forward().rotate_rad(parent_rot).rotate_rad(rand_angle) *
                speed;

            const std::shared_ptr<Entity> particle =
                m_entity_manager.add_entity(Tag::Bullets);
            particle->transform =
                std::make_shared<CTransform>(CTransform(pos, parent_rot));
            particle->velocity = std::make_shared<CVelocity>(CVelocity(vel));
            particle->shape = std::make_shared<CShape>(CShape(shape));
            particle->collider =
                std::make_shared<CCollider>(CCollider(collider_radius));
            particle->lifespan =
                std::make_shared<CLifespan>(CLifespan(lifespan));
          }
        }
      } else {
        entity->destroy();
      }
    }
  }
}
