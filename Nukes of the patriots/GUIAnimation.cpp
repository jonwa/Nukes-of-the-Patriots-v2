#include "GUIAnimation.h"
#include "GUIElement.h"
#include "AnimationHandler.h"
#include "Timer.h"
#include "TimerHandler.h"
#include <iostream>

std::shared_ptr<GUIAnimation> GUIAnimation::create(std::shared_ptr<GUIElement> guiElement)
{
	std::shared_ptr<GUIAnimation> result = std::make_shared<GUIAnimation>(guiElement);
	result->init();
	return result;
}

void GUIAnimation::move(std::shared_ptr<GUIElement> guiElement, int time, sf::FloatRect startRect, sf::FloatRect endRect)
{
	std::shared_ptr<GUIAnimation> animation = create(guiElement);
	animation->mMoveTimer = Timer::setTimer([=](){}, time, 1);

	animation->mStartRect = startRect;
	animation->mEndRect = endRect;
	animation->mStartColor = guiElement->getColor();
	animation->mEndColor = animation->mStartColor;
}

void GUIAnimation::fadeToColor(std::shared_ptr<GUIElement> guiElement, int time, sf::Color startColor, sf::Color endColor)
{
	std::shared_ptr<GUIAnimation> animation = create(guiElement);
	animation->mColorFadeTimer = Timer::setTimer([=](){}, time, 1);
	animation->mStartColor = startColor;
	animation->mEndColor = endColor;
	animation->mStartRect = guiElement->getRectangle();
	animation->mEndRect = guiElement->getRectangle();
}

GUIAnimation::GUIAnimation(std::shared_ptr<GUIElement> guiElement):
	Animation(),
	mGUIElement(guiElement),
	mMoveTimer(nullptr),
	mColorFadeTimer(nullptr)
{
}

void GUIAnimation::tick()
{
	float timeleft = 0;
	float duration = 1;
	if(Timer::isTimer(mMoveTimer))
	{
		timeleft = mMoveTimer->getTimeLeft();
		duration = mMoveTimer->getTimerDuration();
	}
	float progress = 1 - (timeleft/duration);

	sf::FloatRect diff = sf::FloatRect(mEndRect.left - mStartRect.left, mEndRect.top - mStartRect.top, mEndRect.width - mStartRect.width, mEndRect.height - mStartRect.height);
	mGUIElement->setX(mStartRect.left + diff.left*progress);
	mGUIElement->setY(mStartRect.top + diff.top*progress);

	mGUIElement->setSize(mStartRect.width + diff.width*progress, mStartRect.height + diff.height*progress);

	float fadeTimeLeft = 0;
	float fadeDuration = 1;
	if(Timer::isTimer(mColorFadeTimer))
	{
		fadeTimeLeft = mColorFadeTimer->getTimeLeft();
		fadeDuration = mColorFadeTimer->getTimerDuration();
	}
	progress = 1 - (fadeTimeLeft/fadeDuration);

	sf::Color newColor = sf::Color( mStartColor.r + (mEndColor.r - mStartColor.r)*progress, mStartColor.g + (mEndColor.g - mStartColor.g)*progress, 
			mStartColor.b + (mEndColor.b - mStartColor.b)*progress, mStartColor.a + (mEndColor.a - mStartColor.a)*progress);
	mGUIElement->setColor(newColor);
}
