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
	
	if(mParent != nullptr)
		mSprite.setPosition(getX() + parent->getX(), getY() + parent->getY());
	else
		mSprite.setPosition(getX(), getY());
	//setSize((pair.first.width == 0) ? pair.second->getSize().x : pair.first.width, (pair.first.height == 0) ? pair.second->getSize().y : pair.first.height);

	onClickSoundBuff.loadFromFile("Music/click_sound_normal_buttons.wav");
	onClickSound.setBuffer(onClickSoundBuff);
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


void GUIButton::setTexture(std::pair<sf::FloatRect, sf::Texture*> &pair)
{
	mSprite.setTexture(*pair.second);
	mSprite.setPosition(pair.first.left, pair.first.top);
	mSprite.setTextureRect(sf::IntRect(0, 0, pair.second->getSize().x, pair.second->getSize().y));
}

sf::Texture* GUIButton::getTexture()  
{
	sf::Texture* result = const_cast<sf::Texture*>(mSprite.getTexture());
	return result;
}

void GUIButton::setScale(float width, float height)
{
	/*mSprite.setTextureRect(sf::IntRect(0, 0, mRectangle.width, mRectangle.height));*/
	mSprite.setScale(width, height);

	//std::cout << mSprite.getScale().x << " " << mSprite.getScale().y  << " " << mSprite.getTextureRect().width << " " << mSprite.getTextureRect().height << std::endl;
}

void GUIButton::setSize(float width, float height)
{
	float scaleX = width / mSprite.getTexture()->getSize().x;
	float scaleY = height / mSprite.getTexture()->getSize().y;
	mSprite.setScale(scaleX, scaleY); 
	setWidth(width);
	setHeight(height);
}

void GUIButton::onGUIClick(int mouseX, int mouseY)
{
	onClickSound.play();
	onClickSound.setVolume(15);
}