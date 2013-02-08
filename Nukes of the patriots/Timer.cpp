#include "Timer.h"
#include "TimerHandler.h"
#include <sstream>

Timer::Timer(std::function<void()> callbackFunc, int time, int timesToExecute) : 
	mCallbackFunc(callbackFunc),
	mTime(time > 0 ? time : 50),
	mTimesToExecute(timesToExecute >= 0 ? timesToExecute : 1),
	mClock()
{
	mTimesToExecuteLeft = mTimesToExecute;
	TimerHandler::getInstance()->addTimer(this);
}

Timer* Timer::setTimer(std::function<void()> callbackFunc, int time, int timesToExecute)
{
	if(callbackFunc != nullptr)
		return new Timer(callbackFunc, time, timesToExecute);
	return nullptr;
}

int Timer::getTimeLeft()
{
	sf::Time timePassed = mClock.getElapsedTime();
	return mTime - timePassed.asMilliseconds();
}

int Timer::getTimesToExecuteLeft()
{
	return mTimesToExecuteLeft;
}

std::function<void()> Timer::getCallbackFunction()
{
	return mCallbackFunc;
}

bool Timer::tick()
{
	sf::Time timePassed = mClock.getElapsedTime();
	if(timePassed.asMilliseconds() >= mTime)
	{
		// Just to make sure it doesn't go below 2^32 (mTimesToExecute == -1 means execute infinite times)
		if(mTimesToExecuteLeft >= 0)
			mTimesToExecuteLeft--;
		if(mCallbackFunc != nullptr)
			mCallbackFunc();

		if(mTimesToExecuteLeft == 0)
			return false;
		else
			resetTimer();
	}
	return true;
}

void Timer::killTimer()
{
	TimerHandler::getInstance()->removeTimer(this);
}

Timer::~Timer()
{

}

void Timer::resetTimer()
{
	mClock.restart();
	mTimesToExecuteLeft = mTimesToExecute;
}