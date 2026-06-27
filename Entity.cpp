#include "Entity.hpp"

Entity::Entity(const size_t inp_id, const std::string& inp_tag)
	: m_id(inp_id), m_tag(inp_tag) { }

bool Entity::isActive() const
{
	return m_isActive;
}
bool Entity::isSmallEnemy() const
{
	return m_smallEnemy;
}
void Entity::setSmallEnemy()
{
	m_smallEnemy = true;
}
const size_t Entity::id() const
{
	return m_id;
}

const std::string& Entity::tag() const
{
	return m_tag;
}

void Entity::destroy()
{
	m_isActive = false;
}