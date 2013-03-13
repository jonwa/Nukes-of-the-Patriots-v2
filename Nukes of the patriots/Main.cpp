#include <SFML\Graphics.hpp>
#include <SFML\Window\Mouse.hpp>
#include <SFML\Window\Keyboard.hpp>
#include "GUIManager.h"
#include "GUIElement.h"
#include "GUIWindow.h"
#include "GUIButton.h"
#include "GUIText.h"
#include "Capitalist.h"
#include "Communist.h"
#include "Menu.h"
#include <string>
#include <sstream>
#include <iostream>
#include <functional>
#include "ResourceHandler.h"
#include "GameManager.h"
#include "TimerHandler.h"
#include "AnimationHandler.h"
#include <SFML\Audio\Listener.hpp>
#include "SoundHandler.h"
#include "tinyxml2.h"
#include "Randomizer.h"

using namespace std;

int main()
{
	sf::RenderWindow window(sf::VideoMode(1024, 768, 32), "Nukes of the Patriots");//, sf::Style::Fullscreen);
	window.setFramerateLimit(60);
	window.setMouseCursorVisible(false);
	Randomizer::getInstance(); //Init randomizer singleton to start timer
	GUIManager::getInstance()->init(&window);
	ResourceHandler::getInstance()->loadImages();
	ResourceHandler::getInstance()->load();
	Menu::getInstance()->setWindow(window);
	Menu::getInstance()->getWindows("MainMenu")->setVisible(true);
	Menu::getInstance()->loadConfig();
	GameManager::getInstance()->loadFileName();

	bool sleeping = false;

	while (window.isOpen())
    {
		sf::Event event;
		while (window.pollEvent(event))
		{

			if(!sleeping)
			{
				GUIManager::getInstance()->update(event);
				Menu::getInstance()->update(event);
				GameManager::getInstance()->update(event);
			}

			if (event.type == sf::Event::Closed)
				window.close();
			if(event.type == sf::Event::LostFocus)
				sleeping = true;
			if(event.type == sf::Event::GainedFocus)
				sleeping = false;
		}
		if(sleeping)
		{
			sf::sleep(sf::seconds(0.5f));
			continue;
		}
	

		AnimationHandler::getInstance()->tick();
		GUIManager::getInstance()->tick();
		AnimationHandler::getInstance()->tick();
		SoundHandler::getInstance()->tick();
		window.clear();
		//grayscale.setParameter("mousePos", sf::Vector2f(mousePos.x, window.getSize().y - mousePos.y));
		sf::RenderStates states;
		//states.shader = &grayscale;
		GUIManager::getInstance()->render(states);
		TimerHandler::getInstance()->tick();
		Menu::getInstance()->tick();
		GameManager::getInstance()->tick(window);
		window.display();
    }
	Menu::getInstance()->saveConfig();
	GameManager::getInstance()->saveFileName();
    return EXIT_SUCCESS;
}