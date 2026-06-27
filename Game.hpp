#pragma once
#include <SFML/Graphics.hpp>
#include "Entity.hpp"
#include "EntityManager.hpp"

struct PlayerConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V; float S; };
struct EnemyConfig { int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI; float SMIN, SMAX; };
struct BulletConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L; float S; };
class Game
{
private:
	sf::RenderWindow m_window;
	sf::Font m_font;
	sf::Text m_text;

	EntityManager m_entities;

	PlayerConfig m_playerConfig;
	EnemyConfig m_enemyConfig;
	BulletConfig m_bulletConfig;

	std::shared_ptr<Entity> m_player;

	int m_score = 0;
	int m_currentFrame = 0;
	int m_lastEnemySpawnTime = 0;
	bool m_paused = false;
	bool m_running = true;


	void init(const std::string& filename);
	void setPaused(bool paused);
	
	// System Functions
	void sRender();
	void sCollision();
	void sMovement();
	void sUserInput();
	void sLifeSpan();
	void sEnemySpawner();

	//Helper Functions
	void spawnPlayer();
	void spawnEnemy();
	void spawnBullet(std::shared_ptr<Entity> &entity, const Vec2& mousePos);
	void spawnSmallEnemies(std::shared_ptr<Entity> entity);
	void spawnSpecialMove(std::shared_ptr<Entity> entity, const Vec2& mousePos);
	
public:
	Game(const std::string& filename);
	void run();
};