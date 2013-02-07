#include "Menu.h"
#include "GUIManager.h"
#include "tinyxml2.h"
#include "Capitalist.h"
#include "Communist.h"
#include "ResourceHandler.h"
#include <sstream>
#include <iostream>
#include <SFML\Window\Mouse.hpp>
#include "GameManager.h"


 /*Konstruktorn k�r initialize-funktionen*/
Menu::Menu(sf::RenderWindow &window) : 
	mWindow(window)
{ 
	initialize(); 
	initializeGuiFuctions();
	MenuMusic["MainMenuTrack"]->play();
	MenuMusic["MainMenuTrack"]->setLoop(true);
}
Menu::~Menu(){}


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

	/*F�r�lder-f�nstret f�r alla menyer*/
	mParentWindow			= std::make_shared<GUIWindow>(WindowPos["MenuInterface"]);

	/*F�nstret och dess barn f�r LOGOMENU*/
	//mLogoMenuWindow		= std::make_shared<GUIWindow>(WindowPos["LogoScreen"], mParentWindow);


	/*F�nstret och dess barn f�r SPLASHSCREEN*/
	//mSplashScreenWindow = std::make_shared<GUIWindow>(WindowPos["SplashScreen"], mParentWindow);
	//mSplashScreenWindow->setVisible(false);

	/*F�nstret och dess barn f�r MAIN MENU*/
	mMainMenuWindow			= std::make_shared<GUIWindow>(WindowPos["MainMenu"], mParentWindow);
	mStartNewGameButton		= std::make_shared<GUIButton>(ButtonPos["StartGame"], mMainMenuWindow);
	mLoadGameButton			= std::make_shared<GUIButton>(ButtonPos["LoadGame"], mMainMenuWindow);
	mSettingsButton			= std::make_shared<GUIButton>(ButtonPos["Settings"], mMainMenuWindow);
	mCreditsButton			= std::make_shared<GUIButton>(ButtonPos["Credits"], mMainMenuWindow);
	mExitButton				= std::make_shared<GUIButton>(ButtonPos["Exit"], mMainMenuWindow);
	//mMainMenuWindow->setVisible(true);

	/*F�nstret och dess barn f�r SETTINGS MENU*/
	//mSettingsMenuWindow	= std::make_shared<GUIWindow>(WindowPos["SettingsMenu"], mParentWindow);
	//mSettingsMenuWindow->setVisible(false);

	/*F�nstret och dess barn f�r CREDITS MENU*/
	mCreditsMenuWindow		= std::make_shared<GUIWindow>(WindowPos["CreditsMenu"], mParentWindow);
	mCreditsMenuWindow->setVisible(false);

	/*F�nstret och dess barn f�r att v�lja lag*/
	mPickTeamWindow			= std::make_shared<GUIWindow>(WindowPos["PickTeam"], mParentWindow);
	mTeamCapitalistButton	= std::make_shared<GUIButton>(ButtonPos["TeamCapitalist"], mPickTeamWindow);
	mTeamCommunistButton	= std::make_shared<GUIButton>(ButtonPos["TeamCommunist"], mPickTeamWindow);
	mPickTeamWindow->setVisible(false);


	/*L�gger in f�nstrerna i vektorn f�r GUIElement*/
	GUIManager::getInstance()->addGUIElement(mParentWindow);
}

 /*
	Initierar lambda funktioner f�r GUIknapparna. Allts� vad som skall
	h�nda d� dessa har blivit tryckta p�. 

	  Av: Jon Wahlstr�m  2013-01-29
																	*/
void Menu::initializeGuiFuctions()
{
	mStartNewGameButton->setMouseEnterFunction([=]()	{ mStartNewGameButton->setTexture(ButtonPos["StartGameHover"]); });
	mStartNewGameButton->setMouseLeaveFunction([=]()	{ mStartNewGameButton->setTexture(ButtonPos["StartGame"]); });
	mStartNewGameButton->setOnClickFunction([=]()		{ mMainMenuWindow->setVisible(false); mPickTeamWindow->setVisible(true); });

	mLoadGameButton->setMouseEnterFunction([=]()		{ mLoadGameButton->setTexture(ButtonPos["LoadGameHover"]); });
	mLoadGameButton->setMouseLeaveFunction([=]()		{ mLoadGameButton->setTexture(ButtonPos["LoadGame"]); });
	mLoadGameButton->setOnClickFunction([=]()			{ });
	
	mSettingsButton->setMouseEnterFunction([=]()		{ mSettingsButton->setTexture(ButtonPos["SettingsHover"]); });
	mSettingsButton->setMouseLeaveFunction([=]()		{ mSettingsButton->setTexture(ButtonPos["Settings"]); });
	mSettingsButton->setOnClickFunction([=]()			{ });

	mCreditsButton->setMouseEnterFunction([=]()			{ mCreditsButton->setTexture(ButtonPos["CreditsHover"]); });
	mCreditsButton->setMouseLeaveFunction([=]()			{ mCreditsButton->setTexture(ButtonPos["Credits"]); });
	mCreditsButton->setOnClickFunction([=]()			{ mMainMenuWindow->setVisible(false); mCreditsMenuWindow->setVisible(true); });
	
	mExitButton->setMouseEnterFunction([=]()			{ mExitButton->setTexture(ButtonPos["ExitHover"]); });
	mExitButton->setMouseLeaveFunction([=]()			{ mExitButton->setTexture(ButtonPos["Exit"]); });
	mExitButton->setOnClickFunction([=]()				{ mWindow.close(); });

	/*n�r spelaren v�ljer att spela kapitalist */
	mTeamCapitalistButton->setOnClickFunction([=]()		{ mParentWindow->setVisible(false); MenuMusic["MainMenuTrack"]->stop(); 
														  GameManager::getInstance()->setCurrentPlayer(std::make_shared<Capitalist>()); });

	/*n�r spelaren v�ljer att spela kommunist*/
	mTeamCommunistButton->setOnClickFunction([=]()		{ mParentWindow->setVisible(false); MenuMusic["MainMenuTrack"]->stop(); 
														  GameManager::getInstance()->setCurrentPlayer(std::make_shared<Communist>()); });



}	