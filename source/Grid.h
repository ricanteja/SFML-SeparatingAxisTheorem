#ifndef GRID_H
#define GRID_H

#include <vector>
#include <SFML/Graphics.hpp>

#include "CheckCollision.h"
#include "CollisionObject.h"


struct GridCell
{
	sf::FloatRect m_rect;
	std::vector<CollisionObject*> m_objects;
};

class Grid : public sf::Drawable
{
public:
	Grid(sf::Vector2u dimensions, sf::Vector2u divisions = sf::Vector2u(10, 10))
	{
		m_divisions = divisions;
		m_dimensions = dimensions;
		float cell_width = static_cast<float>(dimensions.x) / divisions.x;
		float cell_height = static_cast<float>(dimensions.y) / divisions.y;

		//Create Grid
		m_cells = new GridCell[divisions.x * divisions.y];

		for(unsigned int i = 0; i < divisions.y; i++)
		{
			for(unsigned int j = 0; j < divisions.x; j++)
			{
				GridCell* cell = &m_cells[(i * divisions.x) + j];

				cell->m_rect.top = i * cell_height;
				cell->m_rect.left = j * cell_width;
				cell->m_rect.width = cell_width;
				cell->m_rect.height = cell_height;
			}
		}

		// Setup vertex arrays for drawing the grid and cells
		m_verts_lines = sf::VertexArray(sf::Lines, (divisions.x - 1) * 2 + (divisions.y - 1) * 2);
		m_verts_quads = sf::VertexArray(sf::Quads, divisions.x * divisions.y * 4);

		// Setup vertical lines
		float pos_delta = 0;
		for(unsigned int i = 0; i < (divisions.x - 1) * 2; i++)
		{
			if(i % 2 == 0)
			{
				pos_delta += static_cast<float>(dimensions.x) / divisions.x;
				m_verts_lines[i].position.y = 0;
			}
			else
				m_verts_lines[i].position.y = static_cast<float>(dimensions.y);

			m_verts_lines[i].position.x = pos_delta;
			m_verts_lines[i].color = sf::Color(255, 255, 255);
		}

		// Setup horizontal lines
		pos_delta = 0;
		for(unsigned int i = (divisions.x - 1) * 2; i < m_verts_lines.getVertexCount(); i++)
		{
			if(i % 2 == 0)
			{
				pos_delta += static_cast<float>(dimensions.y) / divisions.y;
				m_verts_lines[i].position.x = 0;
			}
			else
				m_verts_lines[i].position.x = static_cast<float>(dimensions.x);

			m_verts_lines[i].position.y = pos_delta;
			m_verts_lines[i].color = sf::Color(255, 255, 255);
		}

		// Setup quads
		for(unsigned int i = 0, j = 0; i < divisions.x * divisions.y; i++, j = i * 4)
		{
			sf::FloatRect rect = m_cells[i].m_rect;

			m_verts_quads[j].position = sf::Vector2f(rect.left, rect.top);
			m_verts_quads[j].color = sf::Color(255, 255, 255, 128);
			j++;

			m_verts_quads[j].position = sf::Vector2f(rect.left + rect.width, rect.top);
			m_verts_quads[j].color = sf::Color(255, 255, 255, 128);
			j++;

			m_verts_quads[j].position = sf::Vector2f(rect.left + rect.width, rect.top + rect.height);
			m_verts_quads[j].color = sf::Color(255, 255, 255, 128);
			j++;

			m_verts_quads[j].position = sf::Vector2f(rect.left, rect.top + rect.height);
			m_verts_quads[j].color = sf::Color(255, 255, 255, 128);
		}
	}

	void checkCollisions(std::vector<CollisionObject*>& objects)
	{
		rebuildGrid(objects);

		//Check for collisions in grid
		for(unsigned int i = 0; i < m_divisions.x * m_divisions.y; i++)
		{
			GridCell* cell = &m_cells[i];
			unsigned int count = cell->m_objects.size();
			
			sf::Color color = sf::Color::Transparent;

			if(count > 0)
			{
				color = sf::Color(35, 155, 86, 128);

				if(count > 1)
				{
					color = sf::Color(255, 195, 0, 128);

					for(unsigned int j = 0; j < count; j++)
					{
						for(unsigned int k = j + 1; k < count; k++)
						{
							CollisionObject* a = cell->m_objects[j];
							CollisionObject* b = cell->m_objects[k];

							sf::Vector3f overlap;

							if(checkCollision(a, b, overlap))
							{
								// Because a was check against b, the overlap distance is actually
								// opposite for a so we flip it to compensate. Works perfectly.
								a->handleCollision(b, overlap);
								overlap.z = -overlap.z;
								b->handleCollision(a, overlap);

								color = sf::Color(231, 76, 60, 128);
							}
						}
					}
				}
			}

			m_verts_quads[(i * 4)].color = color;
			m_verts_quads[(i * 4) + 1].color = color;
			m_verts_quads[(i * 4) + 2].color = color;
			m_verts_quads[(i * 4) + 3].color = color;
		}
	}

	void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		target.draw(m_verts_quads);
		target.draw(m_verts_lines);
		// I could draw the game object shapes from here if I wanted to give them a debug view
	}

private:
	void rebuildGrid(std::vector<CollisionObject*>& objects)
	{
		// Empty Grid Cells
		for(unsigned int i = 0; i < m_divisions.x * m_divisions.y; i++)
		{
			if(!m_cells[i].m_objects.empty())
				m_cells[i].m_objects.clear();
		}

		// Repopulate Cells
		for(CollisionObject* object : objects)
		{
			sf::Vector2i cell_min, cell_max;

			sf::Vector2f cell_size(m_cells[0].m_rect.width, m_cells[0].m_rect.height);
			sf::FloatRect rect = object->m_shape->getGlobalBounds();

			sf::FloatRect screen_space(0, 0, (float)m_dimensions.x, (float)m_dimensions.y);
			if(!rect.intersects(screen_space))
				continue;

			cell_min.x = static_cast<int>(rect.left / cell_size.x);
			cell_min.x -= cell_min.x % 1;
			cell_min.y = static_cast<int>(rect.top / cell_size.y);
			cell_min.y -= cell_min.y % 1;

			cell_max.x = static_cast<int>((rect.width + rect.left) / cell_size.x);
			cell_max.x -= cell_max.x % 1;
			cell_max.y = static_cast<int>((rect.height + rect.top) / cell_size.y);
			cell_max.y -= cell_max.y % 1;

			// Setup the top left cell containing our object
			if(cell_min.x < 0)
				cell_min.x = 0;
			if(static_cast<unsigned int>(cell_min.x) > m_divisions.x - 1)
				cell_min.x = m_divisions.x - 1;

			if(cell_min.y < 0)
				cell_min.y = 0;
			if(static_cast<unsigned int>(cell_min.y) > m_divisions.y - 1)
				cell_min.y = m_divisions.y - 1;

			// Setup the bottom right cell containing our object
			if(cell_max.x < 0)
				cell_max.x = 0;
			if(static_cast<unsigned int>(cell_max.x) > m_divisions.x - 1)
				cell_max.x = m_divisions.x - 1;

			if(cell_max.y < 0)
				cell_max.y = 0;
			if(static_cast<unsigned int>(cell_max.y) > m_divisions.y - 1)
				cell_max.y = m_divisions.y - 1;

			// Put our objects on the list for the cells which it spans
			for(unsigned int i = cell_min.y; i < static_cast<unsigned int>(cell_max.y + 1); i++)
			{
				for(unsigned int j = cell_min.x; j < static_cast<unsigned int>(cell_max.x + 1); j++)
					m_cells[(i * m_divisions.x) + j].m_objects.push_back(object);
			}
		}
	}

	bool checkCollision(const CollisionObject* obj_a, const CollisionObject* obj_b, sf::Vector3f& overlap)
	{
		bool result;

		ShapeType type_a = obj_a->m_type;
		ShapeType type_b = obj_b->m_type;

		const sf::Shape* shape_a = obj_a->m_shape;
		const sf::Shape* shape_b = obj_b->m_shape;

		overlap.z = -std::numeric_limits<float>::max();

		if(type_a == ShapeType::POLYGON)
		{
			if(type_b == ShapeType::POLYGON)
				result = checkPolyVSPoly(shape_a, shape_b, overlap);
			else
				result = checkPolyVSCirc(shape_a, shape_b, overlap);
		}

		if(type_a == ShapeType::CIRCLE)
		{
			if(type_b == ShapeType::CIRCLE)
				result = checkCircVSCirc(shape_a, shape_b, overlap);
			else
			{
				result = checkPolyVSCirc(shape_b, shape_a, overlap);
				overlap.z = -overlap.z;
			}
		}
		
		return result;
	}

	sf::Vector2u m_dimensions;
	sf::Vector2u m_divisions;
	GridCell* m_cells;
	sf::VertexArray m_verts_lines;
	sf::VertexArray m_verts_quads;
};


#endif
