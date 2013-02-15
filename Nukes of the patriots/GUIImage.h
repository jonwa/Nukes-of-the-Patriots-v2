/*
 *  GUIImage används för att ladda in bilder 
 *  denna använder i sin tur ResourceHandler för att lägga in dessa
 *  bilder i std::map av sf::Images
 *
 *
 *		Av : Jon Wahlström 
 *           Arvid Backman
 *
 *		2013-01-22
 */
#ifndef GUIIMAGE_H
#define GUIIMAGE_H

#include "GUIElement.h"

class GUIImage : public GUIElement
{
public:
	static std::shared_ptr<GUIImage> create(std::pair<sf::FloatRect, sf::Texture*> &pair, std::shared_ptr<GUIElement> parent = 0);
	GUIImage(std::pair<sf::FloatRect, sf::Texture*> &pair, std::shared_ptr<GUIElement> parent = 0);

	~GUIImage(){}

	void		 setSize(float width, float height);
	void		 setScale(float width, float height);
	void		 setImage(sf::Texture &texture);
	bool		 render (sf::RenderWindow *window);
	void		 setTexture(std::pair<sf::FloatRect, sf::Texture*> &pair);
	sf::Texture* getTexture();

private:
	GUIImage(const GUIImage &guiImage);
	GUIImage& operator=(const GUIImage &guiImage);
	sf::Sprite mSprite;
};

#endif