#ifndef _REMOTE_CLIENT_H
#define _REMOTE_CLIENT_H
#include <SFML\Graphics.hpp>
#include <string>

class RemoteClient
{
public:
	RemoteClient();
	~RemoteClient(){}

	void setMousePosition(sf::Vector2i mousePos);
	sf::Vector2i getMousePosition()const;
private:
	sf::Vector2i mMousePos;
};

#endif