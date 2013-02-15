#include "GUIManager.h"
#include <iostream>

GUIManager* GUIManager::mInstance = NULL;
GUIManager* GUIManager::getInstance()
{
	if(mInstance == NULL)
	{
		mInstance = new GUIManager();
	}
	return mInstance;
}

void GUIManager::setWindow(sf::RenderWindow *window)
{
	mWindow = window;
}

void GUIManager::init(sf::RenderWindow *window)
{
	getInstance()->setWindow(window);
}

GUIManager::GUIManager() :
	mMouseDown(false),
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

void GUIManager::render()
{
	for(std::vector<std::shared_ptr<GUIElement> >::iterator it = mGuiElements.begin(); it != mGuiElements.end(); ++it)
	{
		if((*it)->getVisible())
		{
			(*it)->render(mWindow);
		}
	}
}

void GUIManager::update(sf::Event event)
{
	bool clicked = false;
	bool moved = false;
	if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
	{
		if(!mMouseDown)
		{
			mMouseDown = true;
			clicked = true;
		}
	}
	else if(event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
		mMouseDown = false;
	else if(event.type == sf::Event::MouseMoved)
		moved = true;
	for(std::vector<std::shared_ptr<GUIElement> >::size_type i = 0; i < mGuiElements.size(); ++i)
	{
		if(clicked)
			mGuiElements[i]->onClick(mWindow);
		if(moved)
			mGuiElements[i]->onMove(mWindow);
		mGuiElements[i]->update(mWindow, event);
	}
}

void GUIManager::tick()
{
	for(std::vector<std::shared_ptr<GUIElement> >::size_type i = 0; i < mGuiElements.size(); ++i)
	{
		mGuiElements[i]->tick();
	}
}