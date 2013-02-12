#include "GUIElement.h"
#include <iostream>

GUIElement::GUIElement(sf::FloatRect rect, std::shared_ptr<GUIElement> parent, GUIType guiType) : 
	mRectangle(rect),
	mVisible(true),
	mParent(parent),
	mGUIType(guiType),
	mMouseInside(false),
	mCallClickFunc(false), 
	mCallMouseEnterFunc(false),
	mCallMouseLeaveFunc(false),
	mEnabled(true)
{
}

GUIElement::~GUIElement()
{
	while(!mChilds.empty())
	{
		for(std::vector<std::shared_ptr<GUIElement>>::size_type i = 0; i < mChilds.size(); ++i)
		{
			mChilds.erase(mChilds.begin() + i);
		}
	}
}

void GUIElement::init()
{
	if(mParent != NULL)
	{
		mParent->addChild(getPtr());
		setX(mRectangle.left += mParent->mRectangle.left);
		setY(mRectangle.top += mParent->mRectangle.top);
		//setVisible(mParent->getVisible());
	}
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
int	GUIElement::getAlpha()const
{ 
	return mAlpha; 
}
bool GUIElement::getMouseIsInside()const
{
	return mMouseInside;
}

bool GUIElement::isEnabled()const
{
	return mEnabled;
}

sf::FloatRect GUIElement::getRectangle()
{
	return mRectangle;
}

void GUIElement::setX(float x)
{
	mRectangle.left = x; 
}
void GUIElement::setY(float y)
{ 
	mRectangle.top = y;
}
void GUIElement::setWidth(float width)
{
	mRectangle.width = width;
}
void GUIElement::setHeight(float height)
{ 
	mRectangle.height = height;
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

void GUIElement::setAlpha(int alpha)
{
	mAlpha = alpha; 
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
		if(mOnClickFunction != nullptr)
			mCallClickFunc = true;
	}

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
				if(mMouseEnterFunction != nullptr)
				{
					mCallMouseEnterFunc = true;
				}
			}
		}
		else
		{
			if(isMouseInside)
			{
				setMouseIsInside(false);
				if(mMouseLeaveFunction != nullptr)
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

void GUIElement::tick()
{
	if(mCallClickFunc)
	{
		mCallClickFunc = false;
		if(mOnClickFunction != nullptr)
			mOnClickFunction();
	}
	if(mCallMouseEnterFunc)
	{
		mCallMouseEnterFunc = false;
		if(mMouseEnterFunction != nullptr)
			mMouseEnterFunction();
	}
	if(mCallMouseLeaveFunc)
	{
		mCallMouseLeaveFunc = false;
		if(mMouseLeaveFunction != nullptr)
			mMouseLeaveFunction();
	}
	if(!mChilds.empty())
	{
		for(std::vector<std::shared_ptr<GUIElement>>::size_type i = 0; i < mChilds.size(); ++i)
		{
			mChilds[i]->tick();
		}
	}
}