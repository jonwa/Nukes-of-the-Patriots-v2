#include "GUIWindow.h"

GUIWindow::GUIWindow(std::pair<sf::FloatRect, sf::Texture*> &pair, std::shared_ptr<GUIElement> parent) :
	GUIElement(pair.first, parent, WINDOW)
{
	if (pair.second)
		mSprite.setTexture(*pair.second);
	
	//mSprite.setPosition(pair.first.left, pair.first.top);
}

void GUIWindow::render(sf::RenderWindow &window)
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
		sf::RectangleShape rect(sf::Vector2f(getWidth(), getHeight()));
		rect.setPosition(getX(), getY());
		rect.setFillColor(sf::Color::Color(255, 0, 0, 100));
		//window.draw(rect);
		mSprite.setPosition(getX(), getY());
		window.draw(mSprite);
	}


	/*if(!mChilds.empty())
	{
		for(std::vector<GUIElement*>::size_type i = 0; i < mChilds.size(); ++i)
		{
			mChilds[i]->render(window);
		}
	}*/
}