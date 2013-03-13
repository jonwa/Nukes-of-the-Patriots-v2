#ifndef MENU_H
#define MENU_H

#include "GUIElement.h"
#include "GUIWindow.h"
#include "GUIButton.h"
#include "GUIEditField.h"
#include "GUIImage.h"
#include "GUIScrollBar.h"
#include "GUIText.h"
#include "GUIImage.h"
#include <memory>
#include <map>
#include "Timer.h"
#include "Sound.h"
#include "SoundHandler.h"
#include <SFML\Graphics\RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML\Audio\Music.hpp>
//#include <sfeMovie\Movie.hpp>
class Capitalist;
class Communist;

class Menu
{
public:
	static Menu* getInstance();
	
	void saveConfig();
	void loadConfig();

	std::shared_ptr<GUIWindow> getWindows(std::string string);
	std::shared_ptr<GUIEditField> getEditField(std::string string);

	void setWindow(sf::RenderWindow& window);
	sf::RenderWindow& getWindow();

	void clear();
	void loadTeamAnimation();
	void tick();
	int mMasterVolume;
	void update(sf::Event &event);
	void startGame();

	void resetChooseTeamValues();

	void setLoadGameButtonText();

	void playMusic();
	void stopMusic();
	void pauseMusic();

	void playVideo();
	void stopVideo();
	//sfe::Movie &getVideo();

public:
	std::shared_ptr<GUIWindow> mWaitingForClientWindow;
	std::shared_ptr<GUIText>   mWaitingForClientText;
	std::shared_ptr<GUIButton> mCloseWaitingForClientWindow;

private:
	static Menu* mInstance;
	//sfe::Movie mIntroMovie;
	bool fullscreen;
	std::shared_ptr<Sound> mMenuMusic;
	Menu();
	//Menu(const Menu&){}
	//Menu operator=(const Menu&){}
	~Menu();
	
	bool mCapitalistTeamChosen, mCommunistTeamChosen;
	std::map<std::string, std::pair<sf::FloatRect, sf::Texture*> > ButtonPos;
	std::map<std::string, std::pair<sf::FloatRect, sf::Texture*> > WindowPos;
	std::map<std::string, std::shared_ptr<sf::Music> > MenuMusic;

	std::vector<std::string> tempVec; //used for load game buttons

	void initialize();
	void initializeGuiFuctions();
	void loadButtonPosition();
	void loadWindowPosition();
	void loadMenuMusic();
	void initializeIntroVideo();
	sf::Texture mTeamAnimationFrames[150];
	Timer *mTeamAnimationTimer;
	bool mShowTeamChooseAnimation;

	void resetPickTeamValues();


	//THIS IS A PLACEHOLDER, REMOVE IT WHEN GRAPHIC IS UPDATED
	std::shared_ptr<GUIImage> mCreditsPlaceholder;

	std::shared_ptr<GUIWindow> mParentWindow;
	std::shared_ptr<GUIWindow> mMainMenuWindow;
	std::shared_ptr<GUIWindow> mSettingsMenuWindow;
	std::shared_ptr<GUIWindow> mCreditsMenuWindow;
	//std::shared_ptr<GUIWindow> mSplashScreenWindow;
	std::shared_ptr<GUIWindow> mChooseTeamWindow;

	std::shared_ptr<GUIButton> mStartNewGameButton;
	std::shared_ptr<GUIButton> mMultiPlayerButton;
	std::shared_ptr<GUIButton> mLoadGameButton;
	std::shared_ptr<GUIWindow> mLoadGameWindow;	
	std::shared_ptr<GUIButton> mSavedGameSlots[3];
	std::shared_ptr<GUIText>   mSavedGameText[3];
	std::shared_ptr<GUIButton> mCloseLoadGameWindow;
	std::shared_ptr<GUIText>   mLoadGameText;
	std::shared_ptr<GUIButton> mSettingsButton[2];
	std::shared_ptr<GUIButton> mCreditsButton;
	std::shared_ptr<GUIButton> mCloseCreditsButton;
	std::shared_ptr<GUIButton> mExitButton[2];

	std::shared_ptr<GUIText>   mVolumeText;
	std::shared_ptr<GUIScrollBar> mVolumeScrollBar;
	std::shared_ptr<GUIImage>  mFullscreenImage;
	//std::shared_ptr<GUIButton> mMuteSound;
	std::shared_ptr<GUIText>   mWindowSizeText;
	std::shared_ptr<GUIText>   mFullscreenModeText;
	std::shared_ptr<GUIButton> mFullscreenModeButton;
	std::shared_ptr<GUIButton> mCloseSettingsWindow;

	std::shared_ptr<GUIWindow> mInGameMenuWindow;
	std::shared_ptr<GUIButton> mResumeGameButton;
	std::shared_ptr<GUIButton> mRestartGameButton;
	std::shared_ptr<GUIButton> mSaveGameButton;
	std::shared_ptr<GUIWindow> mSaveGameWindow[2];
	std::shared_ptr<GUIButton> mCloseSaveGameWindow[2];
	std::shared_ptr<GUIText>   mSaveGameText[2];
	std::shared_ptr<GUIEditField> mSaveGameTextField;

	std::shared_ptr<GUIEditField> mCapitalistNameField;
	std::shared_ptr<GUIEditField> mCommunistNameField;
	std::shared_ptr<GUIButton> mCapitalistOkayButton;
	std::shared_ptr<GUIButton> mCommunistOkayButton;

	std::shared_ptr<GUIImage> mTeamCommunist;
	std::shared_ptr<GUIImage> mTeamCommunistIsPicked;
	std::shared_ptr<GUIImage> mTeamCapitalist;
	std::shared_ptr<GUIImage> mTeamCapitalistIsPicked;

	std::shared_ptr<GUIWindow> mLanPlayWindow;
	std::shared_ptr<GUIButton> mLanPlayQuickConnect;

	sf::RenderWindow* mWindow;
};

#endif