#include "Event.h"
#include "EventHandler.h"
#include <iostream>

Event* Event::addEventHandler(std::string eventName, std::function<void(sf::Packet)> callbackFunction)
{
	Event *event = new Event(eventName, callbackFunction);
	EventHandler::getInstance()->addEvent(event);
	return event;
}

Event::Event(std::string eventName, std::function<void(sf::Packet)> callbackFunction):mEventName(eventName),mCallbackFunction(callbackFunction){}

void Event::triggerEvent(std::string eventName, sf::Packet packet)
{
	EventHandler::getInstance()->triggerEvent(eventName, packet);
}

void Event::triggerEvent(sf::Packet packet)
{
	mCallbackFunction(packet);
}