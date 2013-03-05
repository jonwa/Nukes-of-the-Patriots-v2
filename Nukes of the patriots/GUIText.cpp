#include "GUIText.h"
#include <SFML\System\String.hpp>
#include <iostream>

std::shared_ptr<GUIText> GUIText::create(sf::FloatRect rect, std::string text, std::shared_ptr<GUIElement> parent)
{
	std::shared_ptr<GUIText> ret = std::make_shared<GUIText>(rect, text, parent);
	ret->init();
	return ret;
}

GUIText::GUIText(sf::FloatRect rect, std::string text, std::shared_ptr<GUIElement> parent) :
	GUIElement(rect, parent, TEXT),
	mAlignment("left"),
	mFont(sf::Font::getDefaultFont())
{
	mFont.loadFromFile("Font/MyriadPro-Regular.otf");
	mText.setFont(mFont);
	mText.setString(text);
	mText.setColor(sf::Color::Black);

	sf::FloatRect boundBox = mText.getLocalBounds();
	setWidth(boundBox.width);
	setHeight(boundBox.height);
}

void GUIText::setText(std::string text)
{
	mText.setString(text);
	sf::FloatRect boundBox = mText.getLocalBounds();
	setWidth(boundBox.width);
	setHeight(boundBox.height);
}

std::string GUIText::intToString(int i)
{
	std::stringstream converter;
	converter << i;
	return converter.str();
}

void GUIText::setText(int value)
{
	mText.setString(intToString(value));
	sf::FloatRect boundBox = mText.getLocalBounds();
	setWidth(boundBox.width);
	setHeight(boundBox.height);
}

void GUIText::setScale(float width, float height)
{
	mText.setScale(width, height);
	sf::FloatRect boundBox = mText.getLocalBounds();
	setWidth(boundBox.width);
	setHeight(boundBox.height);
}

void GUIText::setSize(float width, float height)
{
	/*float scaleX = width / mText.getLocalBounds().width;
	float scaleY = height / mText.getLocalBounds().height;
	std::cout << "Scale X: " << scaleX << " Scale Y: " << scaleY << std::endl;
	mText.setScale(scaleX, scaleY);
	setWidth(width);
	setHeight(height);*/
}

bool GUIText::render(sf::RenderWindow *window, sf::RenderStates &states)
{
	bool visible = getVisible();
	if(!visible)return false;
	float x = getLocalX(), y = getLocalY();
	std::shared_ptr<GUIElement> parent = getParent();
	while(parent != NULL)
	{
		visible = parent->getVisible();
		x += parent->getLocalX();
		y += parent->getLocalY();
		if(!visible)
			return false;
		parent = parent->getParent();
	}
	if(visible)
	{		
		float posX = x, posY = y;
		if(mAlignment == "left")
		{
			mText.setPosition(getX(), getY());
			mText.setOrigin(0, 0);
		}
		else if(mAlignment == "middle")
		{
			mText.setPosition(getX(), getY());
			mText.setOrigin(mText.getLocalBounds().width/2, 0);
		}
		else if(mAlignment == "right")
		{
			mText.setPosition(getX(), getY());
			mText.setOrigin(mText.getLocalBounds().width, 0);
		}

		window->draw(mText, states);
	}

	

	if(!mChilds.empty())
	{
		for(std::vector<std::shared_ptr<GUIElement> >::size_type i = 0; i < mChilds.size(); ++i)
		{
			mChilds[i]->render(window, states);
		}
	}
	return true;
}

void GUIText::setAlignment(std::string alignment)
{
	/*if(strcmp(alignment.c_str(), "left") == 0)
		mText.setOrigin(mText.getPosition().x, mText.getPosition().y);
	else if(strcmp(alignment.c_str(), "right") == 0)
		mText.setOrigin(mText.getPosition().x + mText.getGlobalBounds().width, mText.getPosition().y);
	else if(strcmp(alignment.c_str(), "middle") == 0)

		mText.setOrigin(mText.getLocalBounds().width/2, mText.getPosition().y);*/

	mAlignment = alignment;



}


void GUIText::setColor(sf::Color color)
{
	mText.setColor(color);
	GUIElement::setColor(color);
}