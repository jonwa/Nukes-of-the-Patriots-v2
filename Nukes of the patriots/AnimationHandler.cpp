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

void AnimationHandler::reset()
{
	for(std::vector< std::shared_ptr<Animation> >::size_type it = 0; it < mAnimationVector.size(); it++)
	{
		mAnimationVector[it].reset();
	}
	mAnimationVector.clear();	
}

void AnimationHandler::tick()
{
	for(std::vector< std::shared_ptr<Animation> >::size_type it = 0; it < mAnimationVector.size(); ++it)
	{

		if(!mAnimationVector[it]->tick())
		{
			mAnimationVector[it].reset();
			mAnimationVector.erase(mAnimationVector.begin() + it);
		}

	}
}

void AnimationHandler::addAnimation(std::shared_ptr<Animation> animation)
{
	//mMutex.lock();
	mAnimationVector.push_back(animation);
	//mMutex.unlock();
}

