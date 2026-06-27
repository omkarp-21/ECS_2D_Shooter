#pragma once
#include "Component.hpp"
#include <memory>
#include <string>

class Entity
{
private:
	friend class EntityManager;
	bool m_isActive = true;
	bool m_smallEnemy = false;
	size_t m_id = 0;
	std::string m_tag = "Default";

	Entity(const size_t inp_id, const std::string& inp_tag);

public:
	std::shared_ptr<CTransform> cTransform;
	std::shared_ptr<CShape>     cShape;
	std::shared_ptr<CCollision> cCollision;
	std::shared_ptr<CScore>     cScore;
	std::shared_ptr<CLifespan>  cLifespan;
	std::shared_ptr<CInput>     cInput;

	bool isActive() const;
	bool isSmallEnemy() const;
	void setSmallEnemy();
	const size_t id() const;
	const std::string& tag() const;
	void destroy();
};