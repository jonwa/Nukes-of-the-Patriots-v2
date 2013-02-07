#include "GUIElement.h"

GUIElement::GUIElement(sf::FloatRect rect, std::shared_ptr<GUIElement> parent, GUIType guiType) : 
	mRectangle(rect),
	mVisible(true),
	mParent(parent),
	mGUIType(guiType),
	mMouseInside(false),
	mUpdated(false)
{
	if(mParent != NULL)
	{
		mParent->addChild(getPtr());
		setX(mRectangle.left += mParent->getX());
		setY(mRectangle.top += mParent->getY());
	}
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

std::shared_ptr<GUIElement> GUIElement::getPtr()
{
	return std::shared_ptr<GUIElement>(this);
	//return std::shared_ptr<GUIElement>(shared_from_this());
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
	for(std::vector<std::shared_ptr<GUIElement>>::size_type i = 0; i < mChilds.size(); ++i)
	{
		mChilds[i]->setVisible(visible);
	}
}
void GUIElement::setAlpha(int alpha)
{
	mAlpha = alpha; 
}

void GUIElement::setUpdated(bool update)
{
	mUpdated = update;
}

void GUIElement::update(sf::Event &event)
{
	if(mVisible && !mUpdated)
	{
		mUpdated = true;
		sf::Vector2i mousePos = sf::Mouse::getPosition();

		bool isMouseInside = getMouseIsInside();
		// Check if mouse is colliding with gui element
		if(mRectangle.contains(mousePos.x, mousePos.y))
		{
			if(!isMouseInside)
			{
				setMouseIsInside(true);
				if(mMouseEnterFunction != nullptr)
					mMouseEnterFunction();
			}
			if(event.type == sf::Event::MouseButtonPressed)
			{
				if(event.mouseButton.button == sf::Mouse::Left)
				{
					if(mOnClickFunction != nullptr)
						mOnClickFunction();
				}
			}
		}
		else
		{
			if(isMouseInside)
			{
				setMouseIsInside(false);
				if(mMouseLeaveFunction != nullptr)
					mMouseLeaveFunction();
			}
		}

		if(!mChilds.empty())
		{
			for(std::vector<std::shared_ptr<GUIElement>>::size_type i = 0; i < mChilds.size(); ++i)
			{
				mChilds[i]->update(event);
			}
		}
	}
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
