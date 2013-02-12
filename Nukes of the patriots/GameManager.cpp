#include "GameManager.h"
#include "GUIManager.h"
#include "Randomizer.h"
#include "Capitalist.h"
#include "Communist.h"
#include "SuperPower.h"
#include "President.h"
#include "tinyxml2.h"
#include "GUIText.h"

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

	/*Skriver ut året på interface*/
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
	mCurrentPlayer->setRound(1);
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
	//mCurrentPlayer->playMusic(); // ta bort senare, mest för att testa
	mCurrentPlayer->setRound(mCurrentPlayer->getRound() + 1);
}

void GameManager::selectStartingPlayer(std::shared_ptr<SuperPower> startingPlayer)
{
	setCurrentPlayer(startingPlayer); // Need to set setCurrentPlayer to update player round
	mVecPlayersLeft = mVecSuperPowers;
	for(std::vector<std::shared_ptr<SuperPower> >::iterator it = mVecPlayersLeft.begin(); it != mVecPlayersLeft.end(); it++)
	{
		if((*it) == mCurrentPlayer)
		{
			mVecPlayersLeft.erase(it);
			break;
		}
	}
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
	//std::cout<<"players left to play this round: "<<mVecPlayersLeft.size()<<std::endl;
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
		int randomPlayer = Randomizer::getInstance()->randomNr(nextPlayers.size(), 0);
		setCurrentPlayer(nextPlayers[randomPlayer]); // Need to set setCurrentPlayer to update player round
		//std::cout<<"starts this year: "<<mCurrentPlayer->getType()<<std::endl;
		for(std::vector<std::shared_ptr<SuperPower> >::iterator it = mVecPlayersLeft.begin(); it != mVecPlayersLeft.end(); it++)
		{
			if((*it) == mCurrentPlayer)
			{
				mVecPlayersLeft.erase(it);
				break;
			}
		}
		//nextPlayers[randomPlayer]->selectStartingPlayer();
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
		// Remove current player from players left to play
		for(std::vector<std::shared_ptr<SuperPower> >::iterator it = mVecPlayersLeft.begin(); it != mVecPlayersLeft.end(); it++)
		{
			if((*it) == mCurrentPlayer)
			{
				mVecPlayersLeft.erase(it);
				break;
			}
		}
	}
	/*Ökar år med ett när rundan är slut*/
	mYearText->setText(mYear);

	startRound();
}

