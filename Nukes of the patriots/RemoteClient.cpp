#include "RemoteClient.h"
#include "SuperPower.h"

RemoteClient::RemoteClient():
	mMousePos(0, 0),
	mReady(false)
{}

sf::Vector2i RemoteClient::getMousePosition()
{
	return mMousePos;
}

int RemoteClient::getSuperPower()
{
	return mPoliticalType;
}

bool RemoteClient::isReady()
{
	return mReady;
}

void RemoteClient::setMousePosition(sf::Vector2i mousePos)
{
	mMousePos = mousePos;
}

void RemoteClient::setSuperPower(int type)
{
	mPoliticalType = type;
}

void RemoteClient::setReady(bool ready)
{
	mReady = ready;
}