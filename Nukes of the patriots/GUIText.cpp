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
	mFont.loadFromFile("Font/georgia.ttf");
	mText.setFont(mFont);
	mText.setString(text);
	mText.setColor(sf::Color::Black);
	sf::FloatRect boundBox = mText.getGlobalBounds();
	setWidth(boundBox.width);
	setHeight(boundBox.height);
}

void GUIText::setText(std::string text)
{
	mText.setString(sf::String(text.c_str()));
	sf::FloatRect boundBox = mText.getGlobalBounds();
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
	mText.setString(sf::String(intToString(value).c_str()));
	sf::FloatRect boundBox = mText.getGlobalBounds();
	setWidth(boundBox.width);
	setHeight(boundBox.height);
}

void GUIText::setScale(float width, float height)
{
	mText.setScale(width, height);
	sf::FloatRect boundBox = mText.getGlobalBounds();
	setWidth(boundBox.width);
	setHeight(boundBox.height);
}

bool GUIText::render(sf::RenderWindow *window)
{
	bool visible = getVisible();
	if(!visible)return false;
	std::shared_ptr<GUIElement> parent = getParent();
	while(parent != NULL)
	{
		visible = parent->getVisible();
		if(!visible)
			return false;
		parent = parent->getParent();
	}
	if(visible)
	{

		float posX = getX(), posY = getY();
		if(mAlignment == "left")
			posX += 0;
		else if(mAlignment == "middle")
		{
			posX -= mText.getGlobalBounds().width/2;
			posY -= mText.getGlobalBounds().height;
		}
		else if(mAlignment == "right")
			posX -= mText.getGlobalBounds().width;

		mText.setPosition(sf::Vector2f(posX, posY));

		window->draw(mText);
	}

	

	if(!mChilds.empty())
	{
		for(std::vector<GUIElement*>::size_type i = 0; i < mChilds.size(); ++i)
		{
			mChilds[i]->render(window);
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