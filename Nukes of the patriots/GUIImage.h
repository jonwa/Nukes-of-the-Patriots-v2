///*
// *  GUIImage används för att ladda in bilder 
// *  denna använder i sin tur ResourceHandler för att lägga in dessa
// *  bilder i std::map av sf::Images
// *
// *
// *		Av : Jon Wahlström 
// *           Arvid Backman
// *
// *		2013-01-22
// */
//#ifndef GUIIMAGE_H
//#define GUIIMAGE_H
//
//#include "GUIElement.h"
//
//class GUIImage : public GUIElement
//{
//public:
//	GUIImage(sf::FloatRect rect, std::string string, std::shared_ptr<GUIElement> parent = 0);
//
//	~GUIImage(){}
//
//	sf::Sprite getImage() const;
//
//	void setImage(std::string &string);
//	bool render (sf::RenderWindow &window);
//	
//private:
//	sf::Texture mTexture;
//	sf::Sprite mSprite;
//};
//
//#endif