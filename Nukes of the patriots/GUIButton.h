#ifndef _GUI_BUTTON_H
#define _GUI_BUTTON_H

#include "GUIElement.h"
#include <SFML\Graphics\Sprite.hpp>
#include <SFML\Audio\Sound.hpp>
#include <SFML\Audio\SoundBuffer.hpp>
#include <SFML\Audio\Music.hpp>

class GUIButton: public GUIElement
{
public:

	static std::shared_ptr<GUIButton> create(std::pair<sf::FloatRect, sf::Texture*> &pair, std::shared_ptr<GUIElement> parent = 0);
	
	GUIButton(std::pair<sf::FloatRect, sf::Texture*> &pair, std::shared_ptr<GUIElement> parent = 0);
	bool	render(sf::RenderWindow *window);
	void    setTexture(std::pair<sf::FloatRect, sf::Texture*> &pair);
	sf::Texture*	getTexture();
	void	setScale(float width, float height);
	void	setSize(float width, float height);
	void	onGUIClick(int mouseX, int mouseY);

	~GUIButton(){}

private:
	GUIButton(const GUIButton &guiButton);
	GUIButton& operator=(const GUIButton &guiButton);
	sf::Sprite mSprite;

	std::shared_ptr<sf::Music> mOnClickSound;
};

#endif