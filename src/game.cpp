#include "game.h"
#include "vec2.h"

void Game::run() {
    size_t width = 1920;
    size_t height = 1080;
    size_t framerate = 60;
    sf::RenderWindow window(sf::VideoMode(width, height), "ImGUI + SFML = <3");
    window.setFramerateLimit(framerate);
    ImGui::SFML::Init(window);
    sf::Clock deltaClock;
    sf::CircleShape circle;
    circle.setPosition(100, 100);
    circle.setRadius(200);
    circle.setFillColor(sf::Color::Green);
    std::vector<Vec2> vectors {};
    while (window.isOpen()) {

        m_entity_manager.update();

        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());
        
        //Interface stuff
        ImGui::Begin("Geometry Wars");
        
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
                        const Vec2 sum = vectors[l_idx].clone() + vectors[r_idx];
                        const Vec2 mul_l = vectors[l_idx].clone() * mult;
                        const Vec2 mul_r = vectors[r_idx].clone() * mult;
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
                    }
                    if (entity->velocity) {
                        const float vel = entity->velocity->velocity;
                        char buf[32];
                        sprintf(buf, "Velocity: %.3f##comp_velocity", vel);
                        if (ImGui::Selectable(buf, comp_idx == ComponentType::Velocity)) {
                            comp_idx = ComponentType::Velocity;
                        }
                    }
                }
                ImGui::EndChild();
                
                ImGui::SameLine();

                ImGui::BeginChild("Component add", {200.f, 0});
                if (entity_idx >= 0) {
                    std::vector<std::string> comps;
                    for (auto const & v : name_components) {
                        comps.push_back(v.first);
                    }
                    static int add_comp_idx = 0;
                    const std::string comp_preview = comps[add_comp_idx];
                    if (ImGui::BeginCombo("##comp_select", comp_preview.c_str())) {
                    for (int i = 0; i < comps.size(); ++i) {
                            const bool is_selected = (add_comp_idx == i);
                            if (ImGui::Selectable(comps[i].c_str(), is_selected)) {
                                add_comp_idx = i;
                            }
                            if (is_selected) {
                                ImGui::SetItemDefaultFocus();
                            }
                    }
                    ImGui::EndCombo();
                    }
                    if (ImGui::Button("add##add_component")) {
                        const std::shared_ptr<Entity> entity = m_entity_manager.get_entities()[entity_idx];
                        const ComponentType type = name_components[comps[add_comp_idx]];
                        switch (type) {
                            case ComponentType::Transform: {
                                entity->transform = std::make_shared<CTransform>(CTransform());
                                break;
                            }
                            case ComponentType::Velocity: {
                                entity->velocity = std::make_shared<CVelocity>(CVelocity());
                                break;
                            }
                            case ComponentType::Name: {
                                entity->name = std::make_shared<CName>(CName());
                                break;
                            }
                            default: {}
                        }
                    }

                }
                
                ImGui::EndChild();

                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::End();
        

        window.clear();
        // Render stuff
        window.draw(circle);
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
}

void Game::sGUI() {

}