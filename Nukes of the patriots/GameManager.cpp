#include "GameManager.h"
#include "GUIManager.h"
#include "GUIElement.h"
#include "ResourceHandler.h"
#include "Randomizer.h"
#include "Capitalist.h"
#include "Communist.h"
#include "SuperPower.h"
#include "President.h"
#include "tinyxml2.h"
#include "GUIText.h"
#include "Timer.h"
#include "TimerHandler.h"
#include "GUIAnimation.h"
#include "AnimationHandler.h"
#include "Menu.h"
#include "Event.h"
#include "TcpClient.h"
#include "TcpServer.h"
#include "UdpClient.h"
#include "UdpServer.h"
#include "RemoteClient.h"
#include <SFML\Network.hpp>
#include "RemoteClient.h"

static int width = 1024;
static int height = 768;

static sf::Texture cursorTexture;
static sf::Texture	cursorClickedTexture;
static sf::Sprite cursor;

GameManager* GameManager::mInstance = NULL;

GameManager* GameManager::getInstance()
{
	if(mInstance == NULL)
	{
		mInstance = new GameManager();
	}
	return mInstance;
}

GameManager::GameManager() : 
	mTcpServer(nullptr),
	mTcpClient(nullptr),
	mUdpServer(nullptr),
	mUdpClient(nullptr),
	mCreateServerEvent(nullptr),
	mConnectToServerEvent(nullptr),
	mCreateServerTimer(nullptr),
	mVecPlayersLeft(),		
	mRound(0),
	mLoaded(false),
	mServerState(CLOSED),
	mGameType(VERSUS),
	mRole(CLIENT),
	mRemoteClient(std::make_shared<RemoteClient>()),
	mRemoteIpAddress(""),
	mRemotePort(0),
	mPlayersTurn(0)
{
	cursorTexture.loadFromFile("Images/Mouse/MouseCursor.png");
	cursorClickedTexture.loadFromFile("Images/Mouse/MouseCursorClicked.png");
	cursor.setTexture(cursorTexture);

	mUdpClient = new sf::UdpClient(55001, 55005, sf::IpAddress::Broadcast);
	
	mConnectToServerEvent = Event::addEventHandler("hereIam", 
		[=](sf::Packet packet)
	{
		char ipAddress[1024];
		unsigned short port;
		packet>>ipAddress>>port;
		connectToServer(ipAddress, port);
	});

	Event::addEventHandler("onClientConnected", 
		[=](sf::Packet packet)
	{
		mPlayersTurn = 0;
		packet>>mRemoteIpAddress>>mRemotePort;
		mGameType = GameType::LAN;
		Menu::getInstance()->startGame();
	});

	Event::addEventHandler("syncMousePosition",
		[=](sf::Packet packet)
	{
		int mouseX = 0, mouseY = 0;
		packet>>mouseX>>mouseY;
		mRemoteClient->setMousePosition(sf::Vector2i(mouseX, mouseY));
	});

	Event::addEventHandler("syncGUIClick",
		[=](sf::Packet packet)
	{
		int id = 0;
		packet>>id;
		std::cout<<"gui click - GUIID: "<<id<<std::endl;
	});

	Event::addEventHandler("syncGUIMouseEnter",
		[=](sf::Packet packet)
	{
		int id = 0;
		packet>>id;
		std::cout<<"gui mouse enter - GUIID: "<<id<<std::endl;
	});

	Event::addEventHandler("syncGUIMouseLeave",
		[=](sf::Packet packet)
	{
		int id = 0;
		packet>>id;
		std::cout<<"gui mouse leave - GUIID: "<<id<<std::endl;
	});

	initializeGuiElement();
	initializeGuiFunctions();
}

GameManager::~GameManager()
{
	
}

void GameManager::reset()
{
	mRound = 0;
	mYear = 1952;
	mYearText->setText("1952");
	for(std::vector<std::shared_ptr<SuperPower> >::iterator it = mVecSuperPowers.begin(); it != mVecSuperPowers.end(); it++)
	{
		(*it)->reset();
	}
	AnimationHandler::getInstance()->reset();
	getCap()->hideGUI();
	getCom()->hideGUI();
	mYearText->setVisible(false);
}

void GameManager::init(int year)
{

	if(!mLoaded)
	{
		getInstance()->setYear(year);
		loadPresidents();
		mVecSuperPowers.push_back(std::make_shared<Capitalist>());
		mVecSuperPowers.push_back(std::make_shared<Communist>());
		mVecPlayersLeft = mVecSuperPowers;

		/*Skriver ut året på interface*/
		mYearText = GUIText::create(sf::FloatRect(512, 4, 0, 0), intToString(mYear));
		mYearText->setScale(0.6, 0.6);
		mYearText->setAlignment("middle");
		mYearText->setColor(sf::Color::White);
		GUIManager::getInstance()->addGUIElement(mYearText);
	 /*for(std::vector<std::shared_ptr<SuperPower> >::iterator it = mVecSuperPowers.begin(); it != mVecSuperPowers.end(); it++)
	{
		getInstance()->setYear(year);
		loadPresidents();
		mVecSuperPowers.push_back(std::make_shared<Capitalist>());
		mVecSuperPowers.push_back(std::make_shared<Communist>());
		mVecPlayersLeft = mVecSuperPowers;
	
		/*for(std::vector<std::shared_ptr<SuperPower> >::iterator it = mVecSuperPowers.begin(); it != mVecSuperPowers.end(); it++)
		{
			(*it)->chooseLeader();
		}*/
		mCurrentPlayer = mVecSuperPowers[Randomizer::getInstance()->randomNr(mVecSuperPowers.size(), 0)];
		for(std::vector<std::shared_ptr<SuperPower> >::iterator it = mVecPlayersLeft.begin(); it != mVecPlayersLeft.end(); it++)
		{
			if((*it) == mCurrentPlayer)
			{
				mVecPlayersLeft.erase(it);
				break;
			}
		}
		//startRound();
		mLoaded = true;
	}
	else
	{
		mVecPlayersLeft = mVecSuperPowers;
		mCurrentPlayer = mVecSuperPowers[Randomizer::getInstance()->randomNr(mVecSuperPowers.size(), 0)];
		for(std::vector<std::shared_ptr<SuperPower> >::iterator it = mVecPlayersLeft.begin(); it != mVecPlayersLeft.end(); it++)
		{
			if((*it) == mCurrentPlayer)
			{
				mVecPlayersLeft.erase(it);
				break;
			}
		}
	}

	mCurrentPlayer->setRound(1);
	mCurrentPlayer->showGUI();
	//startRound();

}

std::shared_ptr<SuperPower> GameManager::getCapitalist()
{
	return mVecSuperPowers[0];
}

std::shared_ptr<SuperPower> GameManager::getCommunist()
{
	return mVecSuperPowers[1];
}

std::shared_ptr<Capitalist> GameManager::getCap()
{
	return /*std::shared_ptr<Capitalist> cap = */std::static_pointer_cast<Capitalist> (mVecSuperPowers[0]);
}

std::shared_ptr<Communist> GameManager::getCom()
{
	//std::shared_ptr<Communist> com = std::static_pointer_cast<Communist> (mVecSuperPowers[1]);
	return std::static_pointer_cast<Communist> (mVecSuperPowers[1]);
}

void GameManager::loadPresidents()
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile("XML/Images.xml");

	if(doc.Error())
		std::cout << "Fel!";
	
	tinyxml2::XMLElement *images = doc.FirstChildElement("images");
	while(images != NULL)
	{
		if(strcmp(images->Attribute("directory"), "President") == 0)
		{
			tinyxml2::XMLElement *childNode = images->FirstChildElement("image");
			while (childNode != NULL)
			{
				std::string filename = childNode->Attribute("filename");
				std::string key		 = "President/" + filename.substr(0, filename.length() - 4);
				std::string name	 = key.substr(20, key.length());
				std::shared_ptr<President> president = std::make_shared<President>(key, name);
				mPresidentVector.push_back(president);
				std::string mapKey = "Plaques/" + filename.substr(0, filename.length() - 4) + "-plaque";
				mPresidentPlaqueMap[president] = &ResourceHandler::getInstance()->getTexture(mapKey);
				childNode = childNode->NextSiblingElement("image");
			}
		}

		if(strcmp(images->Attribute("directory"), "Generals") == 0)
		{
			tinyxml2::XMLElement *childNode = images->FirstChildElement("image");
			while (childNode != NULL)
			{
				std::string filename = childNode->Attribute("filename");
				std::string key		 = "Generals/" + filename.substr(0, filename.length() - 4);
				std::string name	 = key.substr(19, key.length());
				std::shared_ptr<President> general = std::make_shared<President>(key, name);
				mGeneralVector.push_back(general);	
				std::string mapKey		 = "Plaques/" + filename.substr(0, filename.length() - 4) + "-plaque";
				mGeneralPlaqueMap[general] = &ResourceHandler::getInstance()->getTexture(mapKey);
				childNode = childNode->NextSiblingElement("image");
			}
		}
		images = images->NextSiblingElement("images");
	}
}

std::shared_ptr<President> GameManager::getRandomPresident()
{
	int random = Randomizer::getInstance()->randomNr(mPresidentVector.size(), 0);
	std::shared_ptr<President> result = mPresidentVector[random];
	mPresidentVector.erase(mPresidentVector.begin() + random);
	return result;
}

std::shared_ptr<President> GameManager::getGeneral(int number)
{
	return mGeneralVector[number];
}

sf::Texture& GameManager::getPresidentPlaque(std::shared_ptr<President> president)
{
	return *mPresidentPlaqueMap[president];
}

sf::Texture& GameManager::getGeneralPlaque(std::shared_ptr<President> general)
{
	return *mGeneralPlaqueMap[general];
}

void GameManager::addSuperPower(std::shared_ptr<SuperPower> power)
{
	mVecSuperPowers.push_back(power);
}

void GameManager::startRound()
{
	/*
	int moneyIntFood = 0;
	int moneyIntGoods = 0;
	int moneyIntTech = 0;
	// Money internationally should be equal to everybodies money together
	for(std::vector<std::shared_ptr<SuperPower> >::iterator it = mVecSuperPowers.begin(); it != mVecSuperPowers.end(); it++)
	{
		moneyIntFood += (*it)->getPopulation();
		moneyIntGoods += (*it)->getPopulation();
		moneyIntTech += (*it)->getPopulation();
	}
	int foodBought = 0;
	int goodsBought = 0;
	int techBought = 0;
	int exports = 0;
	// if nobody bought my exports - then it will be sold internationally
	foodBought = (mCurrentPlayer->getExportedFood() == 0 || mCurrentPlayer->getExportedFoodPrice() == 0) ? 1 : moneyIntFood / mCurrentPlayer->getExportedFoodPrice();
	goodsBought = (mCurrentPlayer->getExportedGoods() == 0 || mCurrentPlayer->getExportedGoodsPrice() == 0) ? 1 : moneyIntGoods / mCurrentPlayer->getExportedGoodsPrice();
	techBought =(mCurrentPlayer->getExportedTech() == 0 || mCurrentPlayer->getExportedTechPrice() == 0) ? 1 :  moneyIntTech / mCurrentPlayer->getExportedTechPrice();
	//// if international market tries to buy more resources than you have
	if(foodBought > mCurrentPlayer->getExportedFood()) foodBought = mCurrentPlayer->getExportedFood();
	if(goodsBought > mCurrentPlayer->getExportedGoods()) goodsBought = mCurrentPlayer->getExportedGoods();
	if(techBought > mCurrentPlayer->getExportedTech()) techBought = mCurrentPlayer->getExportedTech();
	exports += (foodBought * mCurrentPlayer->getExportedFoodPrice()) + (goodsBought * mCurrentPlayer->getExportedGoodsPrice()) + (techBought * mCurrentPlayer->getExportedTechPrice());
	mCurrentPlayer->setExportedFood(mCurrentPlayer->getExportedFood() - foodBought);
	mCurrentPlayer->setExportedGoods(mCurrentPlayer->getExportedGoods() - goodsBought);
	mCurrentPlayer->setExportedTech(mCurrentPlayer->getExportedTech() - techBought);
	mCurrentPlayer->setCurrency(mCurrentPlayer->getCurrency() + exports);
	*/

	mCurrentPlayer->update();

	mCurrentPlayer->showGUI();
}

int GameManager::getYear()const
{
	return mYear;
}

std::shared_ptr<SuperPower> GameManager::getCurrentPlayer()const
{
	return mCurrentPlayer;
}

std::vector<std::shared_ptr<SuperPower> > GameManager::getPlayers()const
{
	return mVecSuperPowers;
}

void GameManager::setCurrentPlayer(std::shared_ptr<SuperPower> newPlayer)
{
	mCurrentPlayer = newPlayer;
	for(std::vector<std::shared_ptr<SuperPower> >::iterator it = mVecPlayersLeft.begin(); it != mVecPlayersLeft.end(); it++)
	{
		if((*it) == mCurrentPlayer)
		{
			mVecPlayersLeft.erase(it);
			break;
		}
	}
	mCurrentPlayer->setRound(mCurrentPlayer->getRound() + 1);
}

void GameManager::selectStartingPlayer(std::shared_ptr<SuperPower> startingPlayer)
{
	mSecondDecideWhoStartWindow->setVisible(true);
	mStatsWindow[1]->setVisible(false);
	mFirstDecideWhoStartWindow->setEnabled(true, true);
	mSecondDecideWhoStartWindow->setEnabled(true, true);
	mSecondCapitalistSpyNetworkText->setText("Spy network: " + intToString(getCapitalist()->getSpyNetwork()));
	mSecondCommunistSpyNetworkText->setText("Spy network: " + intToString(getCommunist()->getSpyNetwork()));
}

void GameManager::setYear(int year)
{
	mYear = year; 
}

void GameManager::updateStatsWindow()
{
	
}

void GameManager::nextRound()
{

	for(std::vector<std::shared_ptr<SuperPower> >::iterator it = mVecSuperPowers.begin(); it != mVecSuperPowers.end(); it++)
	{
		(*it)->hideGUI();
	}
	mRound++;
	// Everybody has played once this round, time to decide next player to start by spy network
	if(mVecPlayersLeft.size() == 0)
	{
		mVecPlayersLeft = mVecSuperPowers;
		setYear(getYear() + 1);
		// Loop all players and put those with highest same spy network in vector to get random
		std::vector<std::shared_ptr<SuperPower> > nextPlayers;
		int max = 0;
		for(std::vector<std::shared_ptr<SuperPower> >::iterator it = mVecSuperPowers.begin(); it != mVecSuperPowers.end(); it++)
		{
			if((*it)->getSpyNetwork() >= max)
				max = (*it)->getSpyNetwork();
		}
		for(std::vector<std::shared_ptr<SuperPower> >::iterator it = mVecSuperPowers.begin(); it != mVecSuperPowers.end(); it++)
		{
			if((*it)->getSpyNetwork() == max)
				nextPlayers.push_back(*it);
		}
		// Sätter statsfönstret till true när ett år har gått
		for(std::vector<std::shared_ptr<SuperPower> >::iterator it = mVecSuperPowers.begin(); it != mVecSuperPowers.end(); it++)
		{
			(*it)->newYearStart();
		}
		for(std::vector<std::shared_ptr<SuperPower> >::iterator it = mVecSuperPowers.begin(); it != mVecSuperPowers.end(); it++)
		{
			(*it)->setExportedFoodSold(0);
			(*it)->setExportedGoodsSold(0);
			(*it)->setExportedTechSold(0);
		}
		updateStatsWindow();

		mStatsWindow[0]->setVisible(true);
		mStatsWindow[0]->setColor(sf::Color(255, 255, 255, 255));
		mStatsWindow[0]->setSize(1024, 768);
		mStatsWindow[0]->setX(0);
		mStatsWindow[0]->setY(0);
		mStatsWindow[1]->setVisible(false);
		mFirstDecideWhoStartWindow->setVisible(false);
		mSecondDecideWhoStartWindow->setVisible(false);
		GUIAnimation::move(mStatsWindow[0], 4000, mStatsWindow[0]->getRectangle(), sf::FloatRect(-(mStatsWindow[0]->getWidth()*1.73067)/2, -(mStatsWindow[0]->getHeight()*1.73067)/2 - 100, mStatsWindow[0]->getWidth()*2.73067, mStatsWindow[0]->getHeight()*2.73067));
		//aktiverar en timer för statswindows, en zoomövergång från det första till det andra
		//TA BORT DETTA OCH ÄNDRA TILL INZOOMAT 
		Timer::setTimer([=]()
		{
			GUIAnimation::fadeToColor(mStatsWindow[0], 1000, mStatsWindow[0]->getColor(), sf::Color(255, 255, 255, 0));
			std::shared_ptr<GUIWindow> _statsWindow = mStatsWindow[0];
			Timer::setTimer([=]()
			{
				_statsWindow->setVisible(false);
			}, 1000, 1);

			mStatsWindow[1]->setColor(sf::Color(255, 255, 255, 0));
			mStatsWindow[1]->setVisible(true);
			GUIAnimation::fadeToColor(mStatsWindow[1], 1000, mStatsWindow[1]->getColor(), sf::Color(255, 255, 255, 255));
		}, 3000, 1);
		int randomPlayer = Randomizer::getInstance()->randomNr(nextPlayers.size(), 0);
		//If both player has same spy network, then select random as next player directly
		mCloseStatsWindow->setOnClickFunction([=]()
		{
			if(nextPlayers.size() == 1)
			{
				selectStartingPlayer(nextPlayers[randomPlayer]);
			}
			else
			{
				setCurrentPlayer(nextPlayers[randomPlayer]); // Need to set setCurrentPlayer to update player round
				mFirstDecideWhoStartWindow->setVisible(true);
				mStatsWindow[1]->setVisible(false);
				//mNextWindowToShow = mFirstDecideWhoStartWindow;
				mFirstDecideWhoStartWindow->setEnabled(true, true);
				mFirstCapitalistSpyNetworkText->setText("Spy network: " + intToString(getCapitalist()->getSpyNetwork()));
				mFirstCommunistSpyNetworkText->setText("Spy network: " + intToString(getCommunist()->getSpyNetwork()));
			} 
		});
	}
	else
	{
		std::vector<std::shared_ptr<SuperPower> > nextPlayers;
		int max = 0;
		for(std::vector<std::shared_ptr<SuperPower> >::iterator it = mVecPlayersLeft.begin(); it != mVecPlayersLeft.end(); it++)
		{
			if((*it)->getSpyNetwork() >= max)
				max = (*it)->getSpyNetwork();
		}
		for(std::vector<std::shared_ptr<SuperPower> >::iterator it = mVecPlayersLeft.begin(); it != mVecPlayersLeft.end(); it++)
		{
			if((*it)->getSpyNetwork() == max)
				nextPlayers.push_back((*it));
		}
		int randomPlayer = Randomizer::getInstance()->randomNr(nextPlayers.size(), 0);
		setCurrentPlayer(nextPlayers[randomPlayer]);
		startRound();
	}
	/*Ökar år med ett när rundan är slut*/
	mYearText->setText(mYear);
}

 //laddar in fönster
void GameManager::loadWindowPosition()
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile("XML/BetweenTurnsWindow.xml");

	if(doc.Error())
		std::cout << "Fel!";
	
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
				
		BetweenTurnsWindow[tempName].first.left = posX;
		BetweenTurnsWindow[tempName].first.top = posY;
		BetweenTurnsWindow[tempName].first.width = Width;
		BetweenTurnsWindow[tempName].first.height = Height;
		BetweenTurnsWindow[tempName].second = &ResourceHandler::getInstance()->getTexture(name);
		
		window = window->NextSiblingElement();
	}	
}
 //laddar in knappar
void GameManager::loadButtonPosition()
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile("XML/BetweenTurnsButton.xml");

	if(doc.Error())
		std::cout << "Fel!";
	
	tinyxml2::XMLElement *element = doc.FirstChildElement("buttons");
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

		BetweenTurnsButton[tempName].first.left = posX;
		BetweenTurnsButton[tempName].first.top = posY;
		BetweenTurnsButton[tempName].first.width = Width;
		BetweenTurnsButton[tempName].first.height = Height;
		BetweenTurnsButton[tempName].second = &ResourceHandler::getInstance()->getTexture(name);
		button = button->NextSiblingElement();
	}
}



 //initierar fönster/knappar till guielement
void GameManager::initializeGuiElement()
{
	loadButtonPosition();
	loadWindowPosition();
	mFirstDecideWhoStartWindow			= GUIWindow::create(BetweenTurnsWindow["BetweenTurnsSameSpy"]);
	mCloseFirstWindow					= GUIButton::create(BetweenTurnsButton["FirstWindowOkay"], mFirstDecideWhoStartWindow);
	mFirstCapitalistSpyNetworkText		= GUIText::create(sf::FloatRect(430, 390, 40, 40), "0", mFirstDecideWhoStartWindow);
	mFirstCapitalistSpyNetworkText->setScale(0.4, 0.4);
	mFirstCommunistSpyNetworkText		= GUIText::create(sf::FloatRect(540, 390, 40, 40), "0", mFirstDecideWhoStartWindow);
	mFirstCommunistSpyNetworkText->setScale(0.4, 0.4);
	mCapitalistHeadline[0]				= GUIText::create(sf::FloatRect(430, 360, 40, 40), "Capitalist", mFirstDecideWhoStartWindow);
	mCapitalistHeadline[0]->setScale(0.5, 0.5);
	mCommunistHeadline[0]				= GUIText::create(sf::FloatRect(540, 360, 40, 40), "Communist", mFirstDecideWhoStartWindow);
	mCommunistHeadline[0]->setScale(0.5, 0.5);
	mFirstDecideWhoStartWindow->setVisible(false);

	mSecondDecideWhoStartWindow			= GUIWindow::create(BetweenTurnsWindow["BetweenTurnsDiffSpy"]);
	mCapitalistButton					= GUIButton::create(BetweenTurnsButton["Capitalist"], mSecondDecideWhoStartWindow);
	mCommunistButton					= GUIButton::create(BetweenTurnsButton["Communist"], mSecondDecideWhoStartWindow);
	mSecondCapitalistSpyNetworkText		= GUIText::create(sf::FloatRect(430, 390, 40, 40), "0", mSecondDecideWhoStartWindow);
	mSecondCapitalistSpyNetworkText->setScale(0.4, 0.4);
    mSecondCommunistSpyNetworkText		= GUIText::create(sf::FloatRect(540, 390, 40, 40), "0", mSecondDecideWhoStartWindow);
	mSecondCommunistSpyNetworkText->setScale(0.4, 0.4);
	mCapitalistHeadline[1]				= GUIText::create(sf::FloatRect(430, 360, 40, 40), "Capitalist", mSecondDecideWhoStartWindow);
	mCapitalistHeadline[1]->setScale(0.5, 0.5);
	mCommunistHeadline[1]				= GUIText::create(sf::FloatRect(540, 360, 40, 40), "Communist", mSecondDecideWhoStartWindow);
	mCommunistHeadline[1]->setScale(0.5, 0.5);
	mSecondDecideWhoStartWindow->setVisible(false);

	mStatsWindow[0]						= GUIWindow::create(BetweenTurnsWindow["Stats"]);     //icke inzoomat, tidsbaserad inzoomning på detta sm leder till "mStatsWindow[1]"
	mStatsWindow[1]						= GUIWindow::create(BetweenTurnsWindow["ShowStats"]); //inzoomat, visar stats
	mCloseStatsWindow					= GUIButton::create(BetweenTurnsButton["CloseStats"], mStatsWindow[1]);

	mStatsWindow[0]->setVisible(false);
	mStatsWindow[1]->setVisible(false);

	GUIManager::getInstance()->addGUIElement(mFirstDecideWhoStartWindow);
	GUIManager::getInstance()->addGUIElement(mSecondDecideWhoStartWindow);
	GUIManager::getInstance()->addGUIElement(mStatsWindow[0]);
	GUIManager::getInstance()->addGUIElement(mStatsWindow[1]);
}

std::shared_ptr<GUIWindow> GameManager::getStatsWindow()
{
	return mStatsWindow[1];
}

 //initiering av gui knappar
void GameManager::initializeGuiFunctions()
{
	//stänger ned fönstret (om bägge har samma spy)
	mCloseFirstWindow->setOnClickFunction([=]()
	{
		mFirstDecideWhoStartWindow->setVisible(false);
		startRound();
	});
	//Kapitalisterna börjar nästa runda
	mCapitalistButton->setOnClickFunction([=]()
	{
		setCurrentPlayer(getCapitalist());
		mSecondDecideWhoStartWindow->setVisible(false);
		startRound();
	});
	//Kommunisterna börjar nästa runda
	mCommunistButton->setOnClickFunction([=]()
	{
		setCurrentPlayer(getCommunist());
		mSecondDecideWhoStartWindow->setVisible(false);
		startRound();
	});
}

void GameManager::searchForServers()
{
	mUdpClient->setReceivingAddress(sf::IpAddress::Broadcast.toString());
	sf::Packet packet;
	packet<<sf::IpAddress::getLocalAddress().toString()<<mUdpClient->getPort();
	mUdpClient->triggerServerEvent("clientSearchingForServers", packet);
	if(mCreateServerTimer == nullptr)
	{
		mCreateServerTimer = Timer::setTimer([=]()
		{
			createServer();
		}, 5000, 1);
	}
}

void GameManager::createServer()
{
	if(mUdpServer == nullptr)
	{
		Menu::getInstance()->mWaitingForClientText->setText("Waiting for players to connect...");
		std::cout<<"No server found... creating server"<<std::endl;
		mUdpClient->setReceivingAddress(sf::IpAddress::Broadcast.toString());
		mUdpServer = new sf::UdpServer(55005);
		mRole = SERVER;
		mServerState = WAITING;
		Event::addEventHandler("heartBeat", [=](sf::Packet packet)
		{
			//char msg[1024];
			//packet>>msg;
			//std::cout<<"client: "<<msg<<std::endl;
		});
		Timer::setTimer([=]()
		{
			sf::Packet packet;
			packet<<"aleksi";
			mUdpClient->triggerServerEvent("heartBeat", packet);
		}, 1000, 0);
		Event::addEventHandler("clientSearchingForServers", 
			[=](sf::Packet packet)
		{
			if(mServerState == ServerState::WAITING)
			{
				char ipAddress[1024];
				unsigned short port;
				packet>>ipAddress>>port;
				std::cout<<"client searching for server: "<<ipAddress<<":"<<port<<std::endl;
				sf::Packet _packet;
				_packet<<sf::IpAddress::getLocalAddress().toString()<<mUdpServer->getPort();
				mUdpServer->triggerClientEvent("hereIam", _packet, sf::IpAddress(ipAddress), port);
			}
		});
		Event::addEventHandler("connectToServer", 
			[=](sf::Packet packet)
		{
			mServerState = ServerState::FULL;
			char ipAddress[1024];
			unsigned short port;
			packet>>ipAddress>>port;
			std::cout<<"connected to server: "<<ipAddress<<" "<<port<<std::endl;
			sf::Packet _packet;
			_packet<<sf::IpAddress::getLocalAddress().toString()<<mUdpServer->getPort();
			mRemoteIpAddress = ipAddress;
			mRemotePort = port;
			mUdpServer->triggerClientEvent("onClientConnected", _packet, sf::IpAddress(ipAddress), port);
			mPlayersTurn = 0;
			mGameType = GameType::LAN;
			Menu::getInstance()->startGame();
		});
	}
}

void GameManager::connectToServer(std::string ipAdress, unsigned short port)
{
	mUdpClient->setReceivingAddress(ipAdress);
	sf::Packet packet;
	packet<<sf::IpAddress::getLocalAddress().toString()<<mUdpClient->getPort();
	mUdpClient->triggerServerEvent("connectToServer", packet);
	mCreateServerTimer->killTimer();
}

void GameManager::tick(sf::RenderWindow &window)
{
	sf::Vector2i mousePos = sf::Mouse::getPosition(window);
	if(mGameType == GameType::LAN)
	{
		sf::Packet packet;
		packet<<mousePos.x<<mousePos.y;
		if(mRole == CLIENT)
			mUdpClient->triggerServerEvent("syncMousePosition", packet);
		else if(mRole == SERVER)
			mUdpServer->triggerClientEvent("syncMousePosition", packet, mRemoteIpAddress, mRemotePort);

		sf::Sprite remoteCursor(cursorTexture);
		sf::Vector2i remoteMousePos = mRemoteClient->getMousePosition();
		remoteCursor.setPosition(remoteMousePos.x, remoteMousePos.y);

		if(mPlayersTurn == 0 && mRole == CLIENT || mPlayersTurn == 1 && mRole == SERVER)
			window.draw(remoteCursor);
	}
	cursor.setPosition(mousePos.x, mousePos.y);

	if(mPlayersTurn == 0 && mRole == SERVER || mPlayersTurn == 1 && mRole == CLIENT || mGameType == VERSUS)
		window.draw(cursor);
}

void GameManager::update(sf::Event &event)
{
	if(event.type == sf::Event::MouseButtonPressed && event.key.code == sf::Mouse::Left)
		cursor.setTexture(cursorClickedTexture);
	else if(event.type == sf::Event::MouseButtonReleased && event.key.code == sf::Mouse::Left)
		cursor.setTexture(cursorTexture);
}

GameType GameManager::getGameType()
{
	return mGameType;
}

void GameManager::syncGUIClick(std::shared_ptr<GUIElement> guiElement)
{
	sf::Packet packet;
	packet<<guiElement->getElementID();
	if(mRole == CLIENT)
		mUdpClient->triggerServerEvent("syncGUIClick", packet);
	if(mRole == SERVER)
		mUdpServer->triggerClientEvent("syncGUIClick", packet, mRemoteIpAddress, mRemotePort);
}

void GameManager::syncGUIMouseEnter(std::shared_ptr<GUIElement> guiElement)
{
	sf::Packet packet;
	packet<<guiElement->getElementID();
	if(mRole == CLIENT)
		mUdpClient->triggerServerEvent("syncGUIMouseEnter", packet);
	if(mRole == SERVER)
		mUdpServer->triggerClientEvent("syncGUIMouseEnter", packet, mRemoteIpAddress, mRemotePort);
}

void GameManager::syncGUIMouseLeave(std::shared_ptr<GUIElement> guiElement)
{
	sf::Packet packet;
	packet<<guiElement->getElementID();
	if(mRole == CLIENT)
		mUdpClient->triggerServerEvent("syncGUIMouseLeave", packet);
	if(mRole == SERVER)
		mUdpServer->triggerClientEvent("syncGUIMouseLeave", packet, mRemoteIpAddress, mRemotePort);
}
