#ifndef _ANIMATION_H
#define _ANIMATION_H

#include <memory>

class AnimationHandler;

class Animation : public std::enable_shared_from_this<Animation>
{
public:
	Animation();
	virtual ~Animation(){};
	virtual bool tick() = 0;
protected:
	void init();
};

#endif