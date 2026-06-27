#include "Game.hpp"
#include <fstream>
#include <iostream>


Game::Game(const std::string& filename)
{
	init(filename);
}

void Game::init(const std::string& filename)
{
	std::ifstream fin(filename);
	std::string type;

	// Window
	unsigned int wFrameLimit = 60;
	unsigned int wWidth = 0, wHeight = 0;
	int wFullScreen = 0;

	// Font & Text
	std::string fontFile;
	unsigned int textR = 0, textG = 0, textB = 0;
	unsigned textCharacterSize = 0;

	while (fin >> type)
	{
		if (type == "Window")
		{
			fin >> wWidth >> wHeight >> wFrameLimit >> wFullScreen;
		}
		else if (type == "Font")
		{
			fin >> fontFile >> textCharacterSize >> textR >> textG >> textB;
		}
		else if (type == "Player")
		{
			fin >> m_playerConfig.SR >> m_playerConfig.CR >> m_playerConfig.S >> m_playerConfig.FR >> m_playerConfig.FG
				>> m_playerConfig.FB >> m_playerConfig.OR >> m_playerConfig.OG >> m_playerConfig.OB >> m_playerConfig.OT
				>> m_playerConfig.V;
		}
		else if (type == "Enemy")
		{
			fin >> m_enemyConfig.SR >> m_enemyConfig.CR >> m_enemyConfig.SMIN >> m_enemyConfig.SMAX >> m_enemyConfig.OR
				>> m_enemyConfig.OG >> m_enemyConfig.OB >> m_enemyConfig.OT >> m_enemyConfig.VMIN >> m_enemyConfig.VMAX
				>> m_enemyConfig.L >> m_enemyConfig.SI;
		}
		else if (type == "Bullet")
		{
			fin >> m_bulletConfig.SR >> m_bulletConfig.CR >> m_bulletConfig.S >> m_bulletConfig.FR >> m_bulletConfig.FG
				>> m_bulletConfig.FB >> m_bulletConfig.OR >> m_bulletConfig.OG >> m_bulletConfig.OB >> m_bulletConfig.OT
				>> m_bulletConfig.V >> m_bulletConfig.L;
		}
	}

	// Init Window
	m_window.create(sf::VideoMode(wWidth, wHeight), "Assignment 2"/*, (wFullScreen ? sf::Style::Fullscreen : sf::Style::Default)*/);
	m_window.setFramerateLimit(wFrameLimit);

	// Init Font
	if (!m_font.loadFromFile(fontFile))
	{
		std::cerr << "Could Not load Font!\n";
		exit(-1);
	}

	// Init Text
	m_text.setCharacterSize(textCharacterSize);
	m_text.setFillColor(sf::Color(textR, textG, textB));
	m_text.setFont(m_font);
	spawnPlayer();
}

void Game::setPaused(bool paused)
{
	m_paused = paused;
}

void Game::sRender()
{
	m_window.clear();

	for (auto& e : m_entities.getEntities())
	{
		e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);

		e->cTransform->angle += 1.0f;
		e->cShape->circle.setRotation(e->cTransform->angle);

		m_window.draw(e->cShape->circle);
	}
	m_text.setString("Score : " + std::to_string(m_score));
	m_text.setScale(2.0f, 2.0f);
	m_window.draw(m_text);
	m_window.display();
}

void Game::sCollision()
{
	// Player-Enemy Collision
	for (auto& e : m_entities.getEntities("Enemy"))
	{
		float distSquared = (m_player->cTransform->pos.x - e->cTransform->pos.x) * (m_player->cTransform->pos.x - e->cTransform->pos.x) +
						    (m_player->cTransform->pos.y - e->cTransform->pos.y) * (m_player->cTransform->pos.y - e->cTransform->pos.y);
		if (distSquared <= (m_playerConfig.CR + m_enemyConfig.CR) * (m_playerConfig.CR + m_enemyConfig.CR))
		{
			m_player->cTransform->pos.x = m_window.getSize().x / 2.0f;
			m_player->cTransform->pos.y = m_window.getSize().y / 2.0f;

			if(!e->isSmallEnemy()) spawnSmallEnemies(e);
			e->destroy();
		}
	}

	// Bullet-Enemy Collision
	for (auto& e : m_entities.getEntities("Enemy"))
	{
		for (auto& b : m_entities.getEntities("Bullet"))
		{
			float distSquared = (b->cTransform->pos.x - e->cTransform->pos.x) * (b->cTransform->pos.x - e->cTransform->pos.x) +
				(b->cTransform->pos.y - e->cTransform->pos.y) * (b->cTransform->pos.y - e->cTransform->pos.y);
			if (distSquared <= (m_bulletConfig.CR + m_enemyConfig.CR) * (m_bulletConfig.CR + m_enemyConfig.CR))
			{
				m_score += e->cScore->score;
				if (!e->isSmallEnemy()) spawnSmallEnemies(e);
				b->cLifespan->remaining = 0;
				e->destroy();
			}
		}
		for (auto& sb : m_entities.getEntities("SpecialBullet"))
		{
			float distSquared = (sb->cTransform->pos.x - e->cTransform->pos.x) * (sb->cTransform->pos.x - e->cTransform->pos.x) +
				(sb->cTransform->pos.y - e->cTransform->pos.y) * (sb->cTransform->pos.y - e->cTransform->pos.y);
			if (distSquared <= (m_bulletConfig.CR + m_enemyConfig.CR) * (m_bulletConfig.CR + m_enemyConfig.CR))
			{
				m_score += e->cScore->score;
				if (!e->isSmallEnemy()) spawnSmallEnemies(e);
				//sb->cLifespan->remaining = 0;
				e->destroy();
			}
		}
	}
	// Special Move -- Ricochet Bullet
	for (auto& e : m_entities.getEntities("SpecialBullet"))
	{
		if (((e->cTransform->pos.x - e->cShape->circle.getRadius()) <= 0) ||
			((e->cTransform->pos.x + e->cShape->circle.getRadius()) >= m_window.getSize().x))
		{
			e->cTransform->velocity.x *= -1;
		}

		if (((e->cTransform->pos.y - e->cShape->circle.getRadius()) <= 0) ||
			((e->cTransform->pos.y + e->cShape->circle.getRadius()) >= m_window.getSize().y))
		{
			e->cTransform->velocity.y *= -1;
		}
	}
}

void Game::sMovement()
{
	// Player
	bool leftBound = ((m_player->cTransform->pos.x - m_player->cShape->circle.getRadius()) > 0);
	bool rightBound = ((m_player->cTransform->pos.x + m_player->cShape->circle.getRadius()) < m_window.getSize().x);
	bool upBound = ((m_player->cTransform->pos.y - m_player->cShape->circle.getRadius()) > 0);
	bool downBound = ((m_player->cTransform->pos.y + m_player->cShape->circle.getRadius()) < m_window.getSize().y);

	m_player->cTransform->velocity = { 0,0 };
	if (m_player->cInput->up && m_player->cInput->left && upBound && leftBound)
	{
		m_player->cTransform->velocity = Vec2(m_playerConfig.S * -cos(atan2(1, 1)), m_playerConfig.S * -sin(atan2(1, 1)));
	}
	else if (m_player->cInput->up && m_player->cInput->right && upBound && rightBound)
	{
		m_player->cTransform->velocity = Vec2(m_playerConfig.S * cos(atan2(1, 1)), m_playerConfig.S * -sin(atan2(1, 1)));
	}
	else if (m_player->cInput->down && m_player->cInput->left && downBound && leftBound)
	{
		m_player->cTransform->velocity = Vec2(m_playerConfig.S * -cos(atan2(1, 1)), m_playerConfig.S * sin(atan2(1, 1)));
	}
	else if (m_player->cInput->down && m_player->cInput->right && downBound && rightBound)
	{
		m_player->cTransform->velocity = Vec2(m_playerConfig.S * cos(atan2(1, 1)), m_playerConfig.S * sin(atan2(1, 1)));
	}

	else if (m_player->cInput->up && upBound) m_player->cTransform->velocity.y = -m_playerConfig.S;
	else if (m_player->cInput->down && downBound) m_player->cTransform->velocity.y = m_playerConfig.S;
	else if (m_player->cInput->left && leftBound) m_player->cTransform->velocity.x = -m_playerConfig.S;
	else if (m_player->cInput->right && rightBound) m_player->cTransform->velocity.x = m_playerConfig.S;

	for (auto& e : m_entities.getEntities("Enemy"))
	{
		if (((e->cTransform->pos.x - e->cShape->circle.getRadius()) <= 0) ||
			((e->cTransform->pos.x + e->cShape->circle.getRadius()) >= m_window.getSize().x))
		{
			e->cTransform->velocity.x *= -1;
		}

		if (((e->cTransform->pos.y - e->cShape->circle.getRadius()) <= 0) ||
			((e->cTransform->pos.y + e->cShape->circle.getRadius()) >= m_window.getSize().y))
		{
			e->cTransform->velocity.y *= -1;
		}
	}

	for (auto& e : m_entities.getEntities())
	{
		e->cTransform->pos += e->cTransform->velocity;
	}

}

void Game::sUserInput()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		if (m_paused)
		{
			if (event.type == sf::Event::Closed)
			{
				m_running = false;
			}
			else if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::P) setPaused(!m_paused);
			}
		}
		else if (event.type == sf::Event::Closed)
		{
			m_running = false;
		}
		else if (event.type == sf::Event::KeyPressed)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::Escape:
				m_running = false;
				break;
			case sf::Keyboard::P:
				setPaused(!m_paused);
				break;
			case sf::Keyboard::W:
				m_player->cInput->up = true;
				break;
			case sf::Keyboard::A:
				m_player->cInput->left = true;
				break;
			case sf::Keyboard::S:
				m_player->cInput->down = true;
				break;
			case sf::Keyboard::D:
				m_player->cInput->right = true;
				break;
			default: break;
			}
		}
		else if (event.type == sf::Event::KeyReleased)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::W:
				m_player->cInput->up = false;
				break;
			case sf::Keyboard::A:
				m_player->cInput->left = false;
				break;
			case sf::Keyboard::S:
				m_player->cInput->down = false;
				break;
			case sf::Keyboard::D:
				m_player->cInput->right = false;
				break;
			default: break;
			}
		}
		else if (event.type == sf::Event::MouseButtonPressed)
		{
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				spawnBullet(m_player, Vec2(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y)));
			}
			else if (event.mouseButton.button == sf::Mouse::Right)
			{
				spawnSpecialMove(m_player, Vec2(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y)));
			}
		}
	}
}

void Game::sLifeSpan()
{
	for (auto& e : m_entities.getEntities())
	{
		if (e->cLifespan != nullptr)
		{
			if(e->cLifespan->remaining > 0) e->cLifespan->remaining--;

			if (e->isActive())
			{
				sf::Color eFillColor = e->cShape->circle.getFillColor();
				sf::Color eOutlineColor = e->cShape->circle.getOutlineColor();
				unsigned int eAlpha = (e->cLifespan->remaining / (e->cLifespan->total * 1.0f)) * 255;

				e->cShape->circle.setFillColor(sf::Color(eFillColor.r, eFillColor.g, eFillColor.b, eAlpha));
				e->cShape->circle.setOutlineColor(sf::Color(eOutlineColor.r, eOutlineColor.g, eOutlineColor.b, eAlpha));
			}

			if (e->cLifespan->remaining <= 0)
			{
				e->destroy();
			}
		}
	}
}

void Game::sEnemySpawner()
{
	if ((m_currentFrame - m_lastEnemySpawnTime) == m_enemyConfig.SI)
	{
		spawnEnemy();
	}
}

void Game::spawnPlayer()
{
	auto entity = m_entities.addEntity("Player");
	float wMidX = m_window.getSize().x / 2.0f;
	float wMidY = m_window.getSize().y / 2.0f;

	entity->cTransform = std::make_shared<CTransform>(Vec2(wMidX, wMidY), Vec2(0.0f, 0.0f), 0);
	
	entity->cShape = std::make_shared<CShape>((m_playerConfig.SR), m_playerConfig.V, sf::Color(m_playerConfig.FR, 
		m_playerConfig.FG, m_playerConfig.FB), sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB), 
		m_playerConfig.OT);

	entity->cCollision = std::make_shared<CCollision>(m_playerConfig.CR);

	entity->cInput = std::make_shared<CInput>();

	m_player = entity;
}

void Game::spawnEnemy()
{
	auto enemy = m_entities.addEntity("Enemy");
	// Randomize Enemy Spawn Properties

	unsigned int enemyVertices = m_enemyConfig.VMIN + rand() % (m_enemyConfig.VMAX - m_enemyConfig.VMIN + 1);

	float enemySpeed = m_enemyConfig.SMIN + rand() % static_cast<unsigned int>(m_enemyConfig.SMAX - m_enemyConfig.SMIN + 1);
	float enemyVelAngle = (rand() % 360) * ((4 * atan2(1, 1)) / 180);

	unsigned int enemyFillColorR = rand() % 256;
	unsigned int enemyFillColorG = rand() % 256;
	unsigned int enemyFillColorB = rand() % 256;

	float enemyPosX = m_enemyConfig.SR + rand() % (m_window.getSize().x - 2 * m_enemyConfig.SR + 1);
	float enemyPosY = m_enemyConfig.SR + rand() % (m_window.getSize().y - 2 * m_enemyConfig.SR + 1);
	
	enemy->cShape = std::make_shared<CShape>(m_enemyConfig.SR, enemyVertices, 
		sf::Color(enemyFillColorR, enemyFillColorG, enemyFillColorB),
		sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB), m_enemyConfig.OT);

	enemy->cTransform = std::make_shared<CTransform>(Vec2(enemyPosX, enemyPosY), 
		Vec2(enemySpeed*cos(enemyVelAngle), enemySpeed * sin(enemyVelAngle)), 0.0f);

	enemy->cCollision = std::make_shared<CCollision>(m_enemyConfig.CR);

	unsigned int enemyScore = enemyVertices * 100;
	enemy->cScore = std::make_shared<CScore>(enemyScore);

	m_lastEnemySpawnTime = m_currentFrame;
}

void Game::spawnBullet(std::shared_ptr<Entity>& entity, const Vec2& mousePos)
{
	auto bullet = m_entities.addEntity("Bullet");
	bullet->cShape = std::make_shared<CShape>(m_bulletConfig.SR, m_bulletConfig.V, sf::Color(m_bulletConfig.FR
		, m_bulletConfig.FG, m_bulletConfig.FR), sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB)
		, m_bulletConfig.OT);

	bullet->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);

	bullet->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L, m_bulletConfig.L);

	// Bullet Velocity Math
	float bulletYComponent = mousePos.y - m_player->cShape->circle.getPosition().y;
	float bulletXComponent = mousePos.x - m_player->cShape->circle.getPosition().x;
	float bulletAngle = atan2(bulletYComponent, bulletXComponent);

	bullet->cTransform = std::make_shared<CTransform>(m_player->cTransform->pos, 
		Vec2(m_bulletConfig.S*cos(bulletAngle), m_bulletConfig.S*sin(bulletAngle)), 0.0f);
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> ent)
{
	float mul = 0;
	EntityVec smallEnemies(ent->cShape->circle.getPointCount());
	for (auto& e : smallEnemies)
	{
		Vec2 ePos = { ent->cTransform->pos.x, ent->cTransform->pos.y };
		float eAngle = ent->cTransform->angle;
		float eRadius = ent->cShape->circle.getRadius() / 2.0f;
		float eCollisionRadius = ent->cCollision->radius;
		float eThickness = ent->cShape->circle.getOutlineThickness();
		unsigned int eVertices = ent->cShape->circle.getPointCount();

		// Velocity Math
		float enemySpeed = sqrtf(ent->cTransform->velocity.x * ent->cTransform->velocity.x +
								 ent->cTransform->velocity.y * ent->cTransform->velocity.y);

		float eVelAngle = (360.0f / eVertices * mul++) * (4 * atan2(1, 1) / 180.0f);
		Vec2 eVel = { enemySpeed * cos(eVelAngle), enemySpeed * sin(eVelAngle) };
		

		e = m_entities.addEntity("Enemy");
		e->setSmallEnemy();
		e->cTransform = std::make_shared<CTransform>(ePos, eVel, eAngle);
		e->cShape = std::make_shared<CShape>(eRadius, eVertices, ent->cShape->circle.getFillColor(), ent->cShape->circle.getOutlineColor(),
											 eThickness);
		e->cCollision = std::make_shared<CCollision>(eCollisionRadius);
		e->cScore = std::make_shared<CScore>(2 * ent->cScore->score);
		e->cLifespan = std::make_shared<CLifespan>(m_enemyConfig.L, m_enemyConfig.L);
	}

}

void Game::spawnSpecialMove(std::shared_ptr<Entity> e, const Vec2& mousePos)
{
	auto bullet = m_entities.addEntity("SpecialBullet");
	bullet->cShape = std::make_shared<CShape>(m_bulletConfig.SR, m_bulletConfig.V, sf::Color(m_bulletConfig.FR
		, m_bulletConfig.FG, m_bulletConfig.FR), sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB)
		, m_bulletConfig.OT);

	bullet->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);

	bullet->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L, m_bulletConfig.L);

	// Bullet Velocity Math
	float bulletYComponent = mousePos.y - m_player->cShape->circle.getPosition().y;
	float bulletXComponent = mousePos.x - m_player->cShape->circle.getPosition().x;
	float bulletAngle = atan2(bulletYComponent, bulletXComponent);

	bullet->cTransform = std::make_shared<CTransform>(m_player->cTransform->pos,
		Vec2(m_bulletConfig.S * cos(bulletAngle), m_bulletConfig.S * sin(bulletAngle)), 0.0f);
}

void Game::run()
{
	while (m_running)
	{
		m_entities.update();

		if (!m_paused)
		{
			sEnemySpawner();
			sMovement();
			sCollision();
			sLifeSpan();
			m_currentFrame++;
		}
		sUserInput();
		sRender();
	}
}