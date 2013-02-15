#include "Communist.h"
#include "GUIManager.h"
#include "GUIWindow.h"
#include "GUIButton.h"
#include "GUIText.h"
#include "tinyxml2.h"
#include "ResourceHandler.h"
#include "Randomizer.h"
#include "President.h"
#include "GameManager.h"
#include <sstream>
#include "Timer.h"

static int foodCost			= 10;
static int goodsCost		= 20;
static int techCost			= 30;
static int propagandaCost	= 100;
static int taxChange		= 5;
static bool activateWindow	= false;

static int generalCount = 0;

Communist::Communist()
{
	mPopulation			= 50;			//Befolkning i miljoner
	mPatriotism			= 20;
	mTaxes				= 30;
	mFood				= 0;
	mTech				= 0;
	mGoods				= 0;
	mSpyNetwork			= 0;
	mSpaceProgram		= 0;
	mNuclearWeapon		= 10;
	mRound				= 0;
	mIncreasePopulation = false;
	mType				= COMMUNIST;

	initializeCommunistWindow();
	initializeGuiFunctions();
	
	fiveYearInitialize();
	propagandaInitialize();
}


Communist::~Communist()
{
}

void Communist::fiveYearInitialize()
{
	for(int i = 0; i < 5; ++i)
	{
		std::map<std::string, int> tempMap;
		tempMap.insert(std::pair<std::string, int> ("food", 0));
		tempMap.insert(std::pair<std::string, int> ("goods", 0));
		tempMap.insert(std::pair<std::string, int> ("tech", 0));
		tempMap.insert(std::pair<std::string, int> ("taxes", 0));
		mYearVector.push_back(tempMap);
	}
}

void Communist::openFiveYearPlan()
{
	if((mRound-1) % 5 == 0 && mRound != 1)
	{
		mCommunistMainWindow->setEnabled(false, true);
		mTaxesWindow->setEnabled(true, true);
		//mCommunistFiveYearPlanButton->setTexture(CommunistButtons["FiveYearPlanIsPressed"]);
		mTaxesWindow->setVisible(true);
	}
}

void Communist::propagandaInitialize()
{
	PropagandaFood.push_back(&ResourceHandler::getInstance()->getTexture(std::string("Communist/PropagandaFoodOne")));
	PropagandaFood.push_back(&ResourceHandler::getInstance()->getTexture(std::string("Communist/PropagandaFoodTwo")));
	PropagandaFood.push_back(&ResourceHandler::getInstance()->getTexture(std::string("Communist/PropagandaFoodThree")));
	PropagandaGoods.push_back(&ResourceHandler::getInstance()->getTexture(std::string("Communist/PropagandaGoodsOne")));
	PropagandaGoods.push_back(&ResourceHandler::getInstance()->getTexture(std::string("Communist/PropagandaGoodsTwo")));
	PropagandaGoods.push_back(&ResourceHandler::getInstance()->getTexture(std::string("Communist/PropagandaGoodsThree")));
	PropagandaTech.push_back(&ResourceHandler::getInstance()->getTexture(std::string("Communist/PropagandaTechOne")));
	PropagandaTech.push_back(&ResourceHandler::getInstance()->getTexture(std::string("Communist/PropagandaTechTwo")));
	PropagandaTech.push_back(&ResourceHandler::getInstance()->getTexture(std::string("Communist/PropagandaTechThree")));
}

void Communist::setYearlyResources(int year, std::string key, int value)
{
	mYearVector[year-1][key] = value;
}

int Communist::getYearlyFood(int round)
{
	int year = round %5;

	if(year == 0)
	{
		year = 5;
	}
	year -= 1;
	return mYearVector[year]["food"];
}

int Communist::getYearlyGoods(int round)
{
	int year = round %5;

	if(year == 0)
	{
		year = 5;
	}
	year -= 1;
	return mYearVector[year]["goods"];
}

int Communist::getYearlyTech(int round)
{
	int year = round %5;

	if(year == 0)
	{
		year = 5;
	}
	year -= 1;
	return mYearVector[year]["tech"];
}

int Communist::getYearlyTaxes(int round)
{
	int year = round %5;

	if(year == 0)
	{
		year = 5;
	}
	year -= 1;

	return mYearVector[year]["taxes"];
}

void Communist::update()
{
	openFiveYearPlan();
}

/*	
	Uppgraderar mNuclearWeapon med ett
	Kostar 10 mGoods och 5 mTech
										*/
bool Communist::upgradeNuclearWeapon(int value)
{
	int goodsNuclearPrice = 10;
	int techNuclearPrice = 5;
	if(mGoods >= goodsNuclearPrice && mTech >= techNuclearPrice)
	{
		++mNuclearWeapon;
		mGoods -= goodsNuclearPrice;
		mTech -= techNuclearPrice;
		return true;
	}
	return false;
}

/*	
	Uppgraderar mSpaceProgram med ett
	Kostar 5 mGoods multiplicerat med den nuvarande nivån
	och 10 mTech multiplicerat med den nuvarande nivån
															*/
bool Communist::upgradeSpaceProgram(int value)
{
	int goodsSpaceProgramPrice = (mSpaceProgram == 0) ? 1 : mSpaceProgram * 5;
	int techSpaceProgramPrice = (mSpaceProgram == 0) ? 1 : mSpaceProgram * 10;
	if(mGoods >= goodsSpaceProgramPrice && mTech >= techSpaceProgramPrice)
	{
		++mSpaceProgram;
		mGoods -= goodsSpaceProgramPrice;
		mTech -= techSpaceProgramPrice;
		return true;
	}
	return false;
}

/*	
	Uppgraderar mSpyNetwork med ett
	Kostar 10 mTech multiplicerat med den nuvarande nivån
															*/
bool Communist::upgradeSpyNetwork(int value)
{
	int spyNetworkPrice = (mSpyNetwork == 0) ? 1 : mSpyNetwork * 10;

	if(mTech >= spyNetworkPrice)
	{
		++mSpyNetwork;
		mTech -= spyNetworkPrice;
		return true;
	}
	return false;
}

//--------------------------------------------
/*Funktioner som ger medlemsvariabler nya värden*/
bool Communist::setFood(int value)
{	
	if(mCurrency >= value * foodCost)
	{
 		mFood		+= value;
		mCurrency	-= value * foodCost;
		return true;
	}
	return false;
}

bool Communist::setGoods(int value)
{
	if(mCurrency >= value * goodsCost)
	{
		mGoods		+= value;
		mCurrency	-= value * goodsCost;
		return true;
	}
	return false;
}

bool Communist::setTech(int value)
{
	if(mCurrency >= value * techCost)
	{
		mTech		+= value;
		mCurrency	-= value * techCost;
		return true;
	}
	return false;
}

void Communist::setTaxesCost(int tax)
{
	mTaxDecreased = (tax < mTaxes);
	mTaxes = tax;
}
/*  Köper en dos propaganda för 100 kr/dos som kan ge upp till 10 av en resurs, 
	antalet man får är ==  10 rolls med en %chans baserat på resursens andel av 
	det årets planerade totala mängd resurser. (Därav måste 5-årsplanen komma före)
	Man kan inte med hjälp av propaganda producera mer av en 
	resurs än den mängd som står i femårsplanen för det året.
																			*/
void Communist::buyPropagandaFood(int round)
{
	int resourcesTotal = 0;
	resourcesTotal += getYearlyFood(round);
	resourcesTotal += getYearlyGoods(round);
	resourcesTotal += getYearlyTech(round);

	float percent = mFood/resourcesTotal;
	percent*=100;

	for(int i=0;i<10;i++)
	{
		float randNr=Randomizer::getInstance()->randomNr(100,1);

		if(randNr<percent)
		{
			mFood++;
		}
	}
	mCurrency -= propagandaCost;
}

void Communist::buyPropagandaGoods(int round)
{
	int resourcesTotal = 0;
	resourcesTotal += getYearlyFood(round);
	resourcesTotal += getYearlyGoods(round);
	resourcesTotal += getYearlyTech(round);

	int percent = mGoods/resourcesTotal;
	percent*=100;

	for(int i=0;i<10;i++)
	{
		float randNr=Randomizer::getInstance()->randomNr(100,1);

		if(randNr<percent)
		{
			mGoods++;
		}
	}
	mCurrency -= propagandaCost;
}

void Communist::buyPropagandaTech(int round)
{
	int resourcesTotal = 0;
	resourcesTotal += getYearlyFood(round);
	resourcesTotal += getYearlyGoods(round);
	resourcesTotal += getYearlyTech(round);
	
	int percent = mTech/resourcesTotal;
	percent*=100;

	for(int i=0;i<10;i++)
	{
		float randNr=Randomizer::getInstance()->randomNr(100,1);

		if(randNr<percent)
		{
		mTech++;
		}
	}
	mCurrency -= propagandaCost;
}

//------------------------------------------------------------------------------------------------------
	
//------------------------------------------------------------------------------------------------------

 /*
	Ladda hem alla knapparnas positioner från XML dokument

	Av: Jon Wahlström 2013-01-31
 
 */
void Communist::loadButtonPosition()
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile("XML/CommunistButtonPos.xml");

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

		CommunistButtons[tempName].first.left = posX;
		CommunistButtons[tempName].first.top = posY;
		CommunistButtons[tempName].first.width = Width;
		CommunistButtons[tempName].first.height = Height;
		CommunistButtons[tempName].second = &ResourceHandler::getInstance()->getTexture(name);
		button = button->NextSiblingElement();
	}
}

 /*
	Laddar in fönstrernas positioner via XML dokument.

		Av: Jon Wahlström 2013-01-31
 */
void Communist::loadWindowPosition()
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile("XML/CommunistWindowPos.xml");

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
				
		CommunistWindows[tempName].first.left = posX;
		CommunistWindows[tempName].first.top = posY;
		CommunistWindows[tempName].first.width = Width;
		CommunistWindows[tempName].first.height = Height;
		CommunistWindows[tempName].second = &ResourceHandler::getInstance()->getTexture(name);
		
		window = window->NextSiblingElement();
	}	
}

void Communist::loadCommunistMusic()
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile("XML/CommunistSounds.xml");

	if(doc.Error())
		std::cout << "Fel! Communist::loadCommunistMusic";
	
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
				
		CommunistMusic[tempName] = ResourceHandler::getInstance()->getMusic(name);
	
		music = music->NextSiblingElement();	
	}
}

 /*Används för att spela upp kommunisternas themesong*/
void Communist::playMusic()
{
	CommunistMusic["CommunistMainTheme"]->play();
}

 /*initierar kommunisternas fönster respektive fönster/knappar etc.*/
void Communist::initializeCommunistWindow()
{
	loadButtonPosition();
	loadWindowPosition();
	loadCommunistMusic();
	//playMusic();

	mCommunistMainWindow			= GUIWindow::create(CommunistWindows["CommunistInterface"]);
	mCommunistGeneralButton			= GUIButton::create(CommunistButtons["General"], mCommunistMainWindow);
	mCommunistFiveYearPlanButton    = GUIButton::create(CommunistButtons["FiveYearPlan"], mCommunistMainWindow);
	mCommunistPropagandaButton		= GUIButton::create(CommunistButtons["Propaganda"], mCommunistMainWindow);
	mCommunistUpgradeButton			= GUIButton::create(CommunistButtons["Upgrade"], mCommunistMainWindow);
	mCommunistExportButton			= GUIButton::create(CommunistButtons["Export"], mCommunistMainWindow);
	mCommunistEndTurnButton			= GUIButton::create(CommunistButtons["EndTurn"], mCommunistMainWindow);
	mCommunistMainWindow->setVisible(false);

	/*GUI text för utskrift av värden på kapitalisternas interface*/
	mNuclearText = GUIText::create(sf::FloatRect(815, 16, 40, 40), intToString(getNuclearWeapon()), mCommunistMainWindow);
	mSpaceText	 = GUIText::create(sf::FloatRect(815, 228, 40, 40), intToString(getSpaceProgram()), mCommunistMainWindow);
	mSpyText	 = GUIText::create(sf::FloatRect(815, 440, 40, 40), intToString(getSpyNetwork()), mCommunistMainWindow);
	mFoodText	 = GUIText::create(sf::FloatRect(10, 16, 40, 40), intToString(getFood()), mCommunistMainWindow);
	mGoodsText   = GUIText::create(sf::FloatRect(10, 228, 40, 40), intToString(getGoods()), mCommunistMainWindow);
	mTechText	 = GUIText::create(sf::FloatRect(10, 440, 40, 40), intToString(getTech()), mCommunistMainWindow);	

	/*Taxes fönster med knappar*/
	mTaxesWindow					= GUIWindow::create(CommunistWindows["CommunistTaxesWindow"], mCommunistMainWindow);	
	mYearOneLowerTaxesButton		= GUIButton::create(CommunistButtons["YearOneLowerTaxes"], mTaxesWindow);	
	mYearOneRaiseTaxesButton		= GUIButton::create(CommunistButtons["YearOneRaiseTaxes"], mTaxesWindow);	
	mYearTwoLowerTaxesButton		= GUIButton::create(CommunistButtons["YearTwoLowerTaxes"], mTaxesWindow);	
	mYearTwoRaiseTaxesButton		= GUIButton::create(CommunistButtons["YearTwoRaiseTaxes"], mTaxesWindow);	
	mYearThreeLowerTaxesButton		= GUIButton::create(CommunistButtons["YearThreeLowerTaxes"], mTaxesWindow);	
	mYearThreeRaiseTaxesButton		= GUIButton::create(CommunistButtons["YearThreeRaiseTaxes"], mTaxesWindow);	
	mYearFourLowerTaxesButton		= GUIButton::create(CommunistButtons["YearFourLowerTaxes"], mTaxesWindow);	
	mYearFourRaiseTaxesButton		= GUIButton::create(CommunistButtons["YearFourRaiseTaxes"], mTaxesWindow);	
	mYearFiveLowerTaxesButton		= GUIButton::create(CommunistButtons["YearFiveLowerTaxes"], mTaxesWindow);	
	mYearFiveRaiseTaxesButton		= GUIButton::create(CommunistButtons["YearFiveRaiseTaxes"], mTaxesWindow);

	mYearOneTaxesText				= GUIText::create(sf::FloatRect(104, 77, 20, 20), "0", mTaxesWindow);
	mYearTwoTaxesText				= GUIText::create(sf::FloatRect(104, 169, 20, 20), "0", mTaxesWindow);
	mYearThreeTaxesText				= GUIText::create(sf::FloatRect(104, 261, 20, 20), "0", mTaxesWindow);
	mYearFourTaxesText				= GUIText::create(sf::FloatRect(104, 353, 20, 20), "0", mTaxesWindow);
	mYearFiveTaxesText				= GUIText::create(sf::FloatRect(104, 445, 20, 20), "0", mTaxesWindow);
	mTaxesWindow->setVisible(false);

	/*Resource GUI window med knappar*/
	mResourcesWindow					= GUIWindow::create(CommunistWindows["CommunistResourcesWindow"], mCommunistMainWindow);	
	/*Food*/
	mYearOneLowerFoodByTenButton		= GUIButton::create(CommunistButtons["YearOneLowerFoodByTen"], mResourcesWindow);	
	mYearOneLowerFoodByFiveButton		= GUIButton::create(CommunistButtons["YearOneLowerFoodByFive"], mResourcesWindow);	
	mYearOneLowerFoodByOneButton		= GUIButton::create(CommunistButtons["YearOneLowerFoodByOne"], mResourcesWindow);	
	mYearOneRaiseFoodByOneButton		= GUIButton::create(CommunistButtons["YearOneRaiseFoodByOne"], mResourcesWindow);	
	mYearOneRaiseFoodByFiveButton		= GUIButton::create(CommunistButtons["YearOneRaiseFoodByFive"], mResourcesWindow);	
	mYearOneRaiseFoodByTenButton		= GUIButton::create(CommunistButtons["YearOneRaiseFoodByTen"], mResourcesWindow);
	mYearOneFoodText					= GUIText::create(sf::FloatRect(104, 77, 20, 20), "0", mResourcesWindow);
	
	mYearTwoLowerFoodByTenButton		= GUIButton::create(CommunistButtons["YearTwoLowerFoodByTen"], mResourcesWindow);	
	mYearTwoLowerFoodByFiveButton		= GUIButton::create(CommunistButtons["YearTwoLowerFoodByFive"], mResourcesWindow);	
	mYearTwoLowerFoodByOneButton		= GUIButton::create(CommunistButtons["YearTwoLowerFoodByOne"], mResourcesWindow);	
	mYearTwoRaiseFoodByOneButton		= GUIButton::create(CommunistButtons["YearTwoRaiseFoodByOne"], mResourcesWindow);	
	mYearTwoRaiseFoodByFiveButton		= GUIButton::create(CommunistButtons["YearTwoRaiseFoodByFive"], mResourcesWindow);	
	mYearTwoRaiseFoodByTenButton		= GUIButton::create(CommunistButtons["YearTwoRaiseFoodByTen"], mResourcesWindow);
	mYearTwoFoodText					= GUIText::create(sf::FloatRect(104, 169, 20, 20), "0", mResourcesWindow);

	mYearThreeLowerFoodByTenButton		= GUIButton::create(CommunistButtons["YearThreeLowerFoodByTen"], mResourcesWindow);	
	mYearThreeLowerFoodByFiveButton		= GUIButton::create(CommunistButtons["YearThreeLowerFoodByFive"], mResourcesWindow);	
	mYearThreeLowerFoodByOneButton		= GUIButton::create(CommunistButtons["YearThreeLowerFoodByOne"], mResourcesWindow);	
	mYearThreeRaiseFoodByOneButton		= GUIButton::create(CommunistButtons["YearThreeRaiseFoodByOne"], mResourcesWindow);	
	mYearThreeRaiseFoodByFiveButton		= GUIButton::create(CommunistButtons["YearThreeRaiseFoodByFive"], mResourcesWindow);	
	mYearThreeRaiseFoodByTenButton		= GUIButton::create(CommunistButtons["YearThreeRaiseFoodByTen"], mResourcesWindow);
	mYearThreeFoodText					= GUIText::create(sf::FloatRect(104, 261, 20, 20), "0", mResourcesWindow);
	
	mYearFourLowerFoodByTenButton		= GUIButton::create(CommunistButtons["YearFourLowerFoodByTen"], mResourcesWindow);	
	mYearFourLowerFoodByFiveButton		= GUIButton::create(CommunistButtons["YearFourLowerFoodByFive"], mResourcesWindow);
	mYearFourLowerFoodByOneButton		= GUIButton::create(CommunistButtons["YearFourLowerFoodByOne"], mResourcesWindow);	
	mYearFourRaiseFoodByOneButton		= GUIButton::create(CommunistButtons["YearFourRaiseFoodByOne"], mResourcesWindow);
	mYearFourRaiseFoodByFiveButton		= GUIButton::create(CommunistButtons["YearFourRaiseFoodByFive"], mResourcesWindow);	
	mYearFourRaiseFoodByTenButton		= GUIButton::create(CommunistButtons["YearFourRaiseFoodByTen"], mResourcesWindow);
	mYearFourFoodText					= GUIText::create(sf::FloatRect(104, 353, 20, 20), "0", mResourcesWindow);
		
	mYearFiveLowerFoodByTenButton		= GUIButton::create(CommunistButtons["YearFiveLowerFoodByTen"], mResourcesWindow);	
	mYearFiveLowerFoodByFiveButton		= GUIButton::create(CommunistButtons["YearFiveLowerFoodByFive"], mResourcesWindow);
	mYearFiveLowerFoodByOneButton		= GUIButton::create(CommunistButtons["YearFiveLowerFoodByOne"], mResourcesWindow);	
	mYearFiveRaiseFoodByOneButton		= GUIButton::create(CommunistButtons["YearFiveRaiseFoodByOne"], mResourcesWindow);
	mYearFiveRaiseFoodByFiveButton		= GUIButton::create(CommunistButtons["YearFiveRaiseFoodByFive"], mResourcesWindow);	
	mYearFiveRaiseFoodByTenButton		= GUIButton::create(CommunistButtons["YearFiveRaiseFoodByTen"], mResourcesWindow);
	mYearFiveFoodText					= GUIText::create(sf::FloatRect(104, 445, 20, 20), "0", mResourcesWindow);
	
	/*Goods*/
	mYearOneLowerGoodsByTenButton		= GUIButton::create(CommunistButtons["YearOneLowerGoodsByTen"], mResourcesWindow);	
	mYearOneLowerGoodsByFiveButton		= GUIButton::create(CommunistButtons["YearOneLowerGoodsByFive"], mResourcesWindow);	
	mYearOneLowerGoodsByOneButton		= GUIButton::create(CommunistButtons["YearOneLowerGoodsByOne"], mResourcesWindow);	
	mYearOneRaiseGoodsByOneButton		= GUIButton::create(CommunistButtons["YearOneRaiseGoodsByOne"], mResourcesWindow);	
	mYearOneRaiseGoodsByFiveButton		= GUIButton::create(CommunistButtons["YearOneRaiseGoodsByFive"], mResourcesWindow);	
	mYearOneRaiseGoodsByTenButton		= GUIButton::create(CommunistButtons["YearOneRaiseGoodsByTen"], mResourcesWindow);
	mYearOneGoodsText					= GUIText::create(sf::FloatRect(285, 77, 20, 20), "0", mResourcesWindow);
	
	mYearTwoLowerGoodsByTenButton		= GUIButton::create(CommunistButtons["YearTwoLowerGoodsByTen"], mResourcesWindow);	
	mYearTwoLowerGoodsByFiveButton		= GUIButton::create(CommunistButtons["YearTwoLowerGoodsByFive"], mResourcesWindow);	
	mYearTwoLowerGoodsByOneButton		= GUIButton::create(CommunistButtons["YearTwoLowerGoodsByOne"], mResourcesWindow);	
	mYearTwoRaiseGoodsByOneButton		= GUIButton::create(CommunistButtons["YearTwoRaiseGoodsByOne"], mResourcesWindow);	
	mYearTwoRaiseGoodsByFiveButton		= GUIButton::create(CommunistButtons["YearTwoRaiseGoodsByFive"], mResourcesWindow);	
	mYearTwoRaiseGoodsByTenButton		= GUIButton::create(CommunistButtons["YearTwoRaiseGoodsByTen"], mResourcesWindow);
	mYearTwoGoodsText					= GUIText::create(sf::FloatRect(285, 169, 20, 20), "0", mResourcesWindow);

	mYearThreeLowerGoodsByTenButton		= GUIButton::create(CommunistButtons["YearThreeLowerGoodsByTen"], mResourcesWindow);	
	mYearThreeLowerGoodsByFiveButton	= GUIButton::create(CommunistButtons["YearThreeLowerGoodsByFive"], mResourcesWindow);	
	mYearThreeLowerGoodsByOneButton		= GUIButton::create(CommunistButtons["YearThreeLowerGoodsByOne"], mResourcesWindow);	
	mYearThreeRaiseGoodsByOneButton		= GUIButton::create(CommunistButtons["YearThreeRaiseGoodsByOne"], mResourcesWindow);	
	mYearThreeRaiseGoodsByFiveButton	= GUIButton::create(CommunistButtons["YearThreeRaiseGoodsByFive"], mResourcesWindow);	
	mYearThreeRaiseGoodsByTenButton		= GUIButton::create(CommunistButtons["YearThreeRaiseGoodsByTen"], mResourcesWindow);
	mYearThreeGoodsText					= GUIText::create(sf::FloatRect(285, 261, 20, 20), "0", mResourcesWindow);
	
	mYearFourLowerGoodsByTenButton		= GUIButton::create(CommunistButtons["YearFourLowerGoodsByTen"], mResourcesWindow);	
	mYearFourLowerGoodsByFiveButton		= GUIButton::create(CommunistButtons["YearFourLowerGoodsByFive"], mResourcesWindow);
	mYearFourLowerGoodsByOneButton		= GUIButton::create(CommunistButtons["YearFourLowerGoodsByOne"], mResourcesWindow);	
	mYearFourRaiseGoodsByOneButton		= GUIButton::create(CommunistButtons["YearFourRaiseGoodsByOne"], mResourcesWindow);
	mYearFourRaiseGoodsByFiveButton		= GUIButton::create(CommunistButtons["YearFourRaiseGoodsByFive"], mResourcesWindow);	
	mYearFourRaiseGoodsByTenButton		= GUIButton::create(CommunistButtons["YearFourRaiseGoodsByTen"], mResourcesWindow);
	mYearFourGoodsText					= GUIText::create(sf::FloatRect(285, 353, 20, 20), "0", mResourcesWindow);
	
	mYearFiveLowerGoodsByTenButton		= GUIButton::create(CommunistButtons["YearFiveLowerGoodsByTen"], mResourcesWindow);	
	mYearFiveLowerGoodsByFiveButton		= GUIButton::create(CommunistButtons["YearFiveLowerGoodsByFive"], mResourcesWindow);
	mYearFiveLowerGoodsByOneButton		= GUIButton::create(CommunistButtons["YearFiveLowerGoodsByOne"], mResourcesWindow);	
	mYearFiveRaiseGoodsByOneButton		= GUIButton::create(CommunistButtons["YearFiveRaiseGoodsByOne"], mResourcesWindow);
	mYearFiveRaiseGoodsByFiveButton		= GUIButton::create(CommunistButtons["YearFiveRaiseGoodsByFive"], mResourcesWindow);	
	mYearFiveRaiseGoodsByTenButton		= GUIButton::create(CommunistButtons["YearFiveRaiseGoodsByTen"], mResourcesWindow);
	mYearFiveGoodsText					= GUIText::create(sf::FloatRect(285, 445, 20, 20), "0", mResourcesWindow);

	/*Tech*/
	mYearOneLowerTechByTenButton		= GUIButton::create(CommunistButtons["YearOneLowerTechByTen"], mResourcesWindow);	
	mYearOneLowerTechByFiveButton		= GUIButton::create(CommunistButtons["YearOneLowerTechByFive"], mResourcesWindow);	
	mYearOneLowerTechByOneButton		= GUIButton::create(CommunistButtons["YearOneLowerTechByOne"], mResourcesWindow);	
	mYearOneRaiseTechByOneButton		= GUIButton::create(CommunistButtons["YearOneRaiseTechByOne"], mResourcesWindow);	
	mYearOneRaiseTechByFiveButton		= GUIButton::create(CommunistButtons["YearOneRaiseTechByFive"], mResourcesWindow);	
	mYearOneRaiseTechByTenButton		= GUIButton::create(CommunistButtons["YearOneRaiseTechByTen"], mResourcesWindow);
	mYearOneTechText					= GUIText::create(sf::FloatRect(465, 77, 20, 20), "0", mResourcesWindow);
	
	mYearTwoLowerTechByTenButton		= GUIButton::create(CommunistButtons["YearTwoLowerTechByTen"], mResourcesWindow);	
	mYearTwoLowerTechByFiveButton		= GUIButton::create(CommunistButtons["YearTwoLowerTechByFive"], mResourcesWindow);	
	mYearTwoLowerTechByOneButton		= GUIButton::create(CommunistButtons["YearTwoLowerTechByOne"], mResourcesWindow);	
	mYearTwoRaiseTechByOneButton		= GUIButton::create(CommunistButtons["YearTwoRaiseTechByOne"], mResourcesWindow);	
	mYearTwoRaiseTechByFiveButton		= GUIButton::create(CommunistButtons["YearTwoRaiseTechByFive"], mResourcesWindow);	
	mYearTwoRaiseTechByTenButton		= GUIButton::create(CommunistButtons["YearTwoRaiseTechByTen"], mResourcesWindow);
	mYearTwoTechText					= GUIText::create(sf::FloatRect(465, 169, 20, 20), "0", mResourcesWindow);

	mYearThreeLowerTechByTenButton		= GUIButton::create(CommunistButtons["YearThreeLowerTechByTen"], mResourcesWindow);	
	mYearThreeLowerTechByFiveButton		= GUIButton::create(CommunistButtons["YearThreeLowerTechByFive"], mResourcesWindow);	
	mYearThreeLowerTechByOneButton		= GUIButton::create(CommunistButtons["YearThreeLowerTechByOne"], mResourcesWindow);	
	mYearThreeRaiseTechByOneButton		= GUIButton::create(CommunistButtons["YearThreeRaiseTechByOne"], mResourcesWindow);	
	mYearThreeRaiseTechByFiveButton		= GUIButton::create(CommunistButtons["YearThreeRaiseTechByFive"], mResourcesWindow);	
	mYearThreeRaiseTechByTenButton		= GUIButton::create(CommunistButtons["YearThreeRaiseTechByTen"], mResourcesWindow);
	mYearThreeTechText					= GUIText::create(sf::FloatRect(465, 261, 20, 20), "0", mResourcesWindow);
	
	mYearFourLowerTechByTenButton		= GUIButton::create(CommunistButtons["YearFourLowerTechByTen"], mResourcesWindow);	
	mYearFourLowerTechByFiveButton		= GUIButton::create(CommunistButtons["YearFourLowerTechByFive"], mResourcesWindow);
	mYearFourLowerTechByOneButton		= GUIButton::create(CommunistButtons["YearFourLowerTechByOne"], mResourcesWindow);	
	mYearFourRaiseTechByOneButton		= GUIButton::create(CommunistButtons["YearFourRaiseTechByOne"], mResourcesWindow);
	mYearFourRaiseTechByFiveButton		= GUIButton::create(CommunistButtons["YearFourRaiseTechByFive"], mResourcesWindow);	
	mYearFourRaiseTechByTenButton		= GUIButton::create(CommunistButtons["YearFourRaiseTechByTen"], mResourcesWindow);
	mYearFourTechText					= GUIText::create(sf::FloatRect(465, 353, 20, 20), "0", mResourcesWindow);
	
	mYearFiveLowerTechByTenButton		= GUIButton::create(CommunistButtons["YearFiveLowerTechByTen"], mResourcesWindow);	
	mYearFiveLowerTechByFiveButton		= GUIButton::create(CommunistButtons["YearFiveLowerTechByFive"], mResourcesWindow);
	mYearFiveLowerTechByOneButton		= GUIButton::create(CommunistButtons["YearFiveLowerTechByOne"], mResourcesWindow);	
	mYearFiveRaiseTechByOneButton		= GUIButton::create(CommunistButtons["YearFiveRaiseTechByOne"], mResourcesWindow);
	mYearFiveRaiseTechByFiveButton		= GUIButton::create(CommunistButtons["YearFiveRaiseTechByFive"], mResourcesWindow);	
	mYearFiveRaiseTechByTenButton		= GUIButton::create(CommunistButtons["YearFiveRaiseTechByTen"], mResourcesWindow);
	mYearFiveTechText					= GUIText::create(sf::FloatRect(465, 445, 20, 20), "0", mResourcesWindow);

	mGoToNextSlideButton				= GUIButton::create(CommunistButtons["GoToNextSlide"], mTaxesWindow);	
	mTaxesCloseButton					= GUIButton::create(CommunistButtons["CloseTaxesWindow"], mTaxesWindow);	
	mGoToPreviousSlideButton			= GUIButton::create(CommunistButtons["GoToPreviousSlide"], mResourcesWindow);
	mResourcesCloseButton				= GUIButton::create(CommunistButtons["CloseResourcesWindow"], mResourcesWindow);	
	mTaxesWindow->setVisible(false);
	mResourcesWindow->setVisible(false);

	/*Propaganda GUI window med knappar*/
	mPropagandaWindowFirst				= GUIWindow::create(CommunistWindows["CommunistPropagandaWindowFirst"], mCommunistMainWindow);
	mPropagandaWindowSecond				= GUIWindow::create(CommunistWindows["CommunistPropagandaWindowSecond"], mCommunistMainWindow);
	mPropagandaBuyFoodButton			= GUIButton::create(CommunistButtons["PropagandaBuyFood"], mPropagandaWindowFirst);
	mPropagandaBuyGoodsButton			= GUIButton::create(CommunistButtons["PropagandaBuyGoods"], mPropagandaWindowFirst);
	mPropagandaBuyTechButton			= GUIButton::create(CommunistButtons["PropagandaBuyTech"], mPropagandaWindowFirst);
	mPropagandaWindowFirstCloseButton	= GUIButton::create(CommunistButtons["PropagandaCloseWindowFirst"], mPropagandaWindowFirst);
	mPropagandaWindowSecondCloseButton	= GUIButton::create(CommunistButtons["PropagandaCloseWindowSecond"], mPropagandaWindowSecond);
	mShowBoughtPropaganda				= GUIButton::create(CommunistButtons["ShowPropaganda"], mPropagandaWindowSecond);
	mPropagandaWindowFirst->setVisible(false);
	mPropagandaWindowSecond->setVisible(false);

	/*Upgrade GUI window med knappar*/
	mUpgradeWindow						= GUIWindow::create(CommunistWindows["CommunistUpgradeWindow"], mCommunistMainWindow);
	mUpgradeNuclearWeaponButton			= GUIButton::create(CommunistButtons["UpgradeNuclearWeapon"], mUpgradeWindow);
	mUpgradeSpaceProgramButton			= GUIButton::create(CommunistButtons["UpgradeSpaceProgram"], mUpgradeWindow);
	mUpgradeSpyNetworkButton			= GUIButton::create(CommunistButtons["UpgradeSpyNetwork"], mUpgradeWindow);
	mUpgradeCloseButton					= GUIButton::create(CommunistButtons["CloseUpgrade"], mUpgradeWindow);
	mCancelUpgradeNuclearWeaponButton	= GUIButton::create(CommunistButtons["CancelUpgradeNuclearWeapon"], mUpgradeWindow);
	mCancelUpgradeSpaceProgramButton	= GUIButton::create(CommunistButtons["CancelUpgradeSpaceProgram"], mUpgradeWindow);
	mCancelUpgradeSpyNetworkButton		= GUIButton::create(CommunistButtons["CancelUpgradeSpyNetwork"], mUpgradeWindow);
	mBuyNuclearText						= GUIText::create(sf::FloatRect(159, 145, 22, 22), "0", mUpgradeWindow);
	mBuySpaceProgramText				= GUIText::create(sf::FloatRect(337, 107, 22, 22), "0", mUpgradeWindow);
	mBuySpyNetworkText					= GUIText::create(sf::FloatRect(517, 78, 22, 22), "0", mUpgradeWindow);
	mUpgradeWindow->setVisible(false);

	/*Export GUI Window med knappar*/
	mExportWindow						= GUIWindow::create(CommunistWindows["CommunistExportWindow"], mCommunistMainWindow);
	mExportLowerFoodButton				= GUIButton::create(CommunistButtons["CommunistLowerFood"], mExportWindow);
	mExportRaiseFoodButton				= GUIButton::create(CommunistButtons["CommunistRaiseFood"], mExportWindow);
	mExportLowerGoodsButton				= GUIButton::create(CommunistButtons["CommunistLowerGoods"], mExportWindow);
	mExportRaiseGoodsButton				= GUIButton::create(CommunistButtons["CommunistRaiseGoods"], mExportWindow);
	mExportLowerTechButton				= GUIButton::create(CommunistButtons["CommunistLowerTech"], mExportWindow);
	mExportRaiseTechButton				= GUIButton::create(CommunistButtons["CommunistRaiseTech"], mExportWindow);
	mExportCloseButton					= GUIButton::create(CommunistButtons["CloseExport"], mExportWindow);
	mExportWindow->setVisible(false);


	mChooseGeneralWindow				= GUIWindow::create(CommunistWindows["ChooseGeneral"], mCommunistMainWindow);
	mPickedGeneralWindow				= GUIWindow::create(CommunistWindows["PickedGeneral"], mCommunistMainWindow);
	mPickedGeneralButton				= GUIButton::create(CommunistButtons["PickedGeneral"], mPickedGeneralWindow);
	mPickedGeneralWindow->setVisible(false);
	mFirstGeneralButton					= GUIButton::create(CommunistButtons["FirstGeneral"], mChooseGeneralWindow);
	
	mGoToNextPortraitButton				= GUIButton::create(CommunistButtons["GoToNextPortrait"], mChooseGeneralWindow);
	mGoToPreviousPortraitButton			= GUIButton::create(CommunistButtons["GoToPreviousPortrait"], mChooseGeneralWindow);
	mCloseGeneralWindow					= GUIButton::create(CommunistButtons["CloseGeneral"], mChooseGeneralWindow);
	mClosePickedGeneralWindow			= GUIButton::create(CommunistButtons["ClosePickedGeneral"], mPickedGeneralWindow);
	//mChooseGeneralWindow->setVisible(false);
	
	chooseLeader();

	
	/*
	 	Lägger in föräldernoden i vektorn som finns i GUIManager
	 	och kommer automatiskt få med sig alla barnnoder till denna
	 	vilket är alla GUIElement som finns i denna klass som har 
	 	mCapitalistMainWindow som parent-argument i dess konstruktor
																		*/

	GUIManager::getInstance()->addGUIElement(mCommunistMainWindow);
}


void Communist::chooseLeader()
{
	GameManager* manager = GameManager::getInstance();
	mCommunistMainWindow->setEnabled(false, true);
	mChooseGeneralWindow->setEnabled(true, true);
	mFirstGeneralButton->setTexture(std::pair<sf::FloatRect, sf::Texture*>(mFirstGeneralButton->getRectangle(), manager->getGeneral(generalCount)->getTexture())); 
}

/*Initierar funktionerna för femårsplansknapparna. Egen funktion för att inte göra allting så grötigt.*/
void Communist::fiveYearGuiFunctions()
{
	mResourcesFoodButtons.push_back(mYearOneLowerFoodByTenButton);
	mResourcesFoodButtons.push_back(mYearOneLowerFoodByFiveButton);
	mResourcesFoodButtons.push_back(mYearOneLowerFoodByOneButton);
	mResourcesFoodButtons.push_back(mYearOneRaiseFoodByOneButton);
	mResourcesFoodButtons.push_back(mYearOneRaiseFoodByFiveButton);
	mResourcesFoodButtons.push_back(mYearOneRaiseFoodByTenButton);

	mResourcesFoodButtons.push_back(mYearTwoLowerFoodByTenButton);
	mResourcesFoodButtons.push_back(mYearTwoLowerFoodByFiveButton);
	mResourcesFoodButtons.push_back(mYearTwoLowerFoodByOneButton);
	mResourcesFoodButtons.push_back(mYearTwoRaiseFoodByOneButton);
	mResourcesFoodButtons.push_back(mYearTwoRaiseFoodByFiveButton);
	mResourcesFoodButtons.push_back(mYearTwoRaiseFoodByTenButton);

	mResourcesFoodButtons.push_back(mYearThreeLowerFoodByTenButton);
	mResourcesFoodButtons.push_back(mYearThreeLowerFoodByFiveButton);
	mResourcesFoodButtons.push_back(mYearThreeLowerFoodByOneButton);
	mResourcesFoodButtons.push_back(mYearThreeRaiseFoodByOneButton);
	mResourcesFoodButtons.push_back(mYearThreeRaiseFoodByFiveButton);
	mResourcesFoodButtons.push_back(mYearThreeRaiseFoodByTenButton);

	mResourcesFoodButtons.push_back(mYearFourLowerFoodByTenButton);
	mResourcesFoodButtons.push_back(mYearFourLowerFoodByFiveButton);
	mResourcesFoodButtons.push_back(mYearFourLowerFoodByOneButton);
	mResourcesFoodButtons.push_back(mYearFourRaiseFoodByOneButton);
	mResourcesFoodButtons.push_back(mYearFourRaiseFoodByFiveButton);
	mResourcesFoodButtons.push_back(mYearFourRaiseFoodByTenButton);
	
	mResourcesFoodButtons.push_back(mYearFiveLowerFoodByTenButton);
	mResourcesFoodButtons.push_back(mYearFiveLowerFoodByFiveButton);
	mResourcesFoodButtons.push_back(mYearFiveLowerFoodByOneButton);
	mResourcesFoodButtons.push_back(mYearFiveRaiseFoodByOneButton);
	mResourcesFoodButtons.push_back(mYearFiveRaiseFoodByFiveButton);
	mResourcesFoodButtons.push_back(mYearFiveRaiseFoodByTenButton);
	
	mResourcesGoodsButtons.push_back(mYearOneLowerGoodsByTenButton);
	mResourcesGoodsButtons.push_back(mYearOneLowerGoodsByFiveButton);
	mResourcesGoodsButtons.push_back(mYearOneLowerGoodsByOneButton);
	mResourcesGoodsButtons.push_back(mYearOneRaiseGoodsByOneButton);
	mResourcesGoodsButtons.push_back(mYearOneRaiseGoodsByFiveButton);
	mResourcesGoodsButtons.push_back(mYearOneRaiseGoodsByTenButton);

	mResourcesGoodsButtons.push_back(mYearTwoLowerGoodsByTenButton);
	mResourcesGoodsButtons.push_back(mYearTwoLowerGoodsByFiveButton);
	mResourcesGoodsButtons.push_back(mYearTwoLowerGoodsByOneButton);
	mResourcesGoodsButtons.push_back(mYearTwoRaiseGoodsByOneButton);
	mResourcesGoodsButtons.push_back(mYearTwoRaiseGoodsByFiveButton);
	mResourcesGoodsButtons.push_back(mYearTwoRaiseGoodsByTenButton);
	
	mResourcesGoodsButtons.push_back(mYearThreeLowerGoodsByTenButton);
	mResourcesGoodsButtons.push_back(mYearThreeLowerGoodsByFiveButton);
	mResourcesGoodsButtons.push_back(mYearThreeLowerGoodsByOneButton);
	mResourcesGoodsButtons.push_back(mYearThreeRaiseGoodsByOneButton);
	mResourcesGoodsButtons.push_back(mYearThreeRaiseGoodsByFiveButton);
	mResourcesGoodsButtons.push_back(mYearThreeRaiseGoodsByTenButton);

	mResourcesGoodsButtons.push_back(mYearFourLowerGoodsByTenButton);
	mResourcesGoodsButtons.push_back(mYearFourLowerGoodsByFiveButton);
	mResourcesGoodsButtons.push_back(mYearFourLowerGoodsByOneButton);
	mResourcesGoodsButtons.push_back(mYearFourRaiseGoodsByOneButton);
	mResourcesGoodsButtons.push_back(mYearFourRaiseGoodsByFiveButton);
	mResourcesGoodsButtons.push_back(mYearFourRaiseGoodsByTenButton);

	mResourcesGoodsButtons.push_back(mYearFiveLowerGoodsByTenButton);
	mResourcesGoodsButtons.push_back(mYearFiveLowerGoodsByFiveButton);
	mResourcesGoodsButtons.push_back(mYearFiveLowerGoodsByOneButton);
	mResourcesGoodsButtons.push_back(mYearFiveRaiseGoodsByOneButton);
	mResourcesGoodsButtons.push_back(mYearFiveRaiseGoodsByFiveButton);
	mResourcesGoodsButtons.push_back(mYearFiveRaiseGoodsByTenButton);
	
	mResourcesTechButtons.push_back(mYearOneLowerTechByTenButton);
	mResourcesTechButtons.push_back(mYearOneLowerTechByFiveButton);
	mResourcesTechButtons.push_back(mYearOneLowerTechByOneButton);
	mResourcesTechButtons.push_back(mYearOneRaiseTechByOneButton);
	mResourcesTechButtons.push_back(mYearOneRaiseTechByFiveButton);
	mResourcesTechButtons.push_back(mYearOneRaiseTechByTenButton);
	
	mResourcesTechButtons.push_back(mYearTwoLowerTechByTenButton);
	mResourcesTechButtons.push_back(mYearTwoLowerTechByFiveButton);
	mResourcesTechButtons.push_back(mYearTwoLowerTechByOneButton);
	mResourcesTechButtons.push_back(mYearTwoRaiseTechByOneButton);
	mResourcesTechButtons.push_back(mYearTwoRaiseTechByFiveButton);
	mResourcesTechButtons.push_back(mYearTwoRaiseTechByTenButton);
	
	mResourcesTechButtons.push_back(mYearThreeLowerTechByTenButton);
	mResourcesTechButtons.push_back(mYearThreeLowerTechByFiveButton);
	mResourcesTechButtons.push_back(mYearThreeLowerTechByOneButton);
	mResourcesTechButtons.push_back(mYearThreeRaiseTechByOneButton);
	mResourcesTechButtons.push_back(mYearThreeRaiseTechByFiveButton);
	mResourcesTechButtons.push_back(mYearThreeRaiseTechByTenButton);
	
	mResourcesTechButtons.push_back(mYearFourLowerTechByTenButton);
	mResourcesTechButtons.push_back(mYearFourLowerTechByFiveButton);
	mResourcesTechButtons.push_back(mYearFourLowerTechByOneButton);
	mResourcesTechButtons.push_back(mYearFourRaiseTechByOneButton);
	mResourcesTechButtons.push_back(mYearFourRaiseTechByFiveButton);
	mResourcesTechButtons.push_back(mYearFourRaiseTechByTenButton);
	
	mResourcesTechButtons.push_back(mYearFiveLowerTechByTenButton);
	mResourcesTechButtons.push_back(mYearFiveLowerTechByFiveButton);
	mResourcesTechButtons.push_back(mYearFiveLowerTechByOneButton);
	mResourcesTechButtons.push_back(mYearFiveRaiseTechByOneButton);
	mResourcesTechButtons.push_back(mYearFiveRaiseTechByFiveButton);
	mResourcesTechButtons.push_back(mYearFiveRaiseTechByTenButton);

	std::vector<std::shared_ptr<GUIText> > foodText;
	std::vector<std::shared_ptr<GUIText> > goodsText;
	std::vector<std::shared_ptr<GUIText> > techText;
	foodText.push_back(mYearOneFoodText);
	foodText.push_back(mYearTwoFoodText);
	foodText.push_back(mYearThreeFoodText);
	foodText.push_back(mYearFourFoodText);
	foodText.push_back(mYearFiveFoodText);
	goodsText.push_back(mYearOneGoodsText);
	goodsText.push_back(mYearTwoGoodsText);
	goodsText.push_back(mYearThreeGoodsText);
	goodsText.push_back(mYearFourGoodsText);
	goodsText.push_back(mYearFiveGoodsText);
	techText.push_back(mYearOneTechText);
	techText.push_back(mYearTwoTechText);
	techText.push_back(mYearThreeTechText);
	techText.push_back(mYearFourTechText);
	techText.push_back(mYearFiveTechText);
	for(int i = 0; i < 5; ++i)
	{
		int amount = -10;
		for(int j = 0; j < 6; ++j)
		{
			mResourcesFoodButtons[j + i*6]->setOnClickFunction([=]()
			{
				foodText[i]->setText(stringToInt(foodText[i]->getText()) + amount);
				if(stringToInt(foodText[i]->getText()) < 0)
					foodText[i]->setText(0);
			});
			if(j == 0 || j == 4)
				amount += 5;
			else if(j == 1 || j == 3)
				amount += 4;
			else
				amount += 2;
		}
	}
	for(int i = 0; i < 5; ++i)
	{
		int amount = -10;
		for(int j = 0; j < 6; ++j)
		{
			mResourcesGoodsButtons[j + i*6]->setOnClickFunction([=]()
			{
				goodsText[i]->setText(stringToInt(goodsText[i]->getText()) + amount);
				if(stringToInt(goodsText[i]->getText()) < 0)
					goodsText[i]->setText(0);
			});
			if(j == 0 || j == 4)
				amount += 5;
			else if(j == 1 || j == 3)
				amount += 4;
			else
				amount += 2;
		}
	}
	for(int i = 0; i < 5; ++i)
	{
		int amount = -10;
		for(int j = 0; j < 6; ++j)
		{
			mResourcesTechButtons[j + i*6]->setOnClickFunction([=]()
			{
				techText[i]->setText(stringToInt(techText[i]->getText()) + amount);
				if(stringToInt(techText[i]->getText()) < 0)
					techText[i]->setText(0);
			});
			if(j == 0 || j == 4)
				amount += 5;
			else if(j == 1 || j == 3)
				amount += 4;
			else
				amount += 2;
		}
	}

	//--------------------------------------------------------------------------
	/*Taxes, år ett*/
	mYearOneLowerTaxesButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearOneTaxesText->getText()) - taxChange;
		if(amount >= 0)
			mYearOneTaxesText->setText(amount);
	});
	mYearOneRaiseTaxesButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearOneTaxesText->getText()) + taxChange;
		mYearOneTaxesText->setText(amount);
	});
	mYearTwoLowerTaxesButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearTwoTaxesText->getText()) - taxChange;
		if(amount >= 0)
			mYearTwoTaxesText->setText(amount);
	});
	mYearTwoRaiseTaxesButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearTwoTaxesText->getText()) + taxChange;
		mYearTwoTaxesText->setText(amount);
	});
	mYearThreeLowerTaxesButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearThreeTaxesText->getText()) - taxChange;
		if(amount >= 0)
			mYearThreeTaxesText->setText(amount);
	});
	mYearThreeRaiseTaxesButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearThreeTaxesText->getText()) + taxChange;
		mYearThreeTaxesText->setText(amount);
	});
	mYearFourLowerTaxesButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFourTaxesText->getText()) - taxChange;
		if(amount >= 0)
			mYearFourTaxesText->setText(amount);
	});
	mYearFourRaiseTaxesButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFourTaxesText->getText()) + taxChange;
		mYearFourTaxesText->setText(amount);
	});
	mYearFiveLowerTaxesButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFiveTaxesText->getText()) - taxChange;
		if(amount >= 0)
			mYearFiveTaxesText->setText(amount);
	});
	mYearFiveRaiseTaxesButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFiveTaxesText->getText()) + taxChange;
		mYearFiveTaxesText->setText(amount);
	});
}

 /**/
void Communist::initializeGuiFunctions()
{
	fiveYearGuiFunctions();
	/*Fem års plan knappen på interface  */
	mCommunistFiveYearPlanButton->setOnClickFunction([=]()		
	{
		mCommunistMainWindow->setEnabled(false, true);
		mTaxesWindow->setEnabled(true, true);

		mTaxesWindow->setVisible(true); 
		mCommunistFiveYearPlanButton->setTexture(CommunistButtons["FiveYearPlanIsPressed"]);
	});
	/*Propaganda knappen på interface*/
	mCommunistPropagandaButton->setOnClickFunction([=]()		
	{ 
		mCommunistMainWindow->setEnabled(false, true);
		mPropagandaWindowFirst->setEnabled(true, true);

		mPropagandaBuyFoodButton->setTexture(std::pair<sf::FloatRect, sf::Texture*>
			(mPropagandaBuyFoodButton->getRectangle(), PropagandaFood[Randomizer::getInstance()->randomNr(PropagandaFood.size(), 0)]));
		mPropagandaBuyGoodsButton->setTexture(std::pair<sf::FloatRect, sf::Texture*>
			(mPropagandaBuyGoodsButton->getRectangle(), PropagandaGoods[Randomizer::getInstance()->randomNr(PropagandaGoods.size(), 0)]));
		mPropagandaBuyTechButton->setTexture(std::pair<sf::FloatRect, sf::Texture*>
			(mPropagandaBuyTechButton->getRectangle(), PropagandaTech[Randomizer::getInstance()->randomNr(PropagandaTech.size(), 0)]));

		mPropagandaWindowFirst->setVisible(true); 
		mCommunistPropagandaButton->setTexture(CommunistButtons["PropagandaIsPressed"]);
	});
	/*Upgrade knappen på interface*/
	mCommunistUpgradeButton->setOnClickFunction([=]()			
	{ 
		mCommunistMainWindow->setEnabled(false, true);
		mUpgradeWindow->setEnabled(true, true);

		mUpgradeWindow->setVisible(true); 
		mCommunistUpgradeButton->setTexture(CommunistButtons["UpgradeIsPressed"]);
	});
	/*Gå till nästa slide*/
	mGoToNextSlideButton->setOnClickFunction([=]()				
	{ 
		mTaxesWindow->setEnabled(false, true);
		mResourcesWindow->setEnabled(true, true);

		mTaxesWindow->setVisible(false); mResourcesWindow->setVisible(true); 
	});
	/*Gå till föregående slide*/
	mGoToPreviousSlideButton->setOnClickFunction([=]()			
	{
		mResourcesWindow->setEnabled(false, true);
		mTaxesWindow->setEnabled(true, true);
		
		mResourcesWindow->setVisible(false); mTaxesWindow->setVisible(true); 
	});
	
	/*Export knappen på interface*/
	mCommunistExportButton->setOnClickFunction([=]()			
	{ 	
		mCommunistMainWindow->setEnabled(false, true);
		mExportWindow->setEnabled(true, true);
		mExportWindow->setVisible(true); 
		mCommunistExportButton->setTexture(CommunistButtons["ExportIsPressed"]);
	});


	mTaxesCloseButton->setOnClickFunction([=]()					
	{ 
		mCommunistMainWindow->setEnabled(true, true);
		updateAllResources();
		mTaxesWindow->setVisible(false); 
		mCommunistFiveYearPlanButton->setTexture(CommunistButtons["FiveYearPlan"]);
	});
	
	mResourcesCloseButton->setOnClickFunction([=]()				
	{ 
		mCommunistMainWindow->setEnabled(true, true);
		updateAllResources();
		mResourcesWindow->setVisible(false); 
		mCommunistFiveYearPlanButton->setTexture(CommunistButtons["FiveYearPlan"]);
	});
	/*Stänger propagandafönster nummer ett*/
	mPropagandaWindowFirstCloseButton->setOnClickFunction([=]()	
	{ 
		mCommunistMainWindow->setEnabled(true, true);

		mPropagandaWindowFirst->setVisible(false);
		mCommunistPropagandaButton->setTexture(CommunistButtons["Propaganda"]);
	});
	/*Stänger propagandafönster nummer två*/
	mPropagandaWindowSecondCloseButton->setOnClickFunction([=]()
	{
		
		mPropagandaWindowFirst->setEnabled(true, true);
		mPropagandaWindowSecond->setVisible(false);
		mCommunistPropagandaButton->setTexture(CommunistButtons["Propaganda"]);
	});

	mPropagandaBuyFoodButton->setOnClickFunction([=]()
	{
		mPropagandaWindowSecond->setEnabled(true, true);
		//buyPropagandaFood(getRound());

		mPropagandaWindowFirst->setEnabled(false, true);
		mPropagandaWindowSecond->setVisible(true);

		mShowBoughtPropaganda->setTexture(std::pair<sf::FloatRect, sf::Texture*>
			(mShowBoughtPropaganda->getRectangle(), mPropagandaBuyFoodButton->getTexture()));
		mShowBoughtPropaganda->setScale(0.7, 0.7);

		//std::cout << "Food: " << mFood << std::endl;
	});
	mPropagandaBuyGoodsButton->setOnClickFunction([=]()
	{
		mPropagandaWindowSecond->setEnabled(true, true);
		//buyPropagandaGoods(getRound());

		mPropagandaWindowFirst->setEnabled(false, true);
		mPropagandaWindowSecond->setVisible(true);

		mShowBoughtPropaganda->setTexture(std::pair<sf::FloatRect, sf::Texture*>
			(mShowBoughtPropaganda->getRectangle(), mPropagandaBuyGoodsButton->getTexture()));
		mShowBoughtPropaganda->setScale(0.7, 0.7);
		//std::cout << "Goods: " << mGoods << std::endl;
	});
	mPropagandaBuyTechButton->setOnClickFunction([=]()
	{
		mPropagandaWindowSecond->setEnabled(true, true);
		//buyPropagandaTech(getRound());

		mPropagandaWindowFirst->setEnabled(false, true);
		mPropagandaWindowSecond->setVisible(true);

		mShowBoughtPropaganda->setTexture(std::pair<sf::FloatRect, sf::Texture*>
			(mShowBoughtPropaganda->getRectangle(), mPropagandaBuyTechButton->getTexture()));
		mShowBoughtPropaganda->setScale(0.7, 0.7);
		//std::cout << "Tech: " << mTech << std::endl;
	});

	/*Upgrade och cancel knappar för nuclear*/
	mUpgradeNuclearWeaponButton->setOnClickFunction([=]() 
	{ 
		int amount = stringToInt(mBuyNuclearText->getText());
		++amount;
		mBuyNuclearText->setText(amount);
	});		
	mCancelUpgradeNuclearWeaponButton->setOnClickFunction([=]() 
	{
		mBuyNuclearText->setText(mNuclearText->getText());
	});
			
	/*Upgrade och cancel knappar för spaceprogram*/
	mUpgradeSpaceProgramButton->setOnClickFunction([=]()  
	{
		int amount = stringToInt(mBuySpaceProgramText->getText());
		++amount;
		mBuySpaceProgramText->setText(amount);
	});		
	mCancelUpgradeSpaceProgramButton->setOnClickFunction([=]() 
	{
		mBuySpaceProgramText->setText(mSpaceText->getText());
	});

	/*Upgrade och cancel knappar för spynetwork*/
	mUpgradeSpyNetworkButton->setOnClickFunction([=]()	 
	{
		int amount = stringToInt(mBuySpyNetworkText->getText());
		++amount;
		mBuySpyNetworkText->setText(amount);
		
	});		
	mCancelUpgradeSpyNetworkButton->setOnClickFunction([=]() 
	{
		mBuySpyNetworkText->setText(mSpyText->getText());
	});

	/*Stänger ned Upgradefönstret*/
	mUpgradeCloseButton->setOnClickFunction([=]()				
	{ 
		mCommunistMainWindow->setEnabled(true, true);

		mUpgradeWindow->setVisible(false); 
		mCommunistUpgradeButton->setTexture(CommunistButtons["Upgrade"]);
		/*mNuclearWeapon = mNuclearWeaponUpdate; mNuclearText->setText(intToString(getNuclearWeapon()));
		mSpaceProgram = mSpaceProgramUpdate; mSpaceText->setText(intToString(getSpaceProgram()));
		mSpyNetwork = mSpyNetworkUpdate; mSpyText->setText(intToString(getSpyNetwork())); std::cout << "HERRRRRO" << std::endl;*/
	});

	/*Stänger ned exportfönstret*/
	mExportCloseButton->setOnClickFunction([=]() 
	{ 
		mCommunistMainWindow->setEnabled(true, true);

		mExportWindow->setVisible(false); 
		mCommunistExportButton->setTexture(CommunistButtons["Export"]);
	});

	/*mUpgradeNuclearWeaponButton->setOnClickFunction(std::bind(&Communist::upgradeNuclearWeapon, this));
	mUpgradeSpaceProgramButton->setOnClickFunction(std::bind(&Communist::upgradeSpaceProgram, this));
	mUpgradeSpyNetworkButton->setOnClickFunction(std::bind(&Communist::upgradeSpyNetwork, this));*/


	/*GUI hantering för valet av general*/
	/*Bläddra mellan generalerna för att välja mellan de fem som finns*/
	mGoToNextPortraitButton->setOnClickFunction([=]()			
	{	
		generalCount++;
		if(generalCount > 4)
			generalCount = 0;

		mFirstGeneralButton->setTexture(std::pair<sf::FloatRect, sf::Texture*>
			(mFirstGeneralButton->getRectangle(), GameManager::getInstance()->getGeneral(generalCount)->getTexture()));
	});

	mGoToPreviousPortraitButton->setOnClickFunction([=]()		
	{	
		generalCount--;
		if(generalCount < 0)
			generalCount = 4;

		mFirstGeneralButton->setTexture(std::pair<sf::FloatRect, sf::Texture*>
			(mFirstGeneralButton->getRectangle(), GameManager::getInstance()->getGeneral(generalCount)->getTexture()));	
	});

	/*När en general har blivit vald*/
	mCloseGeneralWindow->setOnClickFunction([=]()
	{
		mChooseGeneralWindow->setVisible(false);
		mPickedGeneralWindow->setVisible(true);
		mPickedGeneralWindow->setEnabled(true, true);

		mGeneral = GameManager::getInstance()->getGeneral(generalCount);

		mPickedGeneralButton->setTexture(std::pair<sf::FloatRect, sf::Texture*>
			(mPickedGeneralButton->getRectangle(), mGeneral->getTexture())); 

	});
	/*Stänger ner fönster som visar vilken general som blivit vald*/
	mClosePickedGeneralWindow->setOnClickFunction([=]()
	{
		mPickedGeneralWindow->setVisible(false);
		mCommunistGeneralButton->setTexture(std::pair<sf::FloatRect, sf::Texture*>(mCommunistGeneralButton->getRectangle(), mGeneral->getTexture()));
		mCommunistGeneralButton->setScale(0.53, 0.53);
		mTaxesWindow->setVisible(true);
		mTaxesWindow->setEnabled(true, true);
		mCommunistFiveYearPlanButton->setEnabled(true, true);
	});
	mPropagandaBuyFoodButton->setOnClickFunction([=]()
	{
		buyPropagandaFood(getRound());
		std::cout << "Food: " << getYearlyFood(mRound) << std::endl;
	});
	mPropagandaBuyGoodsButton->setOnClickFunction([=]()
	{
		buyPropagandaGoods(getRound());
		std::cout << "Goods: " << getYearlyGoods(mRound) << std::endl;
	});
	mPropagandaBuyTechButton->setOnClickFunction([=]()
	{
		buyPropagandaTech(getRound());
		std::cout << "Tech: " << getYearlyTech(mRound) << std::endl;

	});

	mCommunistEndTurnButton->setOnClickFunction([=]()	
	{
		GameManager::getInstance()->nextRound();
	});
}

void Communist::updateAllResources()
{
	/*År ett*/
	setYearlyResources(1, "food", stringToInt(mYearOneFoodText->getText()));
	setYearlyResources(1, "goods", stringToInt(mYearOneGoodsText->getText()));
	setYearlyResources(1, "tech", stringToInt(mYearOneTechText->getText()));
	setYearlyResources(1, "taxes", stringToInt(mYearOneTaxesText->getText()));
	/*År två*/
	setYearlyResources(2, "food", stringToInt(mYearTwoFoodText->getText()));
	setYearlyResources(2, "goods", stringToInt(mYearTwoGoodsText->getText()));
	setYearlyResources(2, "tech", stringToInt(mYearTwoTechText->getText()));
	setYearlyResources(2, "taxes", stringToInt(mYearTwoTaxesText->getText()));
	/*År tre*/
	setYearlyResources(3, "food", stringToInt(mYearThreeFoodText->getText()));
	setYearlyResources(3, "goods", stringToInt(mYearThreeGoodsText->getText()));
	setYearlyResources(3, "tech", stringToInt(mYearThreeTechText->getText()));
	setYearlyResources(3, "taxes", stringToInt(mYearThreeTaxesText->getText()));
	/*År fyra*/
	setYearlyResources(4, "food", stringToInt(mYearFourFoodText->getText()));
	setYearlyResources(4, "goods", stringToInt(mYearFourGoodsText->getText()));
	setYearlyResources(4, "tech", stringToInt(mYearFourTechText->getText()));
	setYearlyResources(4, "taxes", stringToInt(mYearFourTaxesText->getText()));
	/*År fem*/
	setYearlyResources(5, "food", stringToInt(mYearFiveFoodText->getText()));
	setYearlyResources(5, "goods", stringToInt(mYearFiveGoodsText->getText()));
	setYearlyResources(5, "tech", stringToInt(mYearFiveTechText->getText()));
	setYearlyResources(5, "taxes", stringToInt(mYearFiveTaxesText->getText()));
	
}

void Communist::showGUI()
{
	mCommunistMainWindow->setVisible(true);
}

void Communist::hideGUI()
{
	mCommunistMainWindow->setVisible(false);
} 