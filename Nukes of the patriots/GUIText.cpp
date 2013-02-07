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
	mFont(sf::Font::getDefaultFont())
{
	mText.setFont(mFont);
	mText.setString(text);
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

void GUIText::setScale(float width, float height)
{
	mText.setScale(width, height);
	sf::FloatRect boundBox = mText.getGlobalBounds();
	setWidth(boundBox.width);
	setHeight(boundBox.height);
}

void GUIText::render(sf::RenderWindow &window)
{
	bool visible = getVisible();
	std::shared_ptr<GUIElement> parent = getParent();
	while(parent != NULL)
	{
		visible = parent->getVisible();
		if(!visible)
			break;
		parent = parent->getParent();
	}
	if(visible)
	{
		mText.setColor(sf::Color::Color(0, 255, 255, 255));
		mText.setPosition((sf::Vector2f(getX(), getY())));
		window.draw(mText);
	}

	

	if(!mChilds.empty())
	{
		for(std::vector<GUIElement*>::size_type i = 0; i < mChilds.size(); ++i)
		{
			mChilds[i]->render(window);
		}
	}
}