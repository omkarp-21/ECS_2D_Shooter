#pragma once
#include "Vec2.hpp"
#include <SFML/Graphics.hpp>
class CTransform
{
public:
	Vec2 pos = { 0,0 };
	Vec2 velocity = { 0,0 };
	float angle = 0;

	CTransform(const Vec2& inp_pos, const Vec2& inp_vel, const float& inp_ang)
		: pos(inp_pos), velocity(inp_vel), angle(inp_ang) { }
};

class CShape
{
public:
	sf::CircleShape circle;

	CShape(float radius, int points, const sf::Color& shape_color, const sf::Color& outline_color, float thickness)
		: circle(radius, points)
	{
		circle.setFillColor(shape_color);
		circle.setOutlineColor(outline_color);
		circle.setOutlineThickness(thickness);
		circle.setOrigin(radius, radius);
	}
};

class CCollision
{
public:
	float radius = 0;
	CCollision(float inp_rad)
		: radius(inp_rad) { }
};

class CScore
{
public:
	int score = 0;
	CScore(int inp_scr)
		: score(inp_scr) { }
};

class CLifespan
{
public:

	int remaining = 0;
	int total = 0;
	CLifespan(int inp_rem, int inp_ttl)
		: remaining(inp_rem), total(inp_ttl) { }
};

class CInput
{
public:
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;
	bool shoot = false;

	CInput() {}
};