#ifndef _GUI_MANAGER_H
#define _GUI_MANAGER_H

#include <functional>
#include <SFML/Graphics.hpp>

class GUIElement;

class GUIManager
{
public:
	void clear();
	static GUIManager* getInstance();

	GUIManager();
	void init(sf::RenderWindow *window);
	void addGUIElement(std::shared_ptr<GUIElement> guiElement);
	void render(sf::RenderStates states = sf::RenderStates::Default);
	void update(sf::Event event);
	void tick();
	int getUniqueID();
	std::shared_ptr<GUIElement> getElementByID(int id, std::shared_ptr<GUIElement> parent = nullptr);

	void setOnTop(std::shared_ptr<GUIElement> element);

		~GUIManager();
private:
	void setWindow(sf::RenderWindow *window);
private:
	static GUIManager* mInstance;
	sf::RenderWindow *mWindow;
	std::vector<std::shared_ptr<GUIElement>> mGuiElements;
	bool mMouseDown;
	int mIDIncrement;
};

#endif
