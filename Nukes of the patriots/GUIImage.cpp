#include "GUIImage.h"
#include "ResourceHandler.h"


std::shared_ptr<GUIImage> GUIImage::create(std::pair<sf::FloatRect, sf::Texture*> &pair, std::shared_ptr<GUIElement> parent)
{
	std::shared_ptr<GUIImage> ret = std::make_shared<GUIImage>(pair, parent);
	ret->init();
	return ret;
}

GUIImage::GUIImage(std::pair<sf::FloatRect, sf::Texture*> &pair, std::shared_ptr<GUIElement> parent) :
	GUIElement(pair.first, parent, BUTTON)
{
	if (pair.second)
		mSprite.setTexture(*pair.second);
	
	if(mParent != nullptr)
		mSprite.setPosition(getX() + parent->getX(), getY() + parent->getY());
	else
		mSprite.setPosition(getX(), getY());
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