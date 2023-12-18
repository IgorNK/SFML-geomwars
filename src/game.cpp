#include "game.h"
#include "vec2.h"
#include <ctime>
#include <cstdlib>

void Game::run() {    
    
    while (m_window.isOpen()) {
        // create/cleanup entities
        m_entity_manager.update();

        sf::Time deltaTime = m_delta_clock.restart();

        // update call required by ImGui
        ImGui::SFML::Update(m_window, deltaTime);

        sUserInput();
        if (!m_paused) {
            sMovement(deltaTime);
            sCollision();
            sEnemySpawner(deltaTime);
        }
        sGUI();
        //Rendering should be last
        sRender(deltaTime);
    }
    shutdown();
}

void Game::setPaused(const bool paused) {
    m_paused = paused;
}

Game::Game(const std::string configfile) {
    init(configfile);
}

void Game::init(const std::string configfile) {
    std::srand(std::time(nullptr));
    m_config = Configuration::read_file(configfile);
    const size_t width = std::stoi(m_config["Window"]["width"]);
    const size_t height = std::stoi(m_config["Window"]["height"]);
    const size_t framerate = std::stoi(m_config["Window"]["refreshRate"]);
    m_spawn_interval = std::stof(m_config["Enemy"]["spawnInterval"]);
    m_window.create(sf::VideoMode(width, height), "ImGUI + SFML = <3");
    m_window.setFramerateLimit(framerate);
    ImGui::SFML::Init(m_window);
    spawn_world();
    test_spawn();
}

void Game::shutdown() {
    ImGui::SFML::Shutdown();
}

void Game::spawn_world() {
    const std::shared_ptr<Entity> wb = m_entity_manager.add_entity(Tag::WorldBounds);
    wb->rect = std::make_shared<CRect>(CRect(1920, 1080));
}

void Game::spawn_player() {

}

void Game::spawn_enemy() {
    for (const std::shared_ptr<Entity> wb : m_entity_manager.get_entities(Tag::WorldBounds)) {
        const sf::FloatRect w_bounds = wb->rect->rect;
        const float border = wb->rect->border;
        const sf::FloatRect spawn_bounds = sf::FloatRect(w_bounds.left + border, w_bounds.top + border, w_bounds.width - border, w_bounds.height - border);   
        const std::shared_ptr<Entity> enemy = m_entity_manager.add_entity(Tag::Enemies);
        setup_random_enemy(*enemy.get(), spawn_bounds);
    }
}

void Game::setup_random_enemy(const Entity & enemy, sf::FloatRect spawn_bounds) {    
    const float shapeRadius = std::stof(m_config["Enemy"]["shapeRadius"]);
    const float collisionRadius = std::stof(m_config["Enemy"]["collisionRadius"]);
    const float speedMin = std::stof(m_config["Enemy"]["speedMin"]);
    const float speedMax = std::stof(m_config["Enemy"]["speedMax"]);
    const int outlineRed = std::stoi(m_config["Enemy"]["outlineRed"]);
    const int outlineGreen = std::stoi(m_config["Enemy"]["outlineGreen"]);
    const int outlineBlue = std::stoi(m_config["Enemy"]["outlineBlue"]);
    const int outlineThickness = std::stoi(m_config["Enemy"]["outlineThickness"]);
    const int verticesMin = std::stoi(m_config["Enemy"]["verticesMin"]);
    const int verticesMax = std::stoi(m_config["Enemy"]["verticesMax"]);

    const float rand_x = (float)(std::rand()) / (float)(RAND_MAX) * spawn_bounds.width + spawn_bounds.left;
    const float rand_y = (float)(std::rand()) / (float)(RAND_MAX) * spawn_bounds.height + spawn_bounds.top;
    const float rand_speed = (float)(std::rand()) / (float)(RAND_MAX) * (speedMax - speedMin) + speedMin;
    const float rand_angle = (float)(std::rand()) / (float)(RAND_MAX) * 360;
    const Vec2 rand_velocity = Vec2(0, 1).rotate_deg(rand_angle) * rand_speed;
    const int rand_vertices = (float)(std::rand()) / (float)(RAND_MAX) * (verticesMax - verticesMin) + verticesMin;

    enemy.name = std::make_shared<CName>(CName("Enemy"));
    enemy.transform = std::make_shared<CTransform>(CTransform(rand_x, rand_y));
    enemy.shape = std::make_shared<CShape>(CShape(shapeRadius, rand_vertices, sf::Color(0, 0, 0), sf::Color(outlineRed, outlineGreen, outlineBlue), outlineThickness));
    enemy.collider = std::make_shared<CCollier>(CCollier(collisionRadius));
    enemy.velocity = std::make_shared<CVelocity>(CVelocity(rand_velocity));
}

void Game::spawnSmallEnemies() {

}

void Game::shoot() {
    for (const std::shared_ptr<Entity> player : m_entity_manager.get_entities(Tag::Player)) {
        if (player->weapon && player->transform) {
            const Vec2 position = player->transform->position.clone();
            const float rotation = player->transform->rotation;
            const Vec2 velocity = Vec2(1, 0).rotate_rad(rotation) * player->weapon->speed;
            const std::shared_ptr<Entity> bullet = m_entity_manager.add_entity(Tag::Bullets);
            bullet->transform = std::make_shared<CTransform>(CTransform(position, rotation));
            bullet->velocity = std::make_shared<CVelocity>(CVelocity(velocity));
        }
    }
}

void Game::shootSecialWeapon() {

}

void Game::test_spawn() {
    const std::shared_ptr<Entity> enemy = m_entity_manager.add_entity(Tag::Enemies);
    enemy->name = std::make_shared<CName>(CName());
    enemy->transform = std::make_shared<CTransform>(CTransform());
    enemy->velocity = std::make_shared<CVelocity>(CVelocity());
    enemy->shape = std::make_shared<CShape>(CShape());
    enemy->transform->position = Vec2(50, 50);
    enemy->velocity->velocity = Vec2(5.4, 2.7);
    enemy->shape->shape.setFillColor(sf::Color(0, 255, 255));
}

void Game::sUserInput() {
    sf::Event event;
    while (m_window.pollEvent(event)) {
        ImGui::SFML::ProcessEvent(event);
        if (event.type == sf::Event::Closed) {
            m_window.close();
        }
    }
}

void Game::sRender(const sf::Time deltaTime) {
    m_window.clear();
    // Render stuff
    for (std::shared_ptr<Entity> entity : m_entity_manager.get_entities()) {
        if (entity->shape) {
            if (entity->transform) {
                const Vec2 pos = entity->transform->position;
                entity->shape->shape.setPosition(sf::Vector2f(pos.x, pos.y));
            }
            m_window.draw(entity->shape->shape);
        }
    }
    ImGui::SFML::Render(m_window);
    m_window.display();
}

void Game::sMovement(const sf::Time deltaTime) {
    for (const std::shared_ptr<Entity> wb : m_entity_manager.get_entities(Tag::WorldBounds)) {
        if (wb->rect) {
            const sf::FloatRect w_bounds = wb->rect->rect;
            for (const std::shared_ptr<Entity> enemy : m_entity_manager.get_entities(Tag::Enemies)) {
                if (enemy->velocity && enemy->shape) {
                    const sf::FloatRect s_bounds = enemy->shape->shape.getGlobalBounds();
                    Vec2 & vel = enemy->velocity->velocity;
                    if (s_bounds.left <= w_bounds.left || s_bounds.left + s_bounds.width >= w_bounds.left + w_bounds.width) {
                        vel.x = -vel.x;
                    }
                    if (s_bounds.top <= w_bounds.top || s_bounds.top + s_bounds.height >= w_bounds.top + w_bounds.height) {
                        vel.y = -vel.y;
                    }
                }
            }
        }
    }
    
    for (std::shared_ptr<Entity> entity : m_entity_manager.get_entities()) {
        if (entity->transform && entity->velocity) {
            Vec2 & pos = entity->transform->position;
            Vec2 & vel = entity->velocity->velocity;
            pos += vel;
        }
    }
}

void Game::sCollision() {

}

void Game::sEnemySpawner(const sf::Time deltaTime) {
    static float timer = m_spawn_interval;
}

void Game::sGUI() {
    //Interface stuff
    ImGui::Begin("Geometry Wars");
    std::vector<Vec2> vectors {};
    
    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    if (ImGui::BeginTabBar("TabBar", tab_bar_flags)) {
        if (ImGui::BeginTabItem("Vectors")) {
            static char bufx[32] = ""; ImGui::InputText("x", bufx, 32, ImGuiInputTextFlags_CharsDecimal);
            static char bufy[32] = ""; ImGui::InputText("y", bufy, 32, ImGuiInputTextFlags_CharsDecimal);

            if (ImGui::Button("Add")) {
                vectors.push_back(Vec2(atof(bufx), atof(bufy)));
                strncpy(bufx, "", 32);
                strncpy(bufy, "", 32);
            }

            static int l_idx = -1;
            static int r_idx = -1;
            ImGui::BeginChild("left", {200.f, 0.f}, true);
            for (size_t i = 0; i < vectors.size(); ++i) {
                const bool is_selected = (l_idx == i);
                char buf[32];
                sprintf(buf, "Vector %d (%.3f, %.3f)##left", i, vectors[i].x, vectors[i].y);
                if (ImGui::Selectable(buf, is_selected)) {
                    l_idx = i;
                }
            }
            ImGui::EndChild();

            ImGui::SameLine();
            
            ImGui::BeginChild("right", {200.f, 0.f}, true);
            for (size_t i = 0; i < vectors.size(); ++i) {
                const bool is_selected = (r_idx == i);
                char buf[32];
                sprintf(buf, "Vector %d (%.3f, %.3f)##right", i, vectors[i].x, vectors[i].y);
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
                    ImGui::TableNextColumn(); ImGui::InputFloat("multiplier", &mult, 0.1f);
                    ImGui::TableNextColumn(); ImGui::InputFloat("rotation", &rot_angle, 1.f);
                    const float length_l = vectors[l_idx].length();
                    const float length_r = vectors[r_idx].length();
                    const float dist = vectors[l_idx].distance_to(vectors[r_idx]);
                    const Vec2 sum = vectors[l_idx] + vectors[r_idx];
                    const Vec2 mul_l = vectors[l_idx] * mult;
                    const Vec2 mul_r = vectors[r_idx] * mult;
                    const float angle_to_rad = vectors[l_idx].angle_to_rad(vectors[r_idx]);
                    const float angle_to_deg = vectors[l_idx].angle_to_deg(vectors[r_idx]);
                    Vec2 rot_l = vectors[l_idx].clone();
                    rot_l.rotate_deg(rot_angle);
                    Vec2 rot_r = vectors[r_idx].clone();
                    rot_r.rotate_deg(rot_angle);
                    Vec2 norm_l = vectors[l_idx].clone();
                    norm_l.normalize();
                    Vec2 norm_r = vectors[r_idx].clone();
                    norm_r.normalize();

                    ImGui::TableNextColumn(); ImGui::Text("Length: %.3f, %.3f", length_l, length_r);
                    ImGui::TableNextColumn(); ImGui::Text("Normalized: (%.3f, %.3f), (%.3f, %.3f)", norm_l.x, norm_l.y, norm_r.x, norm_r.y);
                    ImGui::TableNextColumn(); ImGui::Text("Distance: %.3f", dist);
                    ImGui::TableNextColumn(); ImGui::Text("Angle: %.3f (%.3f deg.)", angle_to_rad, angle_to_deg);
                    ImGui::TableNextColumn(); ImGui::Text("Sum: (%.3f, %.3f)", sum.x, sum.y);
                    ImGui::TableNextColumn(); ImGui::Text("Multiple: (%.3f, %.3f), (%.3f, %.3f)", mul_l.x, mul_l.y, mul_r.x, mul_r.y);
                    ImGui::TableNextColumn(); ImGui::Text("Rotated: (%.3f, %.3f), (%.3f, %.3f)", rot_l.x, rot_l.y, rot_r.x, rot_r.y);
                }
                ImGui::EndTable();
            }       
            ImGui::EndChild();

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Entities")) {
            ImGui::BeginGroup();
            // Entity creation form
            std::vector<std::string> tags;
            for (auto const & v : tag_names) {
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

            ImGui::BeginChild("Entity list", {200.f, 0}, true);
            static int entity_idx = -1;
            if (ImGui::BeginTable("Entity table", 1)) {
                const Entities & entities = m_entity_manager.get_entities();
                for (int i = 0; i < entities.size(); ++i) {
                    const bool is_selected = (entity_idx == i);
                    const std::shared_ptr<Entity> entity = entities[i];
                    const std::string tag_name = name_tags[entity->tag()];
                    const size_t entity_id = entity->id();
                    ImGui::TableNextColumn();
                    char bufdel[32];
                    sprintf(bufdel, "D ##del%d", i);
                    if (ImGui::Button(bufdel)) {
                        entity->destroy();
                    }
                    ImGui::SameLine();
                    char bufname[32];
                    sprintf(bufname, "%s (%d)##entity%d", tag_name.c_str(), entity_id, entity_id);
                    if (ImGui::Selectable(bufname, is_selected)) {
                        entity_idx = i;
                    }
                    if (is_selected) {
                        ImGui::SetItemDefaultFocus();
                    }

                }
                ImGui::EndTable();
            }
            ImGui::EndChild();

            ImGui::SameLine();

            ImGui::BeginChild("Components", {400.f, 0}, true);
            static int comp_idx = -1;
            if (entity_idx >= 0) {
                const std::shared_ptr<Entity> entity = m_entity_manager.get_entities()[entity_idx];
                if (entity->name) {
                    char buf[32];
                    sprintf(buf, "Name: %s##comp_name", entity->name->name.c_str());
                    if (ImGui::Selectable(buf, comp_idx == ComponentType::Name)) {
                        comp_idx = ComponentType::Name;
                    }
                } else {
                    if (ImGui::Button("+Name##add_name")) {
                        entity->name = std::make_shared<CName>(CName());
                    }
                }
                if (entity->transform) {
                    const Vec2 pos = entity->transform->position;
                    const float rot = entity->transform->rotation;
                    const float scale = entity->transform->scale;
                    char buf[32];
                    sprintf(buf, "Transform: P(%.3f, %.3f), R%.3f, S%.3f##comp_transform", pos.x, pos.y, rot, scale);
                    if (ImGui::Selectable(buf, comp_idx == ComponentType::Transform)) {
                        comp_idx = ComponentType::Transform;
                    }
                } else {
                    if (ImGui::Button("+Transform##add_transform")) {
                        const std::shared_ptr<CTransform> transform(new CTransform());
                        entity->transform = transform;
                    }

                }
                if (entity->velocity) {
                    const Vec2 vel = entity->velocity->velocity;
                    char buf[32];
                    sprintf(buf, "Velocity: (%.3f, %.3f)##comp_velocity", vel.x, vel.y);
                    if (ImGui::Selectable(buf, comp_idx == ComponentType::Velocity)) {
                        comp_idx = ComponentType::Velocity;
                    }
                } else {
                    if (ImGui::Button("+Velocity##add_velocity")) {
                        entity->velocity = std::make_shared<CVelocity>(CVelocity());
                    }
                }
            }
            ImGui::EndChild();
            
            // ImGui::SameLine();

            // ImGui::BeginChild("Component add", {200.f, 0});
            // if (entity_idx >= 0) {
            //     std::vector<std::string> comps;
            //     for (auto const & v : name_components) {
            //         comps.push_back(v.first);
            //     }
            //     static int add_comp_idx = 0;
            //     const std::string comp_preview = comps[add_comp_idx];
            //     if (ImGui::BeginCombo("##comp_select", comp_preview.c_str())) {
            //     for (int i = 0; i < comps.size(); ++i) {
            //             const bool is_selected = (add_comp_idx == i);
            //             if (ImGui::Selectable(comps[i].c_str(), is_selected)) {
            //                 add_comp_idx = i;
            //             }
            //             if (is_selected) {
            //                 ImGui::SetItemDefaultFocus();
            //             }
            //     }
            //     ImGui::EndCombo();
            //     }
            //     if (ImGui::Button("add##add_component")) {
            //         const std::shared_ptr<Entity> entity = m_entity_manager.get_entities()[entity_idx];
            //         const ComponentType type = name_components[comps[add_comp_idx]];
            //         switch (type) {
            //             case ComponentType::Transform: {
            //                 entity->transform = std::make_shared<CTransform>(CTransform());
            //                 break;
            //             }
            //             case ComponentType::Velocity: {
            //                 entity->velocity = std::make_shared<CVelocity>(CVelocity());
            //                 break;
            //             }
            //             case ComponentType::Name: {
            //                 entity->name = std::make_shared<CName>(CName());
            //                 break;
            //             }
            //             default: {}
            //         }
            //     }

            // }
            
            // ImGui::EndChild();

            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();

}