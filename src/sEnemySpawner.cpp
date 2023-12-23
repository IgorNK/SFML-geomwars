#include "game.h"

void Game::sEnemySpawner(const sf::Time &deltaTime) {
  if (m_frameCount % m_sequence_spawn_delay == 0) {
    m_entity_manager.pop_schedule();
  }

  if (m_enemy_spawn_countdown <= 0) {
    spawn_enemy();
    if (!m_tutorial_spawned) {
      spawn_tutorial_messages();
      m_tutorial_spawned = true;
    }
    m_enemy_spawn_countdown = m_enemy_spawn_interval;
    return;
  }
  m_enemy_spawn_countdown--;
}

void Game::spawn_enemy() {
  for (const std::shared_ptr<Entity> wb :
       m_entity_manager.get_entities(Tag::WorldBounds)) {
    const sf::FloatRect &w_bounds = wb->rect->rect;
    const float border = wb->rect->border;
    const sf::FloatRect spawn_bounds =
        sf::FloatRect(w_bounds.left + border, w_bounds.top + border,
                      w_bounds.width - border, w_bounds.height - border);
    const std::shared_ptr<Entity> enemy =
        m_entity_manager.add_entity(Tag::Enemies);
    const float isBoss = false;
    setup_random_enemy(*enemy.get(), isBoss, spawn_bounds);
  }
}

void Game::spawn_boss() {
  for (const std::shared_ptr<Entity> wb :
       m_entity_manager.get_entities(Tag::WorldBounds)) {
    const sf::FloatRect &w_bounds = wb->rect->rect;
    const float border = wb->rect->border;
    const sf::FloatRect spawn_bounds =
        sf::FloatRect(w_bounds.left + border, w_bounds.top + border,
                      w_bounds.width - border, w_bounds.height - border);
    const std::shared_ptr<Entity> enemy =
        m_entity_manager.add_entity(Tag::Enemies);
    const float isBoss = true;
    setup_random_enemy(*enemy.get(), isBoss, spawn_bounds);
  }
}

void Game::setup_random_enemy(Entity &enemy, const bool isBoss,
                              const sf::FloatRect &spawn_bounds) {
  std::string configHeader = "Enemy";
  if (isBoss) {
    configHeader = "Boss";
  }

  std::shared_ptr<Entity> player;
  for (auto e : m_entity_manager.get_entities(Tag::Player)) {
    player = std::move(e);
    break;
  }

  const float shapeRadius = read_config_f(configHeader, "shapeRadius");
  const float collisionRadius = read_config_f(configHeader, "collisionRadius");
  const float speedMin = read_config_f(configHeader, "speedMin");
  const float speedMax = read_config_f(configHeader, "speedMax");
  const int outlineRed = read_config_i(configHeader, "outlineRed");
  const int outlineGreen = read_config_i(configHeader, "outlineGreen");
  const int outlineBlue = read_config_i(configHeader, "outlineBlue");
  const int outlineThickness = read_config_i(configHeader, "outlineThickness");
  const int verticesMin = read_config_i(configHeader, "verticesMin");
  const int verticesMax = read_config_i(configHeader, "verticesMax");
  const float vertSizeMultiplier =
      read_config_f(configHeader, "vertSizeMultiplier");

  const float rand_speed =
      (float)(std::rand()) / (float)(RAND_MAX) * (speedMax - speedMin) +
      speedMin;
  const float rand_angle = (float)(std::rand()) / (float)(RAND_MAX) * 360;
  const Vec2 rand_velocity = Vec2(0, 1).rotate_deg(rand_angle) * rand_speed;
  const int rand_vertices =
      (float)(std::rand()) / (float)(RAND_MAX) * (verticesMax - verticesMin) +
      verticesMin;

  const float smallRadius = read_config_f(configHeader, "smallRadius");
  const float smallCollisionRadius =
      read_config_f(configHeader, "smallCollisionRadius");
  const float smallSpeed = read_config_f(configHeader, "smallSpeed");
  const int lifespan = read_config_i(configHeader, "smallLifespan");
  const int invincibilityDuration =
      read_config_i(configHeader, "invincibilityDuration");

  const int base_score = read_config_i(configHeader, "score");
  const float score_multiplier =
      read_config_f(configHeader, "scoreSizeMultiplier");
  const int score = base_score + base_score * ((score_multiplier - 1) *
                                               (rand_vertices - verticesMin));
  const float radius =
      shapeRadius +
      shapeRadius * ((vertSizeMultiplier - 1) * (rand_vertices - verticesMin));
  const int scaledCollisionRadius =
      collisionRadius + collisionRadius * ((vertSizeMultiplier - 1) *
                                           (rand_vertices - verticesMin));

  float rand_x = (float)(std::rand()) / (float)(RAND_MAX)*spawn_bounds.width +
                 spawn_bounds.left;
  float rand_y = (float)(std::rand()) / (float)(RAND_MAX)*spawn_bounds.height +
                 spawn_bounds.top;

  if (player) {
    const float right = player->transform->position.x +
                        player->collider->radius * 2 + scaledCollisionRadius;
    const float left = player->transform->position.x -
                       player->collider->radius * 2 - scaledCollisionRadius;
    const float up = player->transform->position.y -
                     player->collider->radius * 2 - scaledCollisionRadius;
    const float down = player->transform->position.y +
                       player->collider->radius * 2 + scaledCollisionRadius;
    while (rand_x < right && rand_x > left) {
      rand_x = (float)(std::rand()) / (float)(RAND_MAX)*spawn_bounds.width +
               spawn_bounds.left;
    }
    while (rand_y < down && rand_y > up) {
      rand_y = (float)(std::rand()) / (float)(RAND_MAX)*spawn_bounds.height +
               spawn_bounds.top;
    }
  }

  sf::Color fillColor = sf::Color(0, 0, 0);
  sf::Color outlineColor = sf::Color(outlineRed, outlineGreen, outlineBlue);
  sf::Color smallFillColor = sf::Color(0, 0, 0);
  sf::Color smallOutlineColor =
      sf::Color(outlineRed, outlineGreen, outlineBlue);
  if (isBoss) {
    std::map<CWeaponPickup::PickupType, sf::Color> types{
        {CWeaponPickup::PickupType::ShotSingle,
         sf::Color(255, outlineGreen, outlineBlue)},
        {CWeaponPickup::PickupType::ShotSpread,
         sf::Color(outlineRed, 255, outlineBlue)},
        {CWeaponPickup::PickupType::ShotLaser,
         sf::Color(outlineRed, outlineGreen, 255)},
    };
    const int pickup_idx = std::round((float)(std::rand()) / (float)(RAND_MAX) *
                                      (types.size() - 1));
    std::cout << "idx: " << pickup_idx << "/" << types.size() - 1 << '\n';
    const CWeaponPickup::PickupType pickup_type =
        (CWeaponPickup::PickupType)pickup_idx;
    outlineColor = types[pickup_type];
    std::cout << "Color raw: " << outlineRed << ", " << outlineGreen << ", "
              << outlineBlue << '\n';
    std::cout << "Color: " << (int)outlineColor.r << ", " << (int)outlineColor.g
              << ", " << (int)outlineColor.b << '\n';
    smallOutlineColor = outlineColor;
    const CWeaponPickup pickup = CWeaponPickup(pickup_type);

    const float p_radius = read_config_f("Pickup", "shapeRadius");
    const float p_collision = read_config_f("Pickup", "collisionRadius");
    const int p_thickness = read_config_i("Pickup", "outlineThickness");
    const int p_verts = read_config_i("Pickup", "vertices");
    const int p_lifespan = read_config_i("Pickup", "lifespan");
    const sf::Color p_fillColor = outlineColor;
    const sf::Color p_outlineColor = sf::Color(0, 0, 0);

    const CShape p_shape =
        CShape(p_radius, p_verts, p_fillColor, p_outlineColor, p_thickness);

    enemy.pickup_spawner = std::make_shared<CPickupSpawner>(
        CPickupSpawner(pickup, p_shape, p_lifespan, p_collision));
  }
  const CShape small_prefab = CShape(smallRadius, rand_vertices, smallFillColor,
                                     smallOutlineColor, outlineThickness);

  const int recursion = 0;

  enemy.name = std::make_shared<CName>(CName("Enemy"));
  enemy.transform = std::make_shared<CTransform>(CTransform(rand_x, rand_y));
  enemy.shape = std::make_shared<CShape>(
      CShape(radius, rand_vertices, fillColor, outlineColor, outlineThickness));
  enemy.collider =
      std::make_shared<CCollider>(CCollider(scaledCollisionRadius));
  enemy.velocity = std::make_shared<CVelocity>(CVelocity(rand_velocity));
  enemy.health = std::make_shared<CHealth>(CHealth(rand_vertices));
  enemy.invincibility =
      std::make_shared<CInvincibility>(CInvincibility(invincibilityDuration));
  enemy.score_reward = std::make_shared<CScoreReward>(CScoreReward(score));
  enemy.spawner = std::make_shared<CDeathSpawner>(
      CDeathSpawner(rand_vertices, small_prefab, lifespan, smallSpeed,
                    recursion, Tag::Enemies));
}

void Game::spawnSmallEntities(const Vec2 &position,
                              const CDeathSpawner &spawner) {
  const float d_angle = 360 / Vec2::rad_to_deg / spawner.amount;
  const CShape &prefab = spawner.prefab;
  const float radius = prefab.shape.getRadius();
  const int score = read_config_i("Enemy", "smallScore");

  for (int i = 0; i < spawner.amount; ++i) {
    const float angle = d_angle * i;
    const Vec2 n_velocity = Vec2::forward().rotate_rad(angle).normalize();
    const Vec2 spawn_pos = position + n_velocity * radius;
    const std::shared_ptr<Entity> enemy =
        m_entity_manager.add_entity(spawner.tag);
    enemy->name = std::make_shared<CName>(CName("SmEnemy"));
    enemy->transform =
        std::make_shared<CTransform>(CTransform(spawn_pos.x, spawn_pos.y));
    enemy->shape = std::make_shared<CShape>(CShape(prefab));
    enemy->collider = std::make_shared<CCollider>(CCollider(radius));
    enemy->velocity =
        std::make_shared<CVelocity>(CVelocity(n_velocity * spawner.speed));
    enemy->health = std::make_shared<CHealth>(CHealth(1));
    enemy->lifespan = std::make_shared<CLifespan>(CLifespan(spawner.lifespan));
    if (spawner.tag == Tag::Enemies) {
      enemy->score_reward = std::make_shared<CScoreReward>(CScoreReward(score));
    }
    if (spawner.recursion > 0) {
      enemy->spawner = std::make_shared<CDeathSpawner>(
          spawner.amount, prefab, spawner.lifespan, spawner.speed,
          spawner.recursion - 1, spawner.tag);
    }
  }
}
