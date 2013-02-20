#ifndef _TIMER_H
#define _TIMER_H

#include <functional>
#include <SFML\System\Clock.hpp>
#include <vector>

class TimerHandler;

class Timer
{
public:
	static Timer* setTimer(std::function<void()> callbackFunc, int time = 50, int timesToExecute = 1);
	static bool isTimer(Timer* timer);
	std::function<void()> getCallbackFunction();
	int getTimeLeft();
	int getTimesToExecuteLeft();
	bool isAlive();
	int getTimerDuration();

	bool tick();
	void killTimer();
	void resetTimer();
	~Timer();
private:
	Timer(std::function<void()> callbackFunc, int time, int timesToExecute);
private:
	std::function<void()> mCallbackFunc;
	int mTime, mTimerDuration;
	int mTimesToExecute, mTimesToExecuteLeft;
	bool mAlive;
	sf::Clock mClock;
};

#endif