#ifndef _REMOTE_CLIENT_H
#define _REMOTE_CLIENT_H

#include <SFML\Graphics.hpp>
#include <string>

class RemoteClient
{
public:
	RemoteClient();
	sf::Vector2i getMousePosition();
	void setMousePosition(sf::Vector2i mousePos);
	~RemoteClient(){};
private:
	sf::Vector2i mMousePos;
};

#endif