#include "GUIImage.h"
#include "ResourceHandler.h"


std::shared_ptr<GUIImage> GUIImage::create(std::pair<sf::FloatRect, sf::Texture*> &pair, std::shared_ptr<GUIElement> parent)
{
	if(pair.second != nullptr)
	{
		pair.first.width = (pair.first.width == 0) ? pair.second->getSize().x : pair.first.width;
		pair.first.height = (pair.first.height == 0) ? pair.second->getSize().y : pair.first.height;
	}
	std::shared_ptr<GUIImage> ret = std::make_shared<GUIImage>(pair, parent);
	ret->init();
	return ret;
}

GUIImage::GUIImage(std::pair<sf::FloatRect, sf::Texture*> &pair, std::shared_ptr<GUIElement> parent) :

	GUIElement(pair.first, parent, IMAGE)

{
	if(pair.second != nullptr)
	{
		mSprite.setTexture(*pair.second);
		setSize(pair.first.width, pair.first.height);
	}
	
	if(mParent != nullptr)
		mSprite.setPosition(getX() + parent->getX(), getY() + parent->getY());
	else
		mSprite.setPosition(getX(), getY());
		

}

bool GUIImage::render(sf::RenderWindow *window, sf::RenderStates &states)
{
	bool visible = getVisible();
	if(!visible)return false;
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
	if(visible)

	{
		mSprite.setPosition(x, y);
		window->draw(mSprite, states);
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


void GUIImage::setTexture(std::pair<sf::FloatRect, sf::Texture*> &pair)
{
	mSprite.setTexture(*pair.second);
	mSprite.setPosition(pair.first.left, pair.first.top);
	mSprite.setTextureRect(sf::IntRect(0, 0, pair.second->getSize().x, pair.second->getSize().y));
	if(pair.second != nullptr)
	{
		pair.first.width = (pair.first.width == 0) ? pair.second->getSize().x : pair.first.width;
		pair.first.height = (pair.first.height == 0) ? pair.second->getSize().y : pair.first.height;
		setWidth(pair.first.width);
		setHeight(pair.first.height);
	}
}

sf::Texture* GUIImage::getTexture()  
{
	sf::Texture* result = const_cast<sf::Texture*>(mSprite.getTexture());
	return result;
}

void GUIImage::setScale(float width, float height)
{
	/*mSprite.setTextureRect(sf::IntRect(0, 0, mRectangle.width, mRectangle.height));*/
	mSprite.setScale(width, height);
}	

void GUIImage::setSize(float width, float height)
{
	float scaleX = width / mSprite.getTexture()->getSize().x;
	float scaleY = height / mSprite.getTexture()->getSize().y;
	mSprite.setScale(scaleX, scaleY); 
	setWidth(width);
	setHeight(height);
}

void GUIImage::setColor(sf::Color color)
{
	mSprite.setColor(color);
	GUIElement::setColor(color);
}

