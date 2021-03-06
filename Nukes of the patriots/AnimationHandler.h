#ifndef _ANIMATION_HANDLER_H
#define _ANIMATION_HANDLER_H

#include <SFML/System.hpp>
#include "Animation.h"
#include <vector>

class AnimationHandler
{
public:
	static AnimationHandler* getInstance();
	void addAnimation(std::shared_ptr<Animation> animation);
	void tick();
	void reset();
private:
	AnimationHandler();
	~AnimationHandler() { };
private:
	static AnimationHandler* instance;
	sf::Thread *mUpdateThread;
	std::vector< std::shared_ptr<Animation> > mAnimationVector;
	sf::Mutex mMutex;
};

#endif