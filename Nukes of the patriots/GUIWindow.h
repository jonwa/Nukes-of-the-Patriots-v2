#ifndef _GUI_WINDOW_H
#define _GUI_WINDOW_H

#include "GUIElement.h"
#include <map>
#include <SFML\Graphics\Sprite.hpp>

class GUIWindow: public GUIElement
{
public:
	static std::shared_ptr<GUIWindow> create(std::pair<sf::FloatRect, sf::Texture*> &pair, std::shared_ptr<GUIElement> parent = 0);
	
	GUIWindow(std::pair<sf::FloatRect, sf::Texture*> &pair, std::shared_ptr<GUIElement> parent = 0);
	bool	render(sf::RenderWindow *window);
	void	setSize(float width, float height);
	void	setColor(sf::Color color);
			~GUIWindow(){}

private:
	GUIWindow(const GUIWindow &guiWindow);
	GUIWindow& operator=(const GUIWindow &guiWindow);
	sf::Sprite mSprite;
};

#endif