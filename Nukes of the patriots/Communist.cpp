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

static int foodCost		= 10;
static int goodsCost	= 20;
static int techCost		= 30;
static int taxChange	= 5;

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



/*	Uppgraderar mNuclearWeapon med ett
	Kostar 10 mGoods och 5 mTech*/
void Communist::upgradeNuclearWeapon()
{
	mGoodsUpdate	-= 10;
	mTechUpdate		-= 5;
	
	++mNuclearWeaponUpdate;
	std::cout << "KLICK" << std::endl;
}

/*	Uppgraderar mSpaceProgram med ett
	Kostar 5 mGoods multiplicerat med den nuvarande nivån
	och 10 mTech multiplicerat med den nuvarande nivån*/
void Communist::upgradeSpaceProgram()
{
	if(mSpaceProgramUpdate > 0)
	{
		mGoodsUpdate	-= 5 * mSpaceProgramUpdate;
		mTechUpdate		-= 10 * mSpaceProgramUpdate;
	}
	else
	{
		mGoodsUpdate	-= 5;
		mTechUpdate		-= 10;
	}
	++mSpaceProgramUpdate;
}

/*	
	Uppgraderar mSpyNetwork med ett
	Kostar 10 mTech multiplicerat med den nuvarande nivån
															 */
void Communist::upgradeSpyNetwork()
{
	if(mSpyNetworkUpdate > 0)
	{
		mTechUpdate -= 10 * mSpyNetworkUpdate;
	}
	else
	{
		mTechUpdate -= 10;
	}
	++mSpyNetworkUpdate;
} 

//--------------------------------------------
/*Funktioner som ger medlemsvariabler nya värden*/
void Communist::setFood(int value)
{
 	mFoodUpdate		+= value;
	mCurrencyUpdate -= value * foodCost;
}

void Communist::setGoods(int value)
{
	mGoodsUpdate	+= value;
	mCurrencyUpdate -= value * goodsCost;
}

void Communist::setTech(int value)
{
	mTechUpdate		+= value;
	mCurrencyUpdate -= value * techCost;
}

int Communist::increaseTaxCost(int currentTax)
{
	currentTax += taxChange;
	
	return currentTax;
}
  
int Communist::decreaseTaxCost(int currentTax)
{
	currentTax -= taxChange;
	
	return currentTax;
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
	mCurrency -= foodCost * 10;
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
	mCurrency -= goodsCost * 10;
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
	mCurrency -= techCost * 10;
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

	mCommunistMainWindow			= std::make_shared<GUIWindow>(CommunistWindows["CommunistInterface"]);
	mCommunistGeneralButton			= std::make_shared<GUIButton>(CommunistButtons["General"], mCommunistMainWindow);
	mCommunistFiveYearPlanButton    = std::make_shared<GUIButton>(CommunistButtons["FiveYearPlan"], mCommunistMainWindow);
	mCommunistPropagandaButton		= std::make_shared<GUIButton>(CommunistButtons["Propaganda"], mCommunistMainWindow);
	mCommunistUpgradeButton			= std::make_shared<GUIButton>(CommunistButtons["Upgrade"], mCommunistMainWindow);
	mCommunistExportButton			= std::make_shared<GUIButton>(CommunistButtons["Export"], mCommunistMainWindow);
	mCommunistEndTurnButton			= std::make_shared<GUIButton>(CommunistButtons["EndTurn"], mCommunistMainWindow);

	/*GUI text för utskrift av värden på kapitalisternas interface*/
	mNuclearText = std::make_shared<GUIText>(sf::FloatRect(962, 16, 40, 40), intToString(getNuclearWeapon()), mCommunistMainWindow);
	mSpaceText	 = std::make_shared<GUIText>(sf::FloatRect(962, 228, 40, 40), intToString(getSpaceProgram()), mCommunistMainWindow);
	mSpyText	 = std::make_shared<GUIText>(sf::FloatRect(962, 440, 40, 40), intToString(getSpyNetwork()), mCommunistMainWindow);
	mFoodText	 = std::make_shared<GUIText>(sf::FloatRect(160, 16, 40, 40), intToString(getFood()), mCommunistMainWindow);
	mGoodsText   = std::make_shared<GUIText>(sf::FloatRect(160, 228, 40, 40), intToString(getGoods()), mCommunistMainWindow);
	mTechText	 = std::make_shared<GUIText>(sf::FloatRect(160, 440, 40, 40), intToString(getTech()), mCommunistMainWindow);	

	/*Taxes fönster med knappar*/
	mTaxesWindow					= std::make_shared<GUIWindow>(CommunistWindows["CommunistTaxesWindow"], mCommunistMainWindow);	
	mYearOneLowerTaxesButton		= std::make_shared<GUIButton>(CommunistButtons["YearOneLowerTaxes"], mTaxesWindow);	
	mYearOneRaiseTaxesButton		= std::make_shared<GUIButton>(CommunistButtons["YearOneRaiseTaxes"], mTaxesWindow);	
	mYearTwoLowerTaxesButton		= std::make_shared<GUIButton>(CommunistButtons["YearTwoLowerTaxes"], mTaxesWindow);	
	mYearTwoRaiseTaxesButton		= std::make_shared<GUIButton>(CommunistButtons["YearTwoRaiseTaxes"], mTaxesWindow);	
	mYearThreeLowerTaxesButton		= std::make_shared<GUIButton>(CommunistButtons["YearThreeLowerTaxes"], mTaxesWindow);	
	mYearThreeRaiseTaxesButton		= std::make_shared<GUIButton>(CommunistButtons["YearThreeRaiseTaxes"], mTaxesWindow);	
	mYearFourLowerTaxesButton		= std::make_shared<GUIButton>(CommunistButtons["YearFourLowerTaxes"], mTaxesWindow);	
	mYearFourRaiseTaxesButton		= std::make_shared<GUIButton>(CommunistButtons["YearFourRaiseTaxes"], mTaxesWindow);	
	mYearFiveLowerTaxesButton		= std::make_shared<GUIButton>(CommunistButtons["YearFiveLowerTaxes"], mTaxesWindow);	
	mYearFiveRaiseTaxesButton		= std::make_shared<GUIButton>(CommunistButtons["YearFiveRaiseTaxes"], mTaxesWindow);	
	mTaxesWindow->setVisible(false);

	/*Resource GUI window med knappar*/
	mResourcesWindow					= std::make_shared<GUIWindow>(CommunistWindows["CommunistResourcesWindow"], mCommunistMainWindow);	
	/*Food*/
	mYearOneLowerFoodByTenButton		= std::make_shared<GUIButton>(CommunistButtons["YearOneLowerFoodByTen"], mResourcesWindow);	
	mYearOneLowerFoodByFiveButton		= std::make_shared<GUIButton>(CommunistButtons["YearOneLowerFoodByFive"], mResourcesWindow);	
	mYearOneLowerFoodByOneButton		= std::make_shared<GUIButton>(CommunistButtons["YearOneLowerFoodByOne"], mResourcesWindow);	
	mYearOneRaiseFoodByOneButton		= std::make_shared<GUIButton>(CommunistButtons["YearOneRaiseFoodByOne"], mResourcesWindow);	
	mYearOneRaiseFoodByFiveButton		= std::make_shared<GUIButton>(CommunistButtons["YearOneRaiseFoodByFive"], mResourcesWindow);	
	mYearOneRaiseFoodByTenButton		= std::make_shared<GUIButton>(CommunistButtons["YearOneRaiseFoodByTen"], mResourcesWindow);	
	
	mYearTwoLowerFoodByTenButton		= std::make_shared<GUIButton>(CommunistButtons["YearTwoLowerFoodByTen"], mResourcesWindow);	
	mYearTwoLowerFoodByFiveButton		= std::make_shared<GUIButton>(CommunistButtons["YearTwoLowerFoodByFive"], mResourcesWindow);	
	mYearTwoLowerFoodByOneButton		= std::make_shared<GUIButton>(CommunistButtons["YearTwoLowerFoodByOne"], mResourcesWindow);	
	mYearTwoRaiseFoodByOneButton		= std::make_shared<GUIButton>(CommunistButtons["YearTwoRaiseFoodByOne"], mResourcesWindow);	
	mYearTwoRaiseFoodByFiveButton		= std::make_shared<GUIButton>(CommunistButtons["YearTwoRaiseFoodByFive"], mResourcesWindow);	
	mYearTwoRaiseFoodByTenButton		= std::make_shared<GUIButton>(CommunistButtons["YearTwoRaiseFoodByTen"], mResourcesWindow);	

	mYearThreeLowerFoodByTenButton		= std::make_shared<GUIButton>(CommunistButtons["YearThreeLowerFoodByTen"], mResourcesWindow);	
	mYearThreeLowerFoodByFiveButton		= std::make_shared<GUIButton>(CommunistButtons["YearThreeLowerFoodByFive"], mResourcesWindow);	
	mYearThreeLowerFoodByOneButton		= std::make_shared<GUIButton>(CommunistButtons["YearThreeLowerFoodByOne"], mResourcesWindow);	
	mYearThreeRaiseFoodByOneButton		= std::make_shared<GUIButton>(CommunistButtons["YearThreeRaiseFoodByOne"], mResourcesWindow);	
	mYearThreeRaiseFoodByFiveButton		= std::make_shared<GUIButton>(CommunistButtons["YearThreeRaiseFoodByFive"], mResourcesWindow);	
	mYearThreeRaiseFoodByTenButton		= std::make_shared<GUIButton>(CommunistButtons["YearThreeRaiseFoodByTen"], mResourcesWindow);	
	
	mYearFourLowerFoodByTenButton		= std::make_shared<GUIButton>(CommunistButtons["YearFourLowerFoodByTen"], mResourcesWindow);	
	mYearFourLowerFoodByFiveButton		= std::make_shared<GUIButton>(CommunistButtons["YearFourLowerFoodByFive"], mResourcesWindow);
	mYearFourLowerFoodByOneButton		= std::make_shared<GUIButton>(CommunistButtons["YearFourLowerFoodByOne"], mResourcesWindow);	
	mYearFourRaiseFoodByOneButton		= std::make_shared<GUIButton>(CommunistButtons["YearFourRaiseFoodByOne"], mResourcesWindow);
	mYearFourRaiseFoodByFiveButton		= std::make_shared<GUIButton>(CommunistButtons["YearFourRaiseFoodByFive"], mResourcesWindow);	
	mYearFourRaiseFoodByTenButton		= std::make_shared<GUIButton>(CommunistButtons["YearFourRaiseFoodByTen"], mResourcesWindow);
		
	mYearFiveLowerFoodByTenButton		= std::make_shared<GUIButton>(CommunistButtons["YearFiveLowerFoodByTen"], mResourcesWindow);	
	mYearFiveLowerFoodByFiveButton		= std::make_shared<GUIButton>(CommunistButtons["YearFiveLowerFoodByFive"], mResourcesWindow);
	mYearFiveLowerFoodByOneButton		= std::make_shared<GUIButton>(CommunistButtons["YearFiveLowerFoodByOne"], mResourcesWindow);	
	mYearFiveRaiseFoodByOneButton		= std::make_shared<GUIButton>(CommunistButtons["YearFiveRaiseFoodByOne"], mResourcesWindow);
	mYearFiveRaiseFoodByFiveButton		= std::make_shared<GUIButton>(CommunistButtons["YearFiveRaiseFoodByFive"], mResourcesWindow);	
	mYearFiveRaiseFoodByTenButton		= std::make_shared<GUIButton>(CommunistButtons["YearFiveRaiseFoodByTen"], mResourcesWindow);
	
	/*Goods*/
	mYearOneLowerGoodsByTenButton		= std::make_shared<GUIButton>(CommunistButtons["YearOneLowerGoodsByTen"], mResourcesWindow);	
	mYearOneLowerGoodsByFiveButton		= std::make_shared<GUIButton>(CommunistButtons["YearOneLowerGoodsByFive"], mResourcesWindow);	
	mYearOneLowerGoodsByOneButton		= std::make_shared<GUIButton>(CommunistButtons["YearOneLowerGoodsByOne"], mResourcesWindow);	
	mYearOneRaiseGoodsByOneButton		= std::make_shared<GUIButton>(CommunistButtons["YearOneRaiseGoodsByOne"], mResourcesWindow);	
	mYearOneRaiseGoodsByFiveButton		= std::make_shared<GUIButton>(CommunistButtons["YearOneRaiseGoodsByFive"], mResourcesWindow);	
	mYearOneRaiseGoodsByTenButton		= std::make_shared<GUIButton>(CommunistButtons["YearOneRaiseGoodsByTen"], mResourcesWindow);	
	
	mYearTwoLowerGoodsByTenButton		= std::make_shared<GUIButton>(CommunistButtons["YearTwoLowerGoodsByTen"], mResourcesWindow);	
	mYearTwoLowerGoodsByFiveButton		= std::make_shared<GUIButton>(CommunistButtons["YearTwoLowerGoodsByFive"], mResourcesWindow);	
	mYearTwoLowerGoodsByOneButton		= std::make_shared<GUIButton>(CommunistButtons["YearTwoLowerGoodsByOne"], mResourcesWindow);	
	mYearTwoRaiseGoodsByOneButton		= std::make_shared<GUIButton>(CommunistButtons["YearTwoRaiseGoodsByOne"], mResourcesWindow);	
	mYearTwoRaiseGoodsByFiveButton		= std::make_shared<GUIButton>(CommunistButtons["YearTwoRaiseGoodsByFive"], mResourcesWindow);	
	mYearTwoRaiseGoodsByTenButton		= std::make_shared<GUIButton>(CommunistButtons["YearTwoRaiseGoodsByTen"], mResourcesWindow);	

	mYearThreeLowerGoodsByTenButton		= std::make_shared<GUIButton>(CommunistButtons["YearThreeLowerGoodsByTen"], mResourcesWindow);	
	mYearThreeLowerGoodsByFiveButton	= std::make_shared<GUIButton>(CommunistButtons["YearThreeLowerGoodsByFive"], mResourcesWindow);	
	mYearThreeLowerGoodsByOneButton		= std::make_shared<GUIButton>(CommunistButtons["YearThreeLowerGoodsByOne"], mResourcesWindow);	
	mYearThreeRaiseGoodsByOneButton		= std::make_shared<GUIButton>(CommunistButtons["YearThreeRaiseGoodsByOne"], mResourcesWindow);	
	mYearThreeRaiseGoodsByFiveButton	= std::make_shared<GUIButton>(CommunistButtons["YearThreeRaiseGoodsByFive"], mResourcesWindow);	
	mYearThreeRaiseGoodsByTenButton		= std::make_shared<GUIButton>(CommunistButtons["YearThreeRaiseGoodsByTen"], mResourcesWindow);	
	
	mYearFourLowerGoodsByTenButton		= std::make_shared<GUIButton>(CommunistButtons["YearFourLowerGoodsByTen"], mResourcesWindow);	
	mYearFourLowerGoodsByFiveButton		= std::make_shared<GUIButton>(CommunistButtons["YearFourLowerGoodsByFive"], mResourcesWindow);
	mYearFourLowerGoodsByOneButton		= std::make_shared<GUIButton>(CommunistButtons["YearFourLowerGoodsByOne"], mResourcesWindow);	
	mYearFourRaiseGoodsByOneButton		= std::make_shared<GUIButton>(CommunistButtons["YearFourRaiseGoodsByOne"], mResourcesWindow);
	mYearFourRaiseGoodsByFiveButton		= std::make_shared<GUIButton>(CommunistButtons["YearFourRaiseGoodsByFive"], mResourcesWindow);	
	mYearFourRaiseGoodsByTenButton		= std::make_shared<GUIButton>(CommunistButtons["YearFourRaiseGoodsByTen"], mResourcesWindow);
	
	mYearFiveLowerGoodsByTenButton		= std::make_shared<GUIButton>(CommunistButtons["YearFiveLowerGoodsByTen"], mResourcesWindow);	
	mYearFiveLowerGoodsByFiveButton		= std::make_shared<GUIButton>(CommunistButtons["YearFiveLowerGoodsByFive"], mResourcesWindow);
	mYearFiveLowerGoodsByOneButton		= std::make_shared<GUIButton>(CommunistButtons["YearFiveLowerGoodsByOne"], mResourcesWindow);	
	mYearFiveRaiseGoodsByOneButton		= std::make_shared<GUIButton>(CommunistButtons["YearFiveRaiseGoodsByOne"], mResourcesWindow);
	mYearFiveRaiseGoodsByFiveButton		= std::make_shared<GUIButton>(CommunistButtons["YearFiveRaiseGoodsByFive"], mResourcesWindow);	
	mYearFiveRaiseGoodsByTenButton		= std::make_shared<GUIButton>(CommunistButtons["YearFiveRaiseGoodsByTen"], mResourcesWindow);
	
	/*Tech*/
	mYearOneLowerTechByTenButton		= std::make_shared<GUIButton>(CommunistButtons["YearOneLowerTechByTen"], mResourcesWindow);	
	mYearOneLowerTechByFiveButton		= std::make_shared<GUIButton>(CommunistButtons["YearOneLowerTechByFive"], mResourcesWindow);	
	mYearOneLowerTechByOneButton		= std::make_shared<GUIButton>(CommunistButtons["YearOneLowerTechByOne"], mResourcesWindow);	
	mYearOneRaiseTechByOneButton		= std::make_shared<GUIButton>(CommunistButtons["YearOneRaiseTechByOne"], mResourcesWindow);	
	mYearOneRaiseTechByFiveButton		= std::make_shared<GUIButton>(CommunistButtons["YearOneRaiseTechByFive"], mResourcesWindow);	
	mYearOneRaiseTechByTenButton		= std::make_shared<GUIButton>(CommunistButtons["YearOneRaiseTechByTen"], mResourcesWindow);	
	
	mYearTwoLowerTechByTenButton		= std::make_shared<GUIButton>(CommunistButtons["YearTwoLowerTechByTen"], mResourcesWindow);	
	mYearTwoLowerTechByFiveButton		= std::make_shared<GUIButton>(CommunistButtons["YearTwoLowerTechByFive"], mResourcesWindow);	
	mYearTwoLowerTechByOneButton		= std::make_shared<GUIButton>(CommunistButtons["YearTwoLowerTechByOne"], mResourcesWindow);	
	mYearTwoRaiseTechByOneButton		= std::make_shared<GUIButton>(CommunistButtons["YearTwoRaiseTechByOne"], mResourcesWindow);	
	mYearTwoRaiseTechByFiveButton		= std::make_shared<GUIButton>(CommunistButtons["YearTwoRaiseTechByFive"], mResourcesWindow);	
	mYearTwoRaiseTechByTenButton		= std::make_shared<GUIButton>(CommunistButtons["YearTwoRaiseTechByTen"], mResourcesWindow);	

	mYearThreeLowerTechByTenButton		= std::make_shared<GUIButton>(CommunistButtons["YearThreeLowerTechByTen"], mResourcesWindow);	
	mYearThreeLowerTechByFiveButton		= std::make_shared<GUIButton>(CommunistButtons["YearThreeLowerTechByFive"], mResourcesWindow);	
	mYearThreeLowerTechByOneButton		= std::make_shared<GUIButton>(CommunistButtons["YearThreeLowerTechByOne"], mResourcesWindow);	
	mYearThreeRaiseTechByOneButton		= std::make_shared<GUIButton>(CommunistButtons["YearThreeRaiseTechByOne"], mResourcesWindow);	
	mYearThreeRaiseTechByFiveButton		= std::make_shared<GUIButton>(CommunistButtons["YearThreeRaiseTechByFive"], mResourcesWindow);	
	mYearThreeRaiseTechByTenButton		= std::make_shared<GUIButton>(CommunistButtons["YearThreeRaiseTechByTen"], mResourcesWindow);	
	
	mYearFourLowerTechByTenButton		= std::make_shared<GUIButton>(CommunistButtons["YearFourLowerTechByTen"], mResourcesWindow);	
	mYearFourLowerTechByFiveButton		= std::make_shared<GUIButton>(CommunistButtons["YearFourLowerTechByFive"], mResourcesWindow);
	mYearFourLowerTechByOneButton		= std::make_shared<GUIButton>(CommunistButtons["YearFourLowerTechByOne"], mResourcesWindow);	
	mYearFourRaiseTechByOneButton		= std::make_shared<GUIButton>(CommunistButtons["YearFourRaiseTechByOne"], mResourcesWindow);
	mYearFourRaiseTechByFiveButton		= std::make_shared<GUIButton>(CommunistButtons["YearFourRaiseTechByFive"], mResourcesWindow);	
	mYearFourRaiseTechByTenButton		= std::make_shared<GUIButton>(CommunistButtons["YearFourRaiseTechByTen"], mResourcesWindow);
	
	mYearFiveLowerTechByTenButton		= std::make_shared<GUIButton>(CommunistButtons["YearFiveLowerTechByTen"], mResourcesWindow);	
	mYearFiveLowerTechByFiveButton		= std::make_shared<GUIButton>(CommunistButtons["YearFiveLowerTechByFive"], mResourcesWindow);
	mYearFiveLowerTechByOneButton		= std::make_shared<GUIButton>(CommunistButtons["YearFiveLowerTechByOne"], mResourcesWindow);	
	mYearFiveRaiseTechByOneButton		= std::make_shared<GUIButton>(CommunistButtons["YearFiveRaiseTechByOne"], mResourcesWindow);
	mYearFiveRaiseTechByFiveButton		= std::make_shared<GUIButton>(CommunistButtons["YearFiveRaiseTechByFive"], mResourcesWindow);	
	mYearFiveRaiseTechByTenButton		= std::make_shared<GUIButton>(CommunistButtons["YearFiveRaiseTechByTen"], mResourcesWindow);

	mGoToNextSlideButton				= std::make_shared<GUIButton>(CommunistButtons["GoToNextSlide"], mTaxesWindow);	
	mTaxesCloseButton					= std::make_shared<GUIButton>(CommunistButtons["CloseTaxesWindow"], mTaxesWindow);	
	mGoToPreviousSlideButton			= std::make_shared<GUIButton>(CommunistButtons["GoToPreviousSlide"], mResourcesWindow);
	mResourcesCloseButton				= std::make_shared<GUIButton>(CommunistButtons["CloseResourcesWindow"], mResourcesWindow);	
	mTaxesWindow->setVisible(false);
	mResourcesWindow->setVisible(false);

	/*Propaganda GUI window med knappar*/
	mPropagandaWindowFirst				= std::make_shared<GUIWindow>(CommunistWindows["CommunistPropagandaWindowFirst"], mCommunistMainWindow);
	mPropagandaWindowSecond				= std::make_shared<GUIWindow>(CommunistWindows["CommunistPropagandaWindowSecond"], mCommunistMainWindow);
	/*mPropagandaBuyFoodButton			= std::make_shared<GUIButton>(CommunistButtons["PropagandaBuyFood"], mPropagandaWindowFirst);
	mPropagandaBuyGoodsButton			= std::make_shared<GUIButton>(CommunistButtons["PropagandaBuyGoods"], mPropagandaWindowFirst);
	mPropagandaBuyTechButton			= std::make_shared<GUIButton>(CommunistButtons["PropagandaBuyTech"], mPropagandaWindowFirst);*/
	mPropagandaWindowFirstCloseButton	= std::make_shared<GUIButton>(CommunistButtons["PropagandaCloseWindowFirst"], mPropagandaWindowFirst);
	mPropagandaWindowSecondCloseButton	= std::make_shared<GUIButton>(CommunistButtons["PropagandaCloseWindowSecond"], mPropagandaWindowSecond);
	mPropagandaWindowFirst->setVisible(false);
	mPropagandaWindowSecond->setVisible(false);

	/*Upgrade GUI window med knappar*/
	mUpgradeWindow						= std::make_shared<GUIWindow>(CommunistWindows["CommunistUpgradeWindow"], mCommunistMainWindow);
	mUpgradeNuclearWeaponButton			= std::make_shared<GUIButton>(CommunistButtons["UpgradeNuclearWeapon"], mUpgradeWindow);
	mUpgradeSpaceProgramButton			= std::make_shared<GUIButton>(CommunistButtons["UpgradeSpaceProgram"], mUpgradeWindow);
	mUpgradeSpyNetworkButton			= std::make_shared<GUIButton>(CommunistButtons["UpgradeSpyNetwork"], mUpgradeWindow);
	mUpgradeCloseButton					= std::make_shared<GUIButton>(CommunistButtons["CloseUpgrade"], mUpgradeWindow);
	mUpgradeWindow->setVisible(false);

	/*Export GUI Window med knappar*/
	mExportWindow						= std::make_shared<GUIWindow>(CommunistWindows["CommunistExportWindow"], mCommunistMainWindow);
	mExportLowerFoodButton				= std::make_shared<GUIButton>(CommunistButtons["CommunistLowerFood"], mExportWindow);
	mExportRaiseFoodButton				= std::make_shared<GUIButton>(CommunistButtons["CommunistRaiseFood"], mExportWindow);
	mExportLowerGoodsButton				= std::make_shared<GUIButton>(CommunistButtons["CommunistLowerGoods"], mExportWindow);
	mExportRaiseGoodsButton				= std::make_shared<GUIButton>(CommunistButtons["CommunistRaiseGoods"], mExportWindow);
	mExportLowerTechButton				= std::make_shared<GUIButton>(CommunistButtons["CommunistLowerTech"], mExportWindow);
	mExportRaiseTechButton				= std::make_shared<GUIButton>(CommunistButtons["CommunistRaiseTech"], mExportWindow);
	mExportCloseButton					= std::make_shared<GUIButton>(CommunistButtons["CloseExport"], mExportWindow);
	mExportWindow->setVisible(false);


	mChooseGeneralWindow				= std::make_shared<GUIWindow>(CommunistWindows["ChooseGeneral"], mCommunistMainWindow);
	mPickedGeneralWindow				= std::make_shared<GUIWindow>(CommunistWindows["PickedGeneral"], mCommunistMainWindow);
	mPickedGeneralButton				= std::make_shared<GUIButton>(CommunistButtons["PickedGeneral"], mPickedGeneralWindow);
	mPickedGeneralWindow->setVisible(false);
	mFirstGeneralButton					= std::make_shared<GUIButton>(CommunistButtons["FirstGeneral"], mChooseGeneralWindow);
	
	mGoToNextPortraitButton				= std::make_shared<GUIButton>(CommunistButtons["GoToNextPortrait"], mChooseGeneralWindow);
	mGoToPreviousPortraitButton			= std::make_shared<GUIButton>(CommunistButtons["GoToPreviousPortrait"], mChooseGeneralWindow);
	mCloseGeneralWindow					= std::make_shared<GUIButton>(CommunistButtons["CloseGeneral"], mChooseGeneralWindow);
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
	
	mFirstGeneralButton->setTexture(std::pair<sf::FloatRect, sf::Texture*>(mFirstGeneralButton->getRectangle(), manager->getGeneral(generalCount)->getTexture())); 
}

 /**/
void Communist::initializeGuiFunctions()
{

	mCommunistFiveYearPlanButton->setOnClickFunction([=]()		{ mTaxesWindow->setVisible(true); });
	mCommunistPropagandaButton->setOnClickFunction([=]()		{ mPropagandaWindowFirst->setVisible(true); });
	mCommunistUpgradeButton->setOnClickFunction([=]()			{ mUpgradeWindow->setVisible(true); });
	mGoToNextSlideButton->setOnClickFunction([=]()				{ mTaxesWindow->setVisible(false); mResourcesWindow->setVisible(true); });
	mGoToPreviousSlideButton->setOnClickFunction([=]()			{ mResourcesWindow->setVisible(false); mTaxesWindow->setVisible(true); });
	mCommunistExportButton->setOnClickFunction([=]()			{ mExportWindow->setVisible(true); });

	mTaxesCloseButton->setOnClickFunction([=]()					{ mTaxesWindow->setVisible(false); std::cout << "taxes" << std::endl;});
	mResourcesCloseButton->setOnClickFunction([=]()				{ mResourcesWindow->setVisible(false); });
	mPropagandaWindowFirstCloseButton->setOnClickFunction([=]()	{ mPropagandaWindowFirst->setVisible(false); std::cout << "Propaganda" << std::endl;});

	//Vad som skall hända då spelaren väljer att uppgradera
	//antingen nuclear, space eller spy
	mUpgradeCloseButton->setOnClickFunction([=]()				
	{ 
		mUpgradeWindow->setVisible(false); 
		mNuclearWeapon = mNuclearWeaponUpdate; mNuclearText->setText(intToString(getNuclearWeapon()));
		mSpaceProgram = mSpaceProgramUpdate; mSpaceText->setText(intToString(getSpaceProgram()));
		mSpyNetwork = mSpyNetworkUpdate; mSpyText->setText(intToString(getSpyNetwork())); std::cout << "HERRRRRO" << std::endl;
	});

	mExportCloseButton->setOnClickFunction([=]() { mExportWindow->setVisible(false); });

	mUpgradeNuclearWeaponButton->setOnClickFunction(std::bind(&Communist::upgradeNuclearWeapon, this));
	mUpgradeSpaceProgramButton->setOnClickFunction(std::bind(&Communist::upgradeSpaceProgram, this));
	mUpgradeSpyNetworkButton->setOnClickFunction(std::bind(&Communist::upgradeSpyNetwork, this));


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
		std::cout<<generalCount<<std::endl;

		mFirstGeneralButton->setTexture(std::pair<sf::FloatRect, sf::Texture*>
			(mFirstGeneralButton->getRectangle(), GameManager::getInstance()->getGeneral(generalCount)->getTexture()));
			
	});


	mCloseGeneralWindow->setOnClickFunction([=]()
	{
		mChooseGeneralWindow->setVisible(false);
		mPickedGeneralWindow->setVisible(true);

		mGeneral = GameManager::getInstance()->getGeneral(generalCount);

		mPickedGeneralButton->setTexture(std::pair<sf::FloatRect, sf::Texture*>
			(mPickedGeneralButton->getRectangle(), mGeneral->getTexture())); 
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