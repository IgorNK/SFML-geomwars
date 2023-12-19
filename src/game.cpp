#include "game.h"
#include "vec2.h"
#include <ctime>
#include <cstdlib>
#include <fstream>



void Game::run() {    
    
    while (m_window.isOpen()) {
        // create/cleanup entities
        m_entity_manager.update();

        sf::Time deltaTime = m_delta_clock.restart();

        // update call required by ImGui
        ImGui::SFML::Update(m_window, deltaTime);

        sUserInput();
        if (!m_paused) {
            sInputHandling();
            sPlayerInvincibility(deltaTime);
            sLifespan(deltaTime);
            sDamageReact(deltaTime);
            sMovement(deltaTime);
            sCollision();
            sPlayerSpawner(deltaTime);
            sEnemySpawner(deltaTime);
        }
        sGUI();
        //Rendering should be last
        sRender(deltaTime);
        m_frameCount++;
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
    m_config = read_file(configfile);
    const size_t width = std::stoi(m_config["Window"]["width"]);
    const size_t height = std::stoi(m_config["Window"]["height"]);
    const size_t framerate = std::stoi(m_config["Window"]["refreshRate"]);
    m_enemy_spawn_interval = std::stoi(m_config["Enemy"]["spawnInterval"]);
    m_player_spawn_interval = std::stoi(m_config["Player"]["spawnInterval"]);
    m_window.create(sf::VideoMode(width, height), "ImGUI + SFML = <3");
    m_window.setFramerateLimit(framerate);
    ImGui::SFML::Init(m_window);
    spawn_world();
}

void Game::shutdown() {
    ImGui::SFML::Shutdown();
}

void Game::spawn_world() {
    const std::shared_ptr<Entity> wb = m_entity_manager.add_entity(Tag::WorldBounds);
    wb->rect = std::make_shared<CRect>(CRect(1920, 1080));
}

void Game::spawn_player() {
    for (const std::shared_ptr<Entity> wb : m_entity_manager.get_entities(Tag::WorldBounds)) {
        const sf::FloatRect & w_bounds = wb->rect->rect;
        const Vec2 center = Vec2(w_bounds.left + w_bounds.width / 2, w_bounds.top + w_bounds.height / 2);
        const std::shared_ptr<Entity> player = m_entity_manager.add_entity(Tag::Player);
        setup_player(*player.get(), center);
    }
}

void Game::setup_player(Entity & player, const Vec2 & position) {
// Bullet [
//     shapeRadius=10,
//     collisionRadius=10,
//     speed=20,
//     fillRed=255,
//     fillGreen=255,
//     fillBlue=255,
//     outlineRed=255,
//     outlineGreen=255,
//     outlineBlue=255,
//     outlineThickness=2,
//     vertices=20,
//     lifespan=90,
// ]
    
    const float shapeRadius = std::stof(m_config["Player"]["shapeRadius"]);
    const float collisionRadius = std::stof(m_config["Player"]["collisionRadius"]);
    const float speed = std::stof(m_config["Player"]["speed"]);
    const int fireRate = std::stoi(m_config["Player"]["fireRate"]);
    const int specialFireRate = std::stoi(m_config["Player"]["specialFireRate"]);
    const int invincibilityDuration = std::stoi(m_config["Player"]["invincibilityDuration"]);
    const int flickerRate = std::stoi(m_config["Player"]["flickerRate"]);
    const int fillRed = std::stoi(m_config["Player"]["fillRed"]);
    const int fillGreen = std::stoi(m_config["Player"]["fillGreen"]);
    const int fillBlue = std::stoi(m_config["Player"]["fillBlue"]);
    const int outlineRed = std::stoi(m_config["Player"]["outlineRed"]);
    const int outlineGreen = std::stoi(m_config["Player"]["outlineGreen"]);
    const int outlineBlue = std::stoi(m_config["Player"]["outlineBlue"]);
    const int outlineThickness = std::stoi(m_config["Player"]["outlineThickness"]);
    const int vertices = std::stoi(m_config["Player"]["vertices"]);
    const int health = std::stoi(m_config["Player"]["health"]);

    const float bRadius = std::stof(m_config["Bullet"]["shapeRadius"]);
    const float bCollision = std::stof(m_config["Bullet"]["collisionRadius"]);
    const float bSpeed = std::stof(m_config["Bullet"]["speed"]) ;
    const int bFillRed = std::stoi(m_config["Bullet"]["fillRed"]);
    const int bFillGreen = std::stoi(m_config["Bullet"]["fillGreen"]);
    const int bFillBlue = std::stoi(m_config["Bullet"]["fillBlue"]);
    const int bOutlineRed = std::stoi(m_config["Bullet"]["outlineRed"]);
    const int bOutlineGreen = std::stoi(m_config["Bullet"]["outlineGreen"]);
    const int bOutlineBlue = std::stoi(m_config["Bullet"]["outlineBlue"]);
    const int bOutlineThickness = std::stoi(m_config["Bullet"]["outlineThickness"]);
    const int bVertices = std::stoi(m_config["Bullet"]["vertices"]);
    const int bLifespan = std::stoi(m_config["Bullet"]["lifespan"]);

    player.name = std::make_shared<CName>(CName("Player"));
    player.player = std::make_shared<CPlayerStats>(CPlayerStats(3, speed, fireRate, specialFireRate, invincibilityDuration, flickerRate));
    player.transform = std::make_shared<CTransform>(CTransform(position));
    player.velocity = std::make_shared<CVelocity>(CVelocity());
    player.weapon = std::make_shared<CWeapon>(CWeapon(bSpeed, bLifespan, CShape(bRadius, bVertices, sf::Color(bFillRed, bFillGreen, bFillBlue), sf::Color(bOutlineRed, bOutlineGreen, bOutlineBlue), bOutlineThickness)));
    player.special_weapon = std::make_shared<CSpecialWeapon>(CSpecialWeapon());
    player.shape = std::make_shared<CShape>(CShape(shapeRadius, vertices, sf::Color(fillRed, fillGreen, fillBlue), sf::Color(outlineRed, outlineGreen, outlineBlue), outlineThickness));
    player.collider = std::make_shared<CCollider>(CCollider(collisionRadius));
    player.input = std::make_shared<CInput>(CInput());
    player.health = std::make_shared<CHealth>(CHealth(health));
}

void Game::spawn_enemy() {
    for (const std::shared_ptr<Entity> wb : m_entity_manager.get_entities(Tag::WorldBounds)) {
        const sf::FloatRect & w_bounds = wb->rect->rect;
        const float border = wb->rect->border;
        const sf::FloatRect spawn_bounds = sf::FloatRect(w_bounds.left + border, w_bounds.top + border, w_bounds.width - border, w_bounds.height - border);   
        const std::shared_ptr<Entity> enemy = m_entity_manager.add_entity(Tag::Enemies);
        setup_random_enemy(*enemy.get(), spawn_bounds);
    }
}

void Game::setup_random_enemy(Entity & enemy, sf::FloatRect spawn_bounds) {    
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

    const float smallRadius = std::stof(m_config["Enemy"]["smallRadius"]);
    const float smallCollisionRadius = std::stof(m_config["Enemy"]["smallCollisionRadius"]);
    const float smallSpeed = std::stof(m_config["Enemy"]["smallSpeed"]);
    const int lifespan = std::stoi(m_config["Enemy"]["lifespan"]);

    const CShape small_prefab = CShape(smallRadius, rand_vertices, sf::Color(0, 0, 0),  sf::Color(outlineRed, outlineGreen, outlineBlue), outlineThickness);

    enemy.name = std::make_shared<CName>(CName("Enemy"));
    enemy.transform = std::make_shared<CTransform>(CTransform(rand_x, rand_y));
    enemy.shape = std::make_shared<CShape>(CShape(shapeRadius, rand_vertices, sf::Color(0, 0, 0), sf::Color(outlineRed, outlineGreen, outlineBlue), outlineThickness));
    enemy.collider = std::make_shared<CCollider>(CCollider(collisionRadius));
    enemy.velocity = std::make_shared<CVelocity>(CVelocity(rand_velocity));
    enemy.health = std::make_shared<CHealth>(CHealth(rand_vertices));
    enemy.spawner = std::make_shared<CDeathSpawner>(CDeathSpawner(rand_vertices, small_prefab, lifespan, smallSpeed, Tag::Enemies));
}

void Game::shoot() {
    for (const std::shared_ptr<Entity> player : m_entity_manager.get_entities(Tag::Player)) {
        if (player->weapon && player->transform) {
            const Vec2 position = player->transform->position.clone();
            const float rotation = player->transform->rotation;
            const Vec2 velocity = Vec2::forward().rotate_rad(rotation).normalize() * player->weapon->speed;
            const CShape & bullet_prefab = player->weapon->bullet;
            const float collision_radius = bullet_prefab.shape.getRadius();
            const float lifespan = player->weapon->lifespan;
            const std::shared_ptr<Entity> bullet = m_entity_manager.add_entity(Tag::Bullets);
            bullet->transform = std::make_shared<CTransform>(CTransform(position, rotation));
            bullet->velocity = std::make_shared<CVelocity>(CVelocity(velocity));
            bullet->shape = std::make_shared<CShape>(CShape(bullet_prefab));
            bullet->collider = std::make_shared<CCollider>(CCollider(collision_radius));
            bullet->name = std::make_shared<CName>(CName("Bullet"));
            bullet->lifespan = std::make_shared<CLifespan>(CLifespan(lifespan));
            bullet->health = std::make_shared<CHealth>(CHealth());
        }
    }
}

void Game::shootSpecialWeapon() {

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

void Game::sLifespan(const sf::Time deltaTime) {
    for (const std::shared_ptr<Entity> entity : m_entity_manager.get_entities()) {
        if (entity->lifespan) {
            float & countdown = entity->lifespan->countdown;
            const float duration = entity->lifespan->duration;
            if (countdown <= 0) {
                entity->destroy();
            }
            if (entity->shape) {
                float rate = countdown / duration * 255;
                sf::CircleShape & shape = entity->shape->shape;
                const sf::Color color = shape.getFillColor();
                const sf::Color outline = shape.getOutlineColor();
                shape.setFillColor(sf::Color(color.r, color.g, color.b, rate));
                shape.setOutlineColor(sf::Color(outline.r, outline.g, outline.b, rate));
            }
            --countdown;
        }
    }
}

void Game::sPlayerInvincibility(const sf::Time deltaTime) {
    for (const std::shared_ptr<Entity> player : m_entity_manager.get_entities(Tag::Player)) {
        if (player->player) {
            CPlayerStats & p_stats = *player->player.get();
            const int freq = p_stats.flicker_frequency;
            if (p_stats.invincibility_countdown > 0) {
                --p_stats.invincibility_countdown;
                if (player->shape) {
                    sf::CircleShape & shape = player->shape->shape;
                    const sf::Color color = shape.getFillColor();
                    const sf::Color outline = shape.getOutlineColor();
                    int alpha = outline.a;
                    if (p_stats.invincibility_countdown % freq == 0) {
                        if (alpha > 0) {
                            alpha = 0;
                        } else {
                            alpha = 255;
                        }
                    };
                    shape.setFillColor(sf::Color(color.r, color.g, color.b, alpha));
                    shape.setOutlineColor(sf::Color(outline.r, outline.g, outline.b, alpha));
                }
            }
        }
    }
}

void Game::sUserInput() {
    sf::Event event;
    std::shared_ptr<Entity> player;
    for (const std::shared_ptr<Entity> p : m_entity_manager.get_entities(Tag::Player)) {
        if (p->input) {
            player = std::move(p);
        }
    }

    if (player) {
        player->input->axis = Vec2(0, 0);
        player->input->fire = false;
        player->input->secondaryFire = false;
    }

    Vec2 move_axis {0, 0};
    bool fire_input = false;
    bool secondary_fire_input = false;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        move_axis.y = -1;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        move_axis.y = 1;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        move_axis.x = -1;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        move_axis.x = 1;
    }
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        if (!ImGui::GetIO().WantCaptureMouse && player) {
            fire_input = true;
        }
    }
    if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
        if (!ImGui::GetIO().WantCaptureMouse && player) {
            secondary_fire_input = true;
        }
    }

    if (player) {
        player->input->axis = move_axis;
        player->input->fire = fire_input;
        player->input->secondaryFire = secondary_fire_input;
    }

    while (m_window.pollEvent(event)) {
        ImGui::SFML::ProcessEvent(event);
        if (event.type == sf::Event::Closed) {
            m_window.close();
            m_running = false;
        }

        if (event.type == sf::Event::MouseMoved) {
            const sf::Vector2i mouse_pos = sf::Mouse::getPosition(m_window);
            if (player) {
                player->input->mousePosition = Vec2(mouse_pos.x, mouse_pos.y);
            }
        }

        if (event.type == sf::Event::MouseButtonPressed) {
            
        }
    }
}

void Game::sInputHandling() {
    for (std::shared_ptr<Entity> p : m_entity_manager.get_entities(Tag::Player)) {
        if (p->player && p->transform && p->input && p->velocity) {

            const CInput & input = *p->input.get();

            CPlayerStats & player = *p->player.get();
            CTransform & transform = *p->transform.get();
            Vec2 & velocity = p->velocity->velocity;

            const Vec2 direction = input.mousePosition - transform.position;
            const float rotation = Vec2::forward().angle_to_rad(direction);

            transform.rotation = rotation;
            velocity.x = input.axis.x * player.speed;
            velocity.y = input.axis.y * player.speed;

            if (input.fire && player.fire_countdown <= 0) {
                shoot();
                player.fire_countdown = player.fire_delay;
            }
            if (input.secondaryFire && player.special_countdown <= 0) {
                shootSpecialWeapon();
                player.special_countdown = player.special_delay;
            }

            if (player.fire_countdown > 0) {
                --player.fire_countdown;
            }
            if (player.special_countdown > 0) {
                --player.special_countdown;
            }
        }
    }
}

void Game::sRender(const sf::Time deltaTime) {
    m_window.clear();
    // Render stuff
    for (const std::shared_ptr<Entity> entity : m_entity_manager.get_entities()) {
        if (entity->shape) {
            sf::CircleShape & shape = entity->shape->shape;
            if (entity->transform) {
                const Vec2 pos = entity->transform->position;                
                shape.setPosition(sf::Vector2f(pos.x, pos.y));
                shape.rotate(m_shape_rotation);
            }
            m_window.draw(shape);
        }
    }
    ImGui::SFML::Render(m_window);
    m_window.display();
}

void Game::sMovement(const sf::Time deltaTime) {
    for (const std::shared_ptr<Entity> wb : m_entity_manager.get_entities(Tag::WorldBounds)) {
        if (wb->rect) {
            for (const std::shared_ptr<Entity> enemy : m_entity_manager.get_entities(Tag::Enemies)) {
                if (enemy->transform && enemy->collider && enemy->velocity) {
                    enemy->velocity->velocity = bounce_movement(*enemy->velocity.get(), *wb->rect.get(), *enemy->transform.get(), *enemy->collider.get());
                }
            }
        }
    }
    
    for (const std::shared_ptr<Entity> entity : m_entity_manager.get_entities()) {
        if (entity->transform && entity->velocity) {
            Vec2 & pos = entity->transform->position;
            Vec2 & vel = entity->velocity->velocity;
            
            if (entity->player && entity->collider) {
                for (const std::shared_ptr<Entity> wb : m_entity_manager.get_entities(Tag::WorldBounds)) {
                    vel = limit_movement(*entity->velocity.get(), *wb->rect.get(), *entity->transform.get(), *entity->collider.get());
                }
            }
            
            pos += vel;
        }
    }
}

const Vec2 Game::bounce_movement(const CVelocity & velocity, const CRect & bounds, const CTransform & transform, const CCollider & collider) const {
    const sf::FloatRect & w_bounds = bounds.rect;
    const Vec2 & pos = transform.position;
    const float radius = collider.radius;
    Vec2 new_vel = velocity.velocity;

    if (pos.x - radius <= w_bounds.left || pos.x + radius >= w_bounds.left + w_bounds.width) {
        new_vel.x = -new_vel.x;
    }
    if (pos.y - radius <= w_bounds.top || pos.y + radius >= w_bounds.top + w_bounds.height) {
        new_vel.y = -new_vel.y;
    }
    return new_vel;
}

const Vec2 Game::limit_movement(const CVelocity & velocity, const CRect & bounds, const CTransform & transform, const CCollider & collider) const {
    const sf::FloatRect & w_bounds = bounds.rect;
    const Vec2 & pos = transform.position;
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

void Game::sCollision() {
    for (const std::shared_ptr<Entity> bullet : m_entity_manager.get_entities(Tag::Bullets)) {
        if (bullet->transform && bullet->collider) {
            // std::cout << "There are bullets:" << m_entity_manager.get_entities(Tag::Bullets).size() << "\n";
            for (const std::shared_ptr<Entity> enemy : m_entity_manager.get_entities(Tag::Enemies)) {
                if (enemy->transform && enemy->collider) {
                    if (collides(*bullet->transform.get(), *enemy->transform.get(), *bullet->collider.get(), *enemy->collider.get())) {
                        on_entity_hit(*bullet.get());
                        on_entity_hit(*enemy.get());
                    }
                }
            }
        }
    }

    for (const std::shared_ptr<Entity> player : m_entity_manager.get_entities(Tag::Player)) {
        if (player->player && player->transform && player->collider) {
            if (player->player->invincibility_countdown > 0) {
                continue;
            }
            
            for (const std::shared_ptr<Entity> enemy : m_entity_manager.get_entities(Tag::Enemies)) {
                if (enemy->transform && enemy->collider) {
                    if (collides(*player->transform.get(), *enemy->transform.get(), *player->collider.get(), *enemy->collider.get())) {
                        on_entity_hit(*player.get());
                        on_entity_hit(*enemy.get());
                    }
                }
            }
        }
    }
}

const bool Game::collides(const CTransform & t_a, const CTransform & t_b, const CCollider & c_a, const CCollider & c_b) const {
    const float dist = t_a.position.distance_to(t_b.position);
    const float radius_sum = c_a.radius + c_b.radius;
    if (dist * dist < radius_sum * radius_sum) {
        return true;
    }
    return false;
}

void Game::on_game_over() {

}

void Game::on_entity_hit(Entity & entity) {
    if (entity.health) {
        entity.health->react_countdown = entity.health->react_duration;
        if (entity.player) {
            entity.player->invincibility_countdown = entity.player->invincibility_duration;
        }
        if (--entity.health->hp <= 0) {
            if (entity.spawner && entity.transform) {
                spawnSmallEnemies(entity.transform->position, *entity.spawner.get());
            }
            
            entity.destroy();
        }        
    }
}

void Game::spawnSmallEnemies(const Vec2 position, const CDeathSpawner & spawner) {
    const float d_angle = 360 / Vec2::rad_to_deg / spawner.amount;
    const CShape & prefab = spawner.prefab;
    const float radius = prefab.shape.getRadius();

    for (int i = 0; i < spawner.amount; ++i) {
        const float angle = d_angle * i;
        const Vec2 n_velocity = Vec2::forward().rotate_rad(angle).normalize();
        const Vec2 spawn_pos = position + n_velocity * radius;
        const std::shared_ptr<Entity> enemy = m_entity_manager.add_entity(spawner.tag);
        enemy->name = std::make_shared<CName>(CName("SmEnemy"));
        enemy->transform = std::make_shared<CTransform>(CTransform(spawn_pos.x, spawn_pos.y));
        enemy->shape = std::make_shared<CShape>(CShape(prefab));
        enemy->collider = std::make_shared<CCollider>(CCollider(radius));
        enemy->velocity = std::make_shared<CVelocity>(CVelocity(n_velocity * spawner.speed));
        enemy->health = std::make_shared<CHealth>(CHealth(1));
        enemy->lifespan = std::make_shared<CLifespan>(CLifespan(spawner.lifespan));
    }
}

void Game::sDamageReact(const sf::Time deltaTime) {
    for (std::shared_ptr<Entity> entity : m_entity_manager.get_entities()) {
        if (entity->health) {
            CHealth & health = *entity->health.get();
            if (health.react_countdown > 0) {
                --health.react_countdown;
            }
            if (entity->shape) {
                sf::CircleShape & shape = entity->shape->shape;
                const float scale = 1.f + (health.expansion - 1.f) * ((float)health.react_countdown / (float)health.react_duration);
                shape.setScale(scale, scale);
            }
        }
    }
}

void Game::sEnemySpawner(const sf::Time deltaTime) {
    if (m_enemy_spawn_interval <= 0) {
        spawn_enemy();
        m_enemy_spawn_interval = stoi(m_config["Enemy"]["spawnInterval"]);
        return;
    }
    m_enemy_spawn_interval--;
}

void Game::sPlayerSpawner(const sf::Time deltaTime) {
    if (m_entity_manager.get_entities(Tag::Player).size() > 0) {
        return;
    }
    if (m_player_spawn_interval <= 0) {
        spawn_player();
        m_player_spawn_interval = stoi(m_config["Player"]["spawnInterval"]);
        return;
    }
    m_player_spawn_interval--;
}

void Game::sGUI() {
    //Interface stuff
    ImGui::Begin("Geometry Wars");
    static std::vector<Vec2> vectors {};
    
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
            for (int i = 0; i < vectors.size(); ++i) {
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
            for (int i = 0; i < vectors.size(); ++i) {
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
                    const int entity_id = entity->id();
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
                if (entity->player) {
                    const int max_lives = entity->player->max_lives;
                    const int lives = entity->player->lives;
                    const float speed = entity->player->speed;
                    const int fire_delay = entity->player->fire_delay;
                    const int special_delay = entity->player->special_delay;
                    const int inv_dur = entity->player->invincibility_duration;
                    const int inv_count = entity->player->invincibility_countdown;
                    const int fire_count = entity->player->fire_countdown;
                    const int special_count = entity->player->special_countdown;
                    const int flicker_freq = entity->player->flicker_frequency;
                    char buf[32];
                    sprintf(buf, "Player: L:%d/%d, F:%d/%d, SF:%d/%d, I:%d/%d, Fl:%d##comp_player", 
                        lives, max_lives, fire_count, fire_delay, special_count, special_delay, inv_count, inv_dur, flicker_freq);
                    if (ImGui::Selectable(buf, comp_idx == ComponentType::PlayerStats)) {
                        comp_idx = ComponentType::PlayerStats;
                    }
                } else {
                    if (ImGui::Button("+PlayerStats##add_player")) {
                        entity->player = std::make_shared<CPlayerStats>(CPlayerStats());
                    }
                }
                if (entity->collider) {
                    const float radius = entity->collider->radius;
                    char buf[32];
                    sprintf(buf, "Col Radius: %.3f##comp_collider", radius);
                    if (ImGui::Selectable(buf, comp_idx == ComponentType::Collider)) {
                        comp_idx = ComponentType::Collider;
                    }
                } else {
                    if (ImGui::Button("+Collider##add_collider")) {
                        entity->collider = std::make_shared<CCollider>(CCollider());
                    }
                }
                if (entity->lifespan) {
                    const int countdown = entity->lifespan->countdown;
                    const int duration = entity->lifespan->duration;
                    char buf[32];
                    sprintf(buf, "Lifespan: %d/%d##comp_collider", countdown, duration);
                    if (ImGui::Selectable(buf, comp_idx == ComponentType::Lifespan)) {
                        comp_idx = ComponentType::Lifespan;
                    }
                } else {
                    if (ImGui::Button("+Lifespan##add_lifespan")) {
                        entity->lifespan = std::make_shared<CLifespan>(CLifespan());
                    }
                }
                if (entity->health) {
                    const int hp = entity->health->hp;
                    const int max_hp = entity->health->max_hp;
                    const float expansion = entity->health->expansion;
                    const int duration = entity->health->react_duration;
                    const int countdown = entity->health->react_countdown;
                    char buf[32];
                    sprintf(buf, "HP: %d/%d, Ex%.3f D%d/%d##comp_collider", hp, max_hp, expansion, duration, countdown);
                    if (ImGui::Selectable(buf, comp_idx == ComponentType::Health)) {
                        comp_idx = ComponentType::Health;
                    }
                } else {
                    if (ImGui::Button("+Health##add_health")) {
                        entity->health = std::make_shared<CHealth>(CHealth());
                    }
                }
                if (entity->shape) {
                    const sf::CircleShape & shape = entity->shape->shape;
                    const float radius = shape.getRadius();
                    const float scale = shape.getScale().x;
                    char buf[32];
                    sprintf(buf, "Shape R:%.3f S:%.3f##comp_shape", radius, scale);
                    if (ImGui::Selectable(buf, comp_idx == ComponentType::Shape)) {
                        comp_idx = ComponentType::Shape;
                    }
                } else {
                    if (ImGui::Button("+Shape##add_shape")) {
                        entity->shape = std::make_shared<CShape>(CShape());
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

void Game::test_config(Config & config) const {
    std::for_each(config.begin(), config.end(),
    [](std::pair<std::string, std::map<std::string, std::string>> p){
        std::cout << "Heading: " << p.first << '\n';
        std::for_each(p.second.begin(), p.second.end(),
        [](std::pair<std::string, std::string> v){
            std::cout << v.first << ": " << v.second << '\n';
        });
    });
    std::cout << "====Individual tests====\n";
    std::cout << "Window resolution and refresh rate: " << config["Window"]["width"] << "x" << config["Window"]["height"] << ", " << config["Window"]["refreshRate"] << '\n';
}

Config Game::parse_tokens(const std::vector<std::string> tokenstream) const {
    bool nextHeading = true;
    std::string heading = "";
    Config config;
    for (std::string token : tokenstream) {
        
        std::string key, value;
        std::string::size_type begin = token.find_first_not_of( " ,[\f\t\v" );

        if (nextHeading) {
            nextHeading = false;
            heading = token;
            config[heading] = {};
            // std::cout << "Heading: " << heading << "\n";
            continue;
        }
        
        if (token.find(']') != std::string::npos) {
            nextHeading = true;
            // std::cout << "next heading\n";
        }

        // Skip blank lines
        if (begin == std::string::npos) {
            continue;
        }        
        
        // Extract the key value
        std::string::size_type end = token.find( '=', begin );
        key = token.substr( begin, end - begin );

        // (No leading or trailing whitespace allowed)
        key.erase( key.find_last_not_of( " ,]\f\t\v" ) + 1 );

        // No blank keys allowed
        if (key.empty()) {
            continue;
        }

        // Extract the value (no leading or trailing whitespace allowed)
        begin = token.find_first_not_of( " ,]\f\n\r\t\v", end + 1 );
        end   = token.find_last_not_of(  " ,]\f\n\r\t\v" ) + 1;

        value = token.substr( begin, end - begin );

        // std::cout << "key: " << key << ", value: " << value << '\n';
        config[heading][key] = value;     
    }
    return config;
}

const Config Game::read_file(const std::string filename) const {
    std::vector<std::string> tokenstream {};
    std::string word;
    
    std::ifstream ifs(filename, std::ifstream::in);
    while (ifs >> word) {
        tokenstream.push_back(word);
    }

    return parse_tokens(tokenstream);
}