#include "GUIButton.h"
#include "ResourceHandler.h"
#include <iostream>

std::shared_ptr<GUIButton> GUIButton::create(std::pair<sf::FloatRect, sf::Texture*> &pair, std::shared_ptr<GUIElement> parent)
{
	std::shared_ptr<GUIButton> ret = std::make_shared<GUIButton>(pair, parent);
	ret->init();
	return ret;
}

GUIButton::GUIButton(std::pair<sf::FloatRect, sf::Texture*> &pair, std::shared_ptr<GUIElement> parent) :
	GUIElement(pair.first, parent, BUTTON)
{
	if (pair.second)
		mSprite.setTexture(*pair.second);

	mSprite.setPosition(getX(), getY());
}

bool GUIButton::render(sf::RenderWindow *window)
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
		sf::RectangleShape rect(sf::Vector2f(getWidth(), getHeight()));
		rect.setPosition(getX(), getY());
		rect.setFillColor(sf::Color::Color(255, 255, 255, 255));
		window->draw(rect);
		//mSprite.setPosition(getX(), getY());
		window->draw(mSprite);
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


void GUIButton::setTexture(std::pair<sf::FloatRect, sf::Texture*> &pair)
{
	mSprite.setTexture(*pair.second);
	mSprite.setPosition(pair.first.left, pair.first.top);
	mSprite.setTextureRect(sf::IntRect(0, 0, pair.first.width, pair.first.height));
}
