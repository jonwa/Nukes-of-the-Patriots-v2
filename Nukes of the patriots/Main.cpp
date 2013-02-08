#include <SFML\Graphics.hpp>
#include <SFML\Window\Mouse.hpp>
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

using namespace std;

int main()
{
	sf::RenderWindow window(sf::VideoMode(1024, 768, 32), "SFML works!"/*);*/, sf::Style::Fullscreen);
	window.setFramerateLimit(60);
	GUIManager::getInstance()->init(&window);
	ResourceHandler::getInstance()->loadImages();
	ResourceHandler::getInstance()->load();
	Menu menu(window);
	
	
	while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
			GUIManager::getInstance()->update(event);
			if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::F1)
				GameManager::getInstance()->nextRound();
            if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                window.close();
        }
		//GUIManager::getInstance()->update();
		GUIManager::getInstance()->tick();
        window.clear();
		GUIManager::getInstance()->render();
		TimerHandler::getInstance()->tick();
        window.display();
    }
    return EXIT_SUCCESS;
}