#include "Animation.h"
#include "AnimationHandler.h"

Animation::Animation()
{
}

void Animation::init()
{
	AnimationHandler::getInstance()->addAnimation(shared_from_this());
}