#ifndef _GUI_WINDOW_H
#define _GUI_WINDOW_H

#include "GUIElement.h"
#include <map>
#include <SFML\Graphics\Sprite.hpp>

class GUIWindow: public GUIElement
{
public:
			GUIWindow(std::pair<sf::FloatRect, sf::Texture*> &pair, std::shared_ptr<GUIElement> parent = 0);
	void	render(sf::RenderWindow &window);
			~GUIWindow(){}

private:
	sf::Sprite mSprite;
};

#endif