#ifndef _GUIElement_H
#define _GUIElement_H

#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <memory>
#include <vector>
#include <functional>
enum	GUIType{WINDOW, BUTTON, TEXT, IMAGE, EDIT_FIELD, SCROLL_BAR};

class GUIElement : public std::enable_shared_from_this<GUIElement>
{
public:
	GUIElement(sf::FloatRect rect, std::shared_ptr<GUIElement> parent, GUIType guiType);
	float			getX()const;
	float			getY()const;
	float			getLocalX()const;
	float			getLocalY()const;
	float			getWidth()const;
	float			getHeight()const;
	bool			getVisible()const;
	std::shared_ptr<GUIElement> getParent()const;
	GUIType			getGUIType()const;
	sf::Color		getColor()const;
	bool			getMouseIsInside()const;
	bool			isEnabled()const;
	bool			isSelected()const;

	void			init();
	void			setX(float x);
	void			setY(float y);
	void			setWidth(float width);
	void			setHeight(float height);
	void			setRectangle(sf::FloatRect rect);
	virtual void	setSize(float width, float height){};
	void			setVisible(bool visible);
	virtual void	setColor(sf::Color color);
	void			setMouseIsInside(bool inside);
	void			setEnabled(bool enabled, bool effectChildren = false);
	void			setSelected(bool selected);
	sf::FloatRect	getRectangle();
	int				getElementID();
	virtual std::string		getText()const{ return ""; }

	std::function<void()>	getOnClickFunction();
	std::function<void()>	getMouseEnterFunction();
	std::function<void()>	getMouseLeaveFunction();
	std::function<void()>	getOnGuiChangeFunction();

	void			addChild(std::shared_ptr<GUIElement> guiElement);
	std::shared_ptr<GUIElement> getPtr();
	std::vector<std::shared_ptr<GUIElement>>& getChildVector();

	virtual void	setText(std::string text){};
	virtual void	setScale(float width, float height) {}
	void			setOnClickFunction(std::function<void()>);
	void			setMouseEnterFunction(std::function<void ()>);
	void			setMouseLeaveFunction(std::function<void ()>);
	void			setOnGuiChangeFunction(std::function <void()> func);
	virtual bool	update(sf::RenderWindow *window, sf::Event event);
	bool			onClick(sf::RenderWindow *window);
	bool			onMove(sf::RenderWindow *window);
	virtual bool	render(sf::RenderWindow *window, sf::RenderStates &states) = 0;
	virtual void	onGUIClick(int mouseX, int mouseY){};
	virtual void	onGUIHover(int mouseX, int mouseY){};
	void			tick();
	virtual			~GUIElement();
protected:
	int mElementID;
	sf::FloatRect mRectangle, mLocalRectangle;
	sf::Color mColor;
	bool mVisible, mMouseInside, mEnabled, mSelected;
	std::shared_ptr<GUIElement> mParent;
	GUIType mGUIType;
	
	std::function<void ()> mOnClickFunction;
	std::function<void ()> mMouseEnterFunction;
	std::function<void ()> mMouseLeaveFunction;
	std::function<void ()> mOnGuiChange;
	bool mCallClickFunc, mCallMouseEnterFunc, mCallMouseLeaveFunc;
	
	std::vector<std::shared_ptr<GUIElement>> mChilds;


};

#endif