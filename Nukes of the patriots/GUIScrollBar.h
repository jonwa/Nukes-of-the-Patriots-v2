#ifndef GUISCROLLBAR_H
#define GUISCROLLBAR_H

#include "GUIElement.h"

class GUIScrollBar: public GUIElement
{
public:
	enum Type{HORIZONTAL, VERTICAL};
	static std::shared_ptr<GUIScroolBar> create(std::pair<sf::FloatRect, sf::Texture*> &pair, std::shared_ptr<GUIElement> parent = 0);
	GUIScrollBar(sf::FloatRect rect, Type type, std::shared_ptr<GUIElement> parent = 0);
	~GUIScrollBar(){}

	bool			render(sf::RenderWindow *window, sf::RenderStates &states);
	void			setTexture(std::pair<sf::FloatRect, sf::Texture*> &pair);
	sf::Texture*	getTexture();
	void			setScale(float width, float height);
	void			setSize(float width, float height);
	void			onGUIClick(int mouseX, int mouseY);
	void			canClick(bool i);
	void			setColor(sf::Color color);
	void			setOnGuiChangeFunction(std::function<void()>);
	virtual bool	update(sf::RenderWindow *window, sf::Event event);

private:

};

#endif