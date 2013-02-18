#ifndef _GUIEDITFIELD_H
#define _GUIEDITFIELD_H

#include <string>
#include "GUIElement.h"
#include "Timer.h"
#include "TimerHandler.h"

#include <map>

class GUIEditField: public GUIElement
{
public:
	static std::shared_ptr<GUIEditField> create(sf::FloatRect rect, std::string text = "", bool onlyNumbers = false, std::shared_ptr<GUIElement> parent = 0);
				GUIEditField(sf::FloatRect rect, std::string text = "", bool onlyNumbers = false, std::shared_ptr<GUIElement> parent = 0);
	std::string getText()const{ return mText.getString(); }
	bool		getCaretVisible();
	void		setCaretVisible(bool visible);
	void		setText(std::string text);
	void		setScale(float width, float height);
	bool		render(sf::RenderWindow *window);

	void				setOnGuiChangeFunction(std::function<void()>);
	virtual bool		update(sf::RenderWindow *window, sf::Event event);
	virtual void		onGUIClick(int mouseX, int mouseY);

	void				setTexture(std::pair<sf::FloatRect, sf::Texture*> &pair);
				~GUIEditField(){}
private:
	sf::Text mText;
	sf::Font mFont;
	sf::RenderTexture mRenderTexture;
	Timer *mCaretTimer;
	bool mCaretVisible, mOnlyNumbers;
	int mCaretIndex, mSelectedCaret;
	float mOffsetX, mOffsetY;
	sf::RectangleShape mCaretShape, mSelectedShape;
	sf::Sprite mSprite;

	std::function<void()> mOnGuiChange;
};

#endif