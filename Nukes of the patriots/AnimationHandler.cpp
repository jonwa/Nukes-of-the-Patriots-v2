#include "AnimationHandler.h"

AnimationHandler* AnimationHandler::instance = NULL;

AnimationHandler* AnimationHandler::getInstance()
{
	if(instance == nullptr)
		instance = new AnimationHandler();
	return instance;
}

AnimationHandler::AnimationHandler():mAnimationVector(),mUpdateThread(nullptr)
{
	//mUpdateThread = new sf::Thread(&AnimationHandler::tick, this);
	//mUpdateThread->launch();
}

void AnimationHandler::tick()
{
	for(std::vector< std::shared_ptr<Animation> >::size_type it = 0; it < mAnimationVector.size(); it++)
	{
		mAnimationVector[it]->tick();
	}
}

void AnimationHandler::addAnimation(std::shared_ptr<Animation> animation)
{
	//mMutex.lock();
	mAnimationVector.push_back(animation);
	//mMutex.unlock();
}

