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
	
	mGuiElements(),
	mOnMouseClickFuncs(),
	mOnMouseEnterFuncs(),
	mOnMouseLeaveFuncs()
{
}

void GUIManager::addGUIElement(std::shared_ptr<GUIElement> guiElement)
{
	mGuiElements.push_back(guiElement);
	if(!guiElement->getChildVector().empty())
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
	}
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
	//mGuiElements[0]->update(event);
}

void GUIManager::addOnMouseClickEventHandler(std::function <void (std::shared_ptr<GUIElement>)> func)
{
	mOnMouseClickFuncs.push_back(func);
}


void GUIManager::addOnMouseEnterEventHandler(std::function <void (std::shared_ptr<GUIElement>)> func)
{
	mOnMouseEnterFuncs.push_back(func);
}

void GUIManager::addOnMouseLeaveEventHandler(std::function <void (std::shared_ptr<GUIElement>)> func)
{
	mOnMouseLeaveFuncs.push_back(func);
}

//Trigger events (call the callback functions)
void GUIManager::triggerOnMouseClickEvent(std::shared_ptr<GUIElement> guiElement)
{
	for(std::vector<std::function <void (std::shared_ptr<GUIElement>)>>::iterator it = mOnMouseClickFuncs.begin(); it != mOnMouseClickFuncs.end(); it++)
	{
		if((*it) != NULL)
			(*it)(guiElement);
	}
}

void GUIManager::triggerOnMouseEnterEvent(std::shared_ptr<GUIElement> guiElement)
{
	for(std::vector<std::function <void (std::shared_ptr<GUIElement>)>>::iterator it = mOnMouseEnterFuncs.begin(); it != mOnMouseEnterFuncs.end(); it++)
	{
		if((*it) != NULL)
			(*it)(guiElement);
	}
}

void GUIManager::triggerOnMouseLeaveEvent(std::shared_ptr<GUIElement> guiElement)
{
	for(std::vector<std::function <void (std::shared_ptr<GUIElement>)>>::iterator it = mOnMouseLeaveFuncs.begin(); it != mOnMouseLeaveFuncs.end(); it++)
	{
		if((*it) != NULL)
			(*it)(guiElement);
	}
}