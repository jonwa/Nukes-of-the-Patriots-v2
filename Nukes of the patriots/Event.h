#ifndef _EVENT_H
#define _EVENT_H

#include <SFML\Network\Packet.hpp>
#include <string>
#include <functional>

class Event
{
public:
	static Event* addEventHandler(std::string eventName, std::function<void(sf::Packet)> callbackFunction);
	static void triggerEvent(std::string eventName, sf::Packet packet);

	std::string getEventName()const{ return mEventName; }
	std::function<void(sf::Packet)> getCallbackFunction()const{ return mCallbackFunction; }
	void triggerEvent(sf::Packet packet);

	~Event();
private:
	Event(std::string eventName, std::function<void(sf::Packet)> callbackFunction);
private:
	std::function<void(sf::Packet)> mCallbackFunction;
	std::string mEventName;
};

#endif