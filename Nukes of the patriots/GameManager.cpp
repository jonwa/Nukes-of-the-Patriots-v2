#include "GameManager.h"
#include "GUIManager.h"
#include "GUIElement.h"
#include "GUIEditField.h"
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
	mPlayersTurn(0),
	mReady(false)
{
	cursorTexture.loadFromFile("Images/Mouse/MouseCursor.png");
	cursorClickedTexture.loadFromFile("Images/Mouse/MouseCursorClicked.png");
	cursor.setTexture(cursorTexture);

	mUdpClient = new sf::UdpClient(55001, 55005, sf::IpAddress::Broadcast);

	Event::addEventHandler("onPlayerConnected",
		[=](sf::Packet packet)
	{
		mServerState = ServerState::FULL;
		char ipAddress[1024];
		unsigned short port;
		packet>>ipAddress>>port;
		std::cout<<"connected to server: "<<ipAddress<<" "<<port<<std::endl;
		mRemoteIpAddress = ipAddress;
		mRemotePort = 55001;
		mPlayersTurn = 0;
		mGameType = GameType::LAN;
		Menu::getInstance()->startGame();
	});

	Event::addEventHandler("onPlayerDisconnected",
		[=](sf::Packet packet)
	{
		std::cout<<"Player disconnected from my server!"<<std::endl;
	});

	Event::addEventHandler("onClientPlayerConnected",
		[=](sf::Packet packet)
	{
		char ipAddress[1024];
		unsigned short port;
		packet>>ipAddress>>port;
		std::cout<<"connected to server: "<<ipAddress<<" "<<port<<std::endl;
		mRemoteIpAddress = ipAddress;
		mRemotePort = 55005;
		mPlayersTurn = 0;
		mGameType = GameType::LAN;
		Menu::getInstance()->startGame();
	});
	
	mConnectToServerEvent = Event::addEventHandler("hereIam", 
		[=](sf::Packet packet)
	{
		if(mTcpClient == nullptr)
		{
			char ipAddress[1024];
			unsigned short port;
			packet>>ipAddress>>port;
			connectToServer(ipAddress, port);
		}
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
		std::shared_ptr<GUIElement> guiElement = GUIManager::getInstance()->getElementByID(id);
		if(guiElement != NULL)
		{
			std::cout<<"gui element != null"<<std::endl;
			std::function<void()> func = guiElement->getOnClickFunction();
			if(func != NULL)
			{
				std::cout<<"calling gui click func"<<std::endl;
				func();
			}
		}
	});

	Event::addEventHandler("syncGUIMouseEnter",
		[=](sf::Packet packet)
	{
		int id = 0;
		packet>>id;
		std::shared_ptr<GUIElement> guiElement = GUIManager::getInstance()->getElementByID(id);
		if(guiElement != NULL)
		{
			std::function<void()> func = guiElement->getMouseEnterFunction();
			if(func != NULL)
			{
				func();
			}
		}
	});

	Event::addEventHandler("syncGUIMouseLeave",
		[=](sf::Packet packet)
	{
		int id = 0;
		packet>>id;
		std::shared_ptr<GUIElement> guiElement = GUIManager::getInstance()->getElementByID(id);
		if(guiElement != NULL)
		{
			std::function<void()> func = guiElement->getMouseLeaveFunction();
			if(func != NULL)
			{
				func();
			}
		}
	});

	Event::addEventHandler("syncGUIChange",
		[=](sf::Packet packet)
	{
		int id = 0;
		packet>>id;
		std::shared_ptr<GUIElement> guiElement = GUIManager::getInstance()->getElementByID(id);
		if(guiElement != NULL)
		{
			std::function<void()> func = guiElement->getOnGuiChangeFunction();
			if(func != NULL)
			{
				func();
			}
		}
	});

	Event::addEventHandler("syncGUIEditField",
		[=](sf::Packet packet)
	{
		int id = 0;
		char str[1024];
		packet>>id>>str;
		std::shared_ptr<GUIElement> guiElement = GUIManager::getInstance()->getElementByID(id);
		if(guiElement != nullptr)
		{
			guiElement->setText(str);
		}
	});

	Event::addEventHandler("syncRandomFirstPlayerToPlay",
		[=](sf::Packet packet)
	{
		int random = 0;
		packet>>random;
		mCurrentPlayer = mVecSuperPowers[random];
		for(std::vector<std::shared_ptr<SuperPower> >::iterator it = mVecPlayersLeft.begin(); it != mVecPlayersLeft.end(); it++)
		{
			if((*it) == mCurrentPlayer)
			{
				mVecPlayersLeft.erase(it);
				break;
			}
		}
		mReady = true;
		mCurrentPlayer->setRound(1);
		mCurrentPlayer->showGUI();
		if(mRemoteClient->getSuperPower() == random)
			setEnemyTurn();
		else
			setMyTurn();
	});

	Event::addEventHandler("syncRandomPresident",
		[=](sf::Packet packet)
	{
		char name[1024];
		char firstPositiveStat[1024];
		char secondPositiveStat[1024];
		char negativeStat[1024];
		float foodPriceModifier = 0, goodsPriceModifier = 0, techPriceModifier = 0;
		float nuclearPriceModifier = 0, spacePriceModifier = 0, spyPriceModifier = 0;
		float patriotismTaxModifier = 0, popEatsMore = 0;

		char name2[1024];
		char firstPositiveStat2[1024];
		char secondPositiveStat2[1024];
		char negativeStat2[1024];
		float foodPriceModifier2 = 0, goodsPriceModifier2 = 0, techPriceModifier2 = 0;
		float nuclearPriceModifier2 = 0, spacePriceModifier2 = 0, spyPriceModifier2 = 0;
		float patriotismTaxModifier2 = 0, popEatsMore2 = 0;

		packet>>name>>firstPositiveStat>>secondPositiveStat>>negativeStat
			>>foodPriceModifier>>goodsPriceModifier>>techPriceModifier
			>>nuclearPriceModifier>>spacePriceModifier>>spyPriceModifier
			>>patriotismTaxModifier>>popEatsMore;

		packet>>name2>>firstPositiveStat2>>secondPositiveStat2>>negativeStat2
			>>foodPriceModifier2>>goodsPriceModifier2>>techPriceModifier2
			>>nuclearPriceModifier2>>spacePriceModifier2>>spyPriceModifier2
			>>patriotismTaxModifier2>>popEatsMore2;

		std::shared_ptr<President> firstPresident = getPresidentByName(name);
		if(firstPresident != nullptr)
		{
			std::cout<<"First president found!"<<std::endl;
			firstPresident->setFirstPositiveStat(firstPositiveStat);
			firstPresident->setSecondPositiveStat(secondPositiveStat);
			firstPresident->setNegativeStat(negativeStat);
			firstPresident->setFoodPriceModifier(foodPriceModifier);
			firstPresident->setGoodsPriceModifier(goodsPriceModifier);
			firstPresident->setTechPriceModifier(techPriceModifier);
			firstPresident->setNuclearPriceModifier(nuclearPriceModifier);
			firstPresident->setSpacePriceModifier(spacePriceModifier);
			firstPresident->setSpyPriceModifier(spyPriceModifier);
			firstPresident->setPatriotismTaxModifier(patriotismTaxModifier);
			firstPresident->setPopEatsMore(popEatsMore);
		}

		std::shared_ptr<President> secondPresident = getPresidentByName(name2);
		if(secondPresident != nullptr)
		{
			std::cout<<"Second president found!"<<std::endl;
			secondPresident->setFirstPositiveStat(firstPositiveStat2);
			secondPresident->setSecondPositiveStat(secondPositiveStat2);
			secondPresident->setNegativeStat(negativeStat2);
			secondPresident->setFoodPriceModifier(foodPriceModifier2);
			secondPresident->setGoodsPriceModifier(goodsPriceModifier2);
			secondPresident->setTechPriceModifier(techPriceModifier2);
			secondPresident->setNuclearPriceModifier(nuclearPriceModifier2);
			secondPresident->setSpacePriceModifier(spacePriceModifier2);
			secondPresident->setSpyPriceModifier(spyPriceModifier2);
			secondPresident->setPatriotismTaxModifier(patriotismTaxModifier2);
			secondPresident->setPopEatsMore(popEatsMore2);
		}

		if(firstPresident != nullptr && secondPresident != nullptr)
			getCap()->LANChooseLeader(firstPresident, secondPresident);
	});

	Event::addEventHandler("nextPlayerToChooseTeam",
		[=](sf::Packet packet)
	{
		int type = 0;
		packet>>type;
		mRemoteClient->setSuperPower(type);
	});

	Event::addEventHandler("syncPlayersTurn",
		[=](sf::Packet packet)
	{
		int playersTurn = 0;
		packet>>playersTurn;
		mPlayersTurn = playersTurn;

	});

	Event::addEventHandler("loadingCompleted",
		[=](sf::Packet packet)
	{
		mRemoteClient->setReady(true);
		if(mReady && mRemoteClient->getSuperPower() == COMMUNIST)
			getCap()->sendPresidentDataToOtherPlayer();
		mRemoteClient->setReady(false);
	});

	Event::addEventHandler("syncReadyState",
		[=](sf::Packet packet)
	{
		int ready = 0;
		packet>>ready;
		mRemoteClient->setReady(ready == 1 ? true : false);
	});

	Event::addEventHandler("syncRandomPlayerNextRound",
		[=](sf::Packet packet)
	{
		int randomPlayer = 0;
		packet>>randomPlayer;
		if(randomPlayer == 0 && mRemoteClient->getSuperPower() == CAPITALIST || randomPlayer == 1 && mRemoteClient->getSuperPower() == COMMUNIST)
			setEnemyTurn();
		else if(randomPlayer == 0 && mRemoteClient->getSuperPower() == COMMUNIST || randomPlayer == 1 && mRemoteClient->getSuperPower() == CAPITALIST)
			setMyTurn();
		//If both player has same spy network, then select random as next player directly
		std::vector<std::shared_ptr<SuperPower>> _nextPlayers = mVecPlayersLeft;
		//std::function<void(std::shared_ptr<SuperPower>)> _selectStartingPlayer = selectStartingPlayer;
		//mCloseStatsWindow->setOnClickFunction([=]()
		//{
		//	if(_nextPlayers.size() == 1)
		//	{
		//		_selectStartingPlayer(_nextPlayers[randomPlayer]);
		//	}
		//	else
		//	{
		//		setCurrentPlayer(nextPlayers[randomPlayer]); // Need to set setCurrentPlayer to update player round
		//		mFirstDecideWhoStartWindow->setVisible(true);
		//		mStatsWindow[1]->setVisible(false);
		//		//mNextWindowToShow = mFirstDecideWhoStartWindow;
		//		mFirstDecideWhoStartWindow->setEnabled(true, true);
		//		mFirstCapitalistSpyNetworkText->setText("Spy network: " + intToString(getCapitalist()->getSpyNetwork()));
		//		mFirstCommunistSpyNetworkText->setText("Spy network: " + intToString(getCommunist()->getSpyNetwork()));
		//	} 
		//});
	});

	Event::addEventHandler("statsWindowReady",
		[=](sf::Packet packet)
	{
		mRemoteClient->setReady(true);
	});

	Event::addEventHandler("syncRandomStartingPlayer", 
		[=](sf::Packet packet)
	{
		int player = 0;
		packet>>player;
		setRandomPlayer(mVecSuperPowers[player]);
	});

	initializeGuiElement();
	initializeGuiFunctions();
}

GameManager::~GameManager()
{
	
}

void GameManager::setDocumentName(std::string fileName)
{
	//if filename does not already exist, save game in the given filename
	if(std::find(mSaveFiles.begin(), mSaveFiles.end(), fileName) == mSaveFiles.end())
	{
		std::string temp = ("savedFiles/" + fileName + ".xml");
		mFileName = temp;
		saveGame(); //saving the game
		mSaveFiles.push_back(fileName);
	}
	// else choose if you want to overwrite the existing file or if you want to rename the file
	else 
	{
		Menu::getInstance()->getWindows("SaveSuccessful")->setVisible(false);
		GUIManager::getInstance()->setOnTop(mUnableToSaveWindow);
		mUnableToSaveWindow->setVisible(true);
	}
}

void GameManager::overwriteCurrentDocument(std::string fileName)
{
		std::string temp = ("savedFiles/" + fileName + ".xml");
		mFileName = temp;
		saveGame(); //saving the game
}

GameManager::SaveFilesVec& GameManager::getSaveFilesVec()
{
	return mSaveFiles;
}

std::string GameManager::getDocumentName() const
{
	return mFileName;
}

void GameManager::saveGame()
{
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement *gameManager = doc.NewElement("GameManager");
	
	//saving year
	tinyxml2::XMLElement *year = doc.NewElement("Year");
	year->SetAttribute("value", mYear);
	gameManager->InsertEndChild(year);

	//saving round
	tinyxml2::XMLElement *round = doc.NewElement("Round");
	round->SetAttribute("value", mRound);
	gameManager->InsertEndChild(round);

	//saving loaded
	tinyxml2::XMLElement *loaded = doc.NewElement("Loaded");
	loaded->SetAttribute("value", mLoaded);
	gameManager->InsertEndChild(loaded);

	//saving Communist and Capitalist information 
	for(std::vector<std::shared_ptr<SuperPower> >::iterator it = mVecSuperPowers.begin(); it != mVecSuperPowers.end(); it++)
	{
		(*it)->saveGame(doc);
	}

	doc.InsertEndChild(gameManager);
	doc.SaveFile(mFileName.c_str());
}

void GameManager::saveFileName()
{
	tinyxml2::XMLDocument fileNamesDoc;
	tinyxml2::XMLElement *saves = fileNamesDoc.NewElement("Saves");
	for(SaveFilesVec::iterator it = mSaveFiles.begin(); it != mSaveFiles.end(); it++)
	{
		tinyxml2::XMLElement *save = fileNamesDoc.NewElement("Save");
		save->SetAttribute("name", (*it).c_str());
		saves->InsertEndChild(save);
	}
	fileNamesDoc.InsertEndChild(saves);
	fileNamesDoc.SaveFile("savedFiles/savedFileNames.xml");
}

void GameManager::loadGame()
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile(mFileName.c_str());
	tinyxml2::XMLElement *gameManager = doc.FirstChildElement("GameManager");
	
	//loading year
	tinyxml2::XMLElement *year = gameManager->FirstChildElement("Year");
	mYear = atoi(year->Attribute("value"));

	//loading round
	tinyxml2::XMLElement *round = gameManager->FirstChildElement("Round");
	mRound = atoi(round->Attribute("value"));

	//loading loaded
	tinyxml2::XMLElement *loaded = gameManager->FirstChildElement("Loaded");
	mLoaded = atoi(loaded->Attribute("value"));

	for(std::vector<std::shared_ptr<SuperPower> >::iterator it = mVecSuperPowers.begin(); it != mVecSuperPowers.end(); it++)
	{
		(*it)->loadGame(doc);
	}
}

void GameManager::loadFileName()
{
	tinyxml2::XMLDocument fileNamesDoc;
	fileNamesDoc.LoadFile("savedFiles/savedFileNames.xml");
	
	if(fileNamesDoc.Error())
		std::cout << "Fel loadFileName()!";

	tinyxml2::XMLElement *saves = fileNamesDoc.FirstChildElement("Saves");
	for(tinyxml2::XMLElement *save = saves->FirstChildElement("Save"); save; save = save->NextSiblingElement())
	{
		mSaveFiles.push_back(save->Attribute("name"));
	}
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
		mYearText->setText(intToString(mYear));
		mYearText->setVisible(true);
		GUIManager::getInstance()->addGUIElement(mYearText);
		mReady = true;
		if(getGameType() == LAN)
		{
			sf::Packet packet;
			packet<<1;
			triggerOtherPlayersEvent("loadingCompleted", packet);
			if(mRemoteClient->isReady() && mRemoteClient->getSuperPower() == COMMUNIST)
			{
				getCap()->sendPresidentDataToOtherPlayer();
				mRemoteClient->setReady(false);
			}
		}


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
		if(getGameType() == LAN && isMyTurnToPlay())
		{
			int random = Randomizer::getInstance()->randomNr(mVecSuperPowers.size(), 0);
			mCurrentPlayer = mVecSuperPowers[random];
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
			sf::Packet packet;
			packet<<random;
			triggerOtherPlayersEvent("syncRandomFirstPlayerToPlay", packet);
			mCurrentPlayer->setRound(1);
			mCurrentPlayer->showGUI();
			if(mRemoteClient->getSuperPower() == random)
				setEnemyTurn();
			else
				setMyTurn();
		}
		else if(getGameType() == VERSUS)
		{
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
			mCurrentPlayer->setRound(1);
			mCurrentPlayer->showGUI();
		}
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
		mYearText->setVisible(true);
		mCurrentPlayer->setRound(1);
		mCurrentPlayer->showGUI();
	}
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

std::shared_ptr<RemoteClient> GameManager::getRemoteClient()
{
	return mRemoteClient;
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

void GameManager::removePresidentFromList(std::shared_ptr<President> president)
{
	for(std::vector<std::shared_ptr<President> >::iterator it = mPresidentVector.begin(); it != mPresidentVector.end(); ++it)
	{
		if(*it == president)
		{
			mPresidentVector.erase(it);
			break;
		}
	}
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

void GameManager::setRandomPlayer(std::shared_ptr<SuperPower> player)
{
	setCurrentPlayer(player); // Need to set setCurrentPlayer to update player round
	mFirstDecideWhoStartWindow->setVisible(true);
	mStatsWindow[1]->setVisible(false);
	//mNextWindowToShow = mFirstDecideWhoStartWindow;
	mFirstDecideWhoStartWindow->setEnabled(true, true);
	mFirstCapitalistSpyNetworkText->setText("Spy network: " + intToString(getCapitalist()->getSpyNetwork()));
	mFirstCommunistSpyNetworkText->setText("Spy network: " + intToString(getCommunist()->getSpyNetwork()));
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
		if(getGameType() == LAN)
		{
			int randomPlayer = Randomizer::getInstance()->randomNr(nextPlayers.size(), 0);
			if(nextPlayers.size() == 1)
			{
				if(nextPlayers[randomPlayer]->getType() == mRemoteClient->getSuperPower())
					setEnemyTurn();
				else
					setMyTurn();
			}
			//If both player has same spy network, then select random as next player directly
			mCloseStatsWindow->setOnClickFunction([=]()
			{
				if(nextPlayers.size() == 1)
				{
					selectStartingPlayer();
				}
				else
				{
					sf::Packet packet;
					packet<<1;
					mReady = true;
					triggerOtherPlayersEvent("statsWindowReady", packet);
					if(mRemoteClient->isReady())
					{
						if(mRole == SERVER)
						{
							setRandomPlayer(nextPlayers[randomPlayer]);
							sf::Packet _packet;
							_packet<<(nextPlayers[randomPlayer]->getType() == CAPITALIST ? 0 : 1);
							triggerOtherPlayersEvent("syncRandomStartingPlayer", _packet);
						}
					}
				}
				if(isMyTurnToPlay())
					setEnemyTurn();
				else
					setMyTurn();
			});
		}
		else if(getGameType() == VERSUS)
		{
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
		if(getGameType() == LAN)
		{
			if(mCurrentPlayer == getCapitalist())
			{
				if(mRemoteClient->getSuperPower() == CAPITALIST)
					setMyTurn();
				else
					setEnemyTurn();
				setCurrentPlayer(getCommunist());
			}
			else
			{
				if(mRemoteClient->getSuperPower() == COMMUNIST)
					setMyTurn();
				else
					setEnemyTurn();
				setCurrentPlayer(getCapitalist());
			}
			startRound();
		}
		else if(getGameType() == VERSUS)
		{
			int randomPlayer = Randomizer::getInstance()->randomNr(nextPlayers.size(), 0);
			setCurrentPlayer(nextPlayers[randomPlayer]);
			startRound();
		}
	}
	/*Ökar år med ett när rundan är slut*/
	mYearText->setText(mYear);

	if(getCapitalist()->getPatriotism() <= 0)
	{
		mYearText->setVisible(false);
		GUIManager::getInstance()->setOnTop(mWinScreenWindow[1]);
		mWinScreenWindow[1]->setVisible(true);
		mWinningTeamName[1]->setText(Menu::getInstance()->getEditField("CommunistNameField")->getText() + " is Victorius!");
	}
	else if(getCommunist()->getPatriotism() <= 0)
	{
		mYearText->setVisible(false);
		mStatsWindow[0]->setVisible(false);
		mStatsWindow[1]->setVisible(false);
		GUIManager::getInstance()->setOnTop(mWinScreenWindow[0]);
		mWinScreenWindow[0]->setVisible(true);
		mWinningTeamName[0]->setText(Menu::getInstance()->getEditField("CapitalistNameField")->getText() + " is Victorius!");
		mWinningTeamBanners[0]->setTexture(BetweenTurnsButton["CapitalistBanner"]);		
	}
	else if(mYear == 40 && getCommunist()->getPatriotism() < getCapitalist()->getPatriotism())
	{
		mYearText->setVisible(false);
		mStatsWindow[0]->setVisible(false);
		mStatsWindow[1]->setVisible(false);
		GUIManager::getInstance()->setOnTop(mWinScreenWindow[0]);
		mWinScreenWindow[0]->setVisible(true);
		mWinningTeamName[0]->setText(Menu::getInstance()->getEditField("CapitalistNameField")->getText() + " is Victorius!");
		mWinningTeamBanners[0]->setTexture(BetweenTurnsButton["CapitalistBanner"]);		
	}
	else if(mYear == 40 && getCapitalist()->getPatriotism() < getCommunist()->getPatriotism())
	{
		mYearText->setVisible(false);
		mStatsWindow[0]->setVisible(false);
		mStatsWindow[1]->setVisible(false);
		GUIManager::getInstance()->setOnTop(mWinScreenWindow[1]);
		mWinScreenWindow[1]->setVisible(true);
		mWinningTeamName[1]->setText(Menu::getInstance()->getEditField("CommunistNameField")->getText() + " is Victorius!");
		mWinningTeamBanners[1]->setTexture(BetweenTurnsButton["CommunistBanner"]);		
	}
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

	mYearText = GUIText::create(sf::FloatRect(512, 4, 0, 0), intToString(mYear));
	mYearText->setScale(0.6, 0.6);
	mYearText->setAlignment("middle");
	mYearText->setColor(sf::Color::White);
	mYearText->setVisible(false);

	mParentWindow						= GUIWindow::create(BetweenTurnsWindow["ParentWindow"]);
	mFirstDecideWhoStartWindow			= GUIWindow::create(BetweenTurnsWindow["BetweenTurnsSameSpy"], mParentWindow);
	mCloseFirstWindow					= GUIButton::create(BetweenTurnsButton["FirstWindowOkay"], mFirstDecideWhoStartWindow);
	mFirstCapitalistSpyNetworkText		= GUIText::create(sf::FloatRect(430, 390, 40, 40), "0", mFirstDecideWhoStartWindow);
	mFirstCapitalistSpyNetworkText->setScale(0.4, 0.4);
	mFirstCommunistSpyNetworkText		= GUIText::create(sf::FloatRect(540, 390, 40, 40), "0", mFirstDecideWhoStartWindow);
	mFirstCommunistSpyNetworkText->setScale(0.4, 0.4);
	mCapitalistHeadline[0]				= GUIText::create(sf::FloatRect(430, 360, 40, 40),/* Menu::getInstance()->getEditField("CapitalistNameField")->getText()*/"Capitalist", mFirstDecideWhoStartWindow);
	mCapitalistHeadline[0]->setScale(0.5, 0.5);
	mCommunistHeadline[0]				= GUIText::create(sf::FloatRect(540, 360, 40, 40), /*Menu::getInstance()->getEditField("CommunistNameField")->getText()*/"Communist", mFirstDecideWhoStartWindow);
	mCommunistHeadline[0]->setScale(0.5, 0.5);
	mFirstDecideWhoStartWindow->setVisible(false);

	mSecondDecideWhoStartWindow			= GUIWindow::create(BetweenTurnsWindow["BetweenTurnsDiffSpy"], mParentWindow);
	mCapitalistButton					= GUIButton::create(BetweenTurnsButton["Capitalist"], mSecondDecideWhoStartWindow);
	mCommunistButton					= GUIButton::create(BetweenTurnsButton["Communist"], mSecondDecideWhoStartWindow);
	mSecondCapitalistSpyNetworkText		= GUIText::create(sf::FloatRect(430, 390, 40, 40), "0", mSecondDecideWhoStartWindow);
	mSecondCapitalistSpyNetworkText->setScale(0.4, 0.4);
    mSecondCommunistSpyNetworkText		= GUIText::create(sf::FloatRect(540, 390, 40, 40), "0", mSecondDecideWhoStartWindow);
	mSecondCommunistSpyNetworkText->setScale(0.4, 0.4);
	mCapitalistHeadline[1]				= GUIText::create(sf::FloatRect(430, 360, 40, 40),"Capitalist", mSecondDecideWhoStartWindow);
	mCapitalistHeadline[1]->setScale(0.5, 0.5);
	mCommunistHeadline[1]				= GUIText::create(sf::FloatRect(540, 360, 40, 40),"Communist", mSecondDecideWhoStartWindow);
	mCommunistHeadline[1]->setScale(0.5, 0.5);
	mSecondDecideWhoStartWindow->setVisible(false);

	mStatsWindow[0]						= GUIWindow::create(BetweenTurnsWindow["Stats"], mParentWindow);     //icke inzoomat, tidsbaserad inzoomning på detta sm leder till "mStatsWindow[1]"
	mStatsWindow[1]						= GUIWindow::create(BetweenTurnsWindow["ShowStats"], mParentWindow); //inzoomat, visar stats
	mCloseStatsWindow					= GUIButton::create(BetweenTurnsButton["CloseStats"], mStatsWindow[1]);

	mStatsWindow[0]->setVisible(false);
	mStatsWindow[1]->setVisible(false);

	mUnableToSaveWindow					 = GUIWindow::create(BetweenTurnsWindow["UnableToSave"], mParentWindow);
	mCancelSaveButton					 = GUIButton::create(BetweenTurnsButton["CancelSave"], mUnableToSaveWindow);
	mOverWriteButton					 = GUIButton::create(BetweenTurnsButton["Overwrite"], mUnableToSaveWindow);
	mUnableToSaveText					 = GUIText::create(sf::FloatRect(130, 100, 100, 40), "< A saved file with this name already exists >\n< Overwrite? >", mUnableToSaveWindow);
	mUnableToSaveText->setColor(sf::Color::White);
	mUnableToSaveText->setScale(0.7, 0.7);
	mUnableToSaveWindow->setVisible(false);


	mWinScreenWindow[0]						= GUIWindow::create(BetweenTurnsWindow["CapitalistWinScreen"], mParentWindow);
	mWinScreenWindow[1]						= GUIWindow::create(BetweenTurnsWindow["CommunistWinScreen"], mParentWindow);
	mWinningTeamName[0]						= GUIText::create(sf::FloatRect(512, 8, 0, 0), "", mWinScreenWindow[0]);
	mWinningTeamName[1]						= GUIText::create(sf::FloatRect(512, 8, 0, 0), "", mWinScreenWindow[0]);
	mWinningTeamName[0]	->setAlignment("middle");
	mWinningTeamName[0]	->setColor(sf::Color::Black);
	mWinningTeamName[0]	->setScale(2.5, 3);
	mWinningTeamName[0]	->setWidth(700);
	mWinningTeamName[1]	->setAlignment("middle");
	mWinningTeamName[1]	->setColor(sf::Color::White);
	mWinningTeamName[1]	->setScale(2.5, 3);
	mWinningTeamName[1]	->setWidth(700);
	mWinScreenOkayButton[0]					= GUIButton::create(BetweenTurnsButton["WinScreenOkayButton"], mWinScreenWindow[0]);
	mWinScreenOkayButton[1]					= GUIButton::create(BetweenTurnsButton["WinScreenOkayButton"], mWinScreenWindow[1]);
	mWinningTeamBanners[0]					= GUIImage::create(BetweenTurnsButton["CapitalistBanner"], mWinScreenWindow[0]);
	mWinningTeamBanners[1]					= GUIImage::create(BetweenTurnsButton["CommunistBanner"], mWinScreenWindow[1]);

	mWinScreenWindow[0]	->setVisible(false);
	mWinScreenWindow[1]	->setVisible(false);

	GUIManager::getInstance()->addGUIElement(mParentWindow);

	//GUIManager::getInstance()->addGUIElement(mFirstDecideWhoStartWindow);
	//GUIManager::getInstance()->addGUIElement(mSecondDecideWhoStartWindow);
	//GUIManager::getInstance()->addGUIElement(mStatsWindow[0]);
	//GUIManager::getInstance()->addGUIElement(mStatsWindow[1]);
	//GUIManager::getInstance()->addGUIElement(mUnableToSaveWindow);
	//GUIManager::getInstance()->addGUIElement(mWinScreenWindow);

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
	mCancelSaveButton->setOnClickFunction([=]()
	{
		mUnableToSaveWindow->setVisible(false);
		Menu::getInstance()->getWindows("SaveCanceled")->setVisible(true);
	});

	mOverWriteButton->setOnClickFunction([=]()
	{
		overwriteCurrentDocument(Menu::getInstance()->getEditField("SaveFileField")->getText());
		mUnableToSaveWindow->setVisible(false);
		GUIManager::getInstance()->setOnTop(Menu::getInstance()->getWindows("SaveSuccessful"));
		Menu::getInstance()->getWindows("SaveSuccessful")->setVisible(true);
		std::shared_ptr<GUIWindow> _saveWindow = Menu::getInstance()->getWindows("SaveSuccessful");
		std::shared_ptr<GUIWindow> _inGameWindow = Menu::getInstance()->getWindows("InGameMenu");
		Timer::setTimer([=]()
		{
			_saveWindow->setVisible(false);
			_inGameWindow->setEnabled(true, true);

		}, 2000, 1);
	});

	mWinScreenOkayButton[0]->setOnClickFunction([=]()
	{
		reset();
		Menu::getInstance()->reset();
		mWinScreenWindow[0]->setVisible(false);
		mParentWindow->setVisible(false);
		Menu::getInstance()->getWindows("ParentWindow")->setVisible(true);
		GUIManager::getInstance()->setOnTop(Menu::getInstance()->getWindows("MainMenu"));
		Menu::getInstance()->getWindows("MainMenu")->setVisible(true);
	});

	mWinScreenOkayButton[1]->setOnClickFunction([=]()
	{
		reset();
		Menu::getInstance()->reset();
		mWinScreenWindow[1]->setVisible(false);
		mParentWindow->setVisible(false);
		Menu::getInstance()->getWindows("ParentWindow")->setVisible(true);
		GUIManager::getInstance()->setOnTop(Menu::getInstance()->getWindows("MainMenu"));
		Menu::getInstance()->getWindows("MainMenu")->setVisible(true);
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
		mTcpServer = new sf::TcpServer(55006);
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
				_packet<<sf::IpAddress::getLocalAddress().toString()<<mTcpServer->getPort();
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
			_packet<<sf::IpAddress::getLocalAddress().toString()<<mTcpServer->getPort();
			mRemoteIpAddress = ipAddress;
			mRemotePort = port;
			mTcpServer->triggerClientEvent("onClientConnected", _packet, sf::IpAddress(ipAddress), port);
			mPlayersTurn = 0;
			mGameType = GameType::LAN;
			Menu::getInstance()->startGame();
		});
	}
}

void GameManager::connectToServer(std::string ipAdress, unsigned short port)
{
	mUdpClient->setReceivingAddress(ipAdress);
	/*
	sf::Packet packet;
	packet<<sf::IpAddress::getLocalAddress().toString()<<mUdpClient->getPort();
	mUdpClient->triggerServerEvent("connectToServer", packet);
	*/
	mCreateServerTimer->killTimer();
	mTcpClient = new sf::TcpClient(port, sf::IpAddress(ipAdress));
}

void GameManager::tick(sf::RenderWindow &window)
{
	sf::Vector2i mousePos = sf::Mouse::getPosition(window);
	if(mGameType == GameType::LAN && mUdpClient != nullptr)
	{
		sf::Packet packet;
		packet<<mousePos.x<<mousePos.y;
		triggerOtherPlayersEvent("syncMousePosition", packet, "UDP");

		sf::Sprite remoteCursor(cursorTexture);
		sf::Vector2i remoteMousePos = mRemoteClient->getMousePosition();
		remoteCursor.setPosition(remoteMousePos.x, remoteMousePos.y);

		if(!isMyTurnToPlay())
			window.draw(remoteCursor);
	}
	cursor.setPosition(mousePos.x, mousePos.y);

	if(isMyTurnToPlay())
		window.draw(cursor);
		
	if(mTcpClient != nullptr)
		mTcpClient->tick();
	if(mTcpServer != nullptr)
		mTcpServer->tick();
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
	triggerOtherPlayersEvent("syncGUIClick", packet);
}

void GameManager::syncGUIMouseEnter(std::shared_ptr<GUIElement> guiElement)
{
	sf::Packet packet;
	packet<<guiElement->getElementID();
	triggerOtherPlayersEvent("syncGUIMouseEnter", packet);
}

void GameManager::syncGUIMouseLeave(std::shared_ptr<GUIElement> guiElement)
{
	sf::Packet packet;
	packet<<guiElement->getElementID();
	triggerOtherPlayersEvent("syncGUIMouseLeave", packet);
}

void GameManager::syncGUIChange(std::shared_ptr<GUIElement> guiElement)
{
	sf::Packet packet;
	packet<<guiElement->getElementID();
	triggerOtherPlayersEvent("syncGUIChange", packet);
}

void GameManager::syncGUIEditField(std::shared_ptr<GUIElement> guiElement)
{
	sf::Packet packet;
	packet<<guiElement->getElementID()<<guiElement->getText();
	triggerOtherPlayersEvent("syncGUIEditField", packet);
}

bool GameManager::isMyTurnToPlay()
{
	if(mPlayersTurn == 0 && mRole == SERVER || mPlayersTurn == 1 && mRole == CLIENT || mGameType == VERSUS)
		return true;
	return false;
}

void GameManager::triggerOtherPlayersEvent(std::string eventName, sf::Packet &packet, std::string type)
{
	if(type == "UDP")
	{
		if(mRole == CLIENT)
			mUdpClient->triggerServerEvent(eventName, packet);
		else if(mRole == SERVER)
			mUdpServer->triggerClientEvent(eventName, packet, sf::IpAddress(mRemoteIpAddress), mRemotePort);
	}
	else
	{
		if(mRole == CLIENT)
			mTcpClient->triggerServerEvent(eventName, packet);
		else if(mRole == SERVER)
			mTcpServer->triggerClientEvent(eventName, packet, sf::IpAddress(mRemoteIpAddress), mRemotePort);
	}
}

std::shared_ptr<President> GameManager::getPresidentByName(std::string name)
{
	for(std::vector<std::shared_ptr<President> >::iterator it = mPresidentVector.begin(); it != mPresidentVector.end(); ++it)
	{
		if(std::strcmp((*it)->getName().c_str(), name.c_str()) == 0)
			return *it;
	}
	return nullptr;
}

void GameManager::nextPlayersTurn()
{
	std::cout<<"setting next players turn..."<<std::endl;
	setEnemyTurn();
	sf::Packet packet;
	packet<<mPlayersTurn;
	triggerOtherPlayersEvent("syncPlayersTurn", packet);
}

void GameManager::setEnemyTurn()
{
	if(mRole == SERVER)
		mPlayersTurn = 1;
	else if(mRole == CLIENT)
		mPlayersTurn = 0;
}

void GameManager::setMyTurn()
{
	if(mRole == SERVER)
		mPlayersTurn = 0;
	else if(mRole == CLIENT)
		mPlayersTurn = 1;

}



// Sync packet sending - wait until packet has been sent to send next packet

