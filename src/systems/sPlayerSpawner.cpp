#include "../game.h"

void Game::sPlayerSpawner(const sf::Time &deltaTime) {
  if (m_entity_manager.get_entities(Tag::Player).size() > 0) {
    return;
  }
  if (m_player_spawn_countdown <= 0) {
    spawn_player();
    m_player_spawn_countdown = m_enemy_spawn_interval;
    return;
  }
  m_player_spawn_countdown--;
}

void Game::spawn_player() {
  for (const std::shared_ptr<Entity> wb :
       m_entity_manager.get_entities(Tag::WorldBounds)) {
    const sf::FloatRect &w_bounds = wb->rect->rect;
    const Vec2 center = Vec2(w_bounds.left + w_bounds.width / 2,
                             w_bounds.top + w_bounds.height / 2);
    const std::shared_ptr<Entity> player =
        m_entity_manager.add_entity(Tag::Player);
    setup_player(*player.get(), center);
  }
}

void Game::setup_player(Entity &player, const Vec2 &position) {
  try {
    const float shapeRadius = read_config_f("Player", "shapeRadius");
    const float collisionRadius = read_config_f("Player", "collisionRadius");
    const float speed = read_config_f("Player", "speed");
    const int invincibilityDuration =
        read_config_i("Player", "invincibilityDuration");
    const int flickerRate = read_config_i("Player", "flickerRate");
    const int fillRed = read_config_i("Player", "fillRed");
    const int fillGreen = read_config_i("Player", "fillGreen");
    const int fillBlue = read_config_i("Player", "fillBlue");
    const int outlineRed = read_config_i("Player", "outlineRed");
    const int outlineGreen = read_config_i("Player", "outlineGreen");
    const int outlineBlue = read_config_i("Player", "outlineBlue");
    const int outlineThickness = read_config_i("Player", "outlineThickness");
    const int vertices = read_config_i("Player", "vertices");
    const int health = read_config_i("Player", "health");

    player.name = std::make_shared<CName>(CName("Player"));
    player.player =
        std::make_shared<CPlayerStats>(CPlayerStats(3, speed, flickerRate));
    player.transform = std::make_shared<CTransform>(CTransform(position));
    player.velocity = std::make_shared<CVelocity>(CVelocity());
    player.weapon =
        std::make_shared<CWeapon>(CWeapon(CWeapon::FireMode::ShotSingle));
    player.special_weapon = std::make_shared<CSpecialWeapon>(CSpecialWeapon());
    player.shape = std::make_shared<CShape>(CShape(
        shapeRadius, vertices, sf::Color(fillRed, fillGreen, fillBlue),
        sf::Color(outlineRed, outlineGreen, outlineBlue), outlineThickness));
    player.collider = std::make_shared<CCollider>(CCollider(collisionRadius));
    player.input = std::make_shared<CInput>(CInput());
    player.health = std::make_shared<CHealth>(CHealth(health));
    player.invincibility =
        std::make_shared<CInvincibility>(CInvincibility(invincibilityDuration, invincibilityDuration));
    std::cout << "done\n";
  } catch (std::exception &e) {
    throw std::runtime_error(std::string(e.what(), sizeof(e.what())) +
                             " | Error during player setup");
  }
}
