#include "GUIScrollBar.h"
#include "ResourceHandler.h"
#include <iostream>

std::shared_ptr<GUIScrollBar> GUIScrollBar::create(sf::FloatRect rect, std::shared_ptr<GUIElement> parent)
{
	std::shared_ptr<GUIScrollBar> ret = std::make_shared<GUIScrollBar>(rect, parent);
	ret->init();
	ret->mScrollBarBackground.setPosition(ret->getX(), ret->getY());
	ret->mSprite.setPosition(ret->mScrollBarBackground.getPosition().x + ret->mScrollBarBackground.getTexture()->getSize().x - ret->mSprite.getTexture()->getSize().x, ret->mScrollBarBackground.getPosition().y);
	ret->mValue = 100;
	return ret;
}


GUIScrollBar::GUIScrollBar(sf::FloatRect rect, std::shared_ptr<GUIElement> parent) :
	GUIElement(rect, parent, SCROLL_BAR),mFollowMouse(false)
{
	mScrollBarBackground.setTexture(ResourceHandler::getInstance()->getTexture(std::string("Menu/Scrollbar")));
	mSprite.setTexture(ResourceHandler::getInstance()->getTexture(std::string("Menu/scrollbar_slider")));
	setSize(rect.width, rect.height);
}
void GUIScrollBar::setValue(int value)
{
	float _value = (float)value/100.0f;
	mValue = value;
	float minPos = getX() + mSprite.getTexture()->getSize().x/2;
	float mid = minPos + (mScrollBarBackground.getTexture()->getSize().x - mSprite.getTexture()->getSize().x)*_value;
	float maxPos = getX() + mScrollBarBackground.getTexture()->getSize().x - mSprite.getTexture()->getSize().x/2;

	float max = (mScrollBarBackground.getTexture()->getSize().x - mSprite.getTexture()->getSize().x);
	float diff = max - (maxPos - mid - 1);
	float volume = (float)diff/(float)max;
	mValue = 100*volume;
	mSprite.setPosition(mid, getY());
}

int GUIScrollBar::getValue()
{
	return mValue;
}

sf::Sprite* GUIScrollBar::getSprite()
{
	return &mSprite;
}

void GUIScrollBar::setSize(float width, float height)
{
	float scaleX = width / mScrollBarBackground.getTexture()->getSize().x;
	float scaleY = height / mScrollBarBackground.getTexture()->getSize().y;
	mScrollBarBackground.setScale(scaleX, scaleY);
	mSprite.setScale(scaleX, scaleY);
	setWidth(width);
	setHeight(height);
}

bool GUIScrollBar::render(sf::RenderWindow *window, sf::RenderStates &states)
{
	bool visible = getVisible();
	if(!visible)
		return false;
	float x = getLocalX(), y = getLocalY();
	std::shared_ptr<GUIElement> parent = getParent();
	while(parent != NULL)
	{
		x += parent->getLocalX();
		y += parent->getLocalY();
		visible = parent->getVisible();
		if(!visible)
			return false;
		parent = parent->getParent();
	}
	mScrollBarBackground.setPosition(getX(), getY());
	mSprite.setPosition(mSprite.getPosition().x, getY());
	window->draw(mScrollBarBackground);
	window->draw(mSprite, states);


	if(!mChilds.empty())
	{
		for(std::vector<std::shared_ptr<GUIElement> >::size_type i = 0; i < mChilds.size(); ++i)
		{
			mChilds[i]->render(window, states);
		}
	}
	return true;
}

void GUIScrollBar::onGUIClick(int mouseX, int mouseY)
{
	mFollowMouse = true;
}

void GUIScrollBar::setOnGuiChangeFunction(std::function<void()> func)
{
	mOnClickFunction = func;
}

bool GUIScrollBar::update(sf::RenderWindow *window, sf::Event event)
{
	if(!mSelected)
		mFollowMouse = false;
	if(event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
		mFollowMouse = false;
	sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
	if(mFollowMouse)
	{
		float x = mousePos.x - mSprite.getTexture()->getSize().x/2, y = mousePos.y;
		if(x < getX())
			x = getX();
		if(x + mSprite.getTexture()->getSize().x > getX() + mScrollBarBackground.getTexture()->getSize().x)
			x = getX() + mScrollBarBackground.getTexture()->getSize().x - mSprite.getTexture()->getSize().x;
		mSprite.setPosition(x, y);
		
		float mid = x + mSprite.getTexture()->getSize().x/2;
		float minPos = getX() + mSprite.getTexture()->getSize().x/2;
		float maxPos = getX() + mScrollBarBackground.getTexture()->getSize().x - mSprite.getTexture()->getSize().x/2;

		float max = (mScrollBarBackground.getTexture()->getSize().x - mSprite.getTexture()->getSize().x);
		float diff = max - (maxPos - mid - 1);
		float volume = (float)diff/(float)max;
		mValue = 100*volume;
		if(mOnClickFunction != nullptr)
			mOnClickFunction();
	}
	GUIElement::update(window, event);
	return true;
}