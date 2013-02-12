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

		mTaxesWindow->setVisible(true);
	}
}

void Communist::setYearlyResources(int year, std::string key, int value)
{
	mYearVector[year-1][key] += value;
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

	int percent = mFood/resourcesTotal;
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

 /**/
void Communist::initializeCommunistWindow()
{
	loadButtonPosition();
	loadWindowPosition();

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
	mPropagandaWindowFirst->setVisible(false);
	mPropagandaWindowSecond->setVisible(false);

	/*Upgrade GUI window med knappar*/
	mUpgradeWindow						= GUIWindow::create(CommunistWindows["CommunistUpgradeWindow"], mCommunistMainWindow);
	mUpgradeNuclearWeaponButton			= GUIButton::create(CommunistButtons["UpgradeNuclearWeapon"], mUpgradeWindow);
	mUpgradeSpaceProgramButton			= GUIButton::create(CommunistButtons["UpgradeSpaceProgram"], mUpgradeWindow);
	mUpgradeSpyNetworkButton			= GUIButton::create(CommunistButtons["UpgradeSpyNetwork"], mUpgradeWindow);
	mUpgradeCloseButton					= GUIButton::create(CommunistButtons["CloseUpgrade"], mUpgradeWindow);
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
	/*Food, år ett*/
	mYearOneLowerFoodByTenButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearOneFoodText->getText()) - 10;
		if(amount >= 0)
			mYearOneFoodText->setText(amount);
	});
	mYearOneLowerFoodByFiveButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearOneFoodText->getText()) - 5;
		if(amount >= 0)
			mYearOneFoodText->setText(amount);
	});
	mYearOneLowerFoodByOneButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearOneFoodText->getText()) - 1;
		if(amount >= 0)
			mYearOneFoodText->setText(amount);
	});
	mYearOneRaiseFoodByOneButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearOneFoodText->getText()) + 1;
		mYearOneFoodText->setText(amount);
	});
	mYearOneRaiseFoodByFiveButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearOneFoodText->getText()) + 5;
		mYearOneFoodText->setText(amount);
	});
	mYearOneRaiseFoodByTenButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearOneFoodText->getText()) + 10;
		mYearOneFoodText->setText(amount);
	});

	/*Food, år två*/
	mYearTwoLowerFoodByTenButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearTwoFoodText->getText()) - 10;
		if(amount >= 0)
			mYearTwoFoodText->setText(amount);
	});
	mYearTwoLowerFoodByFiveButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearTwoFoodText->getText()) - 5;
		if(amount >= 0)
			mYearTwoFoodText->setText(amount);
	});
	mYearTwoLowerFoodByOneButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearTwoFoodText->getText()) - 1;
		if(amount >= 0)
			mYearTwoFoodText->setText(amount);
	});
	mYearTwoRaiseFoodByOneButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearTwoFoodText->getText()) + 1;
		mYearTwoFoodText->setText(amount);
	});
	mYearTwoRaiseFoodByFiveButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearTwoFoodText->getText()) + 5;
		mYearTwoFoodText->setText(amount);
	});
	mYearTwoRaiseFoodByTenButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearTwoFoodText->getText()) + 10;
		mYearTwoFoodText->setText(amount);
	});

	/*Food, år tre*/
	mYearThreeLowerFoodByTenButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearThreeFoodText->getText()) - 10;
		if(amount >= 0)
			mYearThreeFoodText->setText(amount);
	});
	mYearThreeLowerFoodByFiveButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearThreeFoodText->getText()) - 5;
		if(amount >= 0)
			mYearThreeFoodText->setText(amount);
	});
	mYearThreeLowerFoodByOneButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearThreeFoodText->getText()) - 1;
		if(amount >= 0)
			mYearThreeFoodText->setText(amount);
	});
	mYearThreeRaiseFoodByOneButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearThreeFoodText->getText()) + 1;
		mYearThreeFoodText->setText(amount);
	});
	mYearThreeRaiseFoodByFiveButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearThreeFoodText->getText()) + 5;
		mYearThreeFoodText->setText(amount);
	});
	mYearThreeRaiseFoodByTenButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearThreeFoodText->getText()) + 10;
		mYearThreeFoodText->setText(amount);
	});
	
	/*Food, år fyra*/
	mYearFourLowerFoodByTenButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFourFoodText->getText()) - 10;
		if(amount >= 0)
			mYearFourFoodText->setText(amount);
	});
	mYearFourLowerFoodByFiveButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFourFoodText->getText()) - 5;
		if(amount >= 0)
			mYearFourFoodText->setText(amount);
	});
	mYearFourLowerFoodByOneButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFourFoodText->getText()) - 1;
		if(amount >= 0)
			mYearFourFoodText->setText(amount);
	});
	mYearFourRaiseFoodByOneButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFourFoodText->getText()) + 1;
		mYearFourFoodText->setText(amount);
	});
	mYearFourRaiseFoodByFiveButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFourFoodText->getText()) + 5;
		mYearFourFoodText->setText(amount);
	});
	mYearFourRaiseFoodByTenButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFourFoodText->getText()) + 10;
		mYearFourFoodText->setText(amount);
	});
	
	/*Food, år fem*/
	mYearFiveLowerFoodByTenButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFiveFoodText->getText()) - 10;
		if(amount >= 0)
			mYearFiveFoodText->setText(amount);
	});
	mYearFiveLowerFoodByFiveButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFiveFoodText->getText()) - 5;
		if(amount >= 0)
			mYearFiveFoodText->setText(amount);
	});
	mYearFiveLowerFoodByOneButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFiveFoodText->getText()) - 1;
		if(amount >= 0)
			mYearFiveFoodText->setText(amount);
	});
	mYearFiveRaiseFoodByOneButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFiveFoodText->getText()) + 1;
		mYearFiveFoodText->setText(amount);
	});
	mYearFiveRaiseFoodByFiveButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFiveFoodText->getText()) + 5;
		mYearFiveFoodText->setText(amount);
	});
	mYearFiveRaiseFoodByTenButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFiveFoodText->getText()) + 10;
		mYearFiveFoodText->setText(amount);
	});
	//-------------------------------------------------------------------------------
	/*Goods, år ett*/
	mYearOneLowerGoodsByTenButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearOneGoodsText->getText()) - 10;
		if(amount >= 0)
			mYearOneGoodsText->setText(amount);
	});
	mYearOneLowerGoodsByFiveButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearOneGoodsText->getText()) - 5;
		if(amount >= 0)
			mYearOneGoodsText->setText(amount);
	});
	mYearOneLowerGoodsByOneButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearOneGoodsText->getText()) - 1;
		if(amount >= 0)
			mYearOneGoodsText->setText(amount);
	});
	mYearOneRaiseGoodsByOneButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearOneGoodsText->getText()) + 1;
		mYearOneGoodsText->setText(amount);
	});
	mYearOneRaiseGoodsByFiveButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearOneGoodsText->getText()) + 5;
		mYearOneGoodsText->setText(amount);
	});
	mYearOneRaiseGoodsByTenButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearOneGoodsText->getText()) + 10;
		mYearOneGoodsText->setText(amount);
	});
	
	/*Goods, år två*/
	mYearTwoLowerGoodsByTenButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearTwoGoodsText->getText()) - 10;
		if(amount >= 0)
			mYearTwoGoodsText->setText(amount);
	});
	mYearTwoLowerGoodsByFiveButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearTwoGoodsText->getText()) - 5;
		if(amount >= 0)
			mYearTwoGoodsText->setText(amount);
	});
	mYearTwoLowerGoodsByOneButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearTwoGoodsText->getText()) - 1;
		if(amount >= 0)
			mYearTwoGoodsText->setText(amount);
	});
	mYearTwoRaiseGoodsByOneButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearTwoGoodsText->getText()) + 1;
		mYearTwoGoodsText->setText(amount);
	});
	mYearTwoRaiseGoodsByFiveButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearTwoGoodsText->getText()) + 5;
		mYearTwoGoodsText->setText(amount);
	});
	mYearTwoRaiseGoodsByTenButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearTwoGoodsText->getText()) + 10;
		mYearTwoGoodsText->setText(amount);
	});
	
	/*Goods, år tre*/
	mYearThreeLowerGoodsByTenButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearThreeGoodsText->getText()) - 10;
		if(amount >= 0)
			mYearThreeGoodsText->setText(amount);
	});
	mYearThreeLowerGoodsByFiveButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearThreeGoodsText->getText()) - 5;
		if(amount >= 0)
			mYearThreeGoodsText->setText(amount);
	});
	mYearThreeLowerGoodsByOneButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearThreeGoodsText->getText()) - 1;
		if(amount >= 0)
			mYearThreeGoodsText->setText(amount);
	});
	mYearThreeRaiseGoodsByOneButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearThreeGoodsText->getText()) + 1;
		mYearThreeGoodsText->setText(amount);
	});
	mYearThreeRaiseGoodsByFiveButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearThreeGoodsText->getText()) + 5;
		mYearThreeGoodsText->setText(amount);
	});
	mYearThreeRaiseGoodsByTenButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearThreeGoodsText->getText()) + 10;
		mYearThreeGoodsText->setText(amount);
	});
	
	/*Goods, år fyra*/
	mYearFourLowerGoodsByTenButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFourGoodsText->getText()) - 10;
		if(amount >= 0)
			mYearFourGoodsText->setText(amount);
	});
	mYearFourLowerGoodsByFiveButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFourGoodsText->getText()) - 5;
		if(amount >= 0)
			mYearFourGoodsText->setText(amount);
	});
	mYearFourLowerGoodsByOneButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFourGoodsText->getText()) - 1;
		if(amount >= 0)
			mYearFourGoodsText->setText(amount);
	});
	mYearFourRaiseGoodsByOneButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFourGoodsText->getText()) + 1;
		mYearFourGoodsText->setText(amount);
	});
	mYearFourRaiseGoodsByFiveButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFourGoodsText->getText()) + 5;
		mYearFourGoodsText->setText(amount);
	});
	mYearFourRaiseGoodsByTenButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFourGoodsText->getText()) + 10;
		mYearFourGoodsText->setText(amount);
	});
	
	/*Goods, år fem*/
	mYearFiveLowerGoodsByTenButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFiveGoodsText->getText()) - 10;
		if(amount >= 0)
			mYearFiveGoodsText->setText(amount);
	});
	mYearFiveLowerGoodsByFiveButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFiveGoodsText->getText()) - 5;
		if(amount >= 0)
			mYearFiveGoodsText->setText(amount);
	});
	mYearFiveLowerGoodsByOneButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFiveGoodsText->getText()) - 1;
		if(amount >= 0)
			mYearFiveGoodsText->setText(amount);
	});
	mYearFiveRaiseGoodsByOneButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFiveGoodsText->getText()) + 1;
		mYearFiveGoodsText->setText(amount);
	});
	mYearFiveRaiseGoodsByFiveButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFiveGoodsText->getText()) + 5;
		mYearFiveGoodsText->setText(amount);
	});
	mYearFiveRaiseGoodsByTenButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFiveGoodsText->getText()) + 10;
		mYearFiveGoodsText->setText(amount);
	});
	//--------------------------------------------------------------------------------------
	/*Tech, år ett*/
	mYearOneLowerTechByTenButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearOneTechText->getText()) - 10;
		if(amount >= 0)
			mYearOneTechText->setText(amount);
	});
	mYearOneLowerTechByFiveButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearOneTechText->getText()) - 5;
		if(amount >= 0)
			mYearOneTechText->setText(amount);
	});
	mYearOneLowerTechByOneButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearOneTechText->getText()) - 1;
		if(amount >= 0)
			mYearOneTechText->setText(amount);
	});
	mYearOneRaiseTechByOneButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearOneTechText->getText()) + 1;
		mYearOneTechText->setText(amount);
	});
	mYearOneRaiseTechByFiveButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearOneTechText->getText()) + 5;
		mYearOneTechText->setText(amount);
	});
	mYearOneRaiseTechByTenButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearOneTechText->getText()) + 10;
		mYearOneTechText->setText(amount);
	});
	
	/*Tech, år två*/
	mYearTwoLowerTechByTenButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearTwoTechText->getText()) - 10;
		if(amount >= 0)
			mYearTwoTechText->setText(amount);
	});
	mYearTwoLowerTechByFiveButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearTwoTechText->getText()) - 5;
		if(amount >= 0)
			mYearTwoTechText->setText(amount);
	});
	mYearTwoLowerTechByOneButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearTwoTechText->getText()) - 1;
		if(amount >= 0)
			mYearTwoTechText->setText(amount);
	});
	mYearTwoRaiseTechByOneButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearTwoTechText->getText()) + 1;
		mYearTwoTechText->setText(amount);
	});
	mYearTwoRaiseTechByFiveButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearTwoTechText->getText()) + 5;
		mYearTwoTechText->setText(amount);
	});
	mYearTwoRaiseTechByTenButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearTwoTechText->getText()) + 10;
		mYearTwoTechText->setText(amount);
	});
	
	/*Tech, år tre*/
	mYearThreeLowerTechByTenButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearThreeTechText->getText()) - 10;
		if(amount >= 0)
			mYearThreeTechText->setText(amount);
	});
	mYearThreeLowerTechByFiveButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearThreeTechText->getText()) - 5;
		if(amount >= 0)
			mYearThreeTechText->setText(amount);
	});
	mYearThreeLowerTechByOneButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearThreeTechText->getText()) - 1;
		if(amount >= 0)
			mYearThreeTechText->setText(amount);
	});
	mYearThreeRaiseTechByOneButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearThreeTechText->getText()) + 1;
		mYearThreeTechText->setText(amount);
	});
	mYearThreeRaiseTechByFiveButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearThreeTechText->getText()) + 5;
		mYearThreeTechText->setText(amount);
	});
	mYearThreeRaiseTechByTenButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearThreeTechText->getText()) + 10;
		mYearThreeTechText->setText(amount);
	});
	
	/*Tech, år fyra*/
	mYearFourLowerTechByTenButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFourTechText->getText()) - 10;
		if(amount >= 0)
			mYearFourTechText->setText(amount);
	});
	mYearFourLowerTechByFiveButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFourTechText->getText()) - 5;
		if(amount >= 0)
			mYearFourTechText->setText(amount);
	});
	mYearFourLowerTechByOneButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFourTechText->getText()) - 1;
		if(amount >= 0)
			mYearFourTechText->setText(amount);
	});
	mYearFourRaiseTechByOneButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFourTechText->getText()) + 1;
		mYearFourTechText->setText(amount);
	});
	mYearFourRaiseTechByFiveButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFourTechText->getText()) + 5;
		mYearFourTechText->setText(amount);
	});
	mYearFourRaiseTechByTenButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFourTechText->getText()) + 10;
		mYearFourTechText->setText(amount);
	});
	
	/*Tech, år fem*/
	mYearFiveLowerTechByTenButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFiveTechText->getText()) - 10;
		if(amount >= 0)
			mYearFiveTechText->setText(amount);
	});
	mYearFiveLowerTechByFiveButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFiveTechText->getText()) - 5;
		if(amount >= 0)
			mYearFiveTechText->setText(amount);
	});
	mYearFiveLowerTechByOneButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFiveTechText->getText()) - 1;
		if(amount >= 0)
			mYearFiveTechText->setText(amount);
	});
	mYearFiveRaiseTechByOneButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFiveTechText->getText()) + 1;
		mYearFiveTechText->setText(amount);
	});
	mYearFiveRaiseTechByFiveButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFiveTechText->getText()) + 5;
		mYearFiveTechText->setText(amount);
	});
	mYearFiveRaiseTechByTenButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFiveTechText->getText()) + 10;
		mYearFiveTechText->setText(amount);
	});
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
		int amount = stringToInt(mYearThreeTaxesText->getText()) + taxChange;
		mYearThreeTaxesText->setText(amount);
	});
	mYearThreeLowerTaxesButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearThreeTaxesText->getText()) - taxChange;
		if(amount >= 0)
			mYearThreeTaxesText->setText(amount);
	});
	mYearThreeRaiseTaxesButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearOneTaxesText->getText()) + taxChange;
		mYearThreeTaxesText->setText(amount);
	});
	mYearFourLowerTaxesButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearOneTaxesText->getText()) - taxChange;
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

		mTaxesWindow->setVisible(false); 
		mCommunistFiveYearPlanButton->setTexture(CommunistButtons["FiveYearPlan"]);
	});
	
	mResourcesCloseButton->setOnClickFunction([=]()				
	{ 
		mCommunistMainWindow->setEnabled(true, true);

		mResourcesWindow->setVisible(false); 
		mCommunistFiveYearPlanButton->setTexture(CommunistButtons["FiveYearPlan"]);
	});

	mPropagandaWindowFirstCloseButton->setOnClickFunction([=]()	
	{ 
		mCommunistMainWindow->setEnabled(true, true);

		mPropagandaWindowFirst->setVisible(false);
		mCommunistPropagandaButton->setTexture(CommunistButtons["Propaganda"]);
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

		mGeneral = GameManager::getInstance()->getGeneral(generalCount);

		mPickedGeneralButton->setTexture(std::pair<sf::FloatRect, sf::Texture*>
			(mPickedGeneralButton->getRectangle(), mGeneral->getTexture())); 

		/**/
		std::shared_ptr<GUIElement> _test = mPickedGeneralWindow;
		std::shared_ptr<GUIButton> _generalButton = mCommunistGeneralButton;
		std::shared_ptr<President> _general = mGeneral;
		std::shared_ptr<GUIWindow> _fiveYearPlan = mTaxesWindow;
		Timer::setTimer([=]()
		{
			_test->setVisible(false);

			_generalButton->setTexture(std::pair<sf::FloatRect, sf::Texture*>(_generalButton->getRectangle(), _general->getTexture()));
			_generalButton->setScale(0.53, 0.53);
			_fiveYearPlan->setVisible(true);
			_fiveYearPlan->setEnabled(true, true);
		
		}, 
			5000, 1);//antal millisekunder fönstret visas
	});

	mPropagandaBuyFoodButton->setOnClickFunction([=]()
	{
		buyPropagandaFood(getRound());
		std::cout << "Food: " << mFood << std::endl;
	});
	mPropagandaBuyGoodsButton->setOnClickFunction([=]()
	{
		buyPropagandaGoods(getRound());
		std::cout << "Goods: " << mGoods << std::endl;
	});
	mPropagandaBuyTechButton->setOnClickFunction([=]()
	{
		buyPropagandaTech(getRound());
		std::cout << "Tech: " << mTech << std::endl;
	});

	mCommunistEndTurnButton->setOnClickFunction([=]()	
	{
		GameManager::getInstance()->nextRound();
	});
}

void Communist::showGUI()
{
	mCommunistMainWindow->setVisible(true);
}

void Communist::hideGUI()
{
	mCommunistMainWindow->setVisible(false);
} 