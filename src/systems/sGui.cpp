#include "../game.h"
#include <set>

void Game::sGUI() {
  // Interface stuff
  if (m_game_close_countdown > 0) {
    ImGui::Begin("Quit game");
    ImGui::Text("Press [ESC] again to close game");
    ImGui::ProgressBar((float)m_game_close_countdown /
                       (float)m_game_close_timeout);
    ImGui::End();
  }

  ImGui::Begin("Geometry Wars");
  const int msg_timeout = 100;
  static int saved_timeout = 0;
  if (ImGui::Button("Save")) {
    export_config(m_config, m_userconfig_file);
    saved_timeout = msg_timeout;
  }
  if (saved_timeout > 0) {
    --saved_timeout;
    ImGui::SameLine();
    ImGui::TextColored({0.f, 1.f, 0.f, (float)saved_timeout / msg_timeout}, "Configuration saved");
  }
  static std::vector<Vec2> vectors{};

  ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_Reorderable;
  if (ImGui::BeginTabBar("TabBar", tab_bar_flags)) {

    if (ImGui::BeginTabItem("Entities")) {
      ImGui::BeginGroup();
      // Entity creation form
      std::vector<std::string> tags;
      for (auto const &v : tag_names) {
        tags.push_back(v.first);
      }
      static int tag_idx = 0;
      const std::string tag_preview = tags[tag_idx];
      if (ImGui::BeginCombo("##tag_select", tag_preview.c_str())) {
        for (int i = 0; i < tags.size(); ++i) {
          const bool is_selected = (tag_idx == i);
          if (ImGui::Selectable(tags[i].c_str(), is_selected)) {
            tag_idx = i;
          }
          if (is_selected) {
            ImGui::SetItemDefaultFocus();
          }
        }
        ImGui::EndCombo();
      }
      if (ImGui::Button("add##add_entity")) {
        m_entity_manager.add_entity(tag_names[tags[tag_idx]]);
      }
      ImGui::EndGroup();

      ImGui::BeginChild("Entity list", {400.f, 0}, true);
      static int entity_idx = -1;
      static int entity_collection = -1;
      if (ImGui::BeginTabBar("Entity collections", ImGuiTabBarFlags_None)) {
        if (ImGui::BeginTabItem("All")) {
          if (ImGui::BeginTable("Entity table", 1)) {
            const Entities &entities = m_entity_manager.get_entities();
            for (int i = 0; i < entities.size(); ++i) {
              const bool is_selected = (entity_idx == i);
              const std::shared_ptr<Entity> entity = entities[i];
              const std::string tag_name = name_tags[entity->tag()];
              const int entity_id = entity->id();
              ImGui::TableNextColumn();
              char bufdel[32];
              sprintf(bufdel, "D ##del%d", i);
              if (ImGui::Button(bufdel)) {
                entity->destroy();
              }
              ImGui::SameLine();
              char bufname[32];
              sprintf(bufname, "%s (%d)##entity%d", tag_name.c_str(), entity_id,
                      entity_id);
              if (ImGui::Selectable(bufname, is_selected)) {
                entity_idx = i;
                entity_collection = -1;
              }
              if (is_selected) {
                ImGui::SetItemDefaultFocus();
              }
            }
            ImGui::EndTable();
          }
          ImGui::EndTabItem();
        }
        for (auto &tag_r : tag_names) {
          const std::string tag_name = tag_r.first;
          const Tag tag = tag_r.second;
          if (ImGui::BeginTabItem(tag_name.c_str())) {
            if (ImGui::BeginTable("Entity table", 1)) {
              const Entities &entities = m_entity_manager.get_entities(tag);
              for (int i = 0; i < entities.size(); ++i) {
                const bool is_selected = (entity_idx == i);
                const std::shared_ptr<Entity> entity = entities[i];
                const std::string tag_name = name_tags[entity->tag()];
                const int entity_id = entity->id();
                ImGui::TableNextColumn();
                char bufdel[32];
                sprintf(bufdel, "D ##del%d", i);
                if (ImGui::Button(bufdel)) {
                  entity->destroy();
                }
                ImGui::SameLine();
                char bufname[32];
                sprintf(bufname, "%s (%d)##entity%d", tag_name.c_str(),
                        entity_id, entity_id);
                if (ImGui::Selectable(bufname, is_selected)) {
                  entity_idx = i;
                  entity_collection = (int)tag;
                }
                if (is_selected) {
                  ImGui::SetItemDefaultFocus();
                }
              }
              ImGui::EndTable();
            }
            ImGui::EndTabItem();
          }
        }
        ImGui::EndTabBar();
      }
      ImGui::EndChild();

      ImGui::SameLine();

      ImGui::BeginChild("Components", {400.f, 0}, true);
      static bool show_components = true;
      ImGui::Checkbox("show##show_components", &show_components);
      if (show_components) {
        static int comp_idx = -1;
        if (entity_idx >= 0) {
          const Entities &entities =
              m_entity_manager.get_entities(Tag(entity_collection));
          std::shared_ptr<Entity> entity;
          if (entities.size() > entity_idx) {
            entity = m_entity_manager.get_entities(
                Tag(entity_collection))[entity_idx];
          }
          if (entity) {
            if (entity->name) {
              char buf[32];
              sprintf(buf, "Name: %s##comp_name", entity->name->name.c_str());
              if (ImGui::Selectable(buf, comp_idx == ComponentType::Name)) {
                comp_idx = ComponentType::Name;
              }
            } else if (ImGui::Button("+Name##add_name")) {
              entity->name = std::make_shared<CName>(CName());
            }
            if (entity->transform) {
              const Vec2 pos = entity->transform->position;
              const float rot = entity->transform->rotation;
              const float scale = entity->transform->scale;
              char buf[32];
              sprintf(buf,
                      "Transform: P(%.3f, %.3f), R%.3f, S%.3f##comp_transform",
                      pos.x, pos.y, rot, scale);
              if (ImGui::Selectable(buf,
                                    comp_idx == ComponentType::Transform)) {
                comp_idx = ComponentType::Transform;
              }
            } else if (ImGui::Button("+Transform##add_transform")) {
              const std::shared_ptr<CTransform> transform(new CTransform());
              entity->transform = transform;
            }
            if (entity->velocity) {
              const Vec2 vel = entity->velocity->velocity;
              char buf[32];
              sprintf(buf, "Velocity: (%.3f, %.3f)##comp_velocity", vel.x,
                      vel.y);
              if (ImGui::Selectable(buf, comp_idx == ComponentType::Velocity)) {
                comp_idx = ComponentType::Velocity;
              }
            } else if (ImGui::Button("+Velocity##add_velocity")) {
              entity->velocity = std::make_shared<CVelocity>(CVelocity());
            }
            if (entity->player) {
              const int max_lives = entity->player->max_lives;
              const int lives = entity->player->lives;
              const float speed = entity->player->speed;
              const int flicker_freq = entity->player->flicker_frequency;
              char buf[32];
              sprintf(buf, "Player: L:%d/%d, Fl:%d##comp_player", lives,
                      max_lives, flicker_freq);
              if (ImGui::Selectable(buf,
                                    comp_idx == ComponentType::PlayerStats)) {
                comp_idx = ComponentType::PlayerStats;
              }
            } else if (ImGui::Button("+PlayerStats##add_player")) {
              entity->player = std::make_shared<CPlayerStats>(CPlayerStats());
            }
            if (entity->collider) {
              const float radius = entity->collider->radius;
              char buf[32];
              sprintf(buf, "Col Radius: %.3f##comp_collider", radius);
              if (ImGui::Selectable(buf, comp_idx == ComponentType::Collider)) {
                comp_idx = ComponentType::Collider;
              }
            } else if (ImGui::Button("+Collider##add_collider")) {
              entity->collider = std::make_shared<CCollider>(CCollider());
            }
            if (entity->lifespan) {
              const int countdown = entity->lifespan->countdown;
              const int duration = entity->lifespan->duration;
              char buf[32];
              sprintf(buf, "Lifespan: %d/%d##comp_collider", countdown,
                      duration);
              if (ImGui::Selectable(buf, comp_idx == ComponentType::Lifespan)) {
                comp_idx = ComponentType::Lifespan;
              }
            } else if (ImGui::Button("+Lifespan##add_lifespan")) {
              entity->lifespan = std::make_shared<CLifespan>(CLifespan());
            }
            if (entity->health) {
              const int hp = entity->health->hp;
              const int max_hp = entity->health->max_hp;
              const float expansion = entity->health->expansion;
              const int duration = entity->health->react_duration;
              const int countdown = entity->health->react_countdown;
              char buf[32];
              sprintf(buf, "HP: %d/%d, Ex%.3f D%d/%d##comp_collider", hp,
                      max_hp, expansion, duration, countdown);
              if (ImGui::Selectable(buf, comp_idx == ComponentType::Health)) {
                comp_idx = ComponentType::Health;
              }
            } else if (ImGui::Button("+Health##add_health")) {
              entity->health = std::make_shared<CHealth>(CHealth());
            }
            if (entity->shape) {
              const sf::CircleShape &shape = entity->shape->shape;
              const float radius = shape.getRadius();
              const float scale = shape.getScale().x;
              char buf[32];
              sprintf(buf, "Shape R:%.3f S:%.3f##comp_shape", radius, scale);
              if (ImGui::Selectable(buf, comp_idx == ComponentType::Shape)) {
                comp_idx = ComponentType::Shape;
              }
            } else if (ImGui::Button("+Shape##add_shape")) {
              entity->shape = std::make_shared<CShape>(CShape());
            }
          }
        }
      }
      ImGui::EndChild();

      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Systems")) {
      ImGui::BeginGroup();
      ImGui::Checkbox("Pause", &m_paused);
      ImGui::Checkbox("sMovement", &m_sMovement);
      ImGui::Checkbox("sCollision", &m_sCollision);
      ImGui::Checkbox("sInputHandling", &m_sInputHandling);
      ImGui::Checkbox("sRender", &m_sRender);
      ImGui::DragFloat("rotation", &m_shape_rotation, 0.1f, 0.0f, 10.f, "%.1f");
      ImGui::Checkbox("sGUI", &m_sGUI);
      ImGui::Checkbox("sEnemySpawner", &m_sEnemySpawner);
      ImGui::SameLine();
      if (ImGui::Button("Spawn")) {
        spawn_enemy();
      }
      ImGui::SameLine();
      if (ImGui::Button("Boss")) {
        spawn_boss();
      }
      ImGui::ProgressBar((float)m_enemy_spawn_countdown /
                         (float)m_enemy_spawn_interval);
      ImGui::DragInt("spawn interval", &m_enemy_spawn_interval, 10.f, 0, 1000);
      static float multiplier = read_config_f("Global", "enemySpawnMultiplier");
      if (ImGui::DragFloat("multiplier", &multiplier, 0.1f, 0.0f, 1.0f)) {
        write_config("Global", "enemySpawnMultiplier",
                     std::to_string(multiplier));
      };
      ImGui::EndGroup();

      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Weapons")) {
      std::shared_ptr<Entity> player;
      for (auto e : m_entity_manager.get_entities(Tag::Player)) {
        player = std::move(e);
      }
      if (player && player->weapon && player->special_weapon) {
        CWeapon::FireMode &primary_mode = player->weapon->mode;
        CSpecialWeapon::FireMode &secondary_mode = player->special_weapon->mode;
        int &primary_power = player->weapon->power;
        int &secondary_power = player->special_weapon->power;
        const char min = 0;
        const char max = read_config_i("Global", "maxPower");
        const std::vector<std::string> primary_modes{"Single", "Spread",
                                                     "Laser"};
        const std::vector<std::string> secondary_modes{"Explosion", "Rotor",
                                                       "Flamethrower"};

        if (ImGui::BeginChild("Primary##weapon-primary", {0, 0},
                              ImGuiChildFlags_FrameStyle |
                                  ImGuiChildFlags_Border |
                                  ImGuiChildFlags_AutoResizeY |
                                  ImGuiChildFlags_AutoResizeX)) {
          if (ImGui::BeginCombo("##primary_select",
                                primary_modes[primary_mode].c_str())) {
            for (int i = 0; i < primary_modes.size(); ++i) {
              const bool is_selected = (primary_mode == i);
              if (ImGui::Selectable(primary_modes[i].c_str(), is_selected)) {
                primary_mode = (CWeapon::FireMode)i;
              }
              if (is_selected) {
                ImGui::SetItemDefaultFocus();
              }
            }
            ImGui::EndCombo();
          }
          ImGui::SameLine();
          ImGui::RadioButton("P0", &primary_power, 0);
          ImGui::SameLine();
          ImGui::RadioButton("P1", &primary_power, 1);
          ImGui::SameLine();
          ImGui::RadioButton("P2", &primary_power, 2);
          ImGui::SameLine();
          ImGui::RadioButton("P3", &primary_power, 3);
          ImGui::SameLine();
          ImGui::RadioButton("P4", &primary_power, 4);
          switch (primary_mode) {
          case CWeapon::FireMode::ShotSingle: {
            const std::string configHeader =
                "ShotSingle" + std::to_string(primary_power);
            float speed = read_config_f(configHeader, "speed");
            int fireRate = read_config_i(configHeader, "fireRate");
            int lifespan = read_config_i(configHeader, "lifespan");
            float radius[2] = {read_config_f(configHeader, "collisionRadius"),
                               read_config_f(configHeader, "shapeRadius")};
            int fillColor[3] = {read_config_i(configHeader, "fillRed"),
                                read_config_i(configHeader, "fillGreen"),
                                read_config_i(configHeader, "fillBlue")};
            int outlineColor[3] = {read_config_i(configHeader, "outlineRed"),
                                   read_config_i(configHeader, "outlineGreen"),
                                   read_config_i(configHeader, "outlineBlue")};
            int outlineThickness =
                read_config_i(configHeader, "outlineThickness");
            int vertices = read_config_i(configHeader, "vertices");
            int health = read_config_i(configHeader, "health");

            if (ImGui::InputFloat("Speed", &speed, 0.1, 1.0)) {
              write_config(configHeader, "speed", std::to_string(speed));
            };
            if (ImGui::InputInt("Fire rate", &fireRate, 1, 10)) {
              write_config(configHeader, "fireRate", std::to_string(fireRate));
            };
            if (ImGui::InputInt("Lifespan", &lifespan, 1, 10)) {
              write_config(configHeader, "lifespan", std::to_string(lifespan));
            };
            if (ImGui::InputFloat2("Collision/Radius", radius)) {
              write_config(configHeader, "collisionRadius",
                           std::to_string(radius[0]));
              write_config(configHeader, "shapeRadius",
                           std::to_string(radius[1]));
            };
            if (ImGui::InputInt3("Fill color", fillColor)) {
              write_config(configHeader, "fillRed",
                           std::to_string(fillColor[0]));
              write_config(configHeader, "fillGreen",
                           std::to_string(fillColor[1]));
              write_config(configHeader, "fillBlue",
                           std::to_string(fillColor[2]));
            };
            if (ImGui::InputInt3("Outline color", outlineColor)) {
              write_config(configHeader, "outlineRed",
                           std::to_string(outlineColor[0]));
              write_config(configHeader, "outlineGreen",
                           std::to_string(outlineColor[1]));
              write_config(configHeader, "outlineBlue",
                           std::to_string(outlineColor[2]));
            };
            if (ImGui::InputInt("Thickness", &outlineThickness, 1, 1)) {
              write_config(configHeader, "outlineThickness",
                           std::to_string(outlineThickness));
            };
            if (ImGui::InputInt("Vertices", &vertices, 1, 1)) {
              write_config(configHeader, "vertices", std::to_string(vertices));
            };
            if (ImGui::InputInt("Health", &health, 1, 1)) {
              write_config(configHeader, "health", std::to_string(health));
            };
            break;
          }
          case CWeapon::FireMode::ShotSpread: {
            const std::string configHeader =
                "ShotSpread" + std::to_string(primary_power);
            float speed = read_config_f(configHeader, "speed");
            float spread = read_config_f(configHeader, "spread");
            int amount = read_config_i(configHeader, "amount");
            int fireRate = read_config_i(configHeader, "fireRate");
            int lifespan = read_config_i(configHeader, "lifespan");
            float radius[2] = {
              read_config_f(configHeader, "collisionRadius"),
              read_config_f(configHeader, "shapeRadius")
            };
            int fillColor[3] = {
              read_config_i(configHeader, "fillRed"),
              read_config_i(configHeader, "fillGreen"),
              read_config_i(configHeader, "fillBlue")
            };
            int outlineColor[3] = {
              read_config_i(configHeader, "outlineRed"),
              read_config_i(configHeader, "outlineGreen"),
              read_config_i(configHeader, "outlineBlue")
            };
            int outlineThickness =
                read_config_i(configHeader, "outlineThickness");
            int vertices = read_config_i(configHeader, "vertices");
            int health = read_config_i(configHeader, "health");            
            
            if (ImGui::InputFloat("Speed", &speed, 0.1f, 1.f)) {
              write_config(configHeader, "speed", std::to_string(speed));
            }
            if (ImGui::InputFloat("Spread", &spread, 0.1f, 1.f)) {
              write_config(configHeader, "spread", std::to_string(spread));
            }
            if (ImGui::InputInt("Amount", &amount, 1, 1)) {
              write_config(configHeader, "amount", std::to_string(amount));
            }
            if (ImGui::InputInt("Fire Rate", &fireRate, 1, 1)) {
              write_config(configHeader, "fireRate", std::to_string(fireRate));
            }
            if (ImGui::InputInt("Lifespan", &lifespan, 1, 1)) {
              write_config(configHeader, "lifespan", std::to_string(lifespan));
            }
            if (ImGui::InputFloat2("Collision/Radius", radius)) {
              write_config(configHeader, "collisionRadius", std::to_string(radius[0]));
              write_config(configHeader, "shapeRadius", std::to_string(radius[1]));
            }
            if (ImGui::InputInt3("Fill Color", fillColor)) {
              write_config(configHeader, "fillRed", std::to_string(fillColor[0]));
              write_config(configHeader, "fillGreen", std::to_string(fillColor[1]));
              write_config(configHeader, "fillBlue", std::to_string(fillColor[2]));
            }
            if (ImGui::InputInt3("Outline Color", fillColor)) {
              write_config(configHeader, "outlineRed", std::to_string(outlineColor[0]));
              write_config(configHeader, "outlineGreen", std::to_string(outlineColor[1]));
              write_config(configHeader, "outlineBlue", std::to_string(outlineColor[2]));
            }
            if (ImGui::InputInt("Outline Thickness", &outlineThickness, 1, 1)) {
              write_config(configHeader, "outlineThickness", std::to_string(outlineThickness));
            }
            if (ImGui::InputInt("Vertices", &vertices, 1, 1)) {
              write_config(configHeader, "vertices", std::to_string(vertices));
            }
            if (ImGui::InputInt("Health", &health, 1, 1)) {
              write_config(configHeader, "health", std::to_string(health));
            }
            break;
          }
          case CWeapon::FireMode::ShotLaser: {
            const std::string configHeader =
                "ShotLaser" + std::to_string(primary_power);
            int fireRate = read_config_i(configHeader, "fireRate");
            int lifespan = read_config_i(configHeader, "lifespan");
            float offset = read_config_f(configHeader, "offset");
            float length = read_config_f(configHeader, "length");
            float thickness = read_config_f(configHeader, "thickness");
            int fillColor[4] = {
              read_config_i(configHeader, "fillRed"),
              read_config_i(configHeader, "fillGreen"),
              read_config_i(configHeader, "fillBlue"),
              read_config_i(configHeader, "fillAlpha")
            };
            int outlineColor[4] {
              read_config_i(configHeader, "outlineRed"),
              read_config_i(configHeader, "outlineGreen"),
              read_config_i(configHeader, "outlineBlue"),
              read_config_i(configHeader, "outlineAlpha")
            };
            int outlineThickness =
                read_config_i(configHeader, "outlineThickness");
            
            if (ImGui::InputInt("Fire Rate", &fireRate, 1, 10)) {
              write_config(configHeader, "fireRate", std::to_string(fireRate));
            }
            if (ImGui::InputInt("Lifespan", &lifespan, 1, 10)) {
              write_config(configHeader, "lifespan", std::to_string(lifespan));
            }
            if (ImGui::InputFloat("Offset", &offset, 1.f , 10.f)) {
              write_config(configHeader, "offset", std::to_string(offset));
            }
            if (ImGui::InputFloat("Length", &length, 1.f , 10.f)) {
              write_config(configHeader, "length", std::to_string(length));
            }
            if (ImGui::InputFloat("Thickness", &thickness, 1.f , 10.f)) {
              write_config(configHeader, "thickness", std::to_string(thickness));
            }
            if (ImGui::InputInt4("Fill Color", fillColor)) {
              write_config(configHeader, "fillRed", std::to_string(fillColor[0]));
              write_config(configHeader, "fillGreen", std::to_string(fillColor[1]));
              write_config(configHeader, "fillBlue", std::to_string(fillColor[2]));
              write_config(configHeader, "fillAlpha", std::to_string(fillColor[3]));
            }
            if (ImGui::InputInt4("Outline Color", outlineColor)) {
              write_config(configHeader, "outlineRed", std::to_string(outlineColor[0]));
              write_config(configHeader, "outlineGreen", std::to_string(outlineColor[1]));
              write_config(configHeader, "outlineBlue", std::to_string(outlineColor[2]));
              write_config(configHeader, "outlineAlpha", std::to_string(outlineColor[3]));
            }
            if (ImGui::InputInt("Outline Thickness", &outlineThickness, 1, 1)) {
              write_config(configHeader, "outlineThickness", std::to_string(outlineThickness));
            }
            
            break;
          }
          }
          ImGui::EndChild();
        }
        if (ImGui::BeginChild("Secondary##weapon-secondary", {0, 0},
                              ImGuiChildFlags_FrameStyle |
                                  ImGuiChildFlags_Border |
                                  ImGuiChildFlags_AutoResizeY |
                                  ImGuiChildFlags_AutoResizeX)) {
          if (ImGui::BeginCombo("##secondary_select",
                                secondary_modes[secondary_mode].c_str())) {
            for (int i = 0; i < secondary_modes.size(); ++i) {
              const bool is_selected = (primary_mode == i);
              if (ImGui::Selectable(secondary_modes[i].c_str(), is_selected)) {
                secondary_mode = (CSpecialWeapon::FireMode)i;
              }
              if (is_selected) {
                ImGui::SetItemDefaultFocus();
              }
            }
            ImGui::EndCombo();
          }
          ImGui::SameLine();
          ImGui::RadioButton("P0", &secondary_power, 0);
          ImGui::SameLine();
          ImGui::RadioButton("P1", &secondary_power, 1);
          ImGui::SameLine();
          ImGui::RadioButton("P2", &secondary_power, 2);
          ImGui::SameLine();
          ImGui::RadioButton("P3", &secondary_power, 3);
          ImGui::SameLine();
          ImGui::RadioButton("P4", &secondary_power, 4);
          switch (secondary_mode) {
          case CSpecialWeapon::FireMode::SpecialExplosion: {
           const std::string configHeader =
                "SpecialExplosion" + std::to_string(secondary_power);
           int fireRate = read_config_i(configHeader, "fireRate");
           int lifespan = read_config_i(configHeader, "lifespan");
           int smallLifespan =
                read_config_i(configHeader, "smallLifespan");
           int speed = read_config_i(configHeader, "speed");
           int smallSpeed = read_config_i(configHeader, "smallSpeed");
           int smallAmount = read_config_i(configHeader, "smallAmount");
           float radius[2] = {
            read_config_f(configHeader, "collisionRadius"),
            read_config_f(configHeader, "shapeRadius")
           };
           int fillColor[3] = {
            read_config_i(configHeader, "fillRed"),
            read_config_i(configHeader, "fillGreen"),
            read_config_i(configHeader, "fillBlue")
           };
           int outlineColor[3] = {
            read_config_i(configHeader, "outlineRed"),
            read_config_i(configHeader, "outlineGreen"),
            read_config_i(configHeader, "outlineBlue")
           };
           int outlineThickness =
                read_config_i(configHeader, "outlineThickness");
           int vertices = read_config_i(configHeader, "vertices");
           int health = read_config_i(configHeader, "health");
           int recursion = read_config_i(configHeader, "recursion");

           if (ImGui::InputInt("Fire Rate", &fireRate, 1, 10)) {
            write_config(configHeader, "fireRate", std::to_string(fireRate));
           }
           if (ImGui::InputInt("Lifespan", &lifespan, 1, 10)) {
            write_config(configHeader, "lifespan", std::to_string(lifespan));
           }
           if (ImGui::InputInt("Particle Lifespan", &smallLifespan, 1, 10)) {
            write_config(configHeader, "smallLifespan", std::to_string(smallLifespan));
           }
           if (ImGui::InputInt("Speed", &speed, 1, 10)) {
            write_config(configHeader, "speed", std::to_string(speed));
           }
           if (ImGui::InputFloat2("Collision/Radius", radius)) {
            write_config(configHeader, "collisionRadius", std::to_string(radius[0]));
            write_config(configHeader, "shapeRadius", std::to_string(radius[1]));
           }
           if (ImGui::InputInt3("Fill Color", fillColor)) {
            write_config(configHeader, "fillRed", std::to_string(fillColor[0]));
            write_config(configHeader, "fillGreen", std::to_string(fillColor[1]));
            write_config(configHeader, "fillBlue", std::to_string(fillColor[2]));
           }
           if (ImGui::InputInt3("Outline Color", fillColor)) {
            write_config(configHeader, "outlineRed", std::to_string(outlineColor[0]));
            write_config(configHeader, "outlineGreen", std::to_string(outlineColor[1]));
            write_config(configHeader, "outlineBlue", std::to_string(outlineColor[2]));
           }
           if (ImGui::InputInt("Outline Thickness", &outlineThickness, 1, 10)) {
            write_config(configHeader, "outlineThickness", std::to_string(outlineThickness));
           } 
           if (ImGui::InputInt("Health", &health, 1, 10)) {
            write_config(configHeader, "health", std::to_string(health));
           } 
           if (ImGui::InputInt("Recursion", &recursion, 1, 10)) {
            write_config(configHeader, "recursion", std::to_string(recursion));
           } 

            break;
          }
          case CSpecialWeapon::FireMode::SpecialRotor: {
            const std::string configHeader =
                "SpecialRotor" + std::to_string(secondary_power);
            float angularSpeed =
                read_config_f(configHeader, "angularSpeed");
            int amount = read_config_i(configHeader, "amount");
            float radius = read_config_f(configHeader, "radius");
            int fireRate = read_config_i(configHeader, "fireRate");
            int lifespan = read_config_i(configHeader, "lifespan");
            float shapeRadius[2] = {
              read_config_f(configHeader, "collisionRadius"),
              read_config_f(configHeader, "shapeRadius")
            };
            int fillColor[3] = {
              read_config_i(configHeader, "fillRed"),
              read_config_i(configHeader, "fillGreen"),
              read_config_i(configHeader, "fillBlue")
            };
            int outlineColor[3] = {
              read_config_i(configHeader, "outlineRed"),
              read_config_i(configHeader, "outlineGreen"),
              read_config_i(configHeader, "outlineBlue")
            };
            int outlineThickness =
                read_config_i(configHeader, "outlineThickness");
            int vertices = read_config_i(configHeader, "vertices");
            int health = read_config_i(configHeader, "health");

            if (ImGui::InputFloat("Angular Speed", &angularSpeed, 0.1, 1)) {
              write_config(configHeader, "angularSpeed", std::to_string(angularSpeed));
            }
            if (ImGui::InputInt("Amount", &amount, 1, 1)) {
              write_config(configHeader, "amount", std::to_string(amount));
            }
            if (ImGui::InputFloat("Radius", &radius, 1.f, 10.f)) {
              write_config(configHeader, "radius", std::to_string(radius));
            }
            if (ImGui::InputInt("Fire Rate", &fireRate, 1, 10)) {
              write_config(configHeader, "fireRate", std::to_string(fireRate));
            }
            if (ImGui::InputInt("Lifespan", &lifespan, 1, 10)) {
              write_config(configHeader, "lifespan", std::to_string(lifespan));
            }
            if (ImGui::InputFloat2("Collision/Radius", shapeRadius)) {
              write_config(configHeader, "collisionRadius", std::to_string(shapeRadius[0]));
              write_config(configHeader, "shapeRadius", std::to_string(shapeRadius[1]));
            }
            if (ImGui::InputInt3("Fill Color", fillColor)) {
              write_config(configHeader, "fillRed", std::to_string(fillColor[0]));
              write_config(configHeader, "fillGreen", std::to_string(fillColor[1]));
              write_config(configHeader, "fillBlue", std::to_string(fillColor[2]));
            }
            if (ImGui::InputInt3("Outline Color", outlineColor)) {
              write_config(configHeader, "outlineRed", std::to_string(outlineColor[0]));
              write_config(configHeader, "outlineGreen", std::to_string(outlineColor[1]));
              write_config(configHeader, "outlineBlue", std::to_string(outlineColor[2]));
            }
            if (ImGui::InputInt("Outline Thickness", &outlineThickness, 1, 1)) {
              write_config(configHeader, "outlineThickness", std::to_string(outlineThickness));
            }
            if (ImGui::InputInt("Vertices", &vertices, 1, 1)) {
              write_config(configHeader, "vertices", std::to_string(vertices));
            }
            if (ImGui::InputInt("Health", &health, 1, 1)) {
              write_config(configHeader, "health", std::to_string(health));
            }

            break;
          }
          case CSpecialWeapon::FireMode::SpecialFlamethrower: {
            const std::string configHeader =
                "SpecialFlamethrower" + std::to_string(secondary_power);
            float spread = read_config_f(configHeader, "spread");
            float nozzleSpread = read_config_f(configHeader, "nozzleSpread");
            int speed = read_config_i(configHeader, "speed");
            int duration = read_config_i(configHeader, "duration");
            int fireRate = read_config_i(configHeader, "fireRate");
            float offset = read_config_f(configHeader, "offset");
            int freq = read_config_i(configHeader, "freq");
            int quantity = read_config_i(configHeader, "quantity");
            int smallLifespan =
                read_config_i(configHeader, "smallLifespan");
            float randomScale =
                read_config_f(configHeader, "randomScale");
            int fillRed[2] = {
              read_config_i(configHeader, "fillRedMin"),
              read_config_i(configHeader, "fillRedMax")
            };
            int fillGreen[2] = {
                read_config_i(configHeader, "fillGreenMin"),
                read_config_i(configHeader, "fillGreenMax")
            };
            int fillBlue = read_config_i(configHeader, "fillBlue");
            int alpha[2] = {
              read_config_i(configHeader, "alphaMin"),
              read_config_i(configHeader, "alphaMax")
            };
            float radius[2] = {
              read_config_f(configHeader, "radiusMin"),
              read_config_f(configHeader, "radiusMax")
            };
            int verts[2] = {
              read_config_i(configHeader, "vertsMin"),
              read_config_i(configHeader, "vertsMax")
            };
            int amountTypes = read_config_i(configHeader, "amountTypes");

            if (ImGui::InputFloat("Spread", &spread, 1.f, 10.f)) {
              write_config(configHeader, "spread", std::to_string(spread));
            }
            if (ImGui::InputFloat("Nozzle Spread", &nozzleSpread, 0.1f, 0.1f)) {
              write_config(configHeader, "nozzleSpread", std::to_string(nozzleSpread));
            }
            if (ImGui::InputInt("Speed", &speed, 1, 10)) {
              write_config(configHeader, "speed", std::to_string(speed));
            }
            if (ImGui::InputInt("Duration", &duration, 1, 10)) {
              write_config(configHeader, "duration", std::to_string(duration));
            }
            if (ImGui::InputFloat("Offset", &offset, 1.f, 10.f)) {
              write_config(configHeader, "offset", std::to_string(offset));
            }
            if (ImGui::InputInt("Frequency", &freq, 1, 1)) {
              write_config(configHeader, "freq", std::to_string(freq));
            }
            if (ImGui::InputInt("Quantity", &quantity, 1, 1)) {
              write_config(configHeader, "quantity", std::to_string(quantity));
            }
            if (ImGui::InputInt("Particle Lifespan", &smallLifespan, 1, 10)) {
              write_config(configHeader, "smallLifespan", std::to_string(smallLifespan));
            }
            if (ImGui::InputFloat("Random Scale", &randomScale, 0.1f, 1.0f)) {
              write_config(configHeader, "randomScale", std::to_string(randomScale));
            }
            if (ImGui::DragIntRange2("Fill Red", &fillRed[0], &fillRed[1], 1, 0, 255)) {
              write_config(configHeader, "fillRedMin", std::to_string(fillRed[0]));
              write_config(configHeader, "fillRedMax", std::to_string(fillRed[1]));
            }
            if (ImGui::DragIntRange2("Fill Green", &fillGreen[0], &fillGreen[1], 1, 0, 255)) {
              write_config(configHeader, "fillGreenMin", std::to_string(fillGreen[0]));
              write_config(configHeader, "fillGreenMax", std::to_string(fillGreen[1]));
            }
            if (ImGui::InputInt("Fill Blue", &fillBlue, 1, 10)) {
              write_config(configHeader, "fillBlue", std::to_string(fillBlue));
            }
            if (ImGui::DragIntRange2("Alpha", &alpha[0], &alpha[1], 1, 0, 255)) {
              write_config(configHeader, "alphaMin", std::to_string(alpha[0]));
              write_config(configHeader, "alphaMax", std::to_string(alpha[1]));
            }
            if (ImGui::DragFloatRange2("Radius", &radius[0], &radius[1], 1.f, 0.0f, 10.0f)) {
              write_config(configHeader, "radiusMin", std::to_string(radius[0]));
              write_config(configHeader, "radiusMax", std::to_string(radius[1]));
            }
            if (ImGui::DragIntRange2("Vertices", &verts[0], &verts[1], 1, 3, 8)) {
              write_config(configHeader, "vertsMin", std::to_string(verts[0]));
              write_config(configHeader, "vertsMax", std::to_string(verts[1]));
            }
            if (ImGui::InputInt("Amount Types", &amountTypes, 1, 1)) {
              write_config(configHeader, "amountTypes", std::to_string(amountTypes));
            }

            break;
          }
          }
          ImGui::EndChild();
        }
      }
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Configuration")) {
      static bool res_warning = false;
      static bool refresh_warning = false;
      static bool fullscreen_warning = false;
      if (res_warning || refresh_warning || fullscreen_warning) {
        ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f),
                           "Save config and restart to apply changes.");
      }
      if (ImGui::BeginChild(
              "Window##config-window", {0, 0},
              ImGuiChildFlags_FrameStyle | ImGuiChildFlags_Border |
                  ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AutoResizeX)) {
        ImGui::SeparatorText("Window");
        std::vector<std::string> resolutions {};
        for (const auto & mode : m_video_modes) {
          resolutions.push_back(std::to_string(mode.width) + "x" + std::to_string(mode.height) + " (" + std::to_string(mode.bitsPerPixel) + ")");
        }
        const std::string current_refresh =
            m_config["Window"]["refreshRate"];
        std::vector<std::string> refresh_rates{current_refresh, "144", "120",
                                               "60", "30"};
        const bool current_fullscreen =
            (bool)read_config_i("Window", "fullscreen");
        static int res_idx = 0;
        static int refresh_idx = 0;
        static bool fullscreen = current_fullscreen;
        const std::string current_res = 
          read_config_s("Window", "width") + "x" +
          read_config_s("Window", "height") + " (" +
          read_config_s("Window", "depth") + ")";
        auto iterator = find(resolutions.begin(), resolutions.end(), current_res);
        if (iterator != resolutions.end()) {
          res_idx = iterator - resolutions.begin();
        }
        ImGui::Text("Resolution");
        if (ImGui::BeginCombo("##res_select", resolutions[res_idx].c_str())) {
          for (int i = 0; i < resolutions.size(); ++i) {
            const bool is_selected = (res_idx == i);
            ImGui::PushID(i);
            if (ImGui::Selectable(resolutions[i].c_str(), is_selected)) {
              res_idx = i;
              // if (i != 0) {
              //   res_warning = true;
              // } else {
              //   res_warning = false;
              // }
              write_config("Window", "width", std::to_string(m_video_modes[res_idx].width));
              write_config("Window", "height", std::to_string(m_video_modes[res_idx].height));
              write_config("Window", "depth", std::to_string(m_video_modes[res_idx].bitsPerPixel));
            }
            if (is_selected) {
              ImGui::SetItemDefaultFocus();
            }
            ImGui::PopID();
          }
          ImGui::EndCombo();
        }
        ImGui::Text("Refresh");
        if (ImGui::BeginCombo("##refresh_select",
                              refresh_rates[refresh_idx].c_str())) {
          for (int i = 0; i < refresh_rates.size(); ++i) {
            const bool is_selected = (refresh_idx == i);
            if (ImGui::Selectable(refresh_rates[i].c_str(), is_selected)) {
              refresh_idx = i;
              // if (i != 0) {
              //   refresh_warning = true;
              // } else {
              //   refresh_warning = false;
              // }
              write_config("Window", "refreshRate", refresh_rates[refresh_idx]);
            }
            if (is_selected) {
              ImGui::SetItemDefaultFocus();
            }
          }
          ImGui::EndCombo();
        }
        ImGui::SameLine();
        if (ImGui::Checkbox("fullscreen", &fullscreen)) {
          if (fullscreen != current_fullscreen) {
            write_config("Window", "fullscreen", std::to_string((int)fullscreen));
            // fullscreen_warning = true;
          } else {
            // fullscreen_warning = false;
          }
        }
        ImGui::EndChild();
      }

      ImGui::SameLine();

      if (ImGui::BeginChild(
              "Font##config-font", {0, 0},
              ImGuiChildFlags_FrameStyle | ImGuiChildFlags_Border |
                  ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AutoResizeX)) {
        ImGui::SeparatorText("Font");
        static int font_s_idx = 0;
        std::vector<std::string> font_sizes{"16", "24", "32",
                                            "48",           "60", "72"};
        ImGui::Text("Size");
        if (ImGui::BeginCombo("##font_size_select",
                              font_sizes[font_s_idx].c_str())) {
          for (int i = 0; i < font_sizes.size(); ++i) {
            const bool is_selected = (font_s_idx == i);
            if (ImGui::Selectable(font_sizes[i].c_str(), is_selected)) {
              font_s_idx = i;
            }
            if (is_selected) {
              ImGui::SetItemDefaultFocus();
            }
          }
          ImGui::EndCombo();
        }

        ImGui::EndChild();
      }

      if (ImGui::BeginChild(
              "Player##config-player", {700, 0},
              ImGuiChildFlags_FrameStyle | ImGuiChildFlags_Border |
                  ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AutoResizeX)) {
        ImGui::Text("Player");
        ImGui::EndChild();
      }

      if (ImGui::BeginChild(
              "Enemies##config-enemies", {700, 0},
              ImGuiChildFlags_FrameStyle | ImGuiChildFlags_Border |
                  ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AutoResizeX)) {
        ImGui::Text("Enemies");
        ImGui::EndChild();
      }

      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Vectors")) {
      static char bufx[32] = "";
      ImGui::InputText("x", bufx, 32, ImGuiInputTextFlags_CharsDecimal);
      static char bufy[32] = "";
      ImGui::InputText("y", bufy, 32, ImGuiInputTextFlags_CharsDecimal);

      if (ImGui::Button("Add")) {
        vectors.push_back(Vec2(atof(bufx), atof(bufy)));
        strncpy(bufx, "", 32);
        strncpy(bufy, "", 32);
      }

      static int l_idx = -1;
      static int r_idx = -1;
      ImGui::BeginChild("left", {200.f, 0.f}, true);
      for (int i = 0; i < vectors.size(); ++i) {
        const bool is_selected = (l_idx == i);
        char buf[32];
        sprintf(buf, "Vector %d (%.3f, %.3f)##left", i, vectors[i].x,
                vectors[i].y);
        if (ImGui::Selectable(buf, is_selected)) {
          l_idx = i;
        }
      }
      ImGui::EndChild();

      ImGui::SameLine();

      ImGui::BeginChild("right", {200.f, 0.f}, true);
      for (int i = 0; i < vectors.size(); ++i) {
        const bool is_selected = (r_idx == i);
        char buf[32];
        sprintf(buf, "Vector %d (%.3f, %.3f)##right", i, vectors[i].x,
                vectors[i].y);
        if (ImGui::Selectable(buf, is_selected)) {
          r_idx = i;
        }
      }
      ImGui::EndChild();

      ImGui::SameLine();

      ImGui::BeginChild("Calculations", {400.f, 0.f}, true);
      if (ImGui::BeginTable("Calc results", 1)) {
        if (l_idx >= 0 && r_idx >= 0) {
          ImGui::Text("After");
          static float mult = 1;
          static float rot_angle = 0;
          ImGui::TableNextColumn();
          ImGui::InputFloat("multiplier", &mult, 0.1f);
          ImGui::TableNextColumn();
          ImGui::InputFloat("rotation", &rot_angle, 1.f);
          const float length_l = vectors[l_idx].length();
          const float length_r = vectors[r_idx].length();
          const float dist = vectors[l_idx].distance_to(vectors[r_idx]);
          const Vec2 sum = vectors[l_idx] + vectors[r_idx];
          const Vec2 mul_l = vectors[l_idx] * mult;
          const Vec2 mul_r = vectors[r_idx] * mult;
          const float angle_to_rad =
              vectors[l_idx].angle_to_rad(vectors[r_idx]);
          const float angle_to_deg =
              vectors[l_idx].angle_to_deg(vectors[r_idx]);
          Vec2 rot_l = vectors[l_idx].clone();
          rot_l.rotate_deg(rot_angle);
          Vec2 rot_r = vectors[r_idx].clone();
          rot_r.rotate_deg(rot_angle);
          Vec2 norm_l = vectors[l_idx].clone();
          norm_l.normalize();
          Vec2 norm_r = vectors[r_idx].clone();
          norm_r.normalize();

          ImGui::TableNextColumn();
          ImGui::Text("Length: %.3f, %.3f", length_l, length_r);
          ImGui::TableNextColumn();
          ImGui::Text("Normalized: (%.3f, %.3f), (%.3f, %.3f)", norm_l.x,
                      norm_l.y, norm_r.x, norm_r.y);
          ImGui::TableNextColumn();
          ImGui::Text("Distance: %.3f", dist);
          ImGui::TableNextColumn();
          ImGui::Text("Angle: %.3f (%.3f deg.)", angle_to_rad, angle_to_deg);
          ImGui::TableNextColumn();
          ImGui::Text("Sum: (%.3f, %.3f)", sum.x, sum.y);
          ImGui::TableNextColumn();
          ImGui::Text("Multiple: (%.3f, %.3f), (%.3f, %.3f)", mul_l.x, mul_l.y,
                      mul_r.x, mul_r.y);
          ImGui::TableNextColumn();
          ImGui::Text("Rotated: (%.3f, %.3f), (%.3f, %.3f)", rot_l.x, rot_l.y,
                      rot_r.x, rot_r.y);
        }
        ImGui::EndTable();
      }
      ImGui::EndChild();

      ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
  }
  ImGui::End();
}
