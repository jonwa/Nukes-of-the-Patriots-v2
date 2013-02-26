#include "GameManager.h"
#include "GUIManager.h"
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
#include "Menu.h"

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

//clear them MF containers 
void GameManager::clear()
{
	getCap()->clear();
	getCom()->clear();

	mVecSuperPowers.clear();
	mVecPlayersLeft.clear();
	mPresidentVector.clear();
	mGeneralVector.clear();

	for(std::map<std::string, std::pair<sf::FloatRect, sf::Texture*> >::iterator it = BetweenTurnsWindow.begin(); it != BetweenTurnsWindow.end(); it++)
	{
		delete (*it).second.second;
	}
	BetweenTurnsWindow.clear();

	for(std::map<std::string, std::pair<sf::FloatRect, sf::Texture*> >::iterator it = BetweenTurnsButton.begin(); it != BetweenTurnsButton.end(); it++)
	{
		delete (*it).second.second;
	}
	BetweenTurnsButton.clear();

	for(std::map<std::shared_ptr<President>, sf::Texture*>::iterator it = mPresidentPlaqueMap.begin(); it != mPresidentPlaqueMap.end(); it++)
	{
		delete (*it).second;
	}
	mPresidentPlaqueMap.clear();

	for(std::map<std::shared_ptr<President>, sf::Texture*>::iterator it = mGeneralPlaqueMap.begin(); it != mGeneralPlaqueMap.end(); it++)
	{
		delete (*it).second;
	}
	mGeneralPlaqueMap.clear();

}

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
	mYearText->setAlignment("middle");
	mYearText->setColor(sf::Color::White);
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
	std::shared_ptr<Capitalist> cap = std::static_pointer_cast<Capitalist> (mVecSuperPowers[0]);
	return cap;
}

std::shared_ptr<Communist> GameManager::getCom()
{
	std::shared_ptr<Communist> com = std::static_pointer_cast<Communist> (mVecSuperPowers[1]);
	return com;
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
	foodBought = (mCurrentPlayer->getExportedFood() == 0 || mCurrentPlayer->getExportedFoodPrice() == 0) ? 0 : moneyIntFood / mCurrentPlayer->getExportedFoodPrice();
	goodsBought = (mCurrentPlayer->getExportedGoods() == 0 || mCurrentPlayer->getExportedGoodsPrice() == 0) ? 0 : moneyIntGoods / mCurrentPlayer->getExportedGoodsPrice();
	techBought =(mCurrentPlayer->getExportedTech() == 0 || mCurrentPlayer->getExportedTechPrice() == 0) ? 0 :  moneyIntTech / mCurrentPlayer->getExportedTechPrice();
	//// if international market tries to buy more resources than you have
	if(foodBought > mCurrentPlayer->getExportedFood()) foodBought = mCurrentPlayer->getExportedFood();
	if(goodsBought > mCurrentPlayer->getExportedGoods()) goodsBought = mCurrentPlayer->getExportedGoods();
	if(techBought > mCurrentPlayer->getExportedTech()) techBought = mCurrentPlayer->getExportedTech();
	exports += (foodBought * mCurrentPlayer->getExportedFoodPrice()) + (goodsBought * mCurrentPlayer->getExportedGoodsPrice()) + (techBought * mCurrentPlayer->getExportedTechPrice());
	mCurrentPlayer->setExportedFood(mCurrentPlayer->getExportedFood() - foodBought);
	mCurrentPlayer->setExportedGoods(mCurrentPlayer->getExportedGoods() - goodsBought);
	mCurrentPlayer->setExportedTech(mCurrentPlayer->getExportedTech() - techBought);
	mCurrentPlayer->setCurrency(mCurrentPlayer->getCurrency() + exports);

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
	mSecondCapitalistSpyNetworkText->setText(intToString(getCapitalist()->getSpyNetwork()));
	mSecondCommunistSpyNetworkText->setText(intToString(getCommunist()->getSpyNetwork()));
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
				mFirstCapitalistSpyNetworkText->setText(intToString(getCapitalist()->getSpyNetwork()));
				mFirstCommunistSpyNetworkText->setText(intToString(getCommunist()->getSpyNetwork()));
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
	mFirstCapitalistSpyNetworkText		= GUIText::create(sf::FloatRect(500, 120, 40, 40), "0", mFirstDecideWhoStartWindow);
	mFirstCommunistSpyNetworkText		= GUIText::create(sf::FloatRect(500, 170, 40, 40), "0", mFirstDecideWhoStartWindow);
	mFirstDecideWhoStartWindow->setVisible(false);

	mSecondDecideWhoStartWindow			= GUIWindow::create(BetweenTurnsWindow["BetweenTurnsDiffSpy"]);
	mCapitalistButton					= GUIButton::create(BetweenTurnsButton["Capitalist"], mSecondDecideWhoStartWindow);
	mCommunistButton					= GUIButton::create(BetweenTurnsButton["Communist"], mSecondDecideWhoStartWindow);
	mSecondCapitalistSpyNetworkText		= GUIText::create(sf::FloatRect(500, 120, 40, 40), "0", mSecondDecideWhoStartWindow);
    mSecondCommunistSpyNetworkText		= GUIText::create(sf::FloatRect(500, 170, 40, 40), "0", mSecondDecideWhoStartWindow);
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
	