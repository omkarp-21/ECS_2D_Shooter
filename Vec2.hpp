#pragma once

class Vec2
{
public:
	float x = 0;
	float y = 0;

	Vec2();
	Vec2(float xin, float yin);

	bool operator == (const Vec2&) const;
	bool operator != (const Vec2&) const;

	Vec2 operator + (const Vec2&) const;
	Vec2 operator - (const Vec2&) const;
	Vec2 operator / (const Vec2&) const;
	Vec2 operator * (const Vec2&) const;

	void operator += (const Vec2&);
	void operator -= (const Vec2&);
	void operator *= (const float);
	void operator /= (const float);

	float dist(const Vec2&) const;
};