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
#include "TcpServer.h"
#include "TcpClient.h"
#include "UdpServer.h"
#include "UdpClient.h"
#include "Event.h"
#include <SFML/Network.hpp>
#include <SFML\Audio\Listener.hpp>


 /*Konstruktorn k�r initialize-funktionen*/
Menu::Menu(sf::RenderWindow &window) : 
	mWindow(window),
	mCapitalistTeamChosen(false),
	mCommunistTeamChosen(false),
	mTcpServer(nullptr),
	mTcpClient(nullptr),
	mUdpServer(nullptr),
	mUdpClient(nullptr),
	mShowTeamChooseAnimation(false)
{ 
	//mUdpClient = new sf::UdpClient(55001, 55002, sf::IpAddress::Broadcast);
	Event::addEventHandler("serverAwaitingConnection", [=](sf::Packet packet)
	{
		char serverAddress[1024];
		unsigned short serverPort;
		packet>>serverAddress>>serverPort;
		std::cout<<"Server open: "<<serverAddress<<" port: "<<serverPort<<std::endl;
		//connectToServer(serverPort, sf::IpAddress::IpAddress(serverAddress));
	});
	sf::Packet packet;
	packet<<sf::IpAddress::getLocalAddress().toString();
	//mUdpClient->triggerServerEvent("onPlayerConnect", packet);
	initialize(); 
	initializeGuiFuctions();
	//loadTeamAnimation();
	mTeamAnimationTimer = Timer::setTimer([&](){}, 5000, 1);
	MenuMusic["MainMenuTrack"]->play();
	MenuMusic["MainMenuTrack"]->setLoop(true);
}
Menu::~Menu(){ }

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

void Menu::connectToServer(unsigned short port, sf::IpAddress ipAddress)
{
	if(mTcpClient == nullptr)
		mTcpClient = new sf::TcpClient(port, ipAddress);
}

void Menu::clear()
{
	MenuMusic.clear();
	ButtonPos.clear();
	WindowPos.clear();
}


void Menu:: update(sf::Event &event)
{
	if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Escape)
	{
		//GameManager::getInstance()->getCurrentPlayer()->hideGUI();
		std::cout<<"in game menu is visible"<<std::endl;

		GUIManager::getInstance()->setOnTop(mInGameMenuWindow);

		mInGameMenuWindow->setVisible(true);
	}


}

void Menu::resetChooseTeamValues()
{
	mCapitalistTeamChosen = false;
	mCommunistTeamChosen = false;
}

void Menu::setMainMenuVisible()
{
	mMainMenuWindow->setVisible(true);
}

 /*Laddar in menyknapparnas positions- och storleksinformation
   fr�n ett externbibliotek kallat tinyXML2
   
     Av: Jon Wahlstr�m 2013-01-30*/
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
   Laddar in menyf�nstrernas information via ett XML dokument och placerar in
   dessa i en std::map

    Av: Jon Wahlstr�m 2013-02-03
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


 /*
	Initierar menyernas f�nster, bilder samt knappar som skall finnas med.
	dessa �r av olika typer av GUIElement. Beroende p� vilken knapp som 
	blir vald s� kommer olika GUIWindow att renderas ut som i detta fall
	representerar de olika menyernas f�nster. 

	Av: Jon Wahlstr�m  2013-01-28
																		*/
void Menu::initialize()
{
	loadButtonPosition();
	loadWindowPosition();
	loadMenuMusic();

	mParentWindow			= GUIWindow::create(WindowPos["MenuInterface"]);

	/*F�nstret och dess barn f�r LOGOMENU*/
	//mLogoMenuWindow		= GUIWindow::create(WindowPos["LogoScreen"], mParentWindow);

	/*F�nstret och dess barn f�r SPLASHSCREEN*/
	//mSplashScreenWindow = GUIWindow::create(WindowPos["SplashScreen"], mParentWindow);
	//mSplashScreenWindow->setVisible(false);

	mMainMenuWindow			= GUIWindow::create(WindowPos["MainMenu"], mParentWindow);
	mStartNewGameButton		= GUIButton::create(ButtonPos["StartGame"], mMainMenuWindow);
	mMultiPlayerButton		= GUIButton::create(ButtonPos["MultiPlayer"], mMainMenuWindow);
	mLoadGameButton			= GUIButton::create(ButtonPos["LoadGame"], mMainMenuWindow);
	mSettingsButton			= GUIButton::create(ButtonPos["Settings"], mMainMenuWindow);
	mCreditsButton			= GUIButton::create(ButtonPos["Credits"], mMainMenuWindow);
	mExitButton				= GUIButton::create(ButtonPos["Exit"], mMainMenuWindow);
	mMainMenuWindow->setVisible(false);

	mInGameMenuWindow		= GUIWindow::create(WindowPos["InGameMenu"]);
	mResumeGameButton		= GUIButton::create(ButtonPos["Resume"], mInGameMenuWindow);
	mRestartGameButton		= GUIButton::create(ButtonPos["Restart"], mInGameMenuWindow);
	mSaveGameButton			= GUIButton::create(ButtonPos["SaveGame"], mInGameMenuWindow);
	mInGameMenuWindow->setVisible(false);

	mSettingsMenuWindow		= GUIWindow::create(WindowPos["SettingsMenu"], mParentWindow);
	mVolumeText				= GUIText::create(sf::FloatRect(300, 40, 40, 40), "Volume", mSettingsMenuWindow);
	mVolumeText->setColor(sf::Color::White);
	mLowerVolume			= GUIButton::create(ButtonPos["LowerVolume"], mSettingsMenuWindow);
	mRaiseVolume			= GUIButton::create(ButtonPos["RaiseVolume"], mSettingsMenuWindow);
	mWindowSizeText			= GUIText::create(sf::FloatRect(300, 200, 40, 40), "Window size", mSettingsMenuWindow);
	mWindowSizeText->setColor(sf::Color::White);
	mCloseSettingsWindow	= GUIButton::create(ButtonPos["Esc"], mSettingsMenuWindow);
	mSettingsMenuWindow->setVisible(false);

	mCreditsMenuWindow		= GUIWindow::create(WindowPos["CreditsMenu"]);
	mCreditsMenuWindow->setVisible(false);

	mChooseTeamWindow			= GUIWindow::create(WindowPos["ChooseTeam"], mParentWindow);
	mTeamCommunist				= GUIButton::create(ButtonPos["TeamCommunist"], mChooseTeamWindow);
	mTeamCapitalist				= GUIButton::create(ButtonPos["TeamCapitalist"], mChooseTeamWindow);
	mCapitalistNameField		= GUIEditField::create(sf::FloatRect(43 + 8, 269, 218, 41), GUIEditField::MENU, "", false, mChooseTeamWindow);
	mCommunistNameField			= GUIEditField::create(sf::FloatRect(421 + 8, 269, 218, 41), GUIEditField::MENU, "", false, mChooseTeamWindow);
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
	mLanPlayWindow			= GUIWindow::create(WindowPos["LanPlayWindow"], mParentWindow);
	mLanPlayQuickConnect	= GUIButton::create(ButtonPos["LanPlayQuickConnect"], mLanPlayWindow);
	mLanPlayWindow->setVisible(false);

	GUIManager::getInstance()->addGUIElement(mParentWindow);
	
	GUIManager::getInstance()->addGUIElement(mInGameMenuWindow);
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
		sprite.setPosition(mWindow.getSize().x/2 - mTeamAnimationFrames[frame].getSize().x/2, mWindow.getSize().y/2 - mTeamAnimationFrames[frame].getSize().y/2);
		mWindow.draw(sprite);
	}
}

void Menu::initializeGuiFuctions()
{
	mStartNewGameButton->setMouseEnterFunction([=]()	{ mStartNewGameButton->setTexture(ButtonPos["StartGameHover"]); });
	mStartNewGameButton->setMouseLeaveFunction([=]()	{ mStartNewGameButton->setTexture(ButtonPos["StartGame"]); });
	mStartNewGameButton->setOnClickFunction([=]()		
	{ 
		mMainMenuWindow->setVisible(false);
		//mShowTeamChooseAnimation = true;
		//mTeamAnimationTimer->resetTimer();
		mChooseTeamWindow->setVisible(true); 
	});

	mMultiPlayerButton->setMouseEnterFunction([=]()		{ mMultiPlayerButton->setTexture(ButtonPos["MultiPlayerHover"]); });
	mMultiPlayerButton->setMouseLeaveFunction([=]()		{ mMultiPlayerButton->setTexture(ButtonPos["MultiPlayer"]); });
	mMultiPlayerButton->setOnClickFunction([=]()		
	{ 
		//mParentWindow->setEnabled(false, true);
		mLanPlayWindow->setVisible(true);
		mLanPlayWindow->setEnabled(true, true);
	});

	mLanPlayQuickConnect->setOnClickFunction([=]()
	{

	});

	mLoadGameButton->setMouseEnterFunction([=]()		{ mLoadGameButton->setTexture(ButtonPos["LoadGameHover"]); });
	mLoadGameButton->setMouseLeaveFunction([=]()		{ mLoadGameButton->setTexture(ButtonPos["LoadGame"]); });
	mLoadGameButton->setOnClickFunction([=]()			{ });
	
	mSettingsButton->setMouseEnterFunction([=]()		{ mSettingsButton->setTexture(ButtonPos["SettingsHover"]); });
	mSettingsButton->setMouseLeaveFunction([=]()		{ mSettingsButton->setTexture(ButtonPos["Settings"]); });
	mSettingsButton->setOnClickFunction([=]()			
	{ 
		//mMainMenuWindow->setVisible(false);
		mMainMenuWindow->setEnabled(false, true);
		mSettingsMenuWindow->setVisible(true);
		mSettingsMenuWindow->setEnabled(true, true);
	});

	mLowerVolume->setOnClickFunction([=]()		{ sf::Listener::setGlobalVolume(sf::Listener::getGlobalVolume() - 10); });
	mRaiseVolume->setOnClickFunction([=]()		{ sf::Listener::setGlobalVolume(sf::Listener::getGlobalVolume() + 10); });	
	mCloseSettingsWindow->setOnClickFunction([=]()		
	{ 
		mSettingsMenuWindow->setVisible(false);
		mSettingsMenuWindow->setEnabled(false, true);
		//mMainMenuWindow->setVisible(true); 
		mMainMenuWindow->setEnabled(true, true);
	});	

	mCreditsButton->setMouseEnterFunction([=]()	{ mCreditsButton->setTexture(ButtonPos["CreditsHover"]); });
	mCreditsButton->setMouseLeaveFunction([=]()	{ mCreditsButton->setTexture(ButtonPos["Credits"]); });
	
	mCreditsButton->setOnClickFunction([=]()	{ mMainMenuWindow->setVisible(false); mCreditsMenuWindow->setVisible(true); });
	
	mExitButton->setMouseEnterFunction([=]()	{ mExitButton->setTexture(ButtonPos["ExitHover"]); });
	mExitButton->setMouseLeaveFunction([=]()	{ mExitButton->setTexture(ButtonPos["Exit"]); });
	mExitButton->setOnClickFunction([=]()		{ mWindow.close(); });


	mCapitalistOkayButton->setOnClickFunction([=]()		
	{
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
			Timer::setTimer([=]()
			{
				_parentWindow->setVisible(false); 
				_music.at("MainMenuTrack")->stop(); 
				GameManager::getInstance()->init(1952); // initierar f�rsta �ret
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
			Timer::setTimer([=]()
			{
				_parentWindow->setVisible(false);

				_music.at("MainMenuTrack")->stop(); 

				GameManager::getInstance()->init(1952); // initierar f�rsta �ret
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
		//GameManager::getInstance()->getCurrentPlayer()->showGUI();
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
		mParentWindow->setVisible(true);
		mInGameMenuWindow->setVisible(false); 
		mChooseTeamWindow->setVisible(true);
	
		mCapitalistOkayButton->setTexture(std::pair<sf::FloatRect, sf::Texture*>
			(sf::FloatRect(mCapitalistOkayButton->getX(), mCapitalistOkayButton->getY(), mCapitalistOkayButton->getWidth(), mCapitalistOkayButton->getHeight()), &ResourceHandler::getInstance()->getTexture(std::string("Menu/Ok-knapp-aktiv"))));
		mCapitalistNameField->setTexture(std::pair<sf::FloatRect, sf::Texture*>
			(sf::FloatRect(mCapitalistNameField->getX(), mCapitalistNameField->getY(), mCapitalistNameField->getWidth(), mCapitalistNameField->getHeight()), &ResourceHandler::getInstance()->getTexture(std::string("Menu/Namnruta-aktiv"))));
		mTeamCapitalist->setTexture(std::pair<sf::FloatRect, sf::Texture*> (mTeamCapitalist->getRectangle(), ButtonPos["TeamCapitalist"].second));
		
		mCommunistOkayButton->setTexture(std::pair<sf::FloatRect, sf::Texture*>
			(sf::FloatRect(mCommunistOkayButton->getX(), mCommunistOkayButton->getY(), mCommunistOkayButton->getWidth(), mCommunistOkayButton->getHeight()), &ResourceHandler::getInstance()->getTexture(std::string("Menu/Ok-knapp-aktiv"))));
		mCommunistNameField->setTexture(std::pair<sf::FloatRect, sf::Texture*>
			(sf::FloatRect(mCommunistNameField->getX(), mCommunistNameField->getY(), mCommunistNameField->getWidth(), mCommunistNameField->getHeight()), &ResourceHandler::getInstance()->getTexture(std::string("Menu/Namnruta-aktiv"))));
		mTeamCommunist->setTexture(std::pair<sf::FloatRect, sf::Texture*> (mTeamCommunist->getRectangle(), ButtonPos["TeamCommunist"].second));
		resetChooseTeamValues();
		mChooseTeamWindow->setEnabled(true, true);
	});

	mSaveGameButton->setOnClickFunction([=]()
	{
		
	});
}	