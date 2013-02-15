#include "GameManager.h"
#include "GUIManager.h"
#include "Randomizer.h"
#include "Capitalist.h"
#include "Communist.h"
#include "SuperPower.h"
#include "President.h"
#include "tinyxml2.h"
#include "GUIText.h"
#include "ResourceHandler.h"

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
		mVecPlayersLeft(),
		mRound(0)
{
	initializeGuiElement();
	initializeGuiFunctions();
}


GameManager::~GameManager()
{}

void GameManager::init(int year)
{
	getInstance()->setYear(year);
	loadPresidents();
	mVecSuperPowers.push_back(std::make_shared<Capitalist>());
	mVecSuperPowers.push_back(std::make_shared<Communist>());
	mVecPlayersLeft = mVecSuperPowers;

	/*Skriver ut �ret p� interface*/
	mYearText = GUIText::create(sf::FloatRect(512, 15, 40, 40), intToString(mYear));
	mYearText->setScale(0.5, 0.5);
	GUIManager::getInstance()->addGUIElement(mYearText);
	
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
	mCurrentPlayer->setRound(1);
	mCurrentPlayer->showGUI();
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
				mPresidentVector.push_back(std::make_shared<President>(key));
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
				mGeneralVector.push_back(std::make_shared<President>(key));
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

void GameManager::addSuperPower(std::shared_ptr<SuperPower> power)
{
	mVecSuperPowers.push_back(power);
}

void GameManager::startRound()
{
	int moneyIntFood = 0;
	int moneyIntGoods = 0;
	int moneyIntTech = 0;
	// Money internationally should be equal to everybodies money together
	for(std::vector<std::shared_ptr<SuperPower> >::iterator it = mVecSuperPowers.begin(); it != mVecSuperPowers.end(); it++)
	{
		moneyIntFood += (*it)->getCurrency();
		moneyIntGoods += (*it)->getCurrency();
		moneyIntTech += (*it)->getCurrency();
	}
	int foodBought = 0;
	int goodsBought = 0;
	int techBought = 0;
	int exports = 0;

	// if nobody bought my exports - then it will be sold internationally
	//foodBought = moneyIntFood / mCurrentPlayer->getExportedFoodPrice();
	//goodsBought = moneyIntGoods / mCurrentPlayer->getExportedGoodsPrice();
	//techBought = moneyIntTech / mCurrentPlayer->getExportedTechPrice();
	//// if international market tries to buy more resources than you have
	//if(foodBought > mCurrentPlayer->getExportedFood()) foodBought = mCurrentPlayer->getExportedFood();
	//if(goodsBought > mCurrentPlayer->getExportedGoods()) goodsBought = mCurrentPlayer->getExportedGoods();
	//if(techBought > mCurrentPlayer->getExportedTech()) techBought = mCurrentPlayer->getExportedTech();
	//exports += (foodBought * mCurrentPlayer->getExportedFoodPrice()) + (goodsBought * mCurrentPlayer->getExportedGoodsPrice()) + (techBought * mCurrentPlayer->getExportedTechPrice());
	//mCurrentPlayer->setExportedFood(mCurrentPlayer->getExportedFood() - foodBought);
	//mCurrentPlayer->setExportedGoods(mCurrentPlayer->getExportedGoods() - goodsBought);
	//mCurrentPlayer->setExportedTech(mCurrentPlayer->getExportedTech() - techBought);
	//mCurrentPlayer->getTaxIncome();
	//mCurrentPlayer->setCurrency(mCurrentPlayer->getCurrency() + exports);
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
	std::cout<<"players left to play before: "<<mVecPlayersLeft.size()<<std::endl;
	for(std::vector<std::shared_ptr<SuperPower> >::iterator it = mVecPlayersLeft.begin(); it != mVecPlayersLeft.end(); it++)
	{
		if((*it) == mCurrentPlayer)
		{
			mVecPlayersLeft.erase(it);
			break;
		}
	}
	std::cout<<"players left to play after: "<<mVecPlayersLeft.size()<<std::endl;
	//mCurrentPlayer->playMusic(); // ta bort senare, mest f�r att testa
	mCurrentPlayer->setRound(mCurrentPlayer->getRound() + 1);
}

void GameManager::selectStartingPlayer(std::shared_ptr<SuperPower> startingPlayer)
{
	mSecondDecideWhoStartWindow->setVisible(true);
	//mNextWindowToShow = mSecondDecideWhoStartWindow;
	mSecondDecideWhoStartWindow->setEnabled(false, true);
	mSecondCapitalistSpyNetworkText->setText(intToString(getCapitalist()->getSpyNetwork()));
	mSecondCommunistSpyNetworkText->setText(intToString(getCommunist()->getSpyNetwork()));
}

void GameManager::setYear(int year)
{
	mYear = year; 
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
		// S�tter statsf�nstret till true n�r ett �r har g�tt
		mStatsWindow->setVisible(true); 

		int randomPlayer = Randomizer::getInstance()->randomNr(nextPlayers.size(), 0);
		//If both player has same spy network, then select random as next player directly
		if(nextPlayers.size() == 1)
		{
			selectStartingPlayer(nextPlayers[randomPlayer]);
		}
		else
		{
			setCurrentPlayer(nextPlayers[randomPlayer]); // Need to set setCurrentPlayer to update player round
			mFirstDecideWhoStartWindow->setVisible(true);
			//mNextWindowToShow = mFirstDecideWhoStartWindow;
			mFirstDecideWhoStartWindow->setEnabled(false, true);
			mFirstCapitalistSpyNetworkText->setText(intToString(getCapitalist()->getSpyNetwork()));
			mFirstCommunistSpyNetworkText->setText(intToString(getCommunist()->getSpyNetwork()));
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
		int randomPlayer = Randomizer::getInstance()->randomNr(nextPlayers.size(), 0);
		setCurrentPlayer(nextPlayers[randomPlayer]);
		startRound();
	}
	/*�kar �r med ett n�r rundan �r slut*/
	mYearText->setText(mYear);
}

 //laddar in f�nster
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

std::shared_ptr<SuperPower> GameManager::getCapitalist()
{
	return mVecSuperPowers[0];
}

std::shared_ptr<SuperPower> GameManager::getCommunist()
{
	return mVecSuperPowers[1];
}



 //initierar f�nster/knappar till guielement
void GameManager::initializeGuiElement()
{
	loadButtonPosition();
	loadWindowPosition();

	mFirstDecideWhoStartWindow			= GUIWindow::create(BetweenTurnsWindow["BetweenTurnsSameSpy"]);
	mCloseFirstWindow					= GUIButton::create(BetweenTurnsButton["FirstWindowOkay"], mFirstDecideWhoStartWindow);
	mFirstCapitalistSpyNetworkText		= GUIText::create(sf::FloatRect(500, 120, 40, 40), "0", mFirstDecideWhoStartWindow);
	mFirstCommunistSpyNetworkText		= GUIText::create(sf::FloatRect(500, 170, 40, 40), "0", mFirstDecideWhoStartWindow);
	mFirstDecideWhoStartWindow->setVisible(false);

	mSecondDecideWhoStartWindow			= GUIWindow::create(BetweenTurnsWindow["BetweenTurnsDiffSpy"]);
	mCapitalistButton					= GUIButton::create(BetweenTurnsButton["Capitalist"], mSecondDecideWhoStartWindow);
	mCommunistButton					= GUIButton::create(BetweenTurnsButton["Communist"], mSecondDecideWhoStartWindow);
	mSecondCapitalistSpyNetworkText		= GUIText::create(sf::FloatRect(500, 120, 40, 40), "0", mSecondDecideWhoStartWindow);
    mSecondCommunistSpyNetworkText		= GUIText::create(sf::FloatRect(500, 170, 40, 40), "0", mSecondDecideWhoStartWindow);
	mSecondDecideWhoStartWindow->setVisible(false);

	mStatsWindow						= GUIWindow::create(BetweenTurnsWindow["Stats"]);
	mCloseStatsWindow					= GUIButton::create(BetweenTurnsButton["CloseStats"], mStatsWindow);
	mStatsWindow->setVisible(false);

	GUIManager::getInstance()->addGUIElement(mFirstDecideWhoStartWindow);
	GUIManager::getInstance()->addGUIElement(mSecondDecideWhoStartWindow);
	GUIManager::getInstance()->addGUIElement(mStatsWindow);
}

 //initiering av gui knappar
void GameManager::initializeGuiFunctions()
{
	//St�nger statsf�nstret och g�r in i v�lja lag meny
	mCloseStatsWindow->setOnClickFunction([=]()
	{
		mStatsWindow->setVisible(false);
		mFirstDecideWhoStartWindow->setEnabled(true, true);
		mSecondDecideWhoStartWindow->setEnabled(true, true);
	});
	//st�nger ned f�nstret (om b�gge har samma spy)
	mCloseFirstWindow->setOnClickFunction([=]()
	{
		mFirstDecideWhoStartWindow->setVisible(false);
		startRound();
	});
	//Kapitalisterna b�rjar n�sta runda
	mCapitalistButton->setOnClickFunction([=]()
	{
		setCurrentPlayer(getCapitalist());
		startRound();
	});
	//Kommunisterna b�rjar n�sta runda
	mCommunistButton->setOnClickFunction([=]()
	{
		setCurrentPlayer(getCommunist());
		startRound();
	});
}