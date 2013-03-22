#include "Menu.h"
#include "GUIManager.h"
#include "tinyxml2.h"
#include "Capitalist.h"
#include "Communist.h"
#include "ResourceHandler.h"
#include <sstream>
#include <iostream>
#include <SFML\Window\Mouse.hpp>
#include "AnimationHandler.h"
#include "GameManager.h"
#include "TimerHandler.h"
#include "Timer.h"
#include "Event.h"
#include <SFML/Network.hpp>
#include <SFML\Audio\Listener.hpp>

Menu::Menu() : 
	mWindow(nullptr),
	mCapitalistTeamChosen(false),
	mCommunistTeamChosen(false),
	mShowTeamChooseAnimation(false),
	fullscreen(true),
	mFullscreenCount(true)
{ 
	//initializeIntroVideo();
	initialize(); 
	initializeGuiFuctions();
	//loadTeamAnimation();
	//mChooseTeamAnimation.openFromFile("mainmenu_sign.wmv");
	mTeamAnimationTimer = Timer::setTimer([&](){}, 5000, 1);
	//playVideo();
}

Menu* Menu::mInstance = NULL;
Menu* Menu::getInstance()
{
	if(mInstance == NULL)
		mInstance = new Menu();
	return mInstance;
}
Menu::~Menu(){ }

void Menu::playMusic()
{
	mMenuMusic->playSound(); 
	mMenuMusic->setVolume(100);
}
void Menu::stopMusic()
{
	mMenuMusic->stopSound();
}
void Menu::pauseMusic()
{
	mMenuMusic->pauseSound();
}

void Menu::reset()
{
		mParentWindow->setVisible(true);
		mInGameMenuWindow->setVisible(false); 
		mChooseTeamWindow->setVisible(true);
		GUIManager::getInstance()->setOnTop(mChooseTeamWindow);
	
		mCapitalistOkayButton->setTexture(std::pair<sf::FloatRect, sf::Texture*>
			(sf::FloatRect(mCapitalistOkayButton->getX(), mCapitalistOkayButton->getY(), mCapitalistOkayButton->getWidth(), mCapitalistOkayButton->getHeight()), &ResourceHandler::getInstance()->getTexture(std::string("Menu/Ok-knapp-inaktiv"))));
		mCapitalistNameField->setTexture(std::pair<sf::FloatRect, sf::Texture*>
			(sf::FloatRect(mCapitalistNameField->getX(), mCapitalistNameField->getY(), mCapitalistNameField->getWidth(), mCapitalistNameField->getHeight()), &ResourceHandler::getInstance()->getTexture(std::string("Menu/Namnruta-aktiv"))));
		mCapitalistNameField->setText("");
		mCapitalistNameField->setPlaceHolderText("Enter name here");
		mTeamCapitalist->setTexture(std::pair<sf::FloatRect, sf::Texture*> (mTeamCapitalist->getRectangle(), ButtonPos["TeamCapitalist"].second));
		mCapitalistOkayButton->setEnabled(false);
		
		mCommunistOkayButton->setTexture(std::pair<sf::FloatRect, sf::Texture*>
			(sf::FloatRect(mCommunistOkayButton->getX(), mCommunistOkayButton->getY(), mCommunistOkayButton->getWidth(), mCommunistOkayButton->getHeight()), &ResourceHandler::getInstance()->getTexture(std::string("Menu/Ok-knapp-inaktiv"))));
		mCommunistNameField->setTexture(std::pair<sf::FloatRect, sf::Texture*>
			(sf::FloatRect(mCommunistNameField->getX(), mCommunistNameField->getY(), mCommunistNameField->getWidth(), mCommunistNameField->getHeight()), &ResourceHandler::getInstance()->getTexture(std::string("Menu/Namnruta-aktiv"))));
		mCommunistNameField->setText("");	
		mCommunistNameField->setPlaceHolderText("Enter name here");
		mTeamCommunist->setTexture(std::pair<sf::FloatRect, sf::Texture*> (mTeamCommunist->getRectangle(), ButtonPos["TeamCommunist"].second));
		mCommunistOkayButton->setEnabled(false);
		resetChooseTeamValues();
		mChooseTeamWindow->setEnabled(true, true);
}

void Menu::setWindow(sf::RenderWindow& window)
{
	mWindow = &window;
}

sf::RenderWindow &Menu::getWindow()
{
	return *mWindow;
}

void Menu::saveConfig()
{
	tinyxml2::XMLDocument doc;
	
	tinyxml2::XMLElement *master = doc.NewElement("MasterVolume");
	master->SetAttribute("value", sf::Listener::getGlobalVolume());
	tinyxml2::XMLElement *position = doc.NewElement("VolumePos");
	position->SetAttribute("value", mVolumeScrollBar->getSprite()->getPosition().x);
	
	doc.InsertEndChild(master);
	doc.InsertEndChild(position);
	doc.SaveFile("XML/Config.xml");
}

void Menu::loadConfig()
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile("XML/Config.xml");

	if(doc.Error())
		std::cout << "error! unable to load configs";
	
	tinyxml2::XMLElement *master = doc.FirstChildElement("MasterVolume");
	sf::Listener::setGlobalVolume((float)atof(master->Attribute("value")));
	tinyxml2::XMLElement *position = doc.FirstChildElement("VolumePos");
	mVolumeScrollBar->getSprite()->setPosition((float)atof(position->Attribute("value")), mVolumeScrollBar->getY());
}


void Menu::loadTeamAnimation()
{
	for(int i = 0; i < 150; i++)
	{
		std::stringstream s;
		s<<"teamAnimation/teamsign_animation."<<i+1<<".png";
		//std::cout<<"teamAnimation/teamsign_animation."<<i+1<<".png"<<std::endl;
		mTeamAnimationFrames[i].loadFromFile(s.str());
	}
	std::cout<<"done loading animation!"<<std::endl;
}

void Menu::clear()
{
	MenuMusic.clear();
	ButtonPos.clear();
	WindowPos.clear();
}


void Menu::update(sf::Event &event)
{
	if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Escape)
	{
		//GameManager::getInstance()->getCurrentPlayer()->hideGUI();
		std::cout<<"in game menu is visible"<<std::endl;

		GUIManager::getInstance()->setOnTop(mInGameMenuWindow);



		GameManager::getInstance()->getCurrentPlayer()->pauseMusic();

		mInGameMenuWindow->setVisible(true);
		Timer::setTimer([=]()
		{
			playMusic();
		}, 100, 1);
	}
}

void Menu::setLoadGameButtonText()
{
	int count = 0;
	GameManager::SaveFilesVec& save = GameManager::getInstance()->getSaveFilesVec();
	for(GameManager::SaveFilesVec::reverse_iterator i = save.rbegin(); i != save.rend(); i++)
	{
		if(count < 3)
		{
			mSavedGameText[count]->setText(*i);
			
			count++;
		}
		else 
			break;
		//mTestText->setRectangle(mTestText->getRectangle());
	}
		std::cout << "size of vector " << save.size() << std::endl << std::endl;
}

std::shared_ptr<GUIWindow> Menu::getWindows(std::string string)
{
	if(string == "ParentWindow")
		return mParentWindow;
	else if(string == "MainMenu")
		return mMainMenuWindow;
	else if(string == "InGameMenu")
		return mInGameMenuWindow;
	else if(string == "SaveCanceled")
		return mSaveGameWindow[0];
	else if(string == "SaveSuccessful")
		return mSaveGameWindow[1];
	else if(string == "ChooseTeam")
		return mChooseTeamWindow;
}

std::shared_ptr<GUIEditField> Menu::getEditField(std::string string)
{
	if(string == "SaveFileField")
		return mSaveGameTextField;
	else if(string == "CommunistNameField")
		return mCommunistNameField;	
	else if(string == "CapitalistNameField")
		return mCapitalistNameField;
}			   		


void Menu::resetChooseTeamValues()
{
	mCapitalistTeamChosen = false;
	mCommunistTeamChosen = false;
}


 /*Laddar in menyknapparnas positions- och storleksinformation
   från ett externbibliotek kallat tinyXML2
   
     Av: Jon Wahlström 2013-01-30*/
void Menu::loadButtonPosition()
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile("XML/MenuButton.xml");

	if(doc.Error())
		std::cout << "Fel! Menu::loadButtonPos";
	
	tinyxml2::XMLElement *element = doc.FirstChildElement("menu");
	tinyxml2::XMLElement *button = element->FirstChildElement("button");

	const char* temp;
	while (button != 0)
	{
		std::string tempName;
		if (temp = button->FirstChildElement("name")->GetText())
		{
			tempName = temp;
		}
		float posX;
		if (temp = button->FirstChildElement("xPos")->GetText())
		{
			posX = atof(temp);
		}
		float posY;
		if (temp = button->FirstChildElement("yPos")->GetText())
		{
			posY = atof(temp);
		}
		float Width;			 
		if (temp = button->FirstChildElement("width")->GetText())
		{
			Width = atof(temp);
		}
		float Height;
		if (temp = button->FirstChildElement("height")->GetText())
		{
			Height = atof(temp);
		}
		temp	 = button->FirstChildElement("image")->GetText();
		std::string name;
		if (temp)
			name = temp;

		ButtonPos[tempName].first.left = posX;
		ButtonPos[tempName].first.top = posY;
		ButtonPos[tempName].first.width = Width;
		ButtonPos[tempName].first.height = Height;
		ButtonPos[tempName].second = &ResourceHandler::getInstance()->getTexture(name);
		
		button = button->NextSiblingElement();
	}
}

 /*
   Laddar in menyfönstrernas information via ett XML dokument och placerar in
   dessa i en std::map

    Av: Jon Wahlström 2013-02-03
																			 */
void Menu::loadWindowPosition()
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile("XML/MenuWindow.xml");

	if(doc.Error())
		std::cout << "Fel! Menu::loadWindowPos";
	
	tinyxml2::XMLElement *element = doc.FirstChildElement("windows");
	tinyxml2::XMLElement *window = element->FirstChildElement("window");
	
	const char* temp;
	while (window != 0)
	{
		std::string tempName;
		if (temp = window->FirstChildElement("name")->GetText())
		{
			tempName = temp;
		}
		float posX;
		if (temp = window->FirstChildElement("xPos")->GetText())
		{
			posX = atof(temp);
		}
		float posY;
		if (temp = window->FirstChildElement("yPos")->GetText())
		{
			posY = atof(temp);
		}
		float Width;			 
		if (temp = window->FirstChildElement("width")->GetText())
		{
			Width = atof(temp);
		}
		float Height;
		if (temp = window->FirstChildElement("height")->GetText())
		{
			Height = atof(temp);
		}
		temp	 = window->FirstChildElement("image")->GetText();
		std::string name;
		if (temp)
			name = temp;
				
		WindowPos[tempName].first.left = posX;
		WindowPos[tempName].first.top = posY;
		WindowPos[tempName].first.width = Width;
		WindowPos[tempName].first.height = Height;
		WindowPos[tempName].second = &ResourceHandler::getInstance()->getTexture(name);
		
		window = window->NextSiblingElement();
	}	
}


void Menu::loadMenuMusic()
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile("XML/MenuMusic.xml");

	if(doc.Error())
		std::cout << "Fel! Menu::loadMusic";
	
	tinyxml2::XMLElement *element = doc.FirstChildElement("tracks");
	tinyxml2::XMLElement *music = element->FirstChildElement("music");
	
	const char* temp;
	while (music != 0)
	{
		std::string tempName;
		if (temp = music->FirstChildElement("name")->GetText())
		{
			tempName = temp;
		}
		
		temp	 = music->FirstChildElement("file")->GetText();
		std::string name;
		if (temp)
			name = temp;
				
		MenuMusic[tempName] = ResourceHandler::getInstance()->getMusic(name);
	
		music = music->NextSiblingElement();	
	}
}


//void Menu::initializeIntroVideo()
//{
//	if (!mIntroMovie.openFromFile("FrukostFabriken.wmv"))
//		std::cout << "unable to load video" << std::endl;
//
//	mIntroMovie.useDebugMessages(false);
//}
//
//void Menu::stopVideo()
//{
//	mIntroMovie.stop();
//}
//
//
//void Menu::playVideo()
//{
//	mIntroMovie.play();
//	Timer::setTimer([=]()
//	{
//		stopVideo();
//		playMusic();
//		GUIManager::getInstance()->setOnTop(mMainMenuWindow);
//		mMainMenuWindow->setVisible(true);
//	}, 5000, 1);
//}



 /*
	Initierar menyernas fönster, bilder samt knappar som skall finnas med.
	dessa är av olika typer av GUIElement. Beroende på vilken knapp som 
	blir vald så kommer olika GUIWindow att renderas ut som i detta fall
	representerar de olika menyernas fönster. 

	Av: Jon Wahlström  2013-01-28
																		*/
void Menu::initialize()
{
	loadButtonPosition();
	loadWindowPosition();
	loadMenuMusic();
	mMenuMusic				= Sound::create(MenuMusic["MainMenuTrack"]);
	mParentWindow			= GUIWindow::create(WindowPos["MenuInterface"]);

	mMainMenuWindow			= GUIWindow::create(WindowPos["MainMenu"]);
	mStartNewGameButton		= GUIButton::create(ButtonPos["StartGame"], mMainMenuWindow);
	mMultiPlayerButton		= GUIButton::create(ButtonPos["MultiPlayer"], mMainMenuWindow);
	mLoadGameButton			= GUIButton::create(ButtonPos["LoadGame"], mMainMenuWindow);
	mLoadGameWindow			= GUIWindow::create(WindowPos["LoadGameWindow"]);
	mSavedGameSlots[0]		= GUIButton::create(ButtonPos["FirstSlot"], mLoadGameWindow);
	mSavedGameSlots[1]		= GUIButton::create(ButtonPos["SecondSlot"], mLoadGameWindow);
	mSavedGameSlots[2]		= GUIButton::create(ButtonPos["ThirdSlot"], mLoadGameWindow);
	
	mLoadGameText			= GUIText::create(sf::FloatRect(200, 53, 100, 40), "< Select a file to load >", mLoadGameWindow);
	mLoadGameText->setColor(sf::Color::White);
	mSavedGameText[0]		= GUIText::create(sf::FloatRect(350, 110, 100, 40), " ", mLoadGameWindow);
	mSavedGameText[0]->setColor(sf::Color::White);
	mSavedGameText[0]->setAlignment("middle");
	mSavedGameText[1]		= GUIText::create(sf::FloatRect(350, 180, 100, 40), " ", mLoadGameWindow);
	mSavedGameText[1]->setColor(sf::Color::White);
	mSavedGameText[1]->setAlignment("middle");
	mSavedGameText[2]		= GUIText::create(sf::FloatRect(350, 250, 100, 40), " ", mLoadGameWindow);
	mSavedGameText[2]->setColor(sf::Color::White);
	mSavedGameText[2]->setAlignment("middle");
	mCloseLoadGameWindow	= GUIButton::create(ButtonPos["CloseLoadGameWindow"], mLoadGameWindow);

	mLoadGameWindow->setVisible(false);	
	mSettingsButton[0]		= GUIButton::create(ButtonPos["Settings"], mMainMenuWindow);
	mCreditsButton			= GUIButton::create(ButtonPos["Credits"], mMainMenuWindow);
	mExitButton[0]			= GUIButton::create(ButtonPos["Exit"], mMainMenuWindow);
	mMainMenuWindow->setVisible(false);

	mInGameMenuWindow		= GUIWindow::create(WindowPos["InGameMenu"]);
	mResumeGameButton		= GUIButton::create(ButtonPos["Resume"], mInGameMenuWindow);
	mRestartGameButton		= GUIButton::create(ButtonPos["Restart"], mInGameMenuWindow);
	mSaveGameButton			= GUIButton::create(ButtonPos["SaveGame"], mInGameMenuWindow);
	mSettingsButton[1]		= GUIButton::create(ButtonPos["Settings"], mInGameMenuWindow);
	mExitButton[1]			= GUIButton::create(ButtonPos["Exit"], mInGameMenuWindow);
	mInGameMenuWindow->setVisible(false);

	mSaveGameWindow[0]			= GUIWindow::create(WindowPos["FirstSaveGameWindow"]);
	mSaveGameWindow[1]			= GUIWindow::create(WindowPos["SecondSaveGameWindow"]);
	mCloseSaveGameWindow[0]		= GUIButton::create(ButtonPos["CloseSaveGameWindow"], mSaveGameWindow[0]);
	mSaveGameText[0]			= GUIText::create(sf::FloatRect(100, 63, 100, 40), "< Name your save-file and click okay >", mSaveGameWindow[0]);
	mSaveGameText[1]			= GUIText::create(sf::FloatRect(230, 200, 100, 40), "< Save successful >", mSaveGameWindow[1]);
	mSaveGameText[0]->setColor(sf::Color::White);
	mSaveGameText[1]->setColor(sf::Color::White);
	mSaveGameTextField			= GUIEditField::create(sf::FloatRect(200 + 8, 180, 300, 64), GUIEditField::MENU, "filename", false, mSaveGameWindow[0]);
	mSaveGameWindow[0]->setVisible(false);
	mSaveGameWindow[1]->setVisible(false);

	mSettingsMenuWindow		= GUIWindow::create(WindowPos["SettingsMenu"]);
	mVolumeText				= GUIText::create(sf::FloatRect(90, 120, 40, 40), "Volume: ", mSettingsMenuWindow);
	mVolumeText->setColor(sf::Color::White);
	mVolumeText->setAlignment("left");
	mVolumeScrollBar		= GUIScrollBar::create(sf::FloatRect(260, 130, 400, 20), mSettingsMenuWindow);
	mWindowSizeText			= GUIText::create(sf::FloatRect(90, 260, 40, 40), "Fullscreen: ", mSettingsMenuWindow);
	mWindowSizeText->setColor(sf::Color::White);
	mWindowSizeText->setAlignment("left");

	mFullscreenModeButton			= GUIButton::create(ButtonPos["Fullscreen"], mSettingsMenuWindow);
	mFullscreenImage				= GUIImage::create(ButtonPos["FullscreenClicked"], mSettingsMenuWindow);
	mCloseSettingsWindow			= GUIButton::create(ButtonPos["CloseSettings"], mSettingsMenuWindow);
	mSettingsMenuWindow->setVisible(false);

	mCreditsMenuWindow		= GUIWindow::create(WindowPos["CreditsMenu"]);
	mCreditsPlaceholder		= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(0, 0, 0, 0), &ResourceHandler::getInstance()->getTexture(std::string("Menu/credits_placeholder"))), mCreditsMenuWindow);
	mCloseCreditsButton		= GUIButton::create(ButtonPos["CloseCredits"], mCreditsMenuWindow);
	mCreditsMenuWindow->setVisible(false);

	mChooseTeamWindow			= GUIWindow::create(WindowPos["ChooseTeam"], mParentWindow);
	mBackToMainMenuButton[0]	= GUIButton::create(ButtonPos["CloseChooseTeamWindow"], mChooseTeamWindow);
	mTeamCommunist				= GUIImage::create(ButtonPos["TeamCommunist"], mChooseTeamWindow);
	mTeamCapitalist				= GUIImage::create(ButtonPos["TeamCapitalist"], mChooseTeamWindow);

	//mTeamCapitalistIsPicked	= GUIButton::create(ButtonPos["TeamCapitalistIsPressed"], mChooseTeamWindow);
	mCapitalistNameField		= GUIEditField::create(sf::FloatRect(43 + 8, 269, 218, 41), GUIEditField::MENU, "Enter name here", false, mChooseTeamWindow);
	mCommunistNameField			= GUIEditField::create(sf::FloatRect(421 + 8, 269, 218, 41), GUIEditField::MENU, "Enter name here", false, mChooseTeamWindow);

	mCapitalistNameField->setMaxCharacters(15);
	mCommunistNameField->setMaxCharacters(15);
	mCapitalistOkayButton		= GUIButton::create(ButtonPos["CapitalistOkayIsPressed"], mChooseTeamWindow);
	mCapitalistOkayButton->setEnabled(false);
	mCapitalistOkayButton->setSize(ButtonPos["CapitalistOkayIsPressed"].first.width, ButtonPos["CapitalistOkayIsPressed"].first.height);
	mCommunistOkayButton		= GUIButton::create(ButtonPos["CommunistOkayIsPressed"], mChooseTeamWindow);
	mCommunistOkayButton->setEnabled(false);
	mCommunistOkayButton->setSize(ButtonPos["CommunistOkayIsPressed"].first.width, ButtonPos["CommunistOkayIsPressed"].first.height);
	mChooseTeamWindow->setVisible(false);
	
	// Lan play ("Multi-player")
	mLanPlayWindow					= GUIWindow::create(WindowPos["LanPlayWindow"], mParentWindow);
	mLanPlayQuickConnect			= GUIButton::create(ButtonPos["LanPlayQuickConnect"], mLanPlayWindow);
	mBackToMainMenuButton[1]		= GUIButton::create(ButtonPos["CloseLanWindow"], mLanPlayWindow);
	mWaitingForClientWindow			= GUIWindow::create(WindowPos["WaitingForClient"], mLanPlayWindow);
	mWaitingForClientText			= GUIText::create(sf::FloatRect(100, 100, 0, 0), "Searching for servers...", mWaitingForClientWindow);
	mCloseWaitingForClientWindow	= GUIButton::create(ButtonPos["CloseWaitingForClient"], mWaitingForClientWindow);
	mWaitingForClientWindow->setVisible(false);
	mLanPlayWindow->setVisible(false);

	GUIManager::getInstance()->addGUIElement(mParentWindow);
	GUIManager::getInstance()->addGUIElement(mMainMenuWindow);
	GUIManager::getInstance()->addGUIElement(mInGameMenuWindow);
	GUIManager::getInstance()->addGUIElement(mSettingsMenuWindow);
	GUIManager::getInstance()->addGUIElement(mCreditsMenuWindow);
	GUIManager::getInstance()->addGUIElement(mSaveGameWindow[0]);
	GUIManager::getInstance()->addGUIElement(mSaveGameWindow[1]);
	GUIManager::getInstance()->addGUIElement(mLoadGameWindow);
}


void Menu::tick()
{
	if(mShowTeamChooseAnimation)
	{
		float timeleft = 0;
		float duration = 1;
		if(Timer::isTimer(mTeamAnimationTimer))
		{
			timeleft = mTeamAnimationTimer->getTimeLeft();
			duration = mTeamAnimationTimer->getTimerDuration();
		}

		float progress = 1 - (timeleft / duration);

		int frame = progress * 149; //149 = number of frames in the animation
		sf::Sprite sprite(mTeamAnimationFrames[frame]);
		sprite.setPosition(mWindow->getSize().x/2 - mTeamAnimationFrames[frame].getSize().x/2, mWindow->getSize().y/2 - mTeamAnimationFrames[frame].getSize().y/2);
		mWindow->draw(sprite);
	}

	//if(mChooseTeamAnimation.getStatus() == sfe::Movie::Playing)
		//mWindow->draw(mChooseTeamAnimation);
	//if(mIntroMovie.getStatus() == sfe::Movie::Playing)
	//	mWindow->draw(mIntroMovie);

}

void Menu::startGame()
{
	//mChooseTeamAnimation.play();
	mLanPlayWindow->setVisible(false);
	mMainMenuWindow->setVisible(false);
	mChooseTeamWindow->setVisible(true);
	/*Timer::setTimer([=]()
	{
		mChooseTeamAnimation.stop();
		GUIManager::getInstance()->setOnTop(mMainMenuWindow);
	}, 4000, 1);*/
}

void Menu::initializeGuiFuctions()
{
	mStartNewGameButton->setMouseEnterFunction([=]()	{ mStartNewGameButton->setTexture(ButtonPos["StartGameHover"]); });
	mStartNewGameButton->setMouseLeaveFunction([=]()	{ mStartNewGameButton->setTexture(ButtonPos["StartGame"]); });
	mStartNewGameButton->setOnClickFunction([=]()		
	{ 
		startGame();
	});

	mMultiPlayerButton->setMouseEnterFunction([=]()		{ mMultiPlayerButton->setTexture(ButtonPos["MultiPlayerHover"]); });
	mMultiPlayerButton->setMouseLeaveFunction([=]()		{ mMultiPlayerButton->setTexture(ButtonPos["MultiPlayer"]); });
	mMultiPlayerButton->setOnClickFunction([=]()		
	{ 
		//mParentWindow->setEnabled(false, true);
		mMainMenuWindow->setVisible(false);
		mLanPlayWindow->setVisible(true);
		mLanPlayWindow->setEnabled(true, true);
	});

	mLanPlayQuickConnect->setOnClickFunction([=]()
	{
		mLanPlayWindow->setEnabled(false, true);
		mWaitingForClientWindow->setEnabled(true, true);
		mWaitingForClientWindow->setVisible(true);
		GameManager::getInstance()->searchForServers();
	});

	
	mSettingsButton[0]->setMouseEnterFunction([=]()		{ mSettingsButton[0]->setTexture(ButtonPos["SettingsHover"]); });
	mSettingsButton[0]->setMouseLeaveFunction([=]()		{ mSettingsButton[0]->setTexture(ButtonPos["Settings"]); });
	mSettingsButton[0]->setOnClickFunction([=]()			
	{ 
		mMainMenuWindow->setEnabled(false, true);
		GUIManager::getInstance()->setOnTop(mSettingsMenuWindow);
		mSettingsMenuWindow->setVisible(true);
		mSettingsMenuWindow->setEnabled(true, true);
	});

	mSettingsButton[1]->setMouseEnterFunction([=]()		{ mSettingsButton[1]->setTexture(ButtonPos["SettingsHover"]); });
	mSettingsButton[1]->setMouseLeaveFunction([=]()		{ mSettingsButton[1]->setTexture(ButtonPos["Settings"]); });
	mSettingsButton[1]->setOnClickFunction([=]()			
	{ 
		mInGameMenuWindow->setEnabled(false, true);
		GUIManager::getInstance()->setOnTop(mSettingsMenuWindow);
		mSettingsMenuWindow->setVisible(true);
		mSettingsMenuWindow->setEnabled(true, true);
	});
	
	mVolumeScrollBar->setOnGuiChangeFunction([=]()
	{
		sf::Listener::setGlobalVolume(mVolumeScrollBar->getValue());
	});

	mFullscreenModeButton->setOnClickFunction([=]()
	{
		fullscreen = !fullscreen;
		if(fullscreen)
			mFullscreenImage->setVisible(true);
		else
			mFullscreenImage->setVisible(false);
	});

	mCloseSettingsWindow->setOnClickFunction([=]()		
	{ 
		mSettingsMenuWindow->setVisible(false);
		mSettingsMenuWindow->setEnabled(false, true);
		//mMainMenuWindow->setVisible(true); 
		if(mMainMenuWindow->getVisible() == true)
		{
			mMainMenuWindow->setVisible(true);
			mMainMenuWindow->setEnabled(true, true);
		}
		else if(mInGameMenuWindow->getVisible() == true)
		{
			mInGameMenuWindow->setVisible(true);
			mInGameMenuWindow->setEnabled(true, true);
		}

		if(fullscreen)
		{
			if(mFullscreenCount)
			{
				mFullscreenCount = true;
				mWindow->create(sf::VideoMode(1024, 768, 32), "Nukes of the Patriots", sf::Style::Fullscreen);
			}
			else
				mFullscreenCount = true;
		}
		else if(!fullscreen)
		{
			if(mFullscreenCount)
			{
				mFullscreenCount = false;
				mWindow->create(sf::VideoMode(1024, 768, 32), "Nukes of the Patriots", sf::Style::Titlebar|sf::Style::Close);
			}
			else
				mFullscreenCount = false;
		}

		mWindow->setMouseCursorVisible(false);
	});	

	mCreditsButton->setMouseEnterFunction([=]()	{ mCreditsButton->setTexture(ButtonPos["CreditsHover"]); });
	mCreditsButton->setMouseLeaveFunction([=]()	{ mCreditsButton->setTexture(ButtonPos["Credits"]); });
	
	mCreditsButton->setOnClickFunction([=]()	
	{  
		mMainMenuWindow->setEnabled(false, true);
		GUIManager::getInstance()->setOnTop(mCreditsMenuWindow);
		mCreditsMenuWindow->setVisible(true);
		mCreditsMenuWindow->setEnabled(true, true); 
	});
	
	mExitButton[0]->setMouseEnterFunction([=]()	{ mExitButton[0]->setTexture(ButtonPos["ExitHover"]); });
	mExitButton[0]->setMouseLeaveFunction([=]()	{ mExitButton[0]->setTexture(ButtonPos["Exit"]); });
	mExitButton[0]->setOnClickFunction([=]()	{ mWindow->close(); });


	mExitButton[1]->setMouseEnterFunction([=]()	{ mExitButton[1]->setTexture(ButtonPos["ExitHover"]); });
	mExitButton[1]->setMouseLeaveFunction([=]()	{ mExitButton[1]->setTexture(ButtonPos["Exit"]); });
	mExitButton[1]->setOnClickFunction([=]()	{ mWindow->close(); });

	mCapitalistOkayButton->setOnClickFunction([=]()
	{
		if(GameManager::getInstance()->getGameType() == LAN && GameManager::getInstance()->isMyTurnToPlay())
		{
			//sf::Packet packet;
			//packet<<PoliticalType::CAPITALIST;
			//GameManager::getInstance()->triggerOtherPlayersEvent("nextPlayerToChooseTeam", packet);
			//GameManager::getInstance()->nextPlayersTurn();
			GameManager::getInstance()->setEnemyTurn();
		}
		else
		{
			GameManager::getInstance()->getRemoteClient()->setSuperPower(PoliticalType::CAPITALIST);
			GameManager::getInstance()->setMyTurn();
		}
		mCapitalistTeamChosen = true;
		mCapitalistOkayButton->setTexture(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(mCapitalistOkayButton->getX(), mCapitalistOkayButton->getY(), mCapitalistOkayButton->getWidth(), mCapitalistOkayButton->getHeight()), &ResourceHandler::getInstance()->getTexture(std::string("Menu/Ok-knapp-inaktiv"))));
		mCapitalistNameField->setTexture(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(mCapitalistNameField->getX(), mCapitalistNameField->getY(), mCapitalistNameField->getWidth(), mCapitalistNameField->getHeight()), &ResourceHandler::getInstance()->getTexture(std::string("Menu/Namnruta-inaktiv"))));
		mCapitalistOkayButton->setEnabled(false);
		mCapitalistNameField->setEnabled(false);
		mTeamCapitalist->setTexture(std::pair<sf::FloatRect, sf::Texture*> (mTeamCapitalist->getRectangle(), ButtonPos["TeamCapitalistIsPressed"].second));
		std::shared_ptr<GUIWindow> _parentWindow = mParentWindow;
		std::map<std::string, std::shared_ptr<sf::Music>> _music = MenuMusic;
		if(mCommunistTeamChosen && mCapitalistTeamChosen)
		{
			GameManager::getInstance()->showWaitingScreen(false);
			Timer::setTimer([=]()
			{
				_parentWindow->setVisible(false); 
				_music.at("MainMenuTrack")->stop(); 
				GameManager::getInstance()->init(1952); // initierar första året
			}, 100, 1);
		}
	});


	mCapitalistNameField->setOnGuiChangeFunction([=]()	
	{
		if(!mCapitalistNameField->getText().empty())
		{
			mCapitalistOkayButton->setEnabled(true, true);
			mCapitalistOkayButton->setTexture(std::pair<sf::FloatRect, sf::Texture*> (mTeamCapitalist->getRectangle(), ButtonPos["CapitalistOkay"].second));
		}
		else
		{
			mCapitalistOkayButton->setEnabled(false, true);
			mCapitalistOkayButton->setTexture(std::pair<sf::FloatRect, sf::Texture*> (mTeamCapitalist->getRectangle(), ButtonPos["CapitalistOkayIsPressed"].second));
		}
	});

	mCommunistNameField->setOnGuiChangeFunction([=]()	
	{
		if(!mCommunistNameField->getText().empty())
		{
			mCommunistOkayButton->setEnabled(true);
			mCommunistOkayButton->setTexture(std::pair<sf::FloatRect, sf::Texture*> (mTeamCapitalist->getRectangle(), ButtonPos["CapitalistOkay"].second));
		}
		else
		{
			mCommunistOkayButton->setEnabled(false);
			mCommunistOkayButton->setTexture(std::pair<sf::FloatRect, sf::Texture*> (mTeamCapitalist->getRectangle(), ButtonPos["CapitalistOkayIsPressed"].second));
		}
	});

	mCommunistOkayButton->setOnClickFunction([=]()		
	{
		if(GameManager::getInstance()->getGameType() == LAN && GameManager::getInstance()->isMyTurnToPlay())
		{
			//sf::Packet packet;
			//packet<<PoliticalType::COMMUNIST;
			//GameManager::getInstance()->triggerOtherPlayersEvent("nextPlayerToChooseTeam", packet);
			//GameManager::getInstance()->nextPlayersTurn();
			GameManager::getInstance()->setEnemyTurn();
		}
		else
		{
			GameManager::getInstance()->getRemoteClient()->setSuperPower(PoliticalType::COMMUNIST);
			GameManager::getInstance()->setMyTurn();
		}
		mCommunistTeamChosen = true;
		mCommunistOkayButton->setTexture(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(mCommunistOkayButton->getX(), mCommunistOkayButton->getY(), mCommunistOkayButton->getWidth(), mCommunistOkayButton->getHeight()), &ResourceHandler::getInstance()->getTexture(std::string("Menu/Ok-knapp-inaktiv"))));
		mCommunistNameField->setTexture(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(mCommunistNameField->getX(), mCommunistNameField->getY(), mCommunistNameField->getWidth(), mCommunistNameField->getHeight()), &ResourceHandler::getInstance()->getTexture(std::string("Menu/Namnruta-inaktiv"))));
		mCommunistOkayButton->setEnabled(false);
		mCommunistNameField->setEnabled(false);
		mTeamCommunist->setTexture(std::pair<sf::FloatRect, sf::Texture*> (mTeamCommunist->getRectangle(), ButtonPos["TeamCommunistIsPressed"].second));
		std::shared_ptr<GUIWindow> _parentWindow = mParentWindow;
		std::map<std::string, std::shared_ptr<sf::Music>> _music = MenuMusic;
		if(mCommunistTeamChosen && mCapitalistTeamChosen)
		{
			GameManager::getInstance()->showWaitingScreen(false);
			Timer::setTimer([=]()
			{
				_parentWindow->setVisible(false);
				_music.at("MainMenuTrack")->stop(); 
				GameManager::getInstance()->init(1952); // initierar första året
			}, 100, 1);
		}
	});

	//InGameMenu buttons setOnClickFunctions
	   //Resume game
	mResumeGameButton->setMouseEnterFunction([=]()			
	{ 
		mResumeGameButton->setTexture(ButtonPos["ResumeHover"]); 
	});
	mResumeGameButton->setMouseLeaveFunction([=]()			
	{ 
		mResumeGameButton->setTexture(ButtonPos["Resume"]); 
	});
	mResumeGameButton->setOnClickFunction([=]()
	{
		mInGameMenuWindow->setVisible(false);
		mResumeGameButton->setTexture(ButtonPos["Resume"]); 
		GameManager::getInstance()->getCurrentPlayer()->showGUI();
		stopMusic();
	});
	
	   //Restart game
	mRestartGameButton->setMouseEnterFunction([=]()			
	{ 
		mRestartGameButton->setTexture(ButtonPos["RestartHover"]); 
	});
	mRestartGameButton->setMouseLeaveFunction([=]()			
	{ 
		mRestartGameButton->setTexture(ButtonPos["Restart"]); 
	});
	mRestartGameButton->setOnClickFunction([=]()
	{
		GameManager::getInstance()->reset();
		
		reset();
	});


	mLanPlayQuickConnect->setOnClickFunction([=]()
	{
		mLanPlayWindow->setEnabled(false, true);
		mWaitingForClientWindow->setVisible(true);
		mWaitingForClientWindow->setEnabled(true, true);

		GameManager::getInstance()->searchForServers();
	});

	//mLoadGameButton->setMouseEnterFunction([=]()		{ mLoadGameButton->setTexture(ButtonPos["LoadGameHover"]); });
	//mLoadGameButton->setMouseLeaveFunction([=]()		{ mLoadGameButton->setTexture(ButtonPos["LoadGame"]); });
	//mLoadGameButton->setOnClickFunction([=]()			
	//{
	//	setLoadGameButtonText();
	//	GUIManager::getInstance()->setOnTop(mLoadGameWindow);
	//	mLoadGameWindow->setVisible(true);
	//	mMainMenuWindow->setEnabled(false, true);
	//});

	mCloseLoadGameWindow->setOnClickFunction([=]()
	{
		mLoadGameWindow->setVisible(false);
		mMainMenuWindow->setEnabled(true, true);
	});

	mCloseWaitingForClientWindow->setOnClickFunction([=]()
	{
		GameManager::getInstance()->stopSearchingForServer();
		mWaitingForClientText->setText("Searching for server...");
		mWaitingForClientWindow->setVisible(false);
		mWaitingForClientWindow->setEnabled(false, true);
		mLanPlayWindow->setEnabled(true, true);
	});

	mBackToMainMenuButton[0]->setOnClickFunction([=]()
	{
		mChooseTeamWindow->setVisible(false);
		mMainMenuWindow->setVisible(true);
	});

	mBackToMainMenuButton[1]->setOnClickFunction([=]()
	{
		mLanPlayWindow->setVisible(false);
		mMainMenuWindow->setVisible(true);
	});

	//DETTA ÄR BORTKOMMENTERAT DÅ DET INTE GÅR ATT FÅ KLART SPARA/LADDA

	//mSaveGameButton->setMouseEnterFunction([=]()			{ mSaveGameButton->setTexture(ButtonPos["SaveGameHover"]); });
	//mSaveGameButton->setMouseLeaveFunction([=]()			{ mSaveGameButton->setTexture(ButtonPos["SaveGame"]); });
	//mSaveGameButton->setOnClickFunction([=]()
	//{
	//	mInGameMenuWindow->setEnabled(false, true);
	//	GUIManager::getInstance()->setOnTop(mSaveGameWindow[0]);
	//	mSaveGameWindow[0]->setVisible(true);
	//});

	mCloseSaveGameWindow[0]->setOnClickFunction([=]()
	{
		mSaveGameWindow[0]->setVisible(false);
		GUIManager::getInstance()->setOnTop(mSaveGameWindow[1]);
		mSaveGameWindow[1]->setVisible(true);
		GameManager::getInstance()->setDocumentName(mSaveGameTextField->getText());
				
		std::shared_ptr<GUIWindow> _saveWindow = mSaveGameWindow[1];
		std::shared_ptr<GUIWindow> _inGameWindow = mInGameMenuWindow;
		Timer::setTimer([=]()
		{
			_saveWindow->setVisible(false);
			_inGameWindow->setEnabled(true, true);

		}, 2000, 1);
	});


	mSavedGameSlots[0]->setOnClickFunction([=]()
	{
		std::cout << mSavedGameText[0]->getText() << std::endl;
		GameManager::getInstance()->loadGame(mSavedGameText[0]->getText());
	});
	
	mSavedGameSlots[1]->setOnClickFunction([=]()
	{
		std::cout << mSavedGameText[1]->getText() << std::endl;
		GameManager::getInstance()->loadGame(mSavedGameText[1]->getText());
	});
	
	mSavedGameSlots[2]->setOnClickFunction([=]()
	{
		std::cout << mSavedGameText[2]->getText() << std::endl;
		GameManager::getInstance()->loadGame(mSavedGameText[2]->getText());
	});

	mCloseCreditsButton->setOnClickFunction([=]()
	{ 
		mCreditsMenuWindow->setVisible(false);
		mCreditsMenuWindow->setEnabled(false, true);
		mMainMenuWindow->setEnabled(true, true);
	});
}	