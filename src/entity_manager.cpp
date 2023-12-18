#include "entity_manager.h"
#include <algorithm>

const Entities & EntityManager::get_entities() const {
    return m_entities;
}

const Entities & EntityManager::get_entities(const Tag tag)  { 
    return m_tag_entities[tag];
}

const std::shared_ptr<Entity> EntityManager::add_entity(const Tag tag) {
    const std::shared_ptr<Entity> entity = std::shared_ptr<Entity>(new Entity(tag, m_entity_count++));
    m_to_add.push_back(entity);
    return entity;
}

void EntityManager::update() {
    for (auto const entity : m_to_add) {
        m_entities.push_back(entity);
        const Tag tag = entity->tag();
        auto const search = m_tag_entities.find(tag);
        if (search == m_tag_entities.end()) {
            m_tag_entities[tag] = {};
        }
        m_tag_entities[tag].push_back(entity);
    }

    m_to_add.clear();

    auto pend = std::remove_if(m_entities.begin(), m_entities.end(), 
        [](const std::shared_ptr<Entity> e) 
        { return !e->is_alive(); });
    m_entities.erase(pend, m_entities.end());

    for (auto & collection : m_tag_entities) {
        auto t_pend = std::remove_if(collection.second.begin(), collection.second.end(),
            [](const std::shared_ptr<Entity> e)
            { return !e->is_alive(); });
        collection.second.erase(t_pend, collection.second.end());
    }
}