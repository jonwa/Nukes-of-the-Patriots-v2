#ifndef MENU_H
#define MENU_H

#include "GUIElement.h"
#include "GUIWindow.h"
#include "GUIButton.h"
#include "GUIEditField.h"
#include "GUIText.h"
#include <memory>
#include <map>

#include <SFML\Graphics\RenderWindow.hpp>
#include <SFML\Audio\Music.hpp>

class Capitalist;
class Communist;

class Menu
{
public:
	Menu();
	Menu(sf::RenderWindow &window);
	~Menu();
	void clear();
	void setInGameMenuVisible();

private:
	bool mCapitalistTeamChosen, mCommunistTeamChosen;
	std::map<std::string, std::pair<sf::FloatRect, sf::Texture*> > ButtonPos;
	std::map<std::string, std::pair<sf::FloatRect, sf::Texture*> > WindowPos;
	std::map<std::string, std::shared_ptr<sf::Music> > MenuMusic;

	void initialize();
	void initializeGuiFuctions();
	void loadButtonPosition();
	void loadWindowPosition();
	void loadMenuMusic();

	void resetPickTeamValues();

	std::shared_ptr<GUIWindow> mParentWindow;
	std::shared_ptr<GUIWindow> mMainMenuWindow;
	std::shared_ptr<GUIWindow> mInGameMenuWindow;
	//std::shared_ptr<GUIWindow> mSettingsMenuWindow;
	std::shared_ptr<GUIWindow> mCreditsMenuWindow;
	//std::shared_ptr<GUIWindow> mLogoMenuWindow;
	//std::shared_ptr<GUIWindow> mSplashScreenWindow;
	std::shared_ptr<GUIWindow> mChooseTeamWindow;

	std::shared_ptr<GUIButton> mStartNewGameButton;
	std::shared_ptr<GUIButton> mMultiPlayerButton;
	std::shared_ptr<GUIButton> mLoadGameButton;
	std::shared_ptr<GUIButton> mSettingsButton;
	std::shared_ptr<GUIButton> mCreditsButton;
	std::shared_ptr<GUIButton> mExitButton;

	std::shared_ptr<GUIButton> mResumeGameButton;
	std::shared_ptr<GUIButton> mRestartGameButton;
	std::shared_ptr<GUIButton> mSaveGameButton;

	std::shared_ptr<GUIEditField> mCapitalistNameField;
	std::shared_ptr<GUIEditField> mCommunistNameField;
	std::shared_ptr<GUIButton> mCapitalistOkayButton;
	std::shared_ptr<GUIButton> mCommunistOkayButton;

	std::shared_ptr<GUIButton> mTeamCommunist;
	std::shared_ptr<GUIButton> mTeamCommunistIsPicked;
	std::shared_ptr<GUIButton> mTeamCapitalist;
	std::shared_ptr<GUIButton> mTeamCapitalistIsPicked;


	sf::RenderWindow &mWindow;
};

#endif