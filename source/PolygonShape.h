#ifndef POLYGONSHAPE_H
#define POLYGONSHAPE_H

#include <cmath>
#include <SFML/Graphics.hpp>


namespace sf
{
	class PolygonShape : public sf::Shape
	{
	public:
		explicit PolygonShape(const sf::Vector2f& size = sf::Vector2f(0, 0), unsigned int count = 3) :
			m_size(size),
			m_count(count)
		{
			if(m_count < 3)
				m_count = 3;
			if(m_count > 16)
				m_count = 16;	// If you want more points go make a circle

			if(m_count != 4)
				m_size = sf::Vector2f(m_size.x / 2, m_size.y / 2);
				
			update();
		}

		void setSize(const sf::Vector2f& size)
		{
			m_size = size;
			update();
		}

		const sf::Vector2f& getSize() const
		{
			return m_size;
		}

		void setPointCount(unsigned int count)
		{
			m_count = count;
			update();
		}

		virtual unsigned int getPointCount() const
		{
			return m_count;
		}

		virtual sf::Vector2f getPoint(unsigned int index) const
		{
			if(m_count == 4)
			{
				switch(index)
				{
					default:
					case 0: return sf::Vector2f(0, 0);
					case 1: return sf::Vector2f(m_size.x, 0);
					case 2: return sf::Vector2f(m_size.x, m_size.y);
					case 3: return sf::Vector2f(0, m_size.y);
				}
			}
			else
			{
				static const float pi = 3.141592654f;

				float angle = index * 2 * pi / getPointCount() - pi / 2;
				float x = std::cos(angle) * m_size.x;
				float y = std::sin(angle) * m_size.y;

				return sf::Vector2f(m_size.x + x, m_size.y + y);
			}
		}

		sf::FloatRect getBoundingBox()
		{
			if(m_count == 4)
				return getGlobalBounds();

			m_box.left = std::numeric_limits<float>::max();
			m_box.top = std::numeric_limits<float>::max();

			m_box.width = std::numeric_limits<float>::min();
			m_box.height = std::numeric_limits<float>::min();

			for(int i = getPointCount(); i > 0; i--)
			{
				m_box.left = std::min(getTransform().transformPoint(getPoint(i)).x, m_box.left);
				m_box.top = std::min(getTransform().transformPoint(getPoint(i)).y, m_box.top);

				m_box.width = std::max(getTransform().transformPoint(getPoint(i)).x, m_box.width);
				m_box.height = std::max(getTransform().transformPoint(getPoint(i)).y, m_box.height);
			}

			m_box.width -= m_box.left;
			m_box.height -= m_box.top;

			return m_box;
		}

	private:
		sf::FloatRect m_box;
		sf::Vector2f m_size;
		unsigned int m_count;
	};
}


#endif
