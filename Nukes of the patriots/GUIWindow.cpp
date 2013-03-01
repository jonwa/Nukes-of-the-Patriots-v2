#include "GUIWindow.h"

std::shared_ptr<GUIWindow> GUIWindow::create(std::pair<sf::FloatRect, sf::Texture*> &pair, std::shared_ptr<GUIElement> parent)
{
	std::shared_ptr<GUIWindow> ret = std::make_shared<GUIWindow>(pair, parent);
	ret->init();
	return ret;
}

GUIWindow::GUIWindow(std::pair<sf::FloatRect, sf::Texture*> &pair, std::shared_ptr<GUIElement> parent) :
	GUIElement(pair.first, parent, WINDOW)
{
	if (pair.second)
		mSprite.setTexture(*pair.second);
	
	if(mParent != nullptr)
		mSprite.setPosition(getX() + parent->getX(), getY() + parent->getY());
	else
		mSprite.setPosition(getX(), getY());
}

bool GUIWindow::render(sf::RenderWindow *window, sf::RenderStates &states)
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
		rect.setFillColor(sf::Color::Color(255, 0, 0, 100));
		//window.draw(rect);
		mSprite.setPosition(getX(), getY());
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

void GUIWindow::setSize(float width, float height)
{
	float scaleX = width / mSprite.getTexture()->getSize().x;
	float scaleY = height / mSprite.getTexture()->getSize().y;
	mSprite.setScale(scaleX, scaleY); 
	setWidth(width);
	setHeight(height);

	/*for(std::vector<std::shared_ptr<GUIElement> >::size_type i = 0; i < mChilds.size(); ++i)
	{
		mChilds[i]->setScale(scaleX,scaleY);
	}*/
}

void GUIWindow::setColor(sf::Color color)
{
	mSprite.setColor(color);
	GUIElement::setColor(color);
}