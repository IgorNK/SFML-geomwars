#include "game.h"

void Game::shoot() {
  for (const std::shared_ptr<Entity> player :
       m_entity_manager.get_entities(Tag::Player)) {
    if (player->weapon && player->transform) {
      if (player->weapon->fire_countdown <= 0) {

        switch (player->weapon->mode) {
        case (CWeapon::FireMode::ShotSingle): {
          const std::string configHeader =
              "ShotSingle" + std::to_string(player->weapon->power);
          const float speed = read_config_f(configHeader, "speed");
          const int fireRate = read_config_i(configHeader, "fireRate");
          const int lifespan = read_config_i(configHeader, "lifespan");
          const float collisionRadius =
              read_config_f(configHeader, "collisionRadius");
          const float shapeRadius = read_config_f(configHeader, "shapeRadius");
          const int fillRed = read_config_i(configHeader, "fillRed");
          const int fillGreen = read_config_i(configHeader, "fillGreen");
          const int fillBlue = read_config_i(configHeader, "fillBlue");
          const int outlineRed = read_config_i(configHeader, "outlineRed");
          const int outlineGreen = read_config_i(configHeader, "outlineGreen");
          const int outlineBlue = read_config_i(configHeader, "outlineBlue");
          const int outlineThickness =
              read_config_i(configHeader, "outlineThickness");
          const int vertices = read_config_i(configHeader, "vertices");
          const int health = read_config_i(configHeader, "health");

          const Vec2 position = player->transform->position.clone();
          const float rotation = player->transform->rotation;
          const Vec2 velocity =
              Vec2::forward().rotate_rad(rotation).normalize() * speed;

          const CShape bullet_prefab = CShape(
              shapeRadius, vertices, sf::Color(fillRed, fillGreen, fillBlue),
              sf::Color(outlineRed, outlineGreen, outlineBlue),
              outlineThickness);

          spawn_bullet(position, rotation, velocity, bullet_prefab,
                       collisionRadius, lifespan);
          player->weapon->fire_countdown = fireRate;
          break;
        }
        case (CWeapon::FireMode::ShotSpread): {
          const std::string configHeader =
              "ShotSpread" + std::to_string(player->weapon->power);
          const float speed = read_config_f(configHeader, "speed");
          const float spread = read_config_f(configHeader, "spread");
          const int amount = read_config_i(configHeader, "amount");
          const int fireRate = read_config_i(configHeader, "fireRate");
          const int lifespan = read_config_i(configHeader, "lifespan");
          const float collisionRadius =
              read_config_f(configHeader, "collisionRadius");
          const float shapeRadius = read_config_f(configHeader, "shapeRadius");
          const int fillRed = read_config_i(configHeader, "fillRed");
          const int fillGreen = read_config_i(configHeader, "fillGreen");
          const int fillBlue = read_config_i(configHeader, "fillBlue");
          const int outlineRed = read_config_i(configHeader, "outlineRed");
          const int outlineGreen = read_config_i(configHeader, "outlineGreen");
          const int outlineBlue = read_config_i(configHeader, "outlineBlue");
          const int outlineThickness =
              read_config_i(configHeader, "outlineThickness");
          const int vertices = read_config_i(configHeader, "vertices");
          const int health = read_config_i(configHeader, "health");

          const CShape bullet_prefab = CShape(
              shapeRadius, vertices, sf::Color(fillRed, fillGreen, fillBlue),
              sf::Color(outlineRed, outlineGreen, outlineBlue),
              outlineThickness);

          const Vec2 position = player->transform->position.clone();
          const float rotation = player->transform->rotation;
          const Vec2 velocity =
              Vec2::forward().rotate_rad(rotation).normalize() * speed;

          const float spread_rad = spread / Vec2::rad_to_deg;
          const float rot = spread_rad / amount;
          Vec2 r_vel =
              velocity.clone().rotate_rad(-rot * (((float)amount - 1) / 2));
          for (int i = 0; i < amount; ++i) {
            spawn_bullet(position, rot, r_vel, bullet_prefab, collisionRadius,
                         lifespan);
            r_vel.rotate_rad(rot);
          }
          player->weapon->fire_countdown = fireRate;
          break;
        }
        case (CWeapon::FireMode::ShotLaser): {
          const std::string configHeader =
              "ShotLaser" + std::to_string(player->weapon->power);
          const int fireRate = read_config_i(configHeader, "fireRate");
          const int lifespan = read_config_i(configHeader, "lifespan");
          const float offset = read_config_f(configHeader, "offset");
          const float length = read_config_f(configHeader, "length");
          const float thickness = read_config_f(configHeader, "thickness");
          const int fillRed = read_config_i(configHeader, "fillRed");
          const int fillGreen = read_config_i(configHeader, "fillGreen");
          const int fillBlue = read_config_i(configHeader, "fillBlue");
          const int fillAlpha = read_config_i(configHeader, "fillAlpha");
          const int outlineRed = read_config_i(configHeader, "outlineRed");
          const int outlineGreen = read_config_i(configHeader, "outlineGreen");
          const int outlineBlue = read_config_i(configHeader, "outlineBlue");
          const int outlineAlpha = read_config_i(configHeader, "fillAlpha");
          const int outlineThickness =
              read_config_i(configHeader, "outlineThickness");

          spawn_laser(
              *player->transform.get(), // Parent transform
              Vec2(offset, 0),          // Offset
              length,                   // length
              thickness,                // thickness
              sf::Color(fillRed, fillGreen, fillBlue, fillAlpha), // Fill Color
              sf::Color(outlineRed, outlineGreen, outlineBlue,
                        outlineAlpha), // Outline Color
              outlineThickness,        // Outline Thickness
              lifespan                 // Lifespan
          );
          player->weapon->fire_countdown = fireRate;
          break;
        }
        default:
          break;
        }
      }
    }
  }
}

const std::shared_ptr<Entity>
Game::spawn_bullet(const Vec2 &position, const float rotation,
                   const Vec2 &velocity, const CShape &bullet_prefab,
                   const float collision_radius, const int lifespan) {
  const std::shared_ptr<Entity> bullet =
      m_entity_manager.add_entity(Tag::Bullets);
  bullet->transform =
      std::make_shared<CTransform>(CTransform(position, rotation));
  bullet->velocity = std::make_shared<CVelocity>(CVelocity(velocity));
  bullet->shape = std::make_shared<CShape>(CShape(bullet_prefab));
  bullet->collider = std::make_shared<CCollider>(CCollider(collision_radius));
  bullet->name = std::make_shared<CName>(CName("Bullet"));
  bullet->lifespan = std::make_shared<CLifespan>(CLifespan(lifespan));
  bullet->health = std::make_shared<CHealth>(CHealth(1));
  return bullet;
}

const std::shared_ptr<Entity>
Game::spawn_laser(const CTransform &parent_transform, const Vec2 &offset,
                  const float length, const float thickness,
                  const sf::Color &fill_color, const sf::Color &outline_color,
                  const float outline_thickness, const int lifespan) {
  const std::shared_ptr<Entity> laser =
      m_entity_manager.add_entity(Tag::Bullets);
  laser->transform = std::make_shared<CTransform>(CTransform(
      parent_transform.position + offset, parent_transform.rotation));
  laser->name = std::make_shared<CName>(CName("Laser"));
  laser->lifespan = std::make_shared<CLifespan>(CLifespan(lifespan));
  const Vec2 endpoint = offset + Vec2::forward().normalize() * length;
  laser->line = std::make_shared<CLine>(
      CLine(parent_transform, offset, endpoint, thickness, fill_color,
            outline_color, outline_thickness));
  return laser;
}

void Game::shootSpecialWeapon() {
  for (const std::shared_ptr<Entity> player :
       m_entity_manager.get_entities(Tag::Player)) {
    if (player->special_weapon && player->transform) {
      if (player->special_weapon->fire_countdown <= 0) {

        switch (player->special_weapon->mode) {
        case (CSpecialWeapon::FireMode::SpecialExplosion): {
          const std::string configHeader =
              "SpecialExplosion" +
              std::to_string(player->special_weapon->power);

          const int fireRate = read_config_i(configHeader, "fireRate");
          const int lifespan = read_config_i(configHeader, "lifespan");
          const int smallLifespan =
              read_config_i(configHeader, "smallLifespan");
          const int speed = read_config_i(configHeader, "speed");
          const int smallSpeed = read_config_i(configHeader, "smallSpeed");
          const int smallAmount = read_config_i(configHeader, "smallAmount");
          const float collisionRadius =
              read_config_f(configHeader, "collisionRadius");
          const float shapeRadius = read_config_f(configHeader, "shapeRadius");
          const int fillRed = read_config_i(configHeader, "fillRed");
          const int fillGreen = read_config_i(configHeader, "fillGreen");
          const int fillBlue = read_config_i(configHeader, "fillBlue");
          const int outlineRed = read_config_i(configHeader, "outlineRed");
          const int outlineGreen = read_config_i(configHeader, "outlineGreen");
          const int outlineBlue = read_config_i(configHeader, "outlineBlue");
          const int outlineThickness =
              read_config_i(configHeader, "outlineThickness");
          const int vertices = read_config_i(configHeader, "vertices");
          const int health = read_config_i(configHeader, "health");
          const int recursion = read_config_i(configHeader, "recursion");

          const Vec2 position = player->transform->position.clone();
          const float rotation = player->transform->rotation;
          const Vec2 velocity =
              Vec2::forward().rotate_rad(rotation).normalize() * speed;
          const CShape bullet_prefab = CShape(
              shapeRadius, vertices, sf::Color(fillRed, fillGreen, fillBlue),
              sf::Color(outlineRed, outlineGreen, outlineBlue),
              outlineThickness);

          const std::shared_ptr<Entity> bullet = spawn_special_bullet(
              position, rotation, bullet_prefab, collisionRadius, lifespan);
          bullet->velocity = std::make_shared<CVelocity>(CVelocity(velocity));
          bullet->spawner = std::make_shared<CDeathSpawner>(
              CDeathSpawner(smallAmount, bullet_prefab, smallLifespan,
                            smallSpeed, recursion, Tag::Bullets));
          player->special_weapon->fire_countdown = fireRate;
          break;
        }
        case (CSpecialWeapon::FireMode::SpecialRotor): {
          const std::string configHeader =
              "SpecialRotor" + std::to_string(player->special_weapon->power);

          const float angularSpeed =
              read_config_f(configHeader, "angularSpeed");
          const int amount = read_config_i(configHeader, "amount");
          const float radius = read_config_f(configHeader, "radius");
          const int fireRate = read_config_i(configHeader, "fireRate");
          const int lifespan = read_config_i(configHeader, "lifespan");
          const float collisionRadius =
              read_config_f(configHeader, "collisionRadius");
          const float shapeRadius = read_config_f(configHeader, "shapeRadius");
          const int fillRed = read_config_i(configHeader, "fillRed");
          const int fillGreen = read_config_i(configHeader, "fillGreen");
          const int fillBlue = read_config_i(configHeader, "fillBlue");
          const int outlineRed = read_config_i(configHeader, "outlineRed");
          const int outlineGreen = read_config_i(configHeader, "outlineGreen");
          const int outlineBlue = read_config_i(configHeader, "outlineBlue");
          const int outlineThickness =
              read_config_i(configHeader, "outlineThickness");
          const int vertices = read_config_i(configHeader, "vertices");
          const int health = read_config_i(configHeader, "health");

          const CShape bullet_prefab = CShape(
              shapeRadius, vertices, sf::Color(fillRed, fillGreen, fillBlue),
              sf::Color(outlineRed, outlineGreen, outlineBlue),
              outlineThickness);

          const Vec2 position = player->transform->position.clone();
          const float rotation = player->transform->rotation;

          const float spread_rad = 360.f / Vec2::rad_to_deg;
          const float rot = spread_rad / amount;

          for (int i = 0; i < amount; ++i) {
            const std::shared_ptr<Entity> bullet = spawn_special_bullet(
                position, rot * i, bullet_prefab, collisionRadius, lifespan);
            bullet->orbit = std::make_shared<COrbit>(
                COrbit(*player->transform.get(), radius, angularSpeed));
          }
          player->special_weapon->fire_countdown = fireRate;
          break;
        }
        case (CSpecialWeapon::FireMode::SpecialFlamethrower): {
          const std::string configHeader =
              "SpecialFlamethrower" +
              std::to_string(player->special_weapon->power);
          const float spread = read_config_f(configHeader, "spread");
          const int speed = read_config_i(configHeader, "speed");
          const int duration = read_config_i(configHeader, "duration");
          const int fireRate = read_config_i(configHeader, "fireRate");
          const float offset = read_config_f(configHeader, "offset");
          const int freq = read_config_i(configHeader, "freq");
          const int quantity = read_config_i(configHeader, "quantity");
          const int smallLifespan =
              read_config_i(configHeader, "smallLifespan");
          const float randomScale = read_config_f(configHeader, "randomScale");
          const int fillRedMin = read_config_i(configHeader, "fillRedMin");
          const int fillRedMax = read_config_i(configHeader, "fillRedMax");
          const int fillGreenMin = read_config_i(configHeader, "fillGreenMin");
          const int fillGreenMax = read_config_i(configHeader, "fillGreenMax");
          const int fillBlue = read_config_i(configHeader, "fillBlue");
          const int alphaMin = read_config_i(configHeader, "alphaMin");
          const int alphaMax = read_config_i(configHeader, "alphaMax");
          const int radiusMin = read_config_i(configHeader, "radiusMin");
          const int radiusMax = read_config_i(configHeader, "radiusMax");
          const int vertsMin = read_config_i(configHeader, "vertsMin");
          const int vertsMax = read_config_i(configHeader, "vertsMax");
          const int amountTypes = read_config_i(configHeader, "amountTypes");

          const float spread_rad = 30 / Vec2::rad_to_deg;

          std::vector<CShape> shapes{};
          for (int i = 0; i < amountTypes; i++) {
            const float radius =
                (float)std::rand() / (float)RAND_MAX * (radiusMax - radiusMin) +
                radiusMin;
            const int verts = std::round((float)std::rand() / (float)RAND_MAX *
                                             (vertsMax - vertsMin) +
                                         vertsMin);
            const int red = std::round((float)std::rand() / (float)RAND_MAX *
                                           (fillRedMax - fillRedMin) +
                                       fillRedMin);
            const int green = std::round((float)std::rand() / (float)RAND_MAX *
                                             (fillRedMax - fillRedMin) +
                                         fillRedMin);
            const int alpha = std::round((float)std::rand() / (float)RAND_MAX *
                                             (alphaMax - alphaMin) +
                                         alphaMin);
            shapes.push_back(CShape(radius, verts,
                                    sf::Color(red, green, fillBlue, alpha),
                                    sf::Color(0, 0, 0, 0), // outline
                                    0                      // outline thickness
                                    ));
          }

          const std::shared_ptr<Entity> emitter =
              m_entity_manager.add_entity(Tag::Emitters);
          emitter->transform = std::make_shared<CTransform>(
              CTransform(*player->transform.get()));
          emitter->emitter = std::make_shared<CEmitter>(
              CEmitter(*player->transform.get(), // Parent transform
                       shapes,
                       Vec2::forward() * offset, // Offset
                       duration,
                       spread_rad, // Spread angle
                       speed, freq, quantity, smallLifespan, randomScale));
          player->special_weapon->fire_countdown = fireRate;
          break;
        }
        default:
          break;
        }
      }
    }
  }
}

const std::shared_ptr<Entity>
Game::spawn_special_bullet(const Vec2 &position, const float rotation,
                           const CShape &bullet_prefab,
                           const float collision_radius, const int lifespan) {
  const std::shared_ptr<Entity> bullet =
      m_entity_manager.add_entity(Tag::Bullets);
  bullet->transform =
      std::make_shared<CTransform>(CTransform(position, rotation));
  bullet->shape = std::make_shared<CShape>(CShape(bullet_prefab));
  bullet->collider = std::make_shared<CCollider>(CCollider(collision_radius));
  bullet->name = std::make_shared<CName>(CName("Bullet"));
  bullet->lifespan = std::make_shared<CLifespan>(CLifespan(lifespan));
  bullet->health = std::make_shared<CHealth>(CHealth(1));
  return bullet;
}
