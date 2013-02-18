#include "Communist.h"
#include "GUIManager.h"
#include "GUIWindow.h"
#include "GUIButton.h"
#include "GUIEditField.h"
#include "GUIText.h"
#include "GUIImage.h"
#include "tinyxml2.h"
#include "ResourceHandler.h"
#include "Randomizer.h"
#include "President.h"
#include "GameManager.h"
#include "GUIEditField.h"
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
		mFiveYearPlanWindow->setEnabled(true, true);
		//mCommunistFiveYearPlanButton->setTexture(CommunistButtons["FiveYearPlanIsPressed"]);
		mFiveYearPlanWindow->setVisible(true);
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
//void Communist::playMusic()
//{
//	CommunistMusic["CommunistMainTheme"]->play();
//}

 /*initierar kommunisternas fönster respektive fönster/knappar etc.*/

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
	mFiveYearPlanWindow				= GUIWindow::create(CommunistWindows["FiveYearPlanWindow"], mCommunistMainWindow);	
	mYearOneLowerTaxesButton		= GUIButton::create(CommunistButtons["YearOneLowerTaxes"], mFiveYearPlanWindow);	
	mYearOneRaiseTaxesButton		= GUIButton::create(CommunistButtons["YearOneRaiseTaxes"], mFiveYearPlanWindow);	
	mYearTwoLowerTaxesButton		= GUIButton::create(CommunistButtons["YearTwoLowerTaxes"], mFiveYearPlanWindow);	
	mYearTwoRaiseTaxesButton		= GUIButton::create(CommunistButtons["YearTwoRaiseTaxes"], mFiveYearPlanWindow);	
	mYearThreeLowerTaxesButton		= GUIButton::create(CommunistButtons["YearThreeLowerTaxes"], mFiveYearPlanWindow);	
	mYearThreeRaiseTaxesButton		= GUIButton::create(CommunistButtons["YearThreeRaiseTaxes"], mFiveYearPlanWindow);	
	mYearFourLowerTaxesButton		= GUIButton::create(CommunistButtons["YearFourLowerTaxes"], mFiveYearPlanWindow);	
	mYearFourRaiseTaxesButton		= GUIButton::create(CommunistButtons["YearFourRaiseTaxes"], mFiveYearPlanWindow);	
	mYearFiveLowerTaxesButton		= GUIButton::create(CommunistButtons["YearFiveLowerTaxes"], mFiveYearPlanWindow);	
	mYearFiveRaiseTaxesButton		= GUIButton::create(CommunistButtons["YearFiveRaiseTaxes"], mFiveYearPlanWindow);

	mYearOneTaxesText				= GUIText::create(sf::FloatRect(515, 55, 20, 20), "30", mFiveYearPlanWindow);
	mYearTwoTaxesText				= GUIText::create(sf::FloatRect(515, 143, 20, 20), "30", mFiveYearPlanWindow);
	mYearThreeTaxesText				= GUIText::create(sf::FloatRect(515, 231, 20, 20), "30", mFiveYearPlanWindow);
	mYearFourTaxesText				= GUIText::create(sf::FloatRect(515, 319, 20, 20), "30", mFiveYearPlanWindow);
	mYearFiveTaxesText				= GUIText::create(sf::FloatRect(515, 407, 20, 20), "30", mFiveYearPlanWindow);

	mIncomeYearOne					= GUIText::create(sf::FloatRect(435, 55, 20, 20), "0", mFiveYearPlanWindow);
	mIncomeYearTwo					= GUIText::create(sf::FloatRect(435, 143, 20, 20), "0", mFiveYearPlanWindow);
	mIncomeYearThree				= GUIText::create(sf::FloatRect(435, 231, 20, 20), "0", mFiveYearPlanWindow);
	mIncomeYearFour					= GUIText::create(sf::FloatRect(435, 319, 20, 20), "0", mFiveYearPlanWindow);
	mIncomeYearFive					= GUIText::create(sf::FloatRect(435, 407, 20, 20), "0", mFiveYearPlanWindow);

	mIncomeYearOne->setText(stringToInt(mYearOneTaxesText->getText()) * mPopulation);
	mIncomeYearTwo->setText(stringToInt(mYearTwoTaxesText->getText()) * mPopulation);
	mIncomeYearThree->setText(stringToInt(mYearThreeTaxesText->getText()) * mPopulation);
	mIncomeYearFour->setText(stringToInt(mYearFourTaxesText->getText()) * mPopulation);
	mIncomeYearFive->setText(stringToInt(mYearFiveTaxesText->getText()) * mPopulation);
	/*Food*/
	mYearOneFood					= GUIEditField::create(CommunistButtons["YearOneFoodEditField"].first, "0", true, mFiveYearPlanWindow);
	mYearTwoFood					= GUIEditField::create(CommunistButtons["YearTwoFoodEditField"].first, "0", true, mFiveYearPlanWindow);
	mYearThreeFood					= GUIEditField::create(CommunistButtons["YearThreeFoodEditField"].first, "0", true, mFiveYearPlanWindow);
	mYearFourFood					= GUIEditField::create(CommunistButtons["YearFourFoodEditField"].first, "0", true, mFiveYearPlanWindow);
	mYearFiveFood					= GUIEditField::create(CommunistButtons["YearFiveFoodEditField"].first, "0", true, mFiveYearPlanWindow);

	mYearOneGoods					= GUIEditField::create(CommunistButtons["YearOneGoodsEditField"].first, "0", true, mFiveYearPlanWindow);
	mYearTwoGoods					= GUIEditField::create(CommunistButtons["YearTwoGoodsEditField"].first, "0", true, mFiveYearPlanWindow);
	mYearThreeGoods					= GUIEditField::create(CommunistButtons["YearThreeGoodsEditField"].first, "0", true, mFiveYearPlanWindow);
	mYearFourGoods					= GUIEditField::create(CommunistButtons["YearFourGoodsEditField"].first, "0", true, mFiveYearPlanWindow);
	mYearFiveGoods					= GUIEditField::create(CommunistButtons["YearFiveGoodsEditField"].first, "0", true, mFiveYearPlanWindow);

	mYearOneTech					= GUIEditField::create(CommunistButtons["YearOneTechEditField"].first, "0", true, mFiveYearPlanWindow);
	mYearTwoTech					= GUIEditField::create(CommunistButtons["YearTwoTechEditField"].first, "0", true, mFiveYearPlanWindow);
	mYearThreeTech					= GUIEditField::create(CommunistButtons["YearThreeTechEditField"].first, "0", true, mFiveYearPlanWindow);
	mYearFourTech					= GUIEditField::create(CommunistButtons["YearFourTechEditField"].first, "0", true, mFiveYearPlanWindow);
	mYearFiveTech					= GUIEditField::create(CommunistButtons["YearFiveTechEditField"].first, "0", true, mFiveYearPlanWindow);

	mTotalCostYearOne				= GUIText::create(sf::FloatRect(350, 35, 20, 20), "0", mFiveYearPlanWindow);
	mTotalCostYearTwo				= GUIText::create(sf::FloatRect(350, 123, 20, 20), "0", mFiveYearPlanWindow);
	mTotalCostYearThree				= GUIText::create(sf::FloatRect(350, 211, 20, 20), "0", mFiveYearPlanWindow);
	mTotalCostYearFour				= GUIText::create(sf::FloatRect(350, 299, 20, 20), "0", mFiveYearPlanWindow);
	mTotalCostYearFive				= GUIText::create(sf::FloatRect(350, 387, 20, 20), "0", mFiveYearPlanWindow);

	mFiveYearPlanCloseButton		= GUIButton::create(CommunistButtons["CloseFiveYearPlanWindow"], mFiveYearPlanWindow);	
	mFiveYearPlanWindow->setVisible(false);

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

	mExportFoodPrice					= GUIEditField::create(sf::FloatRect(260, 100, 265, 40), "50", true, mExportWindow); 
	mExportGoodsPrice					= GUIEditField::create(sf::FloatRect(260, 242, 265, 40), "50", true, mExportWindow); 
	mExportTechPrice					= GUIEditField::create(sf::FloatRect(260, 387, 265, 40), "50", true, mExportWindow); 

	mExportCloseButton					= GUIButton::create(CommunistButtons["CloseExport"], mExportWindow);
	mExportWindow->setVisible(false);


	mChooseGeneralWindow				= GUIWindow::create(CommunistWindows["ChooseGeneral"], mCommunistMainWindow);
	mPickedGeneralWindow				= GUIWindow::create(CommunistWindows["PickedGeneral"], mCommunistMainWindow);
	mPickedGeneralButton				= GUIButton::create(CommunistButtons["PickedGeneral"], mPickedGeneralWindow);
	sf::FloatRect pickedRect			= CommunistButtons["PickedGeneral"].first;
	mPickedGeneralPlaque				= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>
		(sf::FloatRect(pickedRect.left, pickedRect.top + pickedRect.height - 5, pickedRect.width, pickedRect.height),
		&GameManager::getInstance()->getGeneralPlaque(GameManager::getInstance()->getGeneral(generalCount))), mPickedGeneralWindow);

	mPickedGeneralWindow->setVisible(false);

	mFirstGeneralButton					= GUIButton::create(CommunistButtons["FirstGeneral"], mChooseGeneralWindow);
	sf::FloatRect generalRect			= CommunistButtons["FirstGeneral"].first;
	mFirstGeneralPlaque					= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>
		(sf::FloatRect(generalRect.left, generalRect.top + generalRect.height - 5, generalRect.width, generalRect.height),
		&GameManager::getInstance()->getGeneralPlaque(GameManager::getInstance()->getGeneral(generalCount))), mChooseGeneralWindow);

	
	mGoToNextPortraitButton				= GUIButton::create(CommunistButtons["GoToNextPortrait"], mChooseGeneralWindow);
	mGoToPreviousPortraitButton			= GUIButton::create(CommunistButtons["GoToPreviousPortrait"], mChooseGeneralWindow);
	mCloseGeneralWindow					= GUIButton::create(CommunistButtons["CloseGeneral"], mChooseGeneralWindow);
	mClosePickedGeneralWindow			= GUIButton::create(CommunistButtons["ClosePickedGeneral"], mPickedGeneralWindow);
	
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
	std::vector<std::shared_ptr<GUIEditField> > foodFields;
	std::vector<std::shared_ptr<GUIEditField> > goodsFields;
	std::vector<std::shared_ptr<GUIEditField> > techFields;
	std::vector<std::shared_ptr<GUIText> >		resourcesTotalCost;
	std::vector<std::shared_ptr<GUIText> >		totalYearCost;

	foodFields.push_back(mYearOneFood);
	foodFields.push_back(mYearTwoFood);
	foodFields.push_back(mYearThreeFood);
	foodFields.push_back(mYearFourFood);
	foodFields.push_back(mYearFiveFood);

	goodsFields.push_back(mYearOneGoods);
	goodsFields.push_back(mYearTwoGoods);
	goodsFields.push_back(mYearThreeGoods);
	goodsFields.push_back(mYearFourGoods);
	goodsFields.push_back(mYearFiveGoods);

	techFields.push_back(mYearOneTech);
	techFields.push_back(mYearTwoTech);
	techFields.push_back(mYearThreeTech);
	techFields.push_back(mYearFourTech);
	techFields.push_back(mYearFiveTech);

	totalYearCost.push_back(mTotalCostYearOne);
	totalYearCost.push_back(mTotalCostYearTwo);
	totalYearCost.push_back(mTotalCostYearThree);
	totalYearCost.push_back(mTotalCostYearFour);
	totalYearCost.push_back(mTotalCostYearFive);

	resourcesTotalCost.push_back(mYearOneFoodCost);
	resourcesTotalCost.push_back(mYearTwoFoodCost);
	resourcesTotalCost.push_back(mYearThreeFoodCost);
	resourcesTotalCost.push_back(mYearFourFoodCost);
	resourcesTotalCost.push_back(mYearFiveFoodCost);

	resourcesTotalCost.push_back(mYearOneGoodsCost);
	resourcesTotalCost.push_back(mYearTwoGoodsCost);
	resourcesTotalCost.push_back(mYearThreeGoodsCost);
	resourcesTotalCost.push_back(mYearFourGoodsCost);
	resourcesTotalCost.push_back(mYearFiveGoodsCost);

	resourcesTotalCost.push_back(mYearOneTechCost);
	resourcesTotalCost.push_back(mYearTwoTechCost);
	resourcesTotalCost.push_back(mYearThreeTechCost);
	resourcesTotalCost.push_back(mYearFourTechCost);
	resourcesTotalCost.push_back(mYearFiveTechCost);

	int resources = resourcesTotalCost.size()/3;
	for(int i = 0; i < 3; ++i)
	{
		for(int j = 0; j < resources; ++j)
		{
			resourcesTotalCost[j + i*resources] = GUIText::create(sf::FloatRect(35 + 101 * i, 83 * (j+1), 20, 20), "0", mFiveYearPlanWindow);
		}
	}

	for(int i = 0; i < 5; ++i)
	{
		foodFields[i]->setOnGuiChangeFunction([=]()
		{
			int foodAmount = stringToInt(foodFields[i]->getText());
			int goodsAmount = stringToInt(goodsFields[i]->getText());
			int techAmount = stringToInt(techFields[i]->getText());
			int amount = foodAmount * foodCost + goodsAmount * goodsCost + techAmount * techCost;
			resourcesTotalCost[i]->setText(foodAmount * foodCost);
			totalYearCost[i]->setText(amount);
		});
		goodsFields[i]->setOnGuiChangeFunction([=]()
		{
			int foodAmount = stringToInt(foodFields[i]->getText());
			int goodsAmount = stringToInt(goodsFields[i]->getText());
			int techAmount = stringToInt(techFields[i]->getText());
			int amount = foodAmount * foodCost + goodsAmount * goodsCost + techAmount * techCost;
			resourcesTotalCost[i + resources]->setText(goodsAmount * goodsCost);
			totalYearCost[i]->setText(amount);
		});
		techFields[i]->setOnGuiChangeFunction([=]()
		{
			int foodAmount = stringToInt(foodFields[i]->getText());
			int goodsAmount = stringToInt(goodsFields[i]->getText());
			int techAmount = stringToInt(techFields[i]->getText());
			int amount = foodAmount * foodCost + goodsAmount * goodsCost + techAmount * techCost;
			resourcesTotalCost[i + resources*2]->setText(techAmount * techCost);
			totalYearCost[i]->setText(amount);
		});
	}

	
	//--------------------------------------------------------------------------
	/*Taxes, år ett*/
	mYearOneLowerTaxesButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearOneTaxesText->getText()) - taxChange;
		if(amount < 5)
			amount = 5;
		mYearOneTaxesText->setText(amount);
		mIncomeYearOne->setText(amount * mPopulation);
	});
	mYearOneRaiseTaxesButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearOneTaxesText->getText()) + taxChange;
		if(amount > 95)
			amount = 95;
		mYearOneTaxesText->setText(amount);
		mIncomeYearOne->setText(amount * mPopulation);

	});
	mYearTwoLowerTaxesButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearTwoTaxesText->getText()) - taxChange;
		if(amount < 5)
			amount = 5;
		mYearTwoTaxesText->setText(amount);
		mIncomeYearTwo->setText(amount * mPopulation);
	});
	mYearTwoRaiseTaxesButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearTwoTaxesText->getText()) + taxChange;
		if(amount > 95)
			amount = 95;
		mYearTwoTaxesText->setText(amount);
		mIncomeYearTwo->setText(amount * mPopulation);
	});
	mYearThreeLowerTaxesButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearThreeTaxesText->getText()) - taxChange;
		if(amount < 5)
			amount = 5;
		mYearThreeTaxesText->setText(amount);
		mIncomeYearThree->setText(amount * mPopulation);
	});
	mYearThreeRaiseTaxesButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearThreeTaxesText->getText()) + taxChange;
		if(amount > 95)
			amount = 95;
		mYearThreeTaxesText->setText(amount);
		mIncomeYearThree->setText(amount * mPopulation);
	});
	mYearFourLowerTaxesButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFourTaxesText->getText()) - taxChange;
		if(amount < 5)
			amount = 5;
		mYearFourTaxesText->setText(amount);
		mIncomeYearFour->setText(amount * mPopulation);
	});
	mYearFourRaiseTaxesButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFourTaxesText->getText()) + taxChange;
		if(amount > 95)
			amount = 95;
		mYearFourTaxesText->setText(amount);
		mIncomeYearFour->setText(amount * mPopulation);
	});
	mYearFiveLowerTaxesButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFiveTaxesText->getText()) - taxChange;
		if(amount < 5)
			amount = 5;
		mYearFiveTaxesText->setText(amount);
		mIncomeYearFive->setText(amount * mPopulation);
	});
	mYearFiveRaiseTaxesButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFiveTaxesText->getText()) + taxChange;
		if(amount > 95)
			amount = 95;
		mYearFiveTaxesText->setText(amount);
		mIncomeYearFive->setText(amount * mPopulation);
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
		mFiveYearPlanWindow->setEnabled(true, true);

		mFiveYearPlanWindow->setVisible(true); 
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

		mBuySpaceProgramText->setText(mSpaceText->getText());
		mBuyNuclearText->setText(mNuclearText->getText());
		mBuySpyNetworkText->setText(mSpyText->getText());
		mUpgradeWindow->setVisible(true); 
		mCommunistUpgradeButton->setTexture(CommunistButtons["UpgradeIsPressed"]);
	});
	
	/*Export knappen på interface*/
	mCommunistExportButton->setOnClickFunction([=]()			
	{ 	
		mCommunistMainWindow->setEnabled(false, true);
		mExportWindow->setEnabled(true, true);
		mExportWindow->setVisible(true); 
		mCommunistExportButton->setTexture(CommunistButtons["ExportIsPressed"]);
	});


	mFiveYearPlanCloseButton->setOnClickFunction([=]()					
	{ 
		mCommunistMainWindow->setEnabled(true, true);
		updateAllResources();
		mFiveYearPlanWindow->setVisible(false); 
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

	/*GUI hantering för valet av general*/
	/*Bläddra mellan generalerna för att välja mellan de fem som finns*/
	mGoToNextPortraitButton->setOnClickFunction([=]()			
	{	
		generalCount++;
		if(generalCount > 4)
			generalCount = 0;

		mFirstGeneralButton->setTexture(std::pair<sf::FloatRect, sf::Texture*>
			(mFirstGeneralButton->getRectangle(), GameManager::getInstance()->getGeneral(generalCount)->getTexture()));

		mFirstGeneralPlaque->setTexture(std::pair<sf::FloatRect, sf::Texture*>
			(mFirstGeneralPlaque->getRectangle(), &GameManager::getInstance()->getGeneralPlaque(GameManager::getInstance()->getGeneral(generalCount))));
	});

	mGoToPreviousPortraitButton->setOnClickFunction([=]()		
	{	
		generalCount--;
		if(generalCount < 0)
			generalCount = 4;

		mFirstGeneralButton->setTexture(std::pair<sf::FloatRect, sf::Texture*>
			(mFirstGeneralButton->getRectangle(), GameManager::getInstance()->getGeneral(generalCount)->getTexture()));

		mFirstGeneralPlaque->setTexture(std::pair<sf::FloatRect, sf::Texture*>
			(mFirstGeneralPlaque->getRectangle(), &GameManager::getInstance()->getGeneralPlaque(GameManager::getInstance()->getGeneral(generalCount))));
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

		mPickedGeneralPlaque->setTexture(std::pair<sf::FloatRect, sf::Texture*>
			(mPickedGeneralPlaque->getRectangle(), mFirstGeneralPlaque->getTexture()));

	});
	/*Stänger ner fönster som visar vilken general som blivit vald*/
	mClosePickedGeneralWindow->setOnClickFunction([=]()
	{
		mPickedGeneralWindow->setVisible(false);
		mCommunistGeneralButton->setTexture(std::pair<sf::FloatRect, sf::Texture*>(mCommunistGeneralButton->getRectangle(), mGeneral->getTexture()));
		mCommunistGeneralButton->setScale(0.63, 0.68);
		mFiveYearPlanWindow->setVisible(true);
		mFiveYearPlanWindow->setEnabled(true, true);
	});

	mCommunistEndTurnButton->setOnClickFunction([=]()	
	{
		GameManager::getInstance()->nextRound();
	});
}

void Communist::updateAllResources()
{
	/*År ett*/
	setYearlyResources(1, "food", stringToInt(mYearOneFood->getText()));
	setYearlyResources(1, "goods", stringToInt(mYearOneGoods->getText()));
	setYearlyResources(1, "tech", stringToInt(mYearOneTech->getText()));
	setYearlyResources(1, "taxes", stringToInt(mYearOneTaxesText->getText()));
	/*År två*/
	setYearlyResources(2, "food", stringToInt(mYearTwoFood->getText()));
	setYearlyResources(2, "goods", stringToInt(mYearTwoGoods->getText()));
	setYearlyResources(2, "tech", stringToInt(mYearTwoTech->getText()));
	setYearlyResources(2, "taxes", stringToInt(mYearTwoTaxesText->getText()));
	/*År tre*/
	setYearlyResources(3, "food", stringToInt(mYearThreeFood->getText()));
	setYearlyResources(3, "goods", stringToInt(mYearThreeGoods->getText()));
	setYearlyResources(3, "tech", stringToInt(mYearThreeTech->getText()));
	setYearlyResources(3, "taxes", stringToInt(mYearThreeTaxesText->getText()));
	/*År fyra*/
	setYearlyResources(4, "food", stringToInt(mYearFourFood->getText()));
	setYearlyResources(4, "goods", stringToInt(mYearFourGoods->getText()));
	setYearlyResources(4, "tech", stringToInt(mYearFourTech->getText()));
	setYearlyResources(4, "taxes", stringToInt(mYearFourTaxesText->getText()));
	/*År fem*/
	setYearlyResources(5, "food", stringToInt(mYearFiveFood->getText()));
	setYearlyResources(5, "goods", stringToInt(mYearFiveGoods->getText()));
	setYearlyResources(5, "tech", stringToInt(mYearFiveTech->getText()));
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