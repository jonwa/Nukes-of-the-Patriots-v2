#include "GUIElement.h"
#include <iostream>
#include "GUIManager.h"
#include "GameManager.h"

GUIElement::GUIElement(sf::FloatRect rect, std::shared_ptr<GUIElement> parent, GUIType guiType) : 
	mRectangle(rect),
	mLocalRectangle(rect),
	mVisible(true),
	mParent(parent),
	mGUIType(guiType),
	mMouseInside(false),
	mCallClickFunc(false), 
	mCallMouseEnterFunc(false),
	mCallMouseLeaveFunc(false),
	mEnabled(true),
	mSelected(false),
	mColor(sf::Color(255, 255, 255, 255)),
	mElementID(0)
{
}

GUIElement::~GUIElement()
{
	mChilds.clear();
}


void GUIElement::init()
{
	if(mParent != NULL)
	{
		mParent->addChild(getPtr());
		setX(mRectangle.left + mParent->mRectangle.left);
		setY(mRectangle.top + mParent->mRectangle.top);
	}
	mElementID = GUIManager::getInstance()->getUniqueID();
}

std::shared_ptr<GUIElement> GUIElement::getPtr()
{
	//return std::shared_ptr<GUIElement>(this);
	return std::shared_ptr<GUIElement>(shared_from_this());
}

void GUIElement::addChild(std::shared_ptr<GUIElement> guiElement)
{
	mChilds.push_back(guiElement);
}

std::vector<std::shared_ptr<GUIElement>>& GUIElement::getChildVector()
{
	return mChilds;
}

void GUIElement::setMouseIsInside(bool inside)
{
	mMouseInside = inside;
};

float	GUIElement::getX()const
{ 
	return mRectangle.left; 
}

float	GUIElement::getY()const
{ 
	return mRectangle.top; 
}

float GUIElement::getLocalX()const
{ 
	float x = mRectangle.left;
	if(mParent != NULL)
		x -= mParent->getX();
	return x; 
}

float GUIElement::getLocalY()const
{ 
	float y = mRectangle.top;
	if(mParent != NULL)
		y -= mParent->getY();
	return y; 
}

float	GUIElement::getWidth()const
{ 
	return mRectangle.width; 
}
float	GUIElement::getHeight()const
{ 
	return mRectangle.height;
}
bool GUIElement::getVisible()const
{ 
	return mVisible; 
}
std::shared_ptr<GUIElement> GUIElement::getParent()const
{
	return mParent;
}
GUIType	GUIElement::getGUIType()const
{ 
	return mGUIType; 
}
sf::Color GUIElement::getColor()const
{ 
	return mColor;
}
bool GUIElement::getMouseIsInside()const
{
	return mMouseInside;
}

bool GUIElement::isEnabled()const
{
	return mEnabled;
}

bool GUIElement::isSelected()const
{
	return mSelected;
}

sf::FloatRect GUIElement::getRectangle()
{
	return mRectangle;
}

int GUIElement::getElementID()
{
	return mElementID;
}

std::function<void()> GUIElement::getOnClickFunction()
{
	return mOnClickFunction;
}

std::function<void()> GUIElement::getMouseEnterFunction()
{
	return mMouseEnterFunction;
}

std::function<void()> GUIElement::getMouseLeaveFunction()
{
	return mMouseLeaveFunction;
}

std::function<void()> GUIElement::getOnGuiChangeFunction()
{
	return mOnGuiChange;
}

void GUIElement::setX(float x)
{
	mRectangle.left = x;

	for(std::vector<std::shared_ptr<GUIElement> >::size_type i = 0; i < mChilds.size(); ++i)
	{
		//mChilds[i]->setX(getX() + mChilds[i]->mLocalRectangle.left);
	}
}
void GUIElement::setY(float y)
{ 
	mRectangle.top = y;

	for(std::vector<std::shared_ptr<GUIElement> >::size_type i = 0; i < mChilds.size(); ++i)
	{
		//mChilds[i]->setY(getY() + mChilds[i]->mLocalRectangle.top);
	}
}
void GUIElement::setWidth(float width)
{
	mRectangle.width = width;
}
void GUIElement::setHeight(float height)
{ 
	mRectangle.height = height;
}

void GUIElement::setRectangle(sf::FloatRect rect)
{
	mRectangle = sf::FloatRect(rect);
}

void GUIElement::setVisible(bool visible)
{ 
	mVisible = visible;
	/*
	if(!mChilds.empty())
	{
		for(std::vector<std::shared_ptr<GUIElement>>::size_type i = 0; i < mChilds.size(); ++i)
		{
			mChilds[i]->setVisible(visible);
		}
	}
	*/
}

void GUIElement::setEnabled(bool enabled, bool effectChildren)
{
	mEnabled = enabled;
	if(effectChildren && !mChilds.empty())
	{
		for(std::vector<std::shared_ptr<GUIElement>>::size_type i = 0; i < mChilds.size(); ++i)
		{
			mChilds[i]->setEnabled(enabled, effectChildren);
		}
	}
}

void GUIElement::setColor(sf::Color color)
{
	mColor = color;
}

void GUIElement::setSelected(bool selected)
{
	mSelected = selected;
}

bool GUIElement::onClick(sf::RenderWindow *window)
{
	bool visible = mVisible;
	if(!visible)return false;
	std::shared_ptr<GUIElement> parent = getParent();
	while(parent != NULL)
	{
		visible = parent->getVisible();
		if(!visible)
			return false;
		parent = parent->getParent();
	}
	sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
	// Check if mouse is colliding with gui element
	if(mEnabled && mRectangle.contains(mousePos.x, mousePos.y))
	{
		mSelected = true;
		onGUIClick(mousePos.x, mousePos.y);
		mCallClickFunc = true;
	}
	else
		mSelected = false;

	if(!mChilds.empty())
	{
		for(std::vector<std::shared_ptr<GUIElement>>::size_type i = 0; i < mChilds.size(); ++i)
		{
			mChilds[i]->onClick(window);
		}
	}
	return true;
}

bool GUIElement::onMove(sf::RenderWindow *window)
{
	bool visible = mVisible;
	if(!visible)return false;
	std::shared_ptr<GUIElement> parent = getParent();
	while(parent != NULL)
	{
		visible = parent->getVisible();
		if(!visible)
			return false;
		parent = parent->getParent();
	}
	sf::Vector2i mousePos = sf::Mouse::getPosition(*window);

	if(mEnabled)
	{
		bool isMouseInside = getMouseIsInside();
		// Check if mouse is colliding with gui element
		if(mRectangle.contains(mousePos.x, mousePos.y))
		{
			if(!isMouseInside)
			{
				setMouseIsInside(true);
				mCallMouseEnterFunc = true;
			}
		}
		else
		{
			if(isMouseInside)
			{
				setMouseIsInside(false);
				mCallMouseLeaveFunc = true;
			}
		}
	}

	if(!mChilds.empty())
	{
		for(std::vector<std::shared_ptr<GUIElement>>::size_type i = 0; i < mChilds.size(); ++i)
		{
			mChilds[i]->onMove(window);
		}
	}
	return true;
}

bool GUIElement::update(sf::RenderWindow *window, sf::Event event)
{
	if(!mChilds.empty())
	{
		for(std::vector<std::shared_ptr<GUIElement>>::size_type i = 0; i < mChilds.size(); ++i)
		{
			mChilds[i]->update(window, event);
		}
	}
	return true;
}

void GUIElement::setOnClickFunction(std::function <void ()> func)
{
	mOnClickFunction = func;
}

void GUIElement::setMouseEnterFunction(std::function <void()> func)
{
	mMouseEnterFunction = func;
}

void GUIElement::setMouseLeaveFunction(std::function <void()> func)
{
	mMouseLeaveFunction = func;
}

void GUIElement::setOnGuiChangeFunction(std::function <void()> func)
{
	mOnGuiChange = func;
}

void GUIElement::tick()
{
	if(mCallClickFunc)
	{
		mCallClickFunc = false;
		if(mOnClickFunction != nullptr)
		{
			mOnClickFunction();
			//if(GameManager::getInstance()->getGameType() == LAN)
			//{
			//	GameManager::getInstance()->syncGUIClick(shared_from_this());
			//}
		}
	}
	if(mCallMouseEnterFunc)
	{
		mCallMouseEnterFunc = false;
		if(mMouseEnterFunction != nullptr)
		{
			mMouseEnterFunction();
			//if(GameManager::getInstance()->getGameType() == LAN)
			//{
			//	GameManager::getInstance()->syncGUIMouseEnter(shared_from_this());
			//}
		}
	}
	if(mCallMouseLeaveFunc)
	{
		mCallMouseLeaveFunc = false;
		if(mMouseLeaveFunction != nullptr)
		{
			mMouseLeaveFunction();
			//if(GameManager::getInstance()->getGameType() == LAN)
			//{
			//	GameManager::getInstance()->syncGUIMouseLeave(shared_from_this());
			//}
		}
	}
	if(!mChilds.empty())
	{
		for(std::vector<std::shared_ptr<GUIElement>>::size_type i = 0; i < mChilds.size(); ++i)
		{
			mChilds[i]->tick();
		}
	}
}