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

	mCommunistMainWindow			= GUIWindow::create(CommunistWindows["CommunistInterface"]);
	mCommunistGeneralButton			= GUIButton::create(CommunistButtons["General"], mCommunistMainWindow);
	mCommunistFiveYearPlanButton    = GUIButton::create(CommunistButtons["FiveYearPlan"], mCommunistMainWindow);
	mCommunistPropagandaButton		= GUIButton::create(CommunistButtons["Propaganda"], mCommunistMainWindow);
	mCommunistUpgradeButton			= GUIButton::create(CommunistButtons["Upgrade"], mCommunistMainWindow);
	mCommunistExportButton			= GUIButton::create(CommunistButtons["Export"], mCommunistMainWindow);
	mCommunistEndTurnButton			= GUIButton::create(CommunistButtons["EndTurn"], mCommunistMainWindow);

	/*GUI text för utskrift av värden på kapitalisternas interface*/
	mNuclearText = GUIText::create(sf::FloatRect(962, 16, 40, 40), intToString(getNuclearWeapon()), mCommunistMainWindow);
	mSpaceText	 = GUIText::create(sf::FloatRect(962, 228, 40, 40), intToString(getSpaceProgram()), mCommunistMainWindow);
	mSpyText	 = GUIText::create(sf::FloatRect(962, 440, 40, 40), intToString(getSpyNetwork()), mCommunistMainWindow);
	mFoodText	 = GUIText::create(sf::FloatRect(160, 16, 40, 40), intToString(getFood()), mCommunistMainWindow);
	mGoodsText   = GUIText::create(sf::FloatRect(160, 228, 40, 40), intToString(getGoods()), mCommunistMainWindow);
	mTechText	 = GUIText::create(sf::FloatRect(160, 440, 40, 40), intToString(getTech()), mCommunistMainWindow);	

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
	
	mYearTwoLowerFoodByTenButton		= GUIButton::create(CommunistButtons["YearTwoLowerFoodByTen"], mResourcesWindow);	
	mYearTwoLowerFoodByFiveButton		= GUIButton::create(CommunistButtons["YearTwoLowerFoodByFive"], mResourcesWindow);	
	mYearTwoLowerFoodByOneButton		= GUIButton::create(CommunistButtons["YearTwoLowerFoodByOne"], mResourcesWindow);	
	mYearTwoRaiseFoodByOneButton		= GUIButton::create(CommunistButtons["YearTwoRaiseFoodByOne"], mResourcesWindow);	
	mYearTwoRaiseFoodByFiveButton		= GUIButton::create(CommunistButtons["YearTwoRaiseFoodByFive"], mResourcesWindow);	
	mYearTwoRaiseFoodByTenButton		= GUIButton::create(CommunistButtons["YearTwoRaiseFoodByTen"], mResourcesWindow);	

	mYearThreeLowerFoodByTenButton		= GUIButton::create(CommunistButtons["YearThreeLowerFoodByTen"], mResourcesWindow);	
	mYearThreeLowerFoodByFiveButton		= GUIButton::create(CommunistButtons["YearThreeLowerFoodByFive"], mResourcesWindow);	
	mYearThreeLowerFoodByOneButton		= GUIButton::create(CommunistButtons["YearThreeLowerFoodByOne"], mResourcesWindow);	
	mYearThreeRaiseFoodByOneButton		= GUIButton::create(CommunistButtons["YearThreeRaiseFoodByOne"], mResourcesWindow);	
	mYearThreeRaiseFoodByFiveButton		= GUIButton::create(CommunistButtons["YearThreeRaiseFoodByFive"], mResourcesWindow);	
	mYearThreeRaiseFoodByTenButton		= GUIButton::create(CommunistButtons["YearThreeRaiseFoodByTen"], mResourcesWindow);	
	
	mYearFourLowerFoodByTenButton		= GUIButton::create(CommunistButtons["YearFourLowerFoodByTen"], mResourcesWindow);	
	mYearFourLowerFoodByFiveButton		= GUIButton::create(CommunistButtons["YearFourLowerFoodByFive"], mResourcesWindow);
	mYearFourLowerFoodByOneButton		= GUIButton::create(CommunistButtons["YearFourLowerFoodByOne"], mResourcesWindow);	
	mYearFourRaiseFoodByOneButton		= GUIButton::create(CommunistButtons["YearFourRaiseFoodByOne"], mResourcesWindow);
	mYearFourRaiseFoodByFiveButton		= GUIButton::create(CommunistButtons["YearFourRaiseFoodByFive"], mResourcesWindow);	
	mYearFourRaiseFoodByTenButton		= GUIButton::create(CommunistButtons["YearFourRaiseFoodByTen"], mResourcesWindow);
		
	mYearFiveLowerFoodByTenButton		= GUIButton::create(CommunistButtons["YearFiveLowerFoodByTen"], mResourcesWindow);	
	mYearFiveLowerFoodByFiveButton		= GUIButton::create(CommunistButtons["YearFiveLowerFoodByFive"], mResourcesWindow);
	mYearFiveLowerFoodByOneButton		= GUIButton::create(CommunistButtons["YearFiveLowerFoodByOne"], mResourcesWindow);	
	mYearFiveRaiseFoodByOneButton		= GUIButton::create(CommunistButtons["YearFiveRaiseFoodByOne"], mResourcesWindow);
	mYearFiveRaiseFoodByFiveButton		= GUIButton::create(CommunistButtons["YearFiveRaiseFoodByFive"], mResourcesWindow);	
	mYearFiveRaiseFoodByTenButton		= GUIButton::create(CommunistButtons["YearFiveRaiseFoodByTen"], mResourcesWindow);
	
	/*Goods*/
	mYearOneLowerGoodsByTenButton		= GUIButton::create(CommunistButtons["YearOneLowerGoodsByTen"], mResourcesWindow);	
	mYearOneLowerGoodsByFiveButton		= GUIButton::create(CommunistButtons["YearOneLowerGoodsByFive"], mResourcesWindow);	
	mYearOneLowerGoodsByOneButton		= GUIButton::create(CommunistButtons["YearOneLowerGoodsByOne"], mResourcesWindow);	
	mYearOneRaiseGoodsByOneButton		= GUIButton::create(CommunistButtons["YearOneRaiseGoodsByOne"], mResourcesWindow);	
	mYearOneRaiseGoodsByFiveButton		= GUIButton::create(CommunistButtons["YearOneRaiseGoodsByFive"], mResourcesWindow);	
	mYearOneRaiseGoodsByTenButton		= GUIButton::create(CommunistButtons["YearOneRaiseGoodsByTen"], mResourcesWindow);	
	
	mYearTwoLowerGoodsByTenButton		= GUIButton::create(CommunistButtons["YearTwoLowerGoodsByTen"], mResourcesWindow);	
	mYearTwoLowerGoodsByFiveButton		= GUIButton::create(CommunistButtons["YearTwoLowerGoodsByFive"], mResourcesWindow);	
	mYearTwoLowerGoodsByOneButton		= GUIButton::create(CommunistButtons["YearTwoLowerGoodsByOne"], mResourcesWindow);	
	mYearTwoRaiseGoodsByOneButton		= GUIButton::create(CommunistButtons["YearTwoRaiseGoodsByOne"], mResourcesWindow);	
	mYearTwoRaiseGoodsByFiveButton		= GUIButton::create(CommunistButtons["YearTwoRaiseGoodsByFive"], mResourcesWindow);	
	mYearTwoRaiseGoodsByTenButton		= GUIButton::create(CommunistButtons["YearTwoRaiseGoodsByTen"], mResourcesWindow);	

	mYearThreeLowerGoodsByTenButton		= GUIButton::create(CommunistButtons["YearThreeLowerGoodsByTen"], mResourcesWindow);	
	mYearThreeLowerGoodsByFiveButton	= GUIButton::create(CommunistButtons["YearThreeLowerGoodsByFive"], mResourcesWindow);	
	mYearThreeLowerGoodsByOneButton		= GUIButton::create(CommunistButtons["YearThreeLowerGoodsByOne"], mResourcesWindow);	
	mYearThreeRaiseGoodsByOneButton		= GUIButton::create(CommunistButtons["YearThreeRaiseGoodsByOne"], mResourcesWindow);	
	mYearThreeRaiseGoodsByFiveButton	= GUIButton::create(CommunistButtons["YearThreeRaiseGoodsByFive"], mResourcesWindow);	
	mYearThreeRaiseGoodsByTenButton		= GUIButton::create(CommunistButtons["YearThreeRaiseGoodsByTen"], mResourcesWindow);	
	
	mYearFourLowerGoodsByTenButton		= GUIButton::create(CommunistButtons["YearFourLowerGoodsByTen"], mResourcesWindow);	
	mYearFourLowerGoodsByFiveButton		= GUIButton::create(CommunistButtons["YearFourLowerGoodsByFive"], mResourcesWindow);
	mYearFourLowerGoodsByOneButton		= GUIButton::create(CommunistButtons["YearFourLowerGoodsByOne"], mResourcesWindow);	
	mYearFourRaiseGoodsByOneButton		= GUIButton::create(CommunistButtons["YearFourRaiseGoodsByOne"], mResourcesWindow);
	mYearFourRaiseGoodsByFiveButton		= GUIButton::create(CommunistButtons["YearFourRaiseGoodsByFive"], mResourcesWindow);	
	mYearFourRaiseGoodsByTenButton		= GUIButton::create(CommunistButtons["YearFourRaiseGoodsByTen"], mResourcesWindow);
	
	mYearFiveLowerGoodsByTenButton		= GUIButton::create(CommunistButtons["YearFiveLowerGoodsByTen"], mResourcesWindow);	
	mYearFiveLowerGoodsByFiveButton		= GUIButton::create(CommunistButtons["YearFiveLowerGoodsByFive"], mResourcesWindow);
	mYearFiveLowerGoodsByOneButton		= GUIButton::create(CommunistButtons["YearFiveLowerGoodsByOne"], mResourcesWindow);	
	mYearFiveRaiseGoodsByOneButton		= GUIButton::create(CommunistButtons["YearFiveRaiseGoodsByOne"], mResourcesWindow);
	mYearFiveRaiseGoodsByFiveButton		= GUIButton::create(CommunistButtons["YearFiveRaiseGoodsByFive"], mResourcesWindow);	
	mYearFiveRaiseGoodsByTenButton		= GUIButton::create(CommunistButtons["YearFiveRaiseGoodsByTen"], mResourcesWindow);
	
	/*Tech*/
	mYearOneLowerTechByTenButton		= GUIButton::create(CommunistButtons["YearOneLowerTechByTen"], mResourcesWindow);	
	mYearOneLowerTechByFiveButton		= GUIButton::create(CommunistButtons["YearOneLowerTechByFive"], mResourcesWindow);	
	mYearOneLowerTechByOneButton		= GUIButton::create(CommunistButtons["YearOneLowerTechByOne"], mResourcesWindow);	
	mYearOneRaiseTechByOneButton		= GUIButton::create(CommunistButtons["YearOneRaiseTechByOne"], mResourcesWindow);	
	mYearOneRaiseTechByFiveButton		= GUIButton::create(CommunistButtons["YearOneRaiseTechByFive"], mResourcesWindow);	
	mYearOneRaiseTechByTenButton		= GUIButton::create(CommunistButtons["YearOneRaiseTechByTen"], mResourcesWindow);	
	
	mYearTwoLowerTechByTenButton		= GUIButton::create(CommunistButtons["YearTwoLowerTechByTen"], mResourcesWindow);	
	mYearTwoLowerTechByFiveButton		= GUIButton::create(CommunistButtons["YearTwoLowerTechByFive"], mResourcesWindow);	
	mYearTwoLowerTechByOneButton		= GUIButton::create(CommunistButtons["YearTwoLowerTechByOne"], mResourcesWindow);	
	mYearTwoRaiseTechByOneButton		= GUIButton::create(CommunistButtons["YearTwoRaiseTechByOne"], mResourcesWindow);	
	mYearTwoRaiseTechByFiveButton		= GUIButton::create(CommunistButtons["YearTwoRaiseTechByFive"], mResourcesWindow);	
	mYearTwoRaiseTechByTenButton		= GUIButton::create(CommunistButtons["YearTwoRaiseTechByTen"], mResourcesWindow);	

	mYearThreeLowerTechByTenButton		= GUIButton::create(CommunistButtons["YearThreeLowerTechByTen"], mResourcesWindow);	
	mYearThreeLowerTechByFiveButton		= GUIButton::create(CommunistButtons["YearThreeLowerTechByFive"], mResourcesWindow);	
	mYearThreeLowerTechByOneButton		= GUIButton::create(CommunistButtons["YearThreeLowerTechByOne"], mResourcesWindow);	
	mYearThreeRaiseTechByOneButton		= GUIButton::create(CommunistButtons["YearThreeRaiseTechByOne"], mResourcesWindow);	
	mYearThreeRaiseTechByFiveButton		= GUIButton::create(CommunistButtons["YearThreeRaiseTechByFive"], mResourcesWindow);	
	mYearThreeRaiseTechByTenButton		= GUIButton::create(CommunistButtons["YearThreeRaiseTechByTen"], mResourcesWindow);	
	
	mYearFourLowerTechByTenButton		= GUIButton::create(CommunistButtons["YearFourLowerTechByTen"], mResourcesWindow);	
	mYearFourLowerTechByFiveButton		= GUIButton::create(CommunistButtons["YearFourLowerTechByFive"], mResourcesWindow);
	mYearFourLowerTechByOneButton		= GUIButton::create(CommunistButtons["YearFourLowerTechByOne"], mResourcesWindow);	
	mYearFourRaiseTechByOneButton		= GUIButton::create(CommunistButtons["YearFourRaiseTechByOne"], mResourcesWindow);
	mYearFourRaiseTechByFiveButton		= GUIButton::create(CommunistButtons["YearFourRaiseTechByFive"], mResourcesWindow);	
	mYearFourRaiseTechByTenButton		= GUIButton::create(CommunistButtons["YearFourRaiseTechByTen"], mResourcesWindow);
	
	mYearFiveLowerTechByTenButton		= GUIButton::create(CommunistButtons["YearFiveLowerTechByTen"], mResourcesWindow);	
	mYearFiveLowerTechByFiveButton		= GUIButton::create(CommunistButtons["YearFiveLowerTechByFive"], mResourcesWindow);
	mYearFiveLowerTechByOneButton		= GUIButton::create(CommunistButtons["YearFiveLowerTechByOne"], mResourcesWindow);	
	mYearFiveRaiseTechByOneButton		= GUIButton::create(CommunistButtons["YearFiveRaiseTechByOne"], mResourcesWindow);
	mYearFiveRaiseTechByFiveButton		= GUIButton::create(CommunistButtons["YearFiveRaiseTechByFive"], mResourcesWindow);	
	mYearFiveRaiseTechByTenButton		= GUIButton::create(CommunistButtons["YearFiveRaiseTechByTen"], mResourcesWindow);

	mGoToNextSlideButton				= GUIButton::create(CommunistButtons["GoToNextSlide"], mTaxesWindow);	
	mTaxesCloseButton					= GUIButton::create(CommunistButtons["CloseTaxesWindow"], mTaxesWindow);	
	mGoToPreviousSlideButton			= GUIButton::create(CommunistButtons["GoToPreviousSlide"], mResourcesWindow);
	mResourcesCloseButton				= GUIButton::create(CommunistButtons["CloseResourcesWindow"], mResourcesWindow);	
	mTaxesWindow->setVisible(false);
	mResourcesWindow->setVisible(false);

	/*Propaganda GUI window med knappar*/
	mPropagandaWindowFirst				= GUIWindow::create(CommunistWindows["CommunistPropagandaWindowFirst"], mCommunistMainWindow);
	mPropagandaWindowSecond				= GUIWindow::create(CommunistWindows["CommunistPropagandaWindowSecond"], mCommunistMainWindow);
	/*mPropagandaBuyFoodButton			= GUIButton::create(CommunistButtons["PropagandaBuyFood"], mPropagandaWindowFirst);
	mPropagandaBuyGoodsButton			= GUIButton::create(CommunistButtons["PropagandaBuyGoods"], mPropagandaWindowFirst);
	mPropagandaBuyTechButton			= GUIButton::create(CommunistButtons["PropagandaBuyTech"], mPropagandaWindowFirst);*/
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

	mCommunistMainWindow->setVisible(false);
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