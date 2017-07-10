#include <iostream>
#include <vector>
#include "CollisionObject.h"
#include "Grid.h"

#define UPDATE_RATE (1.0f / 60.0f)
#define COUNT_LIMIT 10


int main()
{
	std::cout << "SFML Separating Axis Theorem Example" << std::endl;
	std::cout << "Move with arrow keys - Press Space to spin" << std::endl;

	sf::RenderWindow window(sf::VideoMode(800, 600), "Separating Axis Theorem", sf::Style::Close);
	window.setVerticalSyncEnabled(true);
	std::srand(static_cast<unsigned int>(time(NULL)));

	sf::Clock frame_clock;
	float frame_total = 0;
	float frame_rate = 0;

	Grid grid(window.getSize());

	std::vector<CollisionObject*> game_objects;
	
	game_objects.push_back(new Player(200, 200, 3));
	Player* player = (Player*)game_objects.back();
	player->m_shape->setPosition(200, 200);

	game_objects.push_back(new RandomShape(50));
	game_objects.back()->m_shape->setPosition(200, 200);
	game_objects.back()->m_shape->move(25, 25);
	
	for(int i = 0; i < 100; i++)
	{
		game_objects.push_back(new RandomShape(50));
		game_objects.back()->m_shape->setPosition(rand() % 800, rand() % 600);
		game_objects.push_back(new RandomShape(50, 50, 6));
		game_objects.back()->m_shape->setPosition(rand() % 800, rand() % 600);
		game_objects.push_back(new RandomShape(50, 50, 4));
		game_objects.back()->m_shape->setPosition(rand() % 800, rand() % 600);
	}
	
	while(window.isOpen())
	{
		int loop_count = 0;
		frame_total += frame_clock.restart().asSeconds();

		sf::Event event;

		while(window.pollEvent(event))
		{
			if(event.type == sf::Event::Closed)
				window.close();
		}

		// Update Scene
		while(frame_total > UPDATE_RATE && loop_count < COUNT_LIMIT)
		{
			for(CollisionObject* object : game_objects)
				object->loop();

			grid.checkCollisions(game_objects);

			frame_total -= UPDATE_RATE;
			loop_count++;
		}

		std::cout << "\rfps: " << frame_rate;

		window.clear();

		window.draw(grid);

		for(CollisionObject* object : game_objects)
			window.draw(*object->m_shape);

		window.display();

		// Get FPS
		frame_rate = 1.0f / frame_clock.getElapsedTime().asSeconds();
	}
}
