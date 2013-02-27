#include "EventHandler.h"
#include "Event.h"
#include <iostream>

EventHandler* EventHandler::mInstance = NULL;

EventHandler* EventHandler::getInstance()
{
	if(mInstance == nullptr)
		mInstance = new EventHandler();
	return mInstance;
}

EventHandler::EventHandler():mEvents(){}

void EventHandler::addEvent(Event* event)
{
	mEvents.push_back(event);
}

void EventHandler::triggerEvent(std::string eventName, sf::Packet packet)
{
	for(std::vector<Event*>::size_type i = 0; i < mEvents.size(); i++)
	{
		if(strcmp(eventName.c_str(), mEvents[i]->getEventName().c_str()) == 0)
		{
			mEvents[i]->triggerEvent(packet);
		}
	}
}