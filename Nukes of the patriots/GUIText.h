#ifndef _GUI_TEXT_H
#define _GUI_TEXT_H

#include "GUIElement.h"
#include <string>
#include <sstream>

class GUIText: public GUIElement
{
public:
	static std::shared_ptr<GUIText> create(sf::FloatRect rect, std::string text, std::shared_ptr<GUIElement> parent = 0);
		
	GUIText(sf::FloatRect rect, std::string text, std::shared_ptr<GUIElement> parent = 0);
	std::string getText()const{ return mText.getString(); }
	void		setText(std::string text);
	void		setText(int value);
	void		setScale(float width, float height);
	void		setAlignment(std::string alignment);
	bool		render(sf::RenderWindow *window, sf::RenderStates &states);
	void		setColor(sf::Color color);
	std::string intToString(int i);
				~GUIText(){}
private:
	GUIText(const GUIText &guiText);
	GUIText& operator=(const GUIText &guiText);
	sf::Text mText;
	sf::Font mFont;
	std::string mAlignment;
};

#endif