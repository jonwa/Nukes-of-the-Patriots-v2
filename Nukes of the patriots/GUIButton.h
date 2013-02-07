#ifndef _GUI_BUTTON_H
#define _GUI_BUTTON_H

#include "GUIElement.h"
#include <SFML\Graphics\Sprite.hpp>

#include <map>

class GUIButton: public GUIElement
{
public:
			GUIButton(std::pair<sf::FloatRect, sf::Texture*> &pair, std::shared_ptr<GUIElement> parent = 0);
	void	render(sf::RenderWindow &window);
	void    setTexture(std::pair<sf::FloatRect, sf::Texture*> &pair);

	~GUIButton(){}

private:
	sf::Sprite mSprite;
};

#endif