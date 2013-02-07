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

		~GUIManager(){}
private:
	static GUIManager* mInstance;

	std::vector<std::shared_ptr<GUIElement>> mGuiElements;

};

#endif
