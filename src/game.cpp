#include "game.h"
#include "vec2.h"
#include <ctime>
#include <cstdlib>
#include <fstream>

void Game::run() {    
    
    while (m_running) {
        // create/cleanup entities
        m_entity_manager.update();

        sf::Time deltaTime = m_delta_clock.restart();

        // update call required by ImGui
        ImGui::SFML::Update(m_window, deltaTime);

        sUserInput();
        if (!m_paused) {
            if (m_sInputHandling) sInputHandling();
            sTimers(deltaTime);
            sEmitters(deltaTime);
            sEffects(deltaTime);
            sLifespan(deltaTime);
            sDamageReact(deltaTime);
            if (m_sMovement) sMovement(deltaTime);
            if (m_sCollision) sCollision();
            sPlayerSpawner(deltaTime);
            if (m_sEnemySpawner) sEnemySpawner(deltaTime);
        }
        if (m_sGUI) sGUI();
        //Rendering should be last
        sRender(deltaTime);
        m_frameCount++;
    }
    shutdown();
}

void Game::setPaused(const bool paused) {
    m_paused = paused;
}

Game::Game(const std::string & userconfig, const std::string & default_config) {
    m_userconfig_file = userconfig;
    try {
        init(userconfig);
    } catch(std::exception &e) {
        try {
            init(default_config);
        } catch(std::exception &e) {
            char msg[32];
            sprintf(msg, "%s | Could not initialize.", e.what());
            throw std::runtime_error(msg);
        }        
    } 
}

void Game::init(const std::string & configfile) {   
    try { 
        m_config = read_file(configfile);
    } catch(std::exception &e) {
        char msg[32];
        sprintf(msg, "%s | Could not read configuration file: %s", e.what(), configfile.c_str());
        throw std::runtime_error(msg);
    }

    const std::string font_path = m_config["Font"]["path"];
    m_font = sf::Font();
    if (!m_font.loadFromFile(font_path)) {
        throw std::runtime_error("Could not load font at " + font_path);
    }

    std::srand(std::time(nullptr));
    const size_t width = std::stoi(m_config["Window"]["width"]);
    const size_t height = std::stoi(m_config["Window"]["height"]);
    const size_t framerate = std::stoi(m_config["Window"]["refreshRate"]);
    
    
    m_enemy_spawn_interval = std::stoi(m_config["Enemy"]["spawnInterval"]);
    m_enemy_spawn_countdown = m_enemy_spawn_interval;
    m_player_spawn_interval = std::stoi(m_config["Player"]["spawnInterval"]);
    m_player_spawn_countdown = m_player_spawn_interval;

    m_score = 0;
    m_score_to_boss_base = std::stoi(m_config["Boss"]["scoreRequirement"]);
    m_score_to_boss = m_score + m_score_to_boss_base;
    m_score_to_boss_mult = std::stof(m_config["Boss"]["scoreRequirementMultiplier"]);

    m_shape_rotation = std::stof(m_config["Global"]["shapeRotation"]);

    m_window.create(sf::VideoMode(width, height), "ImGUI + SFML = <3");
    m_window.setFramerateLimit(framerate);
    ImGui::SFML::Init(m_window);
    spawn_world();
}

void Game::shutdown() {
    m_window.close();
    ImGui::SFML::Shutdown();
}

void Game::spawn_world() {
    const std::shared_ptr<Entity> wb = m_entity_manager.add_entity(Tag::WorldBounds);
    wb->rect = std::make_shared<CRect>(CRect(1920, 1080));
    const std::shared_ptr<Entity> score = m_entity_manager.add_entity(Tag::ScoreWindow);
    const int font_size = std::stoi(m_config["Font"]["size"]);
    const int font_red = std::stoi(m_config["Font"]["red"]);
    const int font_green = std::stoi(m_config["Font"]["green"]);
    const int font_blue = std::stoi(m_config["Font"]["blue"]);    
    score->text = std::make_shared<CText>(CText("Score:", m_font, font_size, sf::Color(font_red, font_green, font_blue)));
    score->transform = std::make_shared<CTransform>(CTransform(Vec2(0, 0)));
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

    const int bSpecialRecursion = std::stoi(m_config["Bullet"]["specialRecursion"]);
    const int bSpecialAmount = std::stoi(m_config["Bullet"]["specialAmount"]);

    player.name = std::make_shared<CName>(CName("Player"));
    player.player = std::make_shared<CPlayerStats>(CPlayerStats(3, speed, flickerRate));
    player.transform = std::make_shared<CTransform>(CTransform(position));
    player.velocity = std::make_shared<CVelocity>(CVelocity());
    player.weapon = std::make_shared<CWeapon>(CWeapon(bSpeed, bLifespan, fireRate, CWeapon::FireMode::ShotSingle, CShape(bRadius, bVertices, sf::Color(bFillRed, bFillGreen, bFillBlue), sf::Color(bOutlineRed, bOutlineGreen, bOutlineBlue), bOutlineThickness)));
    player.special_weapon = std::make_shared<CSpecialWeapon>(CSpecialWeapon(bSpeed, bLifespan, specialFireRate, bSpecialAmount, bSpecialRecursion, CShape(bRadius, bVertices, sf::Color(bFillRed, bFillGreen, bFillBlue), sf::Color(bOutlineRed, bOutlineGreen, bOutlineBlue), bOutlineThickness)));
    player.shape = std::make_shared<CShape>(CShape(shapeRadius, vertices, sf::Color(fillRed, fillGreen, fillBlue), sf::Color(outlineRed, outlineGreen, outlineBlue), outlineThickness));
    player.collider = std::make_shared<CCollider>(CCollider(collisionRadius));
    player.input = std::make_shared<CInput>(CInput());
    player.health = std::make_shared<CHealth>(CHealth(health));
    player.invincibility = std::make_shared<CInvincibility>(CInvincibility(invincibilityDuration));
}

void Game::spawn_enemy() {
    for (const std::shared_ptr<Entity> wb : m_entity_manager.get_entities(Tag::WorldBounds)) {
        const sf::FloatRect & w_bounds = wb->rect->rect;
        const float border = wb->rect->border;
        const sf::FloatRect spawn_bounds = sf::FloatRect(w_bounds.left + border, w_bounds.top + border, w_bounds.width - border, w_bounds.height - border);   
        const std::shared_ptr<Entity> enemy = m_entity_manager.add_entity(Tag::Enemies);
        const float isBoss = false;
        setup_random_enemy(*enemy.get(), isBoss, spawn_bounds);
    }
}

void Game::spawn_boss() {
    for (const std::shared_ptr<Entity> wb : m_entity_manager.get_entities(Tag::WorldBounds)) {
        const sf::FloatRect & w_bounds = wb->rect->rect;
        const float border = wb->rect->border;
        const sf::FloatRect spawn_bounds = sf::FloatRect(w_bounds.left + border, w_bounds.top + border, w_bounds.width - border, w_bounds.height - border);   
        const std::shared_ptr<Entity> enemy = m_entity_manager.add_entity(Tag::Enemies);
        const float isBoss = true;
        setup_random_enemy(*enemy.get(), isBoss, spawn_bounds);
    }
}

void Game::setup_random_enemy(Entity & enemy, const bool isBoss, const sf::FloatRect & spawn_bounds) {    
    std::string configHeader = "Enemy";
    if (isBoss) {
        configHeader = "Boss";
    }

    std::shared_ptr<Entity> player;
    for (auto e : m_entity_manager.get_entities(Tag::Player)) {
        player = std::move(e);
        break;
    }
    
    const float shapeRadius = std::stof(m_config[configHeader]["shapeRadius"]);
    const float collisionRadius = std::stof(m_config[configHeader]["collisionRadius"]);
    const float speedMin = std::stof(m_config[configHeader]["speedMin"]);
    const float speedMax = std::stof(m_config[configHeader]["speedMax"]);
    const int outlineRed = std::stoi(m_config[configHeader]["outlineRed"]);
    const int outlineGreen = std::stoi(m_config[configHeader]["outlineGreen"]);
    const int outlineBlue = std::stoi(m_config[configHeader]["outlineBlue"]);
    const int outlineThickness = std::stoi(m_config[configHeader]["outlineThickness"]);
    const int verticesMin = std::stoi(m_config[configHeader]["verticesMin"]);
    const int verticesMax = std::stoi(m_config[configHeader]["verticesMax"]);
    const float vertSizeMultiplier = std::stof(m_config[configHeader]["vertSizeMultiplier"]);

    const float rand_speed = (float)(std::rand()) / (float)(RAND_MAX) * (speedMax - speedMin) + speedMin;
    const float rand_angle = (float)(std::rand()) / (float)(RAND_MAX) * 360;
    const Vec2 rand_velocity = Vec2(0, 1).rotate_deg(rand_angle) * rand_speed;
    const int rand_vertices = (float)(std::rand()) / (float)(RAND_MAX) * (verticesMax - verticesMin) + verticesMin;

    const float smallRadius = std::stof(m_config[configHeader]["smallRadius"]);
    const float smallCollisionRadius = std::stof(m_config[configHeader]["smallCollisionRadius"]);
    const float smallSpeed = std::stof(m_config[configHeader]["smallSpeed"]);
    const int lifespan = std::stoi(m_config[configHeader]["smallLifespan"]);
    const int invincibilityDuration = std::stoi(m_config[configHeader]["invincibilityDuration"]);

    const int base_score = std::stoi(m_config[configHeader]["score"]);
    const float score_multiplier = std::stof(m_config[configHeader]["scoreSizeMultiplier"]);
    const int score = base_score + base_score * ((score_multiplier - 1) * (rand_vertices - verticesMin));
    const float radius = shapeRadius + shapeRadius * ((vertSizeMultiplier - 1) * (rand_vertices - verticesMin));
    const int scaledCollisionRadius = collisionRadius + collisionRadius * ((vertSizeMultiplier - 1) * (rand_vertices - verticesMin));
    
    float rand_x = (float)(std::rand()) / (float)(RAND_MAX) * spawn_bounds.width + spawn_bounds.left;
    float rand_y = (float)(std::rand()) / (float)(RAND_MAX) * spawn_bounds.height + spawn_bounds.top;

    if (player) {
        const float right = player->transform->position.x + player->collider->radius * 2 + scaledCollisionRadius;
        const float left = player->transform->position.x - player->collider->radius * 2 - scaledCollisionRadius;
        const float up = player->transform->position.y - player->collider->radius * 2 - scaledCollisionRadius;
        const float down = player->transform->position.y + player->collider->radius * 2 + scaledCollisionRadius;
        while (rand_x < right && rand_x > left) {
            rand_x = (float)(std::rand()) / (float)(RAND_MAX) * spawn_bounds.width + spawn_bounds.left;
        }
        while (rand_y < down && rand_y > up) {
            rand_y = (float)(std::rand()) / (float)(RAND_MAX) * spawn_bounds.height + spawn_bounds.top;
        }
    }

    sf::Color fillColor = sf::Color(0, 0, 0);
    sf::Color outlineColor = sf::Color(outlineRed, outlineGreen, outlineBlue);
    sf::Color smallFillColor = sf::Color(0, 0, 0);
    sf::Color smallOutlineColor = sf::Color(outlineRed, outlineGreen, outlineBlue);
    if (isBoss) {
        std::map<CWeaponPickup::PickupType, sf::Color> types {
            {CWeaponPickup::PickupType::ShotSingle, sf::Color(255, outlineGreen, outlineBlue)},
            {CWeaponPickup::PickupType::ShotSpread, sf::Color(outlineRed, 255, outlineBlue)},
            {CWeaponPickup::PickupType::ShotLaser, sf::Color(outlineRed, outlineGreen, 255)},
        };
        const int pickup_idx = std::round((float)(std::rand()) / (float)(RAND_MAX) * types.size());
        const CWeaponPickup::PickupType pickup_type = (CWeaponPickup::PickupType)pickup_idx;
        outlineColor = types[pickup_type];
        smallOutlineColor = outlineColor;
        const CWeaponPickup pickup = CWeaponPickup(pickup_type);
        
        const float p_radius = std::stof(m_config["Pickup"]["shapeRadius"]);
        const float p_collision = std::stof(m_config["Pickup"]["collisionRadius"]);
        const int p_thickness = std::stoi(m_config["Pickup"]["outlineThickness"]);
        const int p_verts = std::stoi(m_config["Pickup"]["vertices"]);
        const int p_lifespan = std::stoi(m_config["Pickup"]["lifespan"]);
        const sf::Color p_fillColor = outlineColor;
        const sf::Color p_outlineColor = sf::Color(0, 0, 0);
        
        const CShape p_shape = CShape(
            p_radius,
            p_verts,
            p_fillColor,
            p_outlineColor,
            p_thickness
        );

        enemy.pickup_spawner = std::make_shared<CPickupSpawner>(CPickupSpawner(pickup, p_shape, p_lifespan, p_collision));
    }
    const CShape small_prefab = CShape(smallRadius, rand_vertices, smallFillColor, smallOutlineColor, outlineThickness);

    const int recursion = 0;

    enemy.name = std::make_shared<CName>(CName("Enemy"));
    enemy.transform = std::make_shared<CTransform>(CTransform(rand_x, rand_y));
    enemy.shape = std::make_shared<CShape>(CShape(radius, rand_vertices, fillColor, outlineColor, outlineThickness));
    enemy.collider = std::make_shared<CCollider>(CCollider(scaledCollisionRadius));
    enemy.velocity = std::make_shared<CVelocity>(CVelocity(rand_velocity));
    enemy.health = std::make_shared<CHealth>(CHealth(rand_vertices));
    enemy.invincibility = std::make_shared<CInvincibility>(CInvincibility(invincibilityDuration));
    enemy.score_reward = std::make_shared<CScoreReward>(CScoreReward(score));
    enemy.spawner = std::make_shared<CDeathSpawner>(CDeathSpawner(rand_vertices, small_prefab, lifespan, smallSpeed, recursion, Tag::Enemies));
}

void Game::shoot() {
    for (const std::shared_ptr<Entity> player : m_entity_manager.get_entities(Tag::Player)) {
        if (player->weapon && player->transform) {
            if (player->weapon->fire_countdown <= 0) {
                const Vec2 position = player->transform->position.clone();
                const float rotation = player->transform->rotation;
                const Vec2 velocity = Vec2::forward().rotate_rad(rotation).normalize() * player->weapon->speed;
                const CShape & bullet_prefab = player->weapon->bullet;
                const float collision_radius = bullet_prefab.shape.getRadius();
                const float lifespan = player->weapon->lifespan;

                switch (player->weapon->mode) {
                    case (CWeapon::FireMode::ShotSingle): {
                        spawn_bullet(position, rotation, velocity, bullet_prefab, collision_radius, lifespan);
                        break;
                    }
                    case (CWeapon::FireMode::ShotSpread): {
                        const float spread_rad = 30 / Vec2::rad_to_deg;
                        const int amount = 2 + player->weapon->power;
                        const float rot = spread_rad / amount;
                        Vec2 r_vel = velocity.clone().rotate_rad(-rot);
                        for (int i = 0; i < amount; ++i) {
                            spawn_bullet(position, rot, r_vel, bullet_prefab, collision_radius, lifespan);
                            r_vel.rotate_rad(rot);
                        }
                        break;
                    }
                    case (CWeapon::FireMode::ShotLaser): {
                        spawn_laser(
                            *player->transform.get(), // Parent transform
                            Vec2(40.f, 0), // Offset
                            500.f, // length
                            6.f, // thickness
                            sf::Color(10, 10, 255), // Fill Color
                            sf::Color(255, 255, 255), // Outline Color
                            1.f, // Outline Thickness
                            lifespan // Lifespan
                        )
                        break;
                    }
                    default: break;
                }
                player->weapon->fire_countdown = player->weapon->fire_delay;
            }            
        }
    }
}

const std::shared_ptr<Entity> Game::spawn_bullet(
    const Vec2 & position, 
    const float rotation, 
    const Vec2 & velocity, 
    const CShape & bullet_prefab, 
    const float collision_radius, 
    const int lifespan
) 
{
    const std::shared_ptr<Entity> bullet = m_entity_manager.add_entity(Tag::Bullets);
    bullet->transform = std::make_shared<CTransform>(CTransform(position, rotation));
    bullet->velocity = std::make_shared<CVelocity>(CVelocity(velocity));
    bullet->shape = std::make_shared<CShape>(CShape(bullet_prefab));
    bullet->collider = std::make_shared<CCollider>(CCollider(collision_radius));
    bullet->name = std::make_shared<CName>(CName("Bullet"));
    bullet->lifespan = std::make_shared<CLifespan>(CLifespan(lifespan));
    bullet->health = std::make_shared<CHealth>(CHealth(1));
    return bullet;
}

const std::shared_ptr<Entity> Game::spawn_laser(
    const CTransform & parent_transform, 
    const Vec2 & offset,
    const float length, 
    const float thickness, 
    const sf::Color & fill_color, 
    const sf::Color & outline_color, 
    const float outeline_thickness,
    const int lifespan
)
{
    const std::shared_ptr<Entity> laser = m_entity_manager.add_entity(Tag::Bullets);
    laser->transform = std::make_shared<CTransform>(CTransform(parent_position.position + offset, parent_transform.rotation));
    laser->name = std::make_shared<CName>(CName("Laser"));
    laser->lifespan = std::make_shared<CLifespan>(CLifespan(lifespan));
    const Vec2 endpoint = offset + Vec2::forward().normalize() * length;
    laser->line = std::make_shared<CLine>(CLine(
        parent_transform,
        offset,
        endpoint,
        thickness,
        fill_color,
        outline_color,
        outline_thickness
    ));
    return laser;
}

void Game::shootSpecialWeapon() {
    for (const std::shared_ptr<Entity> player : m_entity_manager.get_entities(Tag::Player)) {
        if (player->special_weapon && player->transform) {
            if (player->special_weapon->fire_countdown <= 0) {
                const Vec2 position = player->transform->position.clone();
                const float rotation = player->transform->rotation;
                const Vec2 velocity = Vec2::forward().rotate_rad(rotation).normalize() * player->special_weapon->speed;
                const CShape & bullet_prefab = player->special_weapon->bullet;
                const float collision_radius = bullet_prefab.shape.getRadius();
                const int lifespan = player->special_weapon->lifespan;

                switch(player->special_weapon->mode) {
                    case (CSpecialWeapon::FireMode::SpecialExplosion): {
                        const std::shared_ptr<Entity> bullet = spawn_special_bullet(position, rotation, bullet_prefab, collision_radius, lifespan);
                        const float small_speed = player->special_weapon->speed;
                        const int small_lifespan = lifespan / 4;
                        const int small_amount = player->special_weapon->amount;
                        bullet->velocity = std::make_shared<CVelocity>(CVelocity(velocity));
                        bullet->spawner = std::make_shared<CDeathSpawner>(CDeathSpawner(small_amount, bullet_prefab, small_lifespan, small_speed, player->special_weapon->recursion, Tag::Bullets));
                        break;
                    }
                    case (CSpecialWeapon::FireMode::SpecialRotor): {                                            
                        const float spread_rad = 360.f / Vec2::rad_to_deg;
                        const int amount = 3 + player->special_weapon->power;
                        const float radius = 80.f + player->special_weapon->power * 40;
                        const float speed = 5.f + player->special_weapon->power;
                        const float rot = spread_rad / amount;

                        for (int i = 0; i < amount; ++i) {
                            const std::shared_ptr<Entity> bullet = spawn_special_bullet(position, rot * i, bullet_prefab, collision_radius, lifespan);
                            bullet->orbit = std::make_shared<COrbit>(COrbit(*player->transform.get(), radius, speed));
                        }
                        break;
                    }
                    case (CSpecialWeapon::FireMode::SpecialFlamethrower): {
                        const float spread_rad = 30 / Vec2::rad_to_deg;
                        const float speed = 1;
                        const float offset_distance = 80.f;
                        const int freq = 1;
                        const int quantity = 3;
                        const int particle_lifespan = 15;
                        const float random_scale_mult = 1.2f;

                        const std::vector<sf::Color> colors = {
                            sf::Color(200, 50, 0, 120),
                            sf::Color(150, 80, 0, 200),
                            sf::Color(255, 40, 0, 180),
                            sf::Color(200, 100, 0, 100)
                        };
                        const float min_radius = 3.f;
                        const float max_radius = 10.f;                        
                        const int min_verts = 3;
                        const int max_verts = 5;                        
                        const int amount_types = 8;
                        std::vector<CShape> shapes {};
                        for (int i = 0; i < amount_types; i++) {
                            const float radius = (float)std::rand() / (float)RAND_MAX * (max_radius - min_raduis) + min_radius;
                            const int verts = std::round((float)std::rand() / (float)RAND_MAX * (max_verts - min_verts) + min_verts);
                            const size_t color_idx = std::round((float)std::rand() / (float)RAND_MAX * colors.size());

                            shapes.push( CShape(
                                radius,
                                verts,
                                colors[color_idx],
                                sf::Color(0, 0, 0, 0), // outline
                                0 // outline thickness
                            ));
                        }

                        const std::shared_ptr<Entity> emitter = m_entity_manager.add_entity(Tag::Emitters);
                            emitter->transform = std::make_shared<CTransform>(CTransform(parent_transform));
                            emitter->emitter = std::make_shared<CEmitter>(CEmitter(
                                player->transform, // Parent transform
                                shapes,
                                Vec2.forward() * offset_distance, // Offset
                                spread_rad, // Spread angle
                                speed,
                                freq,
                                quantity,
                                particle_lifespan,
                                random_scale_mult
                            ));
                        break;
                    }
                    default: break;
                }
                player->special_weapon->fire_countdown = player->special_weapon->fire_delay;
            }            
        }
    }
}

const std::shared_ptr<Entity> Game::spawn_special_bullet(
    const Vec2 & position, 
    const float rotation, 
    const CShape & bullet_prefab, 
    const float collision_radius, 
    const int lifespan
)
{
    const std::shared_ptr<Entity> bullet = m_entity_manager.add_entity(Tag::Bullets);
    bullet->transform = std::make_shared<CTransform>(CTransform(position, rotation));
    bullet->shape = std::make_shared<CShape>(CShape(bullet_prefab));
    bullet->collider = std::make_shared<CCollider>(CCollider(collision_radius));
    bullet->name = std::make_shared<CName>(CName("Bullet"));
    bullet->lifespan = std::make_shared<CLifespan>(CLifespan(lifespan));
    bullet->health = std::make_shared<CHealth>(CHealth(1));
    return bullet;
}

void Game::spawn_tutorial_messages() {
    for (const std::shared_ptr<Entity> wb = m_entity_manager.get_entities(Tag::WorldBounds)) {
        const sf::FloatRect w_bounds = wb->rect->rect;
        const int font_size = 48;
        const int spacing = 4;
        const int v_spacing = 40;
        const sf::Color font_color = sf::Color(255, 160, 50);
        const std::string msg_a = "WASD MOVE";
        const std::string msg_b = "LMB SHOOT";
        const std::string msg_c = "RMB SPECIAL";
        const int width_a = msg_a.size() * font_size + spacing;
        const int width_b = msg_b.size() * font_size + spacing;
        const int width_c = msg_c.size() * font_size + spacing;
        const int left_a = (w_bounds.width - width_a) / 2;
        const int left_b = (w_bounds.width - width_b) / 2;
        const int left_c = (w_bounds.width - width_c) / 2;
        const int top_a = w_bounds.top + v_spacing;
        const int top_c = w_bounds.top + w_bounds.height - v_spacing;
        const int top_b = top_c - v_spacing;
        spawn_text_enemies(msg_a, Vec2(left_a, top_a), font_size, font_color, spacing);
        spawn_text_enemies(msg_b, Vec2(left_b, top_b), font_size, font_color, spacing);
        spawn_text_enemies(msg_c, Vec2(left_c, top_c), font_size, font_color, spacing);
    }
}

void Game::spawn_text_enemies(const std::string text, const Vec2 & position, const int font_size, const sf::Color & font_color, const int spacing) {
    Vec2 pos = position;
    for (auto & ch : text) {
        const std::shared_ptr<Entity> enemy = m_entity_manager.schedule(Tag::Enemies);
        enemy->name = std::make_shared<CName>(CName("Text enemy"));
        enemy->transform = std::make_shared<CTransform>(CTransform(pos));
        enemy->text = std::make_shared<CText>(CText(ch, m_font, font_size, font_color));
        enemy->health = std::make_shared<CHealth>(CHealth(1));
        enemy->score = std::make_shared<CScoreReward>(CScoreReward(200));
        pos.x += spacing;
    }
}

void Game::sLifespan(const sf::Time & deltaTime) {
    for (const std::shared_ptr<Entity> entity : m_entity_manager.get_entities()) {
        if (entity->lifespan) {
            const int & countdown = entity->lifespan->countdown;
            const int duration = entity->lifespan->duration;
            if (countdown <= 0) {
                on_entity_death(*entity.get());
            }
            if (entity->shape) {
                const float rate = (float)countdown / (float)duration * 255;
                sf::CircleShape & shape = entity->shape->shape;
                const sf::Color color = shape.getFillColor();
                const sf::Color outline = shape.getOutlineColor();
                shape.setFillColor(sf::Color(color.r, color.g, color.b, rate));
                shape.setOutlineColor(sf::Color(outline.r, outline.g, outline.b, rate));
            }
        }
    }
}

void Game::sEffects(const sf::Time & deltaTime) {
    for (const std::shared_ptr<Entity> player : m_entity_manager.get_entities(Tag::Player)) {
        if (player->player && player->invincibility) {
            const int freq = player->player->flicker_frequency;
            const int countdown = player->invincibility->countdown;
            if (countdown > 0) {
                if (player->shape) {
                    sf::CircleShape & shape = player->shape->shape;
                    const sf::Color color = shape.getFillColor();
                    const sf::Color outline = shape.getOutlineColor();
                    int alpha = outline.a;
                    if (countdown % freq == 0) {
                        if (alpha > 0) {
                            alpha = 0;
                        } else {
                            alpha = 255;
                        }
                    }
                    if (countdown < freq) {
                        alpha = 255;
                    }
                    shape.setFillColor(sf::Color(color.r, color.g, color.b, alpha));
                    shape.setOutlineColor(sf::Color(outline.r, outline.g, outline.b, alpha));
                }
            }
        }
    }
}

void Game::sEmitters(const sf::Time & deltaTime) {
    for (const std::shared_ptr<Entity> entity : m_entity_manager.get_entities(Tag::Emitters)) {
        if (entity->emitter && entity->transform) {
            const CEmitter & emitter = *entity->emitter.get()
            if (emitter.countdown > 0) {
                const parent_pos = emitter.parent.position;
                const parent_rot = emitter.parent.rotation;
                
                if (emitter.countdown % emitter.freq == 0) {
                    for (int i = 0; i < emitter.quantity; ++i) {
                        const int rand_idx = std::round((float)(std::rand()) / (float)(RAND_MAX) * emitter.particles.size());
                        const float rand_angle = (float)(std::rand()) / (float)(RAND_MAX) * emitter.angle - emitter.angle / 2;
                        const int lifespan = emitter.lifespan + emitter.lifespan * ((emitter.random_scale_mult - 1) * ((float)std::rand() / (float)RAND_MAX));
                        const float speed = emitter.speed + emitter.speed * ((emitter.random_scale_mult - 1) * ((float)std::rand() / (float)RAND_MAX));
                        
                        CShape shape = emitter.particles[rand_idx];
                        const float collider_radius = shape.getRadius();
                        const Vec2 pos = parent_pos + emitter.offset;
                        const Vec2 vel = parent_rot.clone().rotate_rad(rand_angle);
                        
                        const std::shared_ptr<Entity> particle = m_entity_manager.add_entity(Tag::Bullet);
                        particle->transform = std::make_shared<CTransform>(CTransform(pos, parent_rot));
                        particle->velocity = std::make_shared<CVelocity>(CVelocity(vel));
                        particle->shape = std::make_shared<CShape>(CShape(shape));
                        particle->collider = std::make_shared<CCollider>(CCollider(collider_radius));
                        particle->lifespan = std::make_shared<CLifespan>(CLifespan(lifespan));
                    }                    
                }
            } else {
                entity->destroy();
            }
        }
    }
}

void Game::sTimers(const sf::Time & deltaTime) {
    for (const std::shared_ptr<Entity> entity : m_entity_manager.get_entities()) {
        if (entity->invincibility && entity->invincibility->countdown > 0) {
            --entity->invincibility->countdown;
        }
        if (entity->lifespan && entity->lifespan->countdown > 0) {
            --entity->lifespan->countdown;
        }
        if (entity->health && entity->health->react_countdown > 0) {
            --entity->health->react_countdown;
        }
        if (entity->weapon && entity->weapon->fire_countdown > 0) {
            --entity->weapon->fire_countdown;
        }
        if (entity->special_weapon && entity->special_weapon->fire_countdown > 0) {
            --entity->special_weapon->fire_countdown;
        }
        if (entity->emitter && entity->emitter->countdown) {
            --entity->emitter->countdown;
        }
    }
    if (m_game_close_countdown > 0) {
        --m_game_close_countdown;
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
            m_running = false;
        }

        if (event.type == sf::Event::KeyPressed) {
            switch (event.key.code) {
                case sf::Keyboard::F5: {
                    m_sGUI = !m_sGUI;
                    break;
                }
                case sf::Keyboard::F1: {
                    m_paused = !m_paused;
                    break;
                }
                case sf::Keyboard::Escape: {
                    if (m_game_close_countdown > 0) {
                        m_running = false;
                    } else {
                        m_game_close_countdown = m_game_close_timeout;
                    }                    
                    break;
                }
            }
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

            if (input.fire) {
                shoot();                
            }
            if (input.secondaryFire) {
                shootSpecialWeapon();                
            }
        }
    }
}

void Game::sRender(const sf::Time & deltaTime) {
    m_window.clear();
    // Render stuff
    if (m_sRender) {
        for (const std::shared_ptr<Entity> entity : m_entity_manager.get_entities()) {
            if (entity->shape) {
                sf::CircleShape & shape = entity->shape->shape;
                if (entity->transform) {
                    const Vec2 & pos = entity->transform->position;                
                    shape.setPosition(sf::Vector2f(pos.x, pos.y));
                    shape.rotate(m_shape_rotation);
                }
                m_window.draw(shape);
            }
            if (entity->text && entity->transform) {
                sf::Text & text = entity->text->text;
                const Vec2 & pos = entity->transform->position;
                text.setPosition(sf::Vector2f(pos.x, pos.y));
                m_window.draw(text);
            }
            if (entity->line) {
                sf::RectangleShape & shape = entity->line->shape;
                if (entity->transform) {
                    const Vec2 & pos = entity->transform->position;
                    const float rot = entity->transform->rotation;
                    shape.setPosition(sf::Vector2f(pos.x, pos.y));
                    shape.setRotation(rot / Vec2::rad_to_deg);
                }
                m_window.draw(shape);
            }
        }
    }
    ImGui::SFML::Render(m_window);
    m_window.display();
}

void Game::sMovement(const sf::Time & deltaTime) {
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
        if (entity->orbit && entity->transform) {
            const Vec2 parent_pos = entity->orbit->parent.position;
            const Vec2 pos = (parent_pos + Vec2::forward().rotate_rad(entity->transform->rotation).rotate_deg(entity->orbit->angle) * entity->orbit->radius);
            entity->orbit->angle += entity->orbit->speed;
            if (entity->orbit->angle == 360) {
                entity->orbit->angle = 0;
            }
            entity->transform->position = pos;
        }
        if (entity->line && entity->transform) {
            const Vec2 parent_pos = entity->line->parent.position;
            const Vec2 parent_rot = entity->line->parent.rotation;
            entity->transform->position = parent_pos;
            entity->transform->rotation = parent_rot;
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
    for (const std::shared_ptr<Entity> enemy : m_entity_manager.get_entities(Tag::Enemies)) {
        if (!enemy->is_alive() || (enemy->invincibility && enemy->invincibility->countdown > 0)) {
            continue;
        }
        if (enemy->transform && enemy->collider) {
            for (const std::shared_ptr<Entity> bullet : m_entity_manager.get_entities(Tag::Bullets)) {
                if (!bullet->is_alive()) {
                    continue;
                }
                if (bullet->transform && bullet->collider) {
                    if (collides(*bullet->transform.get(), *enemy->transform.get(), *bullet->collider.get(), *enemy->collider.get())) {
                        on_entity_hit(*bullet.get());
                        on_entity_hit(*enemy.get());
                    }
                }
                if (bullet->transform && bullet->line) {
                    if (line_intersect(*bullet->transform.get(), *enemy->transform.get(), *bullet->line.get(), *enemy->collider.get())) {
                        on_entity_hit(*bullet.get());
                        on_entity_hit(*enemy.get());
                    }
                }
            }
            for (const std::shared_ptr<Entity> player : m_entity_manager.get_entities(Tag::Player)) {
                if (player->transform && player->collider) {
                    if (!player->is_alive() || (player->invincibility && player->invincibility->countdown > 0)) {
                        continue;
                    }
                    if (collides(*player->transform.get(), *enemy->transform.get(), *player->collider.get(), *enemy->collider.get())) {
                        on_entity_hit(*player.get());
                        on_entity_hit(*enemy.get());
                    }
                }
            }
        }
    }

    for (const std::shared_ptr<Entity> pickup : m_entity_manager.get_entities(Tag::Pickups)) {
        if (pickup->transform && pickup->collider) {
            for (const std::shared_ptr<Entity> player : m_entity_manager.get_entities(Tag::Player)) {
                if (player->transform && player->collider) {
                    if (collides(*player->transform.get(), *pickup->transform.get(), *player->collider.get(), *pickup->collider.get())) {
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

const bool Game::collides(const CTransform & t_a, const CTransform & t_b, const CCollider & c_a, const CCollider & c_b) const {
    const float dist = t_a.position.distance_to(t_b.position);
    const float radius_sum = c_a.radius + c_b.radius;
    if (dist * dist < radius_sum * radius_sum) {
        return true;
    }
    return false;
}

const bool Game::line_intersect(const CTransform & t_line, const CTransform & t_obj, const CLine & line, const CCollider & collider) const {    
    const float dist_sq = Vec2::distance_to_line_sq(
        t_line.position + line.start.clone().rotate_rad(t_line.rotation), 
        t_line.position + line.end.clone().rotate_rad(t_line.rotation), 
        t_obj.position);
    if (dist_sq < collider.radius * collider.radius) {
        return true;
    }
    return false;
}

void Game::on_game_over() {

}

void Game::on_entity_hit(Entity & entity) {
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

void Game::on_entity_death(Entity & entity) {    
    if (entity.transform) {
        if (entity.spawner) {
            spawnSmallEntities(entity.transform->position, *entity.spawner.get());
        }
        if (entity.pickup_spawner) {
            spawnPickup(entity.transform->position, *entity.pickup_spawner.get());
        }
    }

    if (entity.score_reward) {
        m_score += entity.score_reward->score;
        for (std::shared_ptr<Entity> score : m_entity_manager.get_entities(Tag::ScoreWindow)) {
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
    for (std::shared_ptr<Entity> player : m_entity_manager.get_entities(Tag::Player)) {
        if (!player->weapon || !player->special_weapon) {
            return;
        }
        switch (type) {
            case CWeaponPickup::PickupType::ShotSingle: {
                std::cout << "Red Single!\n";
                if (player->weapon->mode == CWeapon::FireMode::ShotSingle) {
                    if (player->weapon->power < 4) {
                        ++player->weapon->power;
                        ++player->special_weapon->power;
                    } else {
                        m_score += 500;
                    }
                } else {
                    player->weapon->power = 0;
                    player->weapon->mode = CWeapon::FireMode::ShotSingle;
                    player->special_weapon->mode = CSpecialWeapon::FireMode::SpecialExplosion;
                }
                break;
            }
            case CWeaponPickup::PickupType::ShotSpread: {
                std::cout << "Green Spread!\n";
                if (player->weapon->mode == CWeapon::FireMode::ShotSpread) {
                    ++player->weapon->power;
                    ++player->special_weapon->power;
                } else {
                    player->weapon->power = 0;
                    player->weapon->mode = CWeapon::FireMode::ShotSpread;
                    player->special_weapon->mode = CSpecialWeapon::FireMode::SpecialRotor;
                }
                break;
            }
            case CWeaponPickup::PickupType::ShotLaser: {
                if (player->weapon->mode == CWeapon::FireMode::ShotLaser) {
                    ++player->weapon->power;
                    ++player->special_weapon->power;
                } else {
                    player->weapon->power = 0;
                    player->weapon->mode = CWeapon::FireMode::ShotLaser;
                    player->special_weapon->mode = CSpecialWeapon::FireMode::SpecialFlamethrower;
                }
                break;
            }
            default: {
                break;
            }
        }

    }
}

void Game::spawnSmallEntities(const Vec2 & position, const CDeathSpawner & spawner) {
    const float d_angle = 360 / Vec2::rad_to_deg / spawner.amount;
    const CShape & prefab = spawner.prefab;
    const float radius = prefab.shape.getRadius();
    const int score = std::stoi(m_config["Enemy"]["smallScore"]);

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
        if (spawner.tag == Tag::Enemies) {
            enemy->score_reward = std::make_shared<CScoreReward>(CScoreReward(score));
        }
        if (spawner.recursion > 0) {
            enemy->spawner = std::make_shared<CDeathSpawner>(spawner.amount, prefab, spawner.lifespan, spawner.speed, spawner.recursion - 1, spawner.tag);
        }
    }
}

void Game::spawnPickup(const Vec2 & position, const CPickupSpawner & spawner) {
    const CWeaponPickup & prefab = spawner.payload;
    const CShape & shape = spawner.shape;
    const float radius = shape.shape.getRadius();

    const std::shared_ptr<Entity> pickup = m_entity_manager.add_entity(Tag::Pickups);
    pickup->name = std::make_shared<CName>(CName("Pickup"));
    pickup->transform = std::make_shared<CTransform>(CTransform(position.x, position.y));
    pickup->shape = std::make_shared<CShape>(CShape(shape));
    pickup->collider = std::make_shared<CCollider>(CCollider(radius));
    pickup->lifespan = std::make_shared<CLifespan>(CLifespan(spawner.lifespan));
    pickup->weapon_pickup = std::make_shared<CWeaponPickup>(CWeaponPickup(prefab));
}

void Game::sDamageReact(const sf::Time & deltaTime) {
    for (std::shared_ptr<Entity> entity : m_entity_manager.get_entities()) {
        if (entity->health) {
            CHealth & health = *entity->health.get();            
            if (entity->shape) {
                sf::CircleShape & shape = entity->shape->shape;
                const float scale = 1.f + (health.expansion - 1.f) * ((float)health.react_countdown / (float)health.react_duration);
                shape.setScale(scale, scale);
            }
        }
    }
}

void Game::sEnemySpawner(const sf::Time & deltaTime) {
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

void Game::sPlayerSpawner(const sf::Time & deltaTime) {
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

void Game::sGUI() {
    //Interface stuff
    if (m_game_close_countdown > 0) {
        ImGui::Begin("Quit game");
        ImGui::Text("Press [ESC] again to close game");
        ImGui::ProgressBar((float)m_game_close_countdown / (float)m_game_close_timeout);
        ImGui::End();
    }    

    ImGui::Begin("Geometry Wars");
    static std::vector<Vec2> vectors {};
    
    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_Reorderable;
    if (ImGui::BeginTabBar("TabBar", tab_bar_flags)) {

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

            ImGui::BeginChild("Entity list", {400.f, 0}, true);
            static int entity_idx = -1;
            static int entity_collection = -1;
            if (ImGui::BeginTabBar("Entity collections", ImGuiTabBarFlags_None)) {
                if (ImGui::BeginTabItem("All")) {
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
                for (auto & tag_r : tag_names) {
                    const std::string tag_name = tag_r.first;
                    const Tag tag = tag_r.second;
                    if (ImGui::BeginTabItem(tag_name.c_str())) {
                        if (ImGui::BeginTable("Entity table", 1)) {
                            const Entities & entities = m_entity_manager.get_entities(tag);
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
                    const Entities & entities = m_entity_manager.get_entities(Tag(entity_collection));
                    std::shared_ptr<Entity> entity;
                    if (entities.size() > entity_idx) {
                        entity = m_entity_manager.get_entities(Tag(entity_collection))[entity_idx];
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
                            sprintf(buf, "Transform: P(%.3f, %.3f), R%.3f, S%.3f##comp_transform", pos.x, pos.y, rot, scale);
                            if (ImGui::Selectable(buf, comp_idx == ComponentType::Transform)) {
                                comp_idx = ComponentType::Transform;
                            }
                        } else if (ImGui::Button("+Transform##add_transform")) {
                                const std::shared_ptr<CTransform> transform(new CTransform());
                                entity->transform = transform;
                        }
                        if (entity->velocity) {
                            const Vec2 vel = entity->velocity->velocity;
                            char buf[32];
                            sprintf(buf, "Velocity: (%.3f, %.3f)##comp_velocity", vel.x, vel.y);
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
                            sprintf(buf, "Player: L:%d/%d, Fl:%d##comp_player", 
                                lives, max_lives, flicker_freq);
                            if (ImGui::Selectable(buf, comp_idx == ComponentType::PlayerStats)) {
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
                            sprintf(buf, "Lifespan: %d/%d##comp_collider", countdown, duration);
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
                            sprintf(buf, "HP: %d/%d, Ex%.3f D%d/%d##comp_collider", hp, max_hp, expansion, duration, countdown);
                            if (ImGui::Selectable(buf, comp_idx == ComponentType::Health)) {
                                comp_idx = ComponentType::Health;
                            }
                        } else if (ImGui::Button("+Health##add_health")) {
                                entity->health = std::make_shared<CHealth>(CHealth());
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
            ImGui::ProgressBar((float)m_enemy_spawn_countdown / (float)m_enemy_spawn_interval);
            ImGui::DragInt("spawn interval", &m_enemy_spawn_interval, 10.f, 0, 1000);
            ImGui::EndGroup();

            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Weapons")) {
            std::shared_ptr<Entity> player;
            for (auto e : m_entity_manager.get_entities(Tag::Player)) {
                player = std::move(e);
            }
            if (player && player->weapon && player->special_weapon) {
                static CWeapon::FireMode & primary_mode = player->weapon->mode;
                static CSpecialWeapon::FireMode & secondary_mode = player->special_weapon->mode;
                static int & primary_power = player->weapon->power;
                static int & secondary_power = player->special_weapon->power;
                const char min = 0;
                const char max = 4;
                const std::vector<std::string> primary_modes {"Single", "Spread", "Laser"};
                const std::vector<std::string> secondary_modes {"Explosion", "Rotor", "Flamethrower"};
                
                if (ImGui::BeginChild("Primary##weapon-primary", {0, 0}, ImGuiChildFlags_FrameStyle | ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AutoResizeX)) {
                    if (ImGui::BeginCombo("##primary_select", primary_modes[primary_mode].c_str())) {
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
                    ImGui::SliderScalar("power", ImGuiDataType_U8, &primary_power, &min, &max, "%d");
                    ImGui::EndChild();
                }
                if (ImGui::BeginChild("Secondary##weapon-secondary", {0, 0}, ImGuiChildFlags_FrameStyle | ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AutoResizeX)) {
                    if (ImGui::BeginCombo("##secondary_select", secondary_modes[secondary_mode].c_str())) {
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
                    ImGui::SliderScalar("power", ImGuiDataType_U8, &secondary_power, &min, &max, "%d");
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
                ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Save config and restart to apply changes.");
            }
            if (ImGui::BeginChild("Window##config-window", {0, 0}, ImGuiChildFlags_FrameStyle | ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AutoResizeX)) {
                ImGui::SeparatorText("Window");
                static int res_idx = 0;
                const std::string current_res = "*"+m_config["Window"]["width"]+"x"+m_config["Window"]["height"];
                std::vector<std::string> resolutions {current_res, "2560x1440", "1920x1080", "1280x720", "1024x576", "960x540"};
                ImGui::Text("Resolution");
                if (ImGui::BeginCombo("##res_select", resolutions[res_idx].c_str())) {
                    for (int i = 0; i < resolutions.size(); ++i) {
                        const bool is_selected = (res_idx == i);
                        if (ImGui::Selectable(resolutions[i].c_str(), is_selected)) {
                            if (i != 0) {
                                res_warning = true;
                            } else {
                                res_warning = false;
                            }
                            res_idx = i;
                        }
                        if (is_selected) {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
                static int refresh_idx = 0;
                const std::string current_refresh = "*"+m_config["Window"]["refreshRate"];
                std::vector<std::string> refresh_rates {current_refresh, "144", "120", "60", "30"};
                ImGui::Text("Refresh");
                if (ImGui::BeginCombo("##refresh_select", refresh_rates[refresh_idx].c_str())) {
                    for (int i = 0; i < refresh_rates.size(); ++i) {
                        const bool is_selected = (refresh_idx == i);
                        if (ImGui::Selectable(refresh_rates[i].c_str(), is_selected)) {
                            if (i != 0) {
                                refresh_warning = true;
                            } else {
                                refresh_warning = false;
                            }
                            refresh_idx = i;
                        }
                        if (is_selected) {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
                ImGui::SameLine();
                const bool current_fullscreen = (bool)std::stoi(m_config["Window"]["fullscreen"]);
                static bool fullscreen = current_fullscreen; 
                if (ImGui::Checkbox("fullscreen", &fullscreen)) {
                    if (fullscreen != current_fullscreen) {
                        fullscreen_warning = true;
                    } else {
                        fullscreen_warning = false;
                    }
                }
                ImGui::EndChild();
            }

            ImGui::SameLine();
            
            if (ImGui::BeginChild("Font##config-font", {0, 0}, ImGuiChildFlags_FrameStyle | ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AutoResizeX)) {
                ImGui::SeparatorText("Font");
                static int font_s_idx = 0;
                const std::string current_font_s = "*"+m_config["Font"]["size"];
                std::vector<std::string> font_sizes {current_font_s, "16", "24", "32", "48", "60", "72"};
                ImGui::Text("Size");
                if (ImGui::BeginCombo("##font_size_select", font_sizes[font_s_idx].c_str())) {
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
            
            if (ImGui::BeginChild("Player##config-player", {700, 0}, ImGuiChildFlags_FrameStyle | ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AutoResizeX)) {
                ImGui::Text("Window");
                ImGui::EndChild();
            }
            
            if (ImGui::BeginChild("Bullet##config-bullet", {700, 0}, ImGuiChildFlags_FrameStyle | ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AutoResizeX)) {
                ImGui::Text("Window");
                ImGui::EndChild();
            }
            
            if (ImGui::BeginChild("Enemy##config-enemy", {700, 0}, ImGuiChildFlags_FrameStyle | ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AutoResizeX)) {
                ImGui::Text("Window");
                ImGui::EndChild();
            }
            
            ImGui::EndTabItem();
        }
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

Config Game::parse_tokens(const std::vector<std::string> & tokenstream) const {
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

const Config Game::read_file(const std::string & filename) const {
    //May throw, should be enclosed in try / catch expression
    std::vector<std::string> tokenstream {};
    std::string word;
    
    std::ifstream ifs(filename, std::ifstream::in);
    if (!ifs) {
        throw std::runtime_error("Could not read file");
    }
    while (ifs >> word) {
        tokenstream.push_back(word);
    }

    return parse_tokens(tokenstream);
}

void Game::export_config(Config & config, const std::string & filename) const {
    //May throw, should be enclosed in try / catch expression

    std::ofstream file;
    file.open(filename, std::ios::out);
    if (!file) {
        throw std::runtime_error("Could not create file");
    }

    // file << "something";

    std::for_each(
        config.begin(), 
        config.end(),
        [&](std::pair<std::string, std::map<std::string, std::string>> p)
        {
            file << p.first << " [\n";
            std::for_each(
                p.second.begin(), 
                p.second.end(),
                [&](std::pair<std::string, std::string> v)
                {   
                    file <<'\t' << v.first << "=" << v.second << ",\n";
                }
            );
            file << "],\n";
        }
    );

    file.close();
}