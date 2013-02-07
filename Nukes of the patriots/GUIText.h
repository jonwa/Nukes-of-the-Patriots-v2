#ifndef _GUI_TEXT_H
#define _GUI_TEXT_H

#include "GUIElement.h"
#include <string>

class GUIText: public GUIElement
{
public:
				GUIText(sf::FloatRect rect, std::string text, std::shared_ptr<GUIElement> parent = 0);
	std::string getText()const{ return mText.getString(); }
	void		setText(std::string text);
	void		setScale(float width, float height);
	void		render(sf::RenderWindow &window);
				~GUIText(){}
private:
	sf::Text mText;
	sf::Font mFont;
};

#endif