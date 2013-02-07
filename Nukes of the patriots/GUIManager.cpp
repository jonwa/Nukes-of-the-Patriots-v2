#include "GUIManager.h"

GUIManager* GUIManager::mInstance = NULL;
GUIManager* GUIManager::getInstance()
{
	if(mInstance == NULL)
	{
		mInstance = new GUIManager();
	}
	return mInstance;
}

GUIManager::GUIManager() :
	
	mGuiElements()
{
}

void GUIManager::addGUIElement(std::shared_ptr<GUIElement> guiElement)
{
	mGuiElements.push_back(guiElement);
	/*if(!guiElement->getChildVector().empty())
	{
		for(std::vector<std::shared_ptr<GUIElement>>::size_type i = 0; i < guiElement->getChildVector().size(); ++i)
		{
			mGuiElements.push_back(guiElement->getChildVector()[i]);
			if(!guiElement->getChildVector().empty())
			{
				for(std::vector<std::shared_ptr<GUIElement> >::size_type j = 0; j < guiElement->getChildVector()[i]->getChildVector().size(); ++j)
				{
					mGuiElements.push_back(guiElement->getChildVector()[i]->getChildVector()[j]);
				}
			}
		}
	}*/
}

void GUIManager::render(sf::RenderWindow &window)
{
	for(std::vector<std::shared_ptr<GUIElement> >::iterator it = mGuiElements.begin(); it != mGuiElements.end(); ++it)
	{
		if((*it)->getVisible())
		{
				(*it)->render(window);
		}
	}
}

void GUIManager::update(sf::Event &event)
{
	sf::Vector2i mousePos = sf::Mouse::getPosition();
	for(std::vector<std::shared_ptr<GUIElement> >::size_type i = 0; i < mGuiElements.size(); ++i)
	{
		mGuiElements[i]->update(event);
	}
	for(std::vector<std::shared_ptr<GUIElement>>::size_type i = 0; i < mGuiElements.size(); ++i)
	{
		mGuiElements[i]->setUpdated(false);
	}
}