#include "game.h"
#include "vec2.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>
void Game::run() {

  while (m_running) {
    // create/cleanup entities
    m_entity_manager.update();

    sf::Time deltaTime = m_delta_clock.restart();

    // update call required by ImGui
    ImGui::SFML::Update(m_window, deltaTime);

    sUserInput();
    if (!m_paused) {
      if (m_sInputHandling)
        sInputHandling();
      sTimers(deltaTime);
      sEmitters(deltaTime);
      sEffects(deltaTime);
      sLifespan(deltaTime);
      sDamageReact(deltaTime);
      if (m_sMovement)
        sMovement(deltaTime);
      if (m_sCollision)
        sCollision();
      sPlayerSpawner(deltaTime);
      if (m_sEnemySpawner)
        sEnemySpawner(deltaTime);
    }
    if (m_sGUI)
      sGUI();
    // Rendering should be last
    sRender(deltaTime);
    m_frameCount++;
  }
  shutdown();
}

void Game::setPaused(const bool paused) { m_paused = paused; }

Game::Game(const std::string &userconfig, const std::string &default_config) {
  m_userconfig_file = userconfig;
  try {
    init(userconfig);
  } catch (std::exception &e) {
    try {
      init(default_config);
    } catch (std::exception &e) {
      throw std::runtime_error(std::string(e.what(), sizeof(e.what())) +
                               " | Could not initialize.");
    }
  }
}

void Game::init(const std::string &configfile) {
  try {
    m_config = read_file(configfile);
  } catch (std::exception &e) {
    throw std::runtime_error(
        std::string(e.what(), sizeof(e.what())) +
        " | Could not read configuration file: " + configfile);
  }
  m_game_close_timeout = read_config_i("Global", "gameCloseTimeout");
  const std::string font_path = read_config_s("Font", "path");
  m_font = sf::Font();
  if (!m_font.loadFromFile(font_path)) {
    throw std::runtime_error("Could load font at: " + font_path);
  }

  std::srand(std::time(nullptr));
  const size_t width = read_config_i("Window", "width");
  const size_t height = read_config_i("Window", "height");
  const size_t depth = read_config_i("Window", "depth");
  const size_t framerate = read_config_i("Window", "refreshRate");
  const bool fullscreen = read_config_i("Window", "fullscreen");

  m_enemy_spawn_interval = read_config_i("Global", "enemySpawnInterval");
  m_enemy_spawn_countdown = m_enemy_spawn_interval;
  m_player_spawn_interval = read_config_i("Player", "spawnInterval");
  m_player_spawn_countdown = m_player_spawn_interval;

  m_score = 0;
  m_score_to_boss_base = read_config_i("Boss", "scoreRequirement");
  m_score_to_boss = m_score + m_score_to_boss_base;
  m_score_to_boss_mult = read_config_f("Boss", "scoreRequirementMultiplier");

  m_shape_rotation = read_config_f("Global", "shapeRotation");
  m_video_modes = sf::VideoMode::getFullscreenModes();

  create_window(width, height, "ImGUI + SFML = <3", depth, framerate, fullscreen);
}

void Game::create_window(const size_t width, const size_t height, const std::string & title, const size_t depth, const size_t framerate, const bool fullscreen) {
  std::cout << "creating window\n";
  if (fullscreen) {
    m_window.create(sf::VideoMode::getDesktopMode(), title, sf::Style::Fullscreen);
  } else {
    m_window.create(sf::VideoMode(width, height, depth), title);
  }
  std::cout << "created\n";
  m_window.setFramerateLimit(framerate);
  std::cout << "set framerate limit\n";
  spawn_world();
  std::cout << "spawned world\n";
  ImGui::SFML::Init(m_window);
  std::cout << "initialized imgui\n";
}
void Game::create_window(const sf::VideoMode & mode, const std::string & title, const size_t framerate, const bool fullscreen) {
  create_window(mode.width, mode.height, title, mode.bitsPerPixel, framerate, fullscreen);
}

void Game::shutdown() {
  m_window.close();
  ImGui::SFML::Shutdown();
}

void Game::spawn_world() {
	m_entity_manager.flush();
  const std::shared_ptr<Entity> wb =
      m_entity_manager.add_entity(Tag::WorldBounds);
  const sf::Vector2u window_res = m_window.getSize();
  wb->rect = std::make_shared<CRect>(CRect(window_res.x, window_res.y));
  const std::shared_ptr<Entity> score =
      m_entity_manager.add_entity(Tag::ScoreWindow);
  const int font_size = read_config_i("Font", "size");
  const int font_red = read_config_i("Font", "red");
  const int font_green = read_config_i("Font", "green");
  const int font_blue = read_config_i("Font", "blue");
  score->text = std::make_shared<CText>(CText(
      "Score:", m_font, font_size, sf::Color(font_red, font_green, font_blue)));
  score->transform = std::make_shared<CTransform>(CTransform(Vec2(0, 0)));
}

void Game::spawn_tutorial_messages() {
  for (const std::shared_ptr<Entity> wb :
       m_entity_manager.get_entities(Tag::WorldBounds)) {
    const sf::FloatRect w_bounds = wb->rect->rect;
    const int font_size = 48;
    const int spacing = 4;
    const int v_spacing = 160;
    const sf::Color font_color = sf::Color(255, 160, 50);
    std::string msg_a = "WASD MOVE";
    std::string msg_b = "LMB SHOOT";
    std::string msg_c = "RMB SPECIAL";
    std::reverse(msg_a.begin(), msg_a.end());
    std::reverse(msg_b.begin(), msg_b.end());
    std::reverse(msg_c.begin(), msg_c.end());
    const int width_a = msg_a.size() * font_size + spacing;
    const int width_b = msg_b.size() * font_size + spacing;
    const int width_c = msg_c.size() * font_size + spacing;
    const int left_a = w_bounds.left + w_bounds.width - width_a;
    const int left_b = (w_bounds.width - width_b) / 2;
    const int left_c = (w_bounds.width - width_c) / 2;
    const int top_a = (w_bounds.top + w_bounds.height / 2) - v_spacing;
    const int top_c = (w_bounds.top + w_bounds.height / 2) + v_spacing * 2;
    const int top_b = top_c - v_spacing;
    spawn_text_enemies(msg_c, Vec2(left_c, top_c), font_size, font_color,
                       spacing);
    spawn_text_enemies(msg_b, Vec2(left_b, top_b), font_size, font_color,
                       spacing);
    spawn_text_enemies(msg_a, Vec2(left_a, top_a), font_size, font_color,
                       spacing);
  }
}

void Game::spawn_text_enemies(const std::string &text, const Vec2 &position,
                              const int font_size, const sf::Color &font_color,
                              const int spacing) {
  Vec2 pos = position;
  for (auto &ch : text) {
    const CText text_shape = CText(std::string(1, ch), m_font, font_size, font_color);
    const float width = text_shape.text.getLocalBounds().width;
    const std::shared_ptr<Entity> enemy =
        m_entity_manager.schedule(Tag::Enemies);
    enemy->name = std::make_shared<CName>(CName("Text enemy"));
    enemy->transform = std::make_shared<CTransform>(CTransform(pos));
    enemy->text = std::make_shared<CText>(text_shape);
    enemy->collider = std::make_shared<CCollider>(CCollider(width * 2));
    enemy->bounce = std::make_shared<CBounce>(CBounce());
    enemy->health = std::make_shared<CHealth>(CHealth(1));
    enemy->score_reward = std::make_shared<CScoreReward>(CScoreReward(200));
    enemy->lifespan = std::make_shared<CLifespan>(CLifespan(300));
    pos.x -= width + spacing * 5;
  }
}

void Game::on_game_over() {}

void Game::on_entity_hit(Entity &entity) {
  if (entity.health) {
    entity.health->react_countdown = entity.health->react_duration;
    if (entity.invincibility) {
      entity.invincibility->countdown = entity.invincibility->duration;
    }
    if (--entity.health->hp <= 0) {
      on_entity_death(entity);
    }
  }
}

void Game::on_entity_death(Entity &entity) {
  if (entity.transform) {
    if (entity.spawner) {
      spawnSmallEntities(entity.transform->position, *entity.spawner.get());
    }
    if (entity.pickup_spawner) {
      spawnPickup(entity.transform->position, *entity.pickup_spawner.get());
      m_enemy_spawn_interval -= m_enemy_spawn_interval *
                                read_config_f("Global", "enemySpawnMultiplier");
    }
  }

  if (entity.score_reward) {
    m_score += entity.score_reward->score;
    for (std::shared_ptr<Entity> score :
         m_entity_manager.get_entities(Tag::ScoreWindow)) {
      score->text->text.setString("Score: " + std::to_string(m_score));
    }
    if (m_score >= m_score_to_boss) {
      spawn_boss();
      m_score_to_boss_base *= m_score_to_boss_mult;
      m_score_to_boss = m_score + m_score_to_boss_base;
    }
  }

  entity.destroy();
}

void Game::on_pickup(const CWeaponPickup::PickupType type) {
  for (std::shared_ptr<Entity> player :
       m_entity_manager.get_entities(Tag::Player)) {
    if (!player->weapon || !player->special_weapon) {
      return;
    }
    const int primary_power = player->weapon->power;
    const int secondary_power = player->special_weapon->power;
    const int max_power = read_config_i("Global", "maxPower");
    switch (type) {
    case CWeaponPickup::PickupType::ShotSingle: {
      std::cout << "Red Single!\n";
      if (player->weapon->mode == CWeapon::FireMode::ShotSingle) {
        if (primary_power < max_power && secondary_power < max_power) {
          ++player->weapon->power;
          ++player->special_weapon->power;
        } else {
          m_score += 500;
        }
      } else {
        player->weapon->power = 0;
        player->weapon->mode = CWeapon::FireMode::ShotSingle;
        player->special_weapon->mode =
            CSpecialWeapon::FireMode::SpecialExplosion;
      }
      break;
    }
    case CWeaponPickup::PickupType::ShotSpread: {
      std::cout << "Green Spread!\n";
      if (player->weapon->mode == CWeapon::FireMode::ShotSpread) {
        if (primary_power < max_power && secondary_power < max_power) {
          ++player->weapon->power;
          ++player->special_weapon->power;
        } else {
          m_score += 500;
        }
      } else {
        player->weapon->power = 0;
        player->weapon->mode = CWeapon::FireMode::ShotSpread;
        player->special_weapon->mode = CSpecialWeapon::FireMode::SpecialRotor;
      }
      break;
    }
    case CWeaponPickup::PickupType::ShotLaser: {
      if (player->weapon->mode == CWeapon::FireMode::ShotLaser) {
        if (primary_power < max_power && secondary_power < max_power) {
          ++player->weapon->power;
          ++player->special_weapon->power;
        } else {
          m_score += 500;
        }
      } else {
        player->weapon->power = 0;
        player->weapon->mode = CWeapon::FireMode::ShotLaser;
        player->special_weapon->mode =
            CSpecialWeapon::FireMode::SpecialFlamethrower;
      }
      break;
    }
    default: {
      break;
    }
    }
  }
}

void Game::spawnPickup(const Vec2 &position, const CPickupSpawner &spawner) {
  const CWeaponPickup &prefab = spawner.payload;
  const CShape &shape = spawner.shape;
  const float radius = shape.shape.getRadius();

  const std::shared_ptr<Entity> pickup =
      m_entity_manager.add_entity(Tag::Pickups);
  pickup->name = std::make_shared<CName>(CName("Pickup"));
  pickup->transform =
      std::make_shared<CTransform>(CTransform(position.x, position.y));
  pickup->shape = std::make_shared<CShape>(CShape(shape));
  pickup->collider = std::make_shared<CCollider>(CCollider(radius));
  pickup->lifespan = std::make_shared<CLifespan>(CLifespan(spawner.lifespan));
  pickup->weapon_pickup =
      std::make_shared<CWeaponPickup>(CWeaponPickup(prefab));
}
