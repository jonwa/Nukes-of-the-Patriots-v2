#ifndef _EVENT_HANDLER_H
#define _EVENT_HANDLER_H

#include <vector>
#include <string>
#include <SFML\Network\Packet.hpp>

class Event;

class EventHandler
{
public:
	static EventHandler* instance;
	static EventHandler* getInstance();
	void addEvent(Event* event);
	void triggerEvent(std::string eventName, sf::Packet packet);
private:
	EventHandler();
	EventHandler(const EventHandler &);
	void operator=(const EventHandler &);
	~EventHandler();
private:
	std::vector<Event*> mEvents;
	static EventHandler* mInstance;
};

#endif