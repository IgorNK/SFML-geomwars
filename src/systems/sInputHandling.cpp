#include "../game.h"

void Game::sInputHandling() {
  for (std::shared_ptr<Entity> p : m_entity_manager.get_entities(Tag::Player)) {
    if (p->player && p->transform && p->input && p->velocity) {

      const CInput &input = *p->input.get();

      CPlayerStats &player = *p->player.get();
      CTransform &transform = *p->transform.get();
      Vec2 &velocity = p->velocity->velocity;

      const Vec2 direction = input.mousePosition - transform.position;
      const float rotation = Vec2::forward().angle_to_rad(direction);

      transform.rotation = rotation;
      velocity.x = input.axis.x * player.speed;
      velocity.y = input.axis.y * player.speed;

      if (input.fire) {
        shoot();
      }
      if (input.secondaryFire) {
        shootSpecialWeapon();
      }
    }
  }
}
