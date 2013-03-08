#ifndef GUISCROLLBAR_H
#define GUISCROLLBAR_H

#include "GUIElement.h"
#include <map>
#include <functional>

class GUIScrollBar: public GUIElement
{
public:	
	static std::shared_ptr<GUIScrollBar> create(sf::FloatRect rect, std::shared_ptr<GUIElement> parent = 0);
	GUIScrollBar(sf::FloatRect rect, std::shared_ptr<GUIElement> parent = 0);
	~GUIScrollBar(){}

	bool			render(sf::RenderWindow *window, sf::RenderStates &states);
	void			onGUIClick(int mouseX, int mouseY);
	void			setSize(float width, float height);
	void			setOnGuiChangeFunction(std::function<void()>);
	int				getValue();
	void			setValue(int value);
	virtual bool	update(sf::RenderWindow *window, sf::Event event);
	sf::Sprite*	getSprite();
private:
	sf::RenderTexture mRenderTexture;
	bool mFollowMouse;
	sf::Sprite mSprite;
	sf::Sprite mScrollBarBackground;

	int mValue;

	std::function<void()> mOnGuiChange;
};

#endif