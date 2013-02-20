#ifndef _GUI_ANIMATION_H
#define _GUI_ANIMATION_H

#include <SFML/Graphics.hpp>
#include "Animation.h"

class GUIElement;
class Timer;

class GUIAnimation: public Animation
{
public:
	static std::shared_ptr<GUIAnimation> create(std::shared_ptr<GUIElement> guiElement);
	GUIAnimation(std::shared_ptr<GUIElement> guiElement);
	static void move(std::shared_ptr<GUIElement> guiElement, int time, sf::FloatRect startRect, sf::FloatRect endRect);
	static void fadeToColor(std::shared_ptr<GUIElement> guiElement, int time, sf::Color startColor, sf::Color endColor);
	void tick();
	~GUIAnimation(){};
private:
	std::shared_ptr<GUIElement> mGUIElement;
	sf::FloatRect mStartRect, mEndRect;
	sf::Color mStartColor, mEndColor;
	Timer* mMoveTimer, *mColorFadeTimer;
};

#endif