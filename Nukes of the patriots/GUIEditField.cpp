#include "GUIEditField.h"
#include <SFML\System\String.hpp>
#include "ResourceHandler.h"
#include "GameManager.h"
#include <iostream>
#include <sstream>

std::shared_ptr<GUIEditField> GUIEditField::create(sf::FloatRect rect, Type type, std::string text, bool onlyNumbers, std::shared_ptr<GUIElement> parent)
{
	std::shared_ptr<GUIEditField> ret = std::make_shared<GUIEditField>(rect, type, text, onlyNumbers, parent);
	ret->init();
	return ret;
}

GUIEditField::GUIEditField(sf::FloatRect rect, Type type, std::string text, bool onlyNumbers, std::shared_ptr<GUIElement> parent) :
	GUIElement(rect, parent, EDIT_FIELD),
	mFont(sf::Font::getDefaultFont()),mRenderTexture(),mCaretVisible(true),mCaretIndex(0),mCaretShape(sf::Vector2f(0.0, 0.0)),mSelectedCaret(-1),mSelectedShape(sf::Vector2f(0.0, 0.0)),
	mOnlyNumbers(onlyNumbers),mOffsetX(1),mOffsetY(1),mMaxCharacters(0)
{
	mFont.loadFromFile("Font/MyriadPro-Regular.otf");
	mText.setFont(mFont);
	mPlaceHolderText.setFont(mFont);
	sf::Texture *texture;
	switch(type)
	{
	case COM:
		texture = &ResourceHandler::getInstance()->getTexture(std::string("Communist/Communist_Edit_Field"));
		break;
	case CAP:
		texture = &ResourceHandler::getInstance()->getTexture(std::string("Capitalist/Capitalist_Edit_Field"));
		break;
	case MENU:
		texture = &ResourceHandler::getInstance()->getTexture(std::string("Menu/Namnruta-aktiv"));
		break;
	}
	
	mSprite.setTexture(*texture);
	
	if(mParent != nullptr)
		mSprite.setPosition(getX() + parent->getX(), getY() + parent->getY());
	else
		mSprite.setPosition(getX(), getY());
	mCaretTimer = Timer::setTimer([&](){ this->setCaretVisible(!(this->getCaretVisible()));}, 500, 0);
	float scaleX = rect.width / (*texture).getSize().x;
	float scaleY = rect.height / (*texture).getSize().y;
	mOffsetX = scaleX;
	mOffsetY = scaleY;
	mRenderTexture.create(rect.width - 14*scaleX, rect.height - 14*scaleY);
	rect.width = (rect.width == 0) ? (*texture).getSize().x : rect.width;
	rect.height = (rect.height == 0) ? (*texture).getSize().y : rect.height;
	mSprite.setScale(scaleX, scaleY);
	mText.setString("");
	mPlaceHolderText.setString(sf::String(text.c_str()));
	mText.setScale(scaleY, scaleY);
	mPlaceHolderText.setScale(scaleY, scaleY);
	//sf::FloatRect boundBox = mText.getGlobalBounds();
	//setWidth(static_cast<int>(boundBox.width));
	//setHeight(static_cast<int>(boundBox.height));
}

bool GUIEditField::getCaretVisible()
{
	return mCaretVisible;
}

void GUIEditField::setPlaceHolderText(std::string string)
{
	mPlaceHolderText.setString(string.c_str());
}

void GUIEditField::setCaretVisible(bool visible)
{
	mCaretVisible = visible;
}

void GUIEditField::setText(std::string text)
{
	mText.setString(sf::String(text.c_str()));
	//sf::FloatRect boundBox = mText.getGlobalBounds();
	//setWidth(static_cast<int>(boundBox.width));
	//setHeight(static_cast<int>(boundBox.height));
}

void GUIEditField::setScale(float width, float height)
{
	if(height > getHeight()) height = getHeight();
	mText.setScale(width, height);
	//sf::FloatRect boundBox = mText.getGlobalBounds();
	//setWidth(static_cast<int>(boundBox.width));
	//setHeight(static_cast<int>(boundBox.height));
}

bool GUIEditField::render(sf::RenderWindow *window, sf::RenderStates &states)
{
	bool visible = getVisible();
	if(!visible)return false;
	float x = getLocalX(), y = getLocalY();
	std::shared_ptr<GUIElement> parent = getParent();
	while(parent != NULL)
	{
		x += parent->getLocalX();
		y += parent->getLocalY();
		visible = parent->getVisible();
		if(!visible)
			return false;
		parent = parent->getParent();
	}
	mRenderTexture.clear(sf::Color::Color(255, 255, 255, 0));
	//mText.setPosition((sf::Vector2f(x, y)));
	mText.setColor(sf::Color::Color(255, 255, 255, 255));
	//mRenderTexture.draw(mSprite);
	mRenderTexture.draw(mText, states);
	if(mPlaceHolderText.getString().getSize() > 0 && mText.getString().getSize() == 0 && !mSelected)
	{
		mRenderTexture.draw(mPlaceHolderText, states);
	}
	if(isSelected())
	{
		if(mSelectedCaret != -1)
		{
			sf::Vector2f caretPos = mText.findCharacterPos(mCaretIndex);
			sf::Vector2f caretSelectedPos = mText.findCharacterPos(mSelectedCaret);
			mSelectedShape.setFillColor(sf::Color::Color(0, 0, 255, 150));
			mSelectedShape.setPosition(caretPos);
			mSelectedShape.setSize(sf::Vector2f(caretSelectedPos.x - caretPos.x, getHeight()));
			mRenderTexture.draw(mSelectedShape, states);
		}
		if(mCaretVisible)
		{
			mCaretShape.setFillColor(sf::Color::Color(0, 0, 0, 255));
			mCaretShape.setSize(sf::Vector2f(2.5, getHeight()));
			mCaretShape.setPosition(mText.findCharacterPos(mSelectedCaret != -1 ? mSelectedCaret : mCaretIndex));
			mRenderTexture.draw(mCaretShape, states);
		}
	}
	mRenderTexture.display();

	sf::Sprite sprite(mRenderTexture.getTexture());
	sprite.setPosition(sf::Vector2f(x + 8*mOffsetX, y + 8*mOffsetY));
	mSprite.setPosition(getX(), getY());
	window->draw(mSprite, states);
	window->draw(sprite, states);

	if(!mChilds.empty())
	{
		for(std::vector<std::shared_ptr<GUIElement> >::size_type i = 0; i < mChilds.size(); ++i)
		{
			mChilds[i]->render(window, states);
		}
	}
	return true;
}

bool GUIEditField::update(sf::RenderWindow *window, sf::Event event)
{
	TimerHandler::getInstance()->tick();
	//int value = 0;
	//std::istringstream("1337")>>value;
	//std::cout<<value<<std::endl;
	if(isSelected())
	{
		if(event.type == sf::Event::KeyPressed)
		{
			int key = event.key.code;
			if(key == 71 && mCaretIndex >= 0)
			{
				mCaretTimer->resetTimer();
				mCaretVisible = true;
				if(isSelected() && sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
				{
					if(mSelectedCaret == -1)
						mSelectedCaret = mCaretIndex - 1;
					else
						mSelectedCaret--;

					if(mSelectedCaret < 0)
						mSelectedCaret = 0;
				}
				if(!sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
				{
					if(mSelectedCaret == -1)
						mCaretIndex--;
					if(mSelectedCaret != -1)
						mCaretIndex = mSelectedCaret--;
					mSelectedCaret = -1;
				}
				if(mCaretIndex < 0)
					mCaretIndex = 0;
			}
			else if(key == 72 && mCaretIndex <= mText.getString().getSize())
			{
				mCaretTimer->resetTimer();
				mCaretVisible = true;
				if(isSelected() && sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
				{
					if(mSelectedCaret == -1)
						mSelectedCaret = mCaretIndex + 1;
					else
						mSelectedCaret++;

					if(mSelectedCaret > mText.getString().getSize())
						mSelectedCaret = mText.getString().getSize();
				}
				if(!sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
				{
					if(mSelectedCaret == -1)
						mCaretIndex++;
					if(mSelectedCaret != -1)
						mCaretIndex = mSelectedCaret++;
					mSelectedCaret = -1;
				}
				if(mCaretIndex > mText.getString().getSize())
					mCaretIndex = mText.getString().getSize();
			}
			else if(key == 59 && mCaretIndex >= 0)
			{
				mCaretTimer->resetTimer();
				mCaretVisible = true;
				sf::String str = mText.getString();
				if(mSelectedCaret == -1)
				{
					if(mCaretIndex > 0)
					{
						str.erase(mCaretIndex - 1);
						mCaretIndex--;
					}
				}
				else
				{
					int characters = mSelectedCaret - mCaretIndex;
					int eraseIndex = (characters < 0 ? mCaretIndex - std::abs(characters) : mCaretIndex);
					characters = (characters == 0 ? 1 : characters);
					str.erase(eraseIndex, std::abs(characters));
					if(characters < 0)
						mCaretIndex -= std::abs(characters);
				}
				mText.setString(str);
				if(mOnGuiChange != NULL)
				{
					mOnGuiChange();
					if(GameManager::getInstance()->getGameType() == LAN)
					{
						GameManager::getInstance()->syncGUIChange(shared_from_this());
					}

					
				}
				if(GameManager::getInstance()->getGameType() == LAN)
				{
					GameManager::getInstance()->syncGUIEditField(shared_from_this());
				}
				mSelectedCaret = -1;
				if(mCaretIndex < 0)
					mCaretIndex = 0;
			}
			else if(key == 66)
			{
				mCaretTimer->resetTimer();
				mCaretVisible = true;
				sf::String str = mText.getString();
				if(mSelectedCaret == -1)
				{
					str.erase(mCaretIndex);
				}
				else
				{
					int characters = mSelectedCaret - mCaretIndex;
					int eraseIndex = (characters < 0 ? mCaretIndex - std::abs(characters) : mCaretIndex);
					characters = (characters == 0 ? 1 : characters);
					str.erase(eraseIndex, std::abs(characters));
					if(characters < 0)
						mCaretIndex -= std::abs(characters);
				}
				mText.setString(str);

				if(mOnGuiChange != NULL)
				{
					mOnGuiChange();
					if(GameManager::getInstance()->getGameType() == LAN)
					{
						GameManager::getInstance()->syncGUIChange(shared_from_this());
					}
				}
				if(GameManager::getInstance()->getGameType() == LAN)
				{
					GameManager::getInstance()->syncGUIEditField(shared_from_this());
				}

				mSelectedCaret = -1;
			}
		}
		if(event.type == sf::Event::TextEntered)
		{
			mCaretTimer->resetTimer();
			mCaretVisible = true;
			char key = static_cast<char>(event.text.unicode);
			if(!mOnlyNumbers || (mOnlyNumbers && event.text.unicode >= 48 && event.text.unicode <= 57))
			{
				if(event.text.unicode < 128 && event.text.unicode != 13 && event.text.unicode != 8)
				{
					sf::String str = mText.getString();
					if(mCaretIndex > str.getSize())
						mCaretIndex = str.getSize();
					if(mSelectedCaret != -1)
					{
						int characters = mSelectedCaret - mCaretIndex;
						int eraseIndex = (characters < 0 ? mCaretIndex - std::abs(characters) : mCaretIndex);
						characters = (characters == 0 ? 1 : characters);
						str.erase(eraseIndex, std::abs(characters));
						if(characters < 0)
							mCaretIndex -= std::abs(characters);
						str.insert(mCaretIndex, key);
					}
					else if(mMaxCharacters == 0 || str.getSize() < mMaxCharacters)
						str.insert(mCaretIndex, key);
					mCaretIndex++;
					mText.setString(str);

					mSelectedCaret = -1;
					if(mOnGuiChange != NULL)
					{
						mOnGuiChange();
						if(GameManager::getInstance()->getGameType() == LAN)
						{
							GameManager::getInstance()->syncGUIChange(shared_from_this());
						}
					}
					if(GameManager::getInstance()->getGameType() == LAN)
					{
						GameManager::getInstance()->syncGUIEditField(shared_from_this());
					}

					mSelectedCaret = -1;
				}
			}
		}
		if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			mCaretTimer->resetTimer();
			mCaretVisible = true;
			sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
			mousePos.x -= getX();
			mousePos.y -= getY();
			sf::String str = mText.getString();
			int newCaret = str.getSize();
			for(size_t i = 0; i < str.getSize(); i++)
			{
				sf::Vector2f strPos = mText.findCharacterPos(i);
				sf::Text text;
				text.setFont(mFont);
				sf::String str = mText.getString();
				text.setString(str[i]);
				sf::FloatRect boundBox = text.getGlobalBounds();
				if(mousePos.x < strPos.x + boundBox.width/2)
				{
					newCaret = i;
					break;
				}
			}
			mSelectedCaret = newCaret;
			if(mSelectedCaret == mCaretIndex)
				mSelectedCaret = -1;
			if(mCaretIndex > str.getSize())
				mCaretIndex = str.getSize();
		}
	}
	GUIElement::update(window, event);
	return true;
}

void GUIEditField::onGUIClick(int mouseX, int mouseY)
{
	float x = getX(), y = getY();
	std::shared_ptr<GUIElement> parent = getParent();
	mouseX -= x;
	mouseY -= y;
	mSelectedCaret = -1;
	mCaretTimer->resetTimer();
	mCaretVisible = true;
	sf::String str = mText.getString();
	int newCaret = str.getSize();
	for(size_t i = 0; i < str.getSize(); i++)
	{
		sf::Vector2f strPos = mText.findCharacterPos(i);
		sf::Text text;
		text.setFont(mFont);
		sf::String str = mText.getString();
		text.setString(str[i]);
		sf::FloatRect boundBox = text.getGlobalBounds();
		if(mouseX < strPos.x + boundBox.width/2)
		{
			newCaret = i;
			break;
		}
	}
	mCaretIndex = newCaret;
	//std::cout<<"on gui click!"<<std::endl;
}

void GUIEditField::setTexture(std::pair<sf::FloatRect, sf::Texture*> &pair)
{
	mSprite.setTexture(*pair.second);
	//mSprite.setPosition(pair.first.left, pair.first.top);
	mSprite.setTextureRect(sf::IntRect(0, 0, pair.second->getSize().x, pair.second->getSize().y));

}