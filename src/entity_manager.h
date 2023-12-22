#pragma once
#include "entity.h"
#include <map>
#include <memory>

typedef std::vector<std::shared_ptr<Entity>> Entities;

class EntityManager {
	Entities m_entities {};    
	Entities m_scheduled {};
	std::map<Tag, Entities> m_tag_entities {};
	size_t m_entity_count = 0;
	Entities m_to_add {};
public:
	EntityManager() {};
	~EntityManager() {};
	const Entities & get_entities() const;
	const Entities & get_entities(const Tag tag);
	const std::shared_ptr<Entity> add_entity(const Tag tag);
	const std::shared_ptr<Entity> schedule(const Tag tag);
	const bool pop_schedule();
	void update();
};