#include "GUIImage.h"
#include "ResourceHandler.h"

std::shared_ptr<GUIImage> GUIImage::create(std::pair<sf::FloatRect, sf::Texture*> &pair, std::shared_ptr<GUIElement> parent)
{
	std::shared_ptr<GUIImage> ret = std::make_shared<GUIImage>(pair, parent);
	ret->init();
	return ret;
}

GUIImage::GUIImage(std::pair<sf::FloatRect, sf::Texture*> &pair, std::shared_ptr<GUIElement> parent) :
	GUIElement(pair.first, parent, IMAGE)
{
	if (pair.second)
		mSprite.setTexture(*pair.second);
	
	if(mParent != nullptr)
		mSprite.setPosition(getX() + parent->getX(), getY() + parent->getY());
	else
		mSprite.setPosition(getX(), getY());
	setSize(pair.first.width, pair.first.height);
}

bool GUIImage::render(sf::RenderWindow *window)
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

		//window->draw(rect);
		
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


void GUIImage::setTexture(std::pair<sf::FloatRect, sf::Texture*> &pair)
{
	mSprite.setTexture(*pair.second);
	mSprite.setPosition(pair.first.left, pair.first.top);
	mSprite.setTextureRect(sf::IntRect(0, 0, pair.second->getSize().x, pair.second->getSize().y));
}

void GUIImage::setSize(float width, float height)
{
	float scaleX = width / mSprite.getTexture()->getSize().x;
	float scaleY = height / mSprite.getTexture()->getSize().y;
	mSprite.setScale(scaleX, scaleY); 
	setWidth(width);
	setHeight(height);
}