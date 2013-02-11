#ifndef _GUIElement_H
#define _GUIElement_H

#include <SFML\Graphics.hpp>
#include <memory>
#include <vector>
#include <functional>
enum	GUIType{WINDOW, BUTTON, TEXT, IMAGE, EDIT_FIELD};

class GUIElement : public std::enable_shared_from_this<GUIElement>
{
public:
	GUIElement(sf::FloatRect rect, std::shared_ptr<GUIElement> parent, GUIType guiType);
	float			getX()const;
	float			getY()const;
	float			getWidth()const;
	float			getHeight()const;
	bool			getVisible()const;
	std::shared_ptr<GUIElement> getParent()const;
	GUIType			getGUIType()const;
	int				getAlpha()const;
	bool			getMouseIsInside()const;

	void			init();
	void			setX(float x);
	void			setY(float y);
	void			setWidth(float width);
	void			setHeight(float height);
	void			setVisible(bool visible);
	void			setAlpha(int alpha);
	void			setMouseIsInside(bool inside);
	sf::FloatRect	getRectangle();

	void			addChild(std::shared_ptr<GUIElement> guiElement);
	std::shared_ptr<GUIElement> getPtr();
	std::vector<std::shared_ptr<GUIElement>>& getChildVector();

	virtual void	setScale(float width, float height) {}
	void			setOnClickFunction(std::function<void()>);
	void			setMouseEnterFunction(std::function<void ()>);
	void			setMouseLeaveFunction(std::function<void ()>);
	//virtual bool	update(sf::RenderWindow &window);
	bool			onClick(sf::RenderWindow *window);
	bool			onMove(sf::RenderWindow *window);
	virtual bool	render(sf::RenderWindow *window) = 0;
	void			tick();
	virtual			~GUIElement();
protected:
	sf::FloatRect mRectangle;
	int mAlpha;
	bool mVisible, mMouseInside;
	std::shared_ptr<GUIElement> mParent;
	GUIType mGUIType;

	std::function<void ()> mOnClickFunction;
	std::function<void ()> mMouseEnterFunction;
	std::function<void ()> mMouseLeaveFunction;
	bool mCallClickFunc, mCallMouseEnterFunc, mCallMouseLeaveFunc;
	
	std::vector<std::shared_ptr<GUIElement>> mChilds;


};

#endif