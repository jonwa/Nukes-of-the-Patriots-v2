#include "RemoteClient.h"

RemoteClient::RemoteClient() :
	mMousePos(0, 0)
{
}

sf::Vector2i RemoteClient::getMousePosition()const
{
	return mMousePos;
}

void RemoteClient::setMousePosition(sf::Vector2i mousePos)
{
	mMousePos = mousePos;
}

