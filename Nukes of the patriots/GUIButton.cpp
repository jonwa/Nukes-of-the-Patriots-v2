#include "GUIButton.h"
#include "ResourceHandler.h"
#include <iostream>
#include "Menu.h"

sf::Shader* GUIButton::mGlowEffect = nullptr;

std::shared_ptr<GUIButton> GUIButton::create(std::pair<sf::FloatRect, sf::Texture*> &pair, std::shared_ptr<GUIElement> parent)
{
	std::shared_ptr<GUIButton> ret = std::make_shared<GUIButton>(pair, parent);
	ret->init();
	return ret;
}

GUIButton::GUIButton(std::pair<sf::FloatRect, sf::Texture*> &pair, std::shared_ptr<GUIElement> parent) :
	GUIElement(pair.first, parent, BUTTON),
	mCanClick(true)
{
	if(mGlowEffect == nullptr)
	{
		mGlowEffect = new sf::Shader();
		if(!mGlowEffect->loadFromFile("effects/glow.frag", sf::Shader::Fragment))
			Menu::getInstance()->getWindow().close();
		mGlowEffect->setParameter("texture", sf::Shader::CurrentTexture);
	}
	if (pair.second)
		mSprite.setTexture(*pair.second);
	
	if(mParent != nullptr)
		mSprite.setPosition(getX() + parent->getX(), getY() + parent->getY());
	else
		mSprite.setPosition(getX(), getY());
	//setSize((pair.first.width == 0) ? pair.second->getSize().x : pair.first.width, (pair.first.height == 0) ? pair.second->getSize().y : pair.first.height);

	mSuccessSound = ResourceHandler::getInstance()->getMusic(std::string("Buttons/click_sound_normal_buttons"));
	mFailedSound  = ResourceHandler::getInstance()->getMusic(std::string("Buttons/click_sound_failed"));
	mOnClickSound = mSuccessSound;
}

bool GUIButton::render(sf::RenderWindow *window, sf::RenderStates &states)
{
	bool visible = getVisible();
	if(!visible) 
		return false;
	std::shared_ptr<GUIElement> parent = getParent();
	float x = getLocalX(), y = getLocalY();
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
		sf::RenderStates _state = states;
		if(getMouseIsInside())
			_state.shader = mGlowEffect;
		//sf::RectangleShape rect(sf::Vector2f(getWidth(), getHeight()));
		//rect.setPosition(x, y);
		//rect.setFillColor(sf::Color::Color(255, 255, 255, 255));

		//window->draw(rect);
		mSprite.setPosition(x, y);
		window->draw(mSprite, _state);
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
	mOnClickSound->play();
}

void GUIButton::canClick(bool i)
{
	if(i)
	{
		mOnClickSound = mSuccessSound;
	}
	else
	{
		mOnClickSound = mFailedSound;
	}
}

void GUIButton::setColor(sf::Color color)
{
	mSprite.setColor(color);
	GUIElement::setColor(color);
}