#include "TimerHandler.h"
#include "Timer.h"
#include <iostream>

TimerHandler::TimerHandler() :
	mVecTimers(){}

TimerHandler::~TimerHandler(){ clear(); }

void TimerHandler::addTimer(Timer* timer)
{
	mVecTimers.push_back(timer);
}

void TimerHandler::clear()
{
	mVecTimers.clear();
}

void TimerHandler::removeTimer(Timer* timer)
{
	for(std::vector<Timer*>::size_type it = 0; it < mVecTimers.size(); it++)
	{
		if(mVecTimers[it] == timer)
		{
			mVecTimers[it] = NULL; // Elements are not going to be deleted here
			std::cout << "remove timer: " << mVecTimers[it]<<std::endl;
			break;
		}
	}
}

bool TimerHandler::isTimer(Timer* timer)
{
	for(std::vector<Timer*>::size_type it = 0; it < mVecTimers.size(); it++)
	{
		if(mVecTimers[it] == timer)
			return true;
	}
	return false;
}

//void TimerHandler::tick()
//{
//	for(std::vector<Timer*>::size_type it = 0; it < mVecTimers.size(); it++)
//	{
//		if(mVecTimers[it] == NULL)
//		{
//			delete mVecTimers[it];
//			mVecTimers.erase(mVecTimers.begin() + it);
//		}
//		else
//		{
//			if(!mVecTimers[it]->tick())
//			{
//				if(mVecTimers[it] != NULL)
//				{
//					delete mVecTimers[it];
//					mVecTimers.erase(mVecTimers.begin() + it);
//				}
//			}
//		}
//	}
//}

void TimerHandler::tick()
{
	 for(std::vector<Timer*>::size_type it = 0; it < mVecTimers.size();it++)
	 {
		 if(!mVecTimers[it]->isAlive())
		 {
			 delete mVecTimers[it];
			 mVecTimers.erase(mVecTimers.begin() + it);
		 }
		 else
		 {
			 mVecTimers[it]->tick();
		 }
	 }
}


TimerHandler* TimerHandler::mInstance = NULL;

TimerHandler* TimerHandler::getInstance()
{
	if(mInstance == NULL)
		mInstance = new TimerHandler();
	return mInstance;
}