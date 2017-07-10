#ifndef GAMEOBJECTS_H
#define GAMEOBJECTS_H

#include "PolygonShape.h"


enum ShapeType
{
	POLYGON,
	CIRCLE
};

struct CollisionObject
{
	// Circle Constructor
	CollisionObject(float radius)
	{
		m_type = ShapeType::CIRCLE;
		m_shape = new sf::CircleShape(radius);
		m_shape->setFillColor(sf::Color::Transparent);
		m_shape->setOutlineThickness(1.0f);
	}

	// Polygon Constructor
	CollisionObject(float width, float height, unsigned int points)
	{
		m_type = ShapeType::POLYGON;
		m_shape = new sf::PolygonShape(sf::Vector2f(width, height), points);
		m_shape->setFillColor(sf::Color::Transparent);
		m_shape->setOutlineThickness(1.0f);
	}

	~CollisionObject() {delete m_shape;}


	virtual void loop() = 0;
	virtual void handleCollision(CollisionObject* other, sf::Vector3f overlap = sf::Vector3f()) = 0;


	sf::Shape* m_shape;
	ShapeType m_type;
};

struct Player : public CollisionObject
{
	Player(float width, float height, unsigned int points) : CollisionObject(width, height, points)
	{
		m_shape->setOutlineColor(sf::Color(218, 247, 166));
	}

	void loop()
	{
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			m_shape->move(0.0f, -5.0f);
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			m_shape->move(0.0f, 5.0f);
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			m_shape->move(-5.0f, 0.0f);
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			m_shape->move(5.0f, 0.0f);

		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
			m_shape->rotate(3.0f);
	}

	void handleCollision(CollisionObject* other, sf::Vector3f overlap = sf::Vector3f())
	{
		// Do collision handling in here
	}
};

struct RandomShape : public CollisionObject
{
	RandomShape(float radius) : CollisionObject(radius)
	{
		m_shape->setOutlineColor(sf::Color(218, 247, 166));
		m_rotate = rand() % 5 * (rand() % 2 == 0 ? -1 : 1);
	}

	RandomShape(float width, float height, unsigned int points) : CollisionObject(width, height, points)
	{
		m_shape->setOutlineColor(sf::Color(218, 247, 166));
		m_rotate = rand() % 5 * (rand() % 2 == 0 ? -1 : 1);
	}

	void loop()
	{
		if(m_type != ShapeType::CIRCLE)
			m_shape->rotate(m_rotate);
	}

	void handleCollision(CollisionObject* other, sf::Vector3f overlap = sf::Vector3f())
	{
		// Move away from collision
		m_shape->move(overlap.x * overlap.z, overlap.y * overlap.z);
	}

	float m_rotate;
};


#endif
