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
#include "AnimationHandler.h"


using namespace std;

int main()
{
	sf::RenderWindow window(sf::VideoMode(1024, 768, 32), "SFML works!"); //, sf::Style::Fullscreen);
	window.setFramerateLimit(60);
	window.setMouseCursorVisible(false);
	sf::Texture cursorTexture;
	sf::Texture	cursorClickedTexture;
	cursorTexture.loadFromFile("Images/Mouse/MouseCursor.png");
	cursorClickedTexture.loadFromFile("Images/Mouse/MouseCursorClicked.png");
	sf::Sprite cursor(cursorTexture);
	
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
		if(event.type == sf::Event::MouseButtonPressed && event.key.code == sf::Mouse::Left)
			cursor.setTexture(cursorClickedTexture);
		else if(event.type == sf::Event::MouseButtonReleased && event.key.code == sf::Mouse::Left)
			cursor.setTexture(cursorTexture);
		
		sf::Vector2i mousePos = sf::Mouse::getPosition(window);
		cursor.setPosition(sf::Vector2f(mousePos.x, mousePos.y));
		//GUIManager::getInstance()->update();
		GUIManager::getInstance()->tick();
		AnimationHandler::getInstance()->tick();
        window.clear();
		GUIManager::getInstance()->render();
		TimerHandler::getInstance()->tick();
		window.draw(cursor);
        window.display();
    }
    return EXIT_SUCCESS;
}