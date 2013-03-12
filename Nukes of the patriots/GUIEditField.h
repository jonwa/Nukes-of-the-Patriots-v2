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
	enum Type { CAP, COM, MENU };
	static std::shared_ptr<GUIEditField> create(sf::FloatRect rect, Type type, std::string text = "", bool onlyNumbers = false, std::shared_ptr<GUIElement> parent = 0);
				GUIEditField(sf::FloatRect rect, Type type, std::string text = "", bool onlyNumbers = false, std::shared_ptr<GUIElement> parent = 0);
				~GUIEditField(){}
	std::string		getText()const{ return mText.getString(); }
	bool			getCaretVisible();
	int				getMaxCharacters(){ return mMaxCharacters; }
	void			setCaretVisible(bool visible);
	void			setText(std::string text);
	void			setScale(float width, float height);
	void			setMaxCharacters(int maxCharacters){ mMaxCharacters = maxCharacters; }
	void			setPlaceHolderText(std::string string);
	bool			render(sf::RenderWindow *window, sf::RenderStates &states);

	virtual bool	update(sf::RenderWindow *window, sf::Event event);
	virtual void	onGUIClick(int mouseX, int mouseY);

	void			setTexture(std::pair<sf::FloatRect, sf::Texture*> &pair);
private:
	sf::Text mText, mPlaceHolderText;
	sf::Font mFont;
	sf::RenderTexture mRenderTexture;
	Timer *mCaretTimer;
	bool mCaretVisible, mOnlyNumbers;
	int mCaretIndex, mSelectedCaret, mMaxCharacters;
	float mOffsetX, mOffsetY;
	sf::RectangleShape mCaretShape, mSelectedShape;
	sf::Sprite mSprite;

};

#endif