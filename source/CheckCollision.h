#ifndef COLLISION_H
#define COLLISION_H

#include <cmath>
#include <SFML/Graphics.hpp>

#define BIG_NUM 9999999


sf::Vector2f getAxis(sf::Vector2f point_a, sf::Vector2f point_b)
{
	sf::Vector2f axis = point_a - point_b;
	// Make it a unit vector
	float unit_len = std::sqrt(axis.x * axis.x + axis.y * axis.y);
	axis.x /= unit_len;
	axis.y /= unit_len;

	return axis;
}

float getDistance(sf::Vector2f point_a, sf::Vector2f point_b)
{
	float dx = std::max(point_a.x, point_b.x) - std::min(point_a.x, point_b.x);
	float dy = std::max(point_a.y, point_b.y) - std::min(point_a.y, point_b.y);
	return std::sqrt(dx * dx + dy * dy);
}

void projectPoint(sf::Vector2f& limit, sf::Vector2f point, sf::Vector2f axis)
{
	// Find the dot product
	float projection = (axis.x * point.x + axis.y * point.y) / (axis.x * axis.x + axis.y * axis.y);
	// Get the min and max values
	limit.x = std::min(projection, limit.x);
	limit.y = std::max(projection, limit.y);
}

bool checkPolyVSPoly(const sf::Shape* shape_a, const sf::Shape* shape_b, sf::Vector3f& overlap)
{
	sf::Vector2f axis, point_a, point_b, limit_a, limit_b;
	float delta;

	unsigned int count_a = shape_a->getPointCount();
	unsigned int count_b = shape_b->getPointCount();

	// Check pionts of shape b against axis of shape a
	for(unsigned int i = 0; i < count_a; i++)
	{
		// Get face
		point_a = shape_a->getTransform().transformPoint(shape_a->getPoint(i));
		point_b = shape_a->getTransform().transformPoint(shape_a->getPoint((i + 1) % count_a));
		// Get projection axis
		axis = getAxis(point_a, point_b);
		axis = sf::Vector2f(-axis.y, axis.x);
		limit_a = sf::Vector2f(BIG_NUM, -BIG_NUM);
		limit_b = sf::Vector2f(BIG_NUM, -BIG_NUM);
		// Project all points in shape a
		for(unsigned int j = 0; j < count_a; j++)
			projectPoint(limit_a, shape_a->getTransform().transformPoint(shape_a->getPoint(j)), axis);
		// Project all points in shape b
		for(unsigned int j = 0; j < count_b; j++)
			projectPoint(limit_b, shape_b->getTransform().transformPoint(shape_b->getPoint(j)), axis);
		// Overlap found, find solution 
		delta = limit_b.x - limit_a.y;
		// No overlap found
		if(delta >= 0)
			return false;

		if(delta > overlap.z)
			overlap = sf::Vector3f(axis.x, axis.y, delta);
	}

	// Check pionts of shape a against axis of shape b
	for(unsigned int i = 0; i < count_b; i++)
	{
		// Get face
		point_a = shape_b->getTransform().transformPoint(shape_b->getPoint(i));
		point_b = shape_b->getTransform().transformPoint(shape_b->getPoint((i + 1) % count_b));
		// Get projection axis
		axis = getAxis(point_a, point_b);
		axis = sf::Vector2f(-axis.y, axis.x);
		limit_a = sf::Vector2f(BIG_NUM, -BIG_NUM);
		limit_b = sf::Vector2f(BIG_NUM, -BIG_NUM);
		// Project all points in shape a
		for(unsigned int j = 0; j < count_a; j++)
			projectPoint(limit_a, shape_a->getTransform().transformPoint(shape_a->getPoint(j)), axis);
		// Project all points in shape b
		for(unsigned int j = 0; j < count_b; j++)
			projectPoint(limit_b, shape_b->getTransform().transformPoint(shape_b->getPoint(j)), axis);
		// Overlap found, find solution 
		delta = limit_b.x - limit_a.y;
		// No overlap found
		if(delta >= 0)
			return false;

		if(delta > overlap.z)
			overlap = sf::Vector3f(axis.x, axis.y, delta);
	}

	return true;
}

bool checkPolyVSCirc(const sf::Shape* shape_a, const sf::Shape* shape_b, sf::Vector3f& overlap)
{
	float delta = 0.0f, radius = ((sf::CircleShape*)shape_b)->getRadius();
	sf::Vector2f axis, limit_a, limit_b, center;
	sf::Vector2f closest = sf::Vector2f(BIG_NUM, -BIG_NUM);

	center.x = shape_b->getGlobalBounds().left + radius;
	center.y = shape_b->getGlobalBounds().top + radius;

	sf::Vector2f point_c(center.x, closest.y);

	// Check polygon regions for circle center
	unsigned int count = shape_a->getPointCount(), cross = 0;
	
	for(unsigned int i = 0; i < count; i++)
	{
		// Get face
		sf::Vector2f point_a = shape_a->getTransform().transformPoint(shape_a->getPoint(i));
		sf::Vector2f point_b = shape_a->getTransform().transformPoint(shape_a->getPoint((i + 1) % count));

		sf::Vector2f vec_a = point_a - point_b;
		sf::Vector2f vec_b = point_c - center;

		float d = (vec_a.x * vec_b.y) - (vec_a.y * vec_b.x);
		float u = ((center.x - point_b.x) * vec_a.y - (center.y - point_b.y) * vec_a.x) / d;
		float t = ((center.x - point_b.x) * vec_b.y - (center.y - point_b.y) * vec_b.x) / d;

		// Is the circle inside the polygon?
		if(u >= 0 && u <= 1 && t >= 0 && t <= 1 && 
		   point_a + sf::Vector2f(vec_a.x * t, vec_a.y * t) != sf::Vector2f(0, 0))
			cross++;

		// Get projection axis
		axis = getAxis(point_a, point_b);
		limit_a = sf::Vector2f(BIG_NUM, -BIG_NUM);
		limit_b = sf::Vector2f(BIG_NUM, -BIG_NUM);
		projectPoint(limit_a, point_a, axis);
		projectPoint(limit_a, point_b, axis);
		projectPoint(limit_b, center, axis);

		// Is the circle colliding at a face?
		if(limit_b.x <= limit_a.y && limit_b.x >= limit_a.x)
		{
			delta = limit_b.x - limit_a.x;
			point_b = point_b + sf::Vector2f(axis.x * delta, axis.y * delta);

			if(getDistance(center, closest) > getDistance(center, point_b))
				closest = point_b;
		}

		// Is this current point the nearest point?
		else if(getDistance(center, closest) > getDistance(center, point_a))
			closest = point_a;
	}

	// If the circle is not inside the polygon
	if(cross % 2 == 0)
	{
		axis = getAxis(center, closest);
		delta = getDistance(closest, center);

		if(delta >= radius)
			return false;

		delta -= radius;
	}
	else
	{
		axis = getAxis(center, closest);
		delta = getDistance(closest, center + sf::Vector2f(axis.x * radius, axis.y * radius));
	}

	overlap = sf::Vector3f(axis.x, axis.y, delta);

	return true;
}

bool checkCircVSCirc(const sf::Shape* shape_a, const sf::Shape* shape_b, sf::Vector3f& overlap)
{
	float radius_a = ((sf::CircleShape*)shape_a)->getRadius();
	float radius_b = ((sf::CircleShape*)shape_b)->getRadius();

	sf::Vector2f center_a(shape_a->getGlobalBounds().left + radius_a, shape_a->getGlobalBounds().top + radius_a);
	sf::Vector2f center_b(shape_b->getGlobalBounds().left + radius_b, shape_b->getGlobalBounds().top + radius_b);
	
	float delta = getDistance(center_a, center_b);

	if(delta >= radius_a + radius_b)
		return false;

	delta -= radius_a + radius_b;

	if(delta == -(radius_a + radius_b))
		center_b -= sf::Vector2f(-10.0f, -10.0f);

	sf::Vector2f axis = getAxis(center_b, center_a);
	overlap = sf::Vector3f(axis.x, axis.y, delta);

	return true;
}


#endif
