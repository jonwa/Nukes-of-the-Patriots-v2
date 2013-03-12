#ifndef _REMOTE_CLIENT_H
#define _REMOTE_CLIENT_H

#include <SFML\Graphics.hpp>
#include <string>

class SuperPower;

class RemoteClient
{
public:
	RemoteClient();
	sf::Vector2i getMousePosition();
	int getSuperPower();
	bool isReady();
	void setMousePosition(sf::Vector2i mousePos);
	void setSuperPower(int type);
	void setReady(bool ready);
	~RemoteClient(){};
private:
	sf::Vector2i mMousePos;
	int mPoliticalType;
	bool mReady;
};

#endif