#ifndef _TIMER_HANDLER_H
#define _TIMER_HANDLER_H

class Timer;
#include <vector>

class TimerHandler
{
public:
	static TimerHandler* getInstance();
	void addTimer(Timer* timer);
	void removeTimer(Timer* timer);
	void tick();
	bool TimerHandler::isTimer(Timer* timer);
private:
	TimerHandler();
	TimerHandler(const TimerHandler &);
	void operator=(const TimerHandler &);
	~TimerHandler();
private:
	std::vector<Timer*> mVecTimers;
	static TimerHandler* mInstance;
};

#endif