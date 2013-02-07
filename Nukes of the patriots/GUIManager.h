#ifndef _GUI_MANAGER_H
#define _GUI_MANAGER_H

#include "GUIElement.h"
#include <functional>

class GUIManager
{
public:
	static GUIManager* getInstance();

	GUIManager();
	void addGUIElement(std::shared_ptr<GUIElement> guiElement);
	void render(sf::RenderWindow &window);
	void update(sf::Event &event);

	void addOnMouseClickEventHandler(std::function <void (std::shared_ptr<GUIElement>)>);
	void addOnMouseEnterEventHandler(std::function <void (std::shared_ptr<GUIElement>)>);
	void addOnMouseLeaveEventHandler(std::function <void (std::shared_ptr<GUIElement>)>);

	void triggerOnMouseClickEvent(std::shared_ptr<GUIElement>);
	void triggerOnMouseEnterEvent(std::shared_ptr<GUIElement>);
	void triggerOnMouseLeaveEvent(std::shared_ptr<GUIElement>);
		~GUIManager(){}
private:
	static GUIManager* mInstance;

	std::vector<std::shared_ptr<GUIElement>> mGuiElements;
	std::vector<std::function <void (std::shared_ptr<GUIElement>)>> mOnMouseClickFuncs;
	std::vector<std::function <void (std::shared_ptr<GUIElement>)>> mOnMouseEnterFuncs;
	std::vector<std::function <void (std::shared_ptr<GUIElement>)>> mOnMouseLeaveFuncs;
};

#endif
