#include "Capitalist.h"
#include "GUIElement.h"
#include "GUIManager.h"
#include "GUIWindow.h"
#include "GUIButton.h"
#include "GUIText.h"
#include "tinyxml2.h"
#include "President.h"
#include "ResourceHandler.h"
#include "Randomizer.h"
#include <iostream>
#include <functional>
#include "GameManager.h"
#include "Timer.h"
#include "TimerHandler.h"
#include <SFML\Window\Mouse.hpp>

static int foodCost		= 10;
static int goodsCost	= 20;
static int techCost		= 30;
static int taxChange	= 5;
static bool activateWindow = false;



Capitalist::Capitalist() :
	mPresident(nullptr),
	mCurrentTax(mTaxes),
	mMinTax(mTaxes-5),
	mMaxTax(mTaxes+5)
{
	mRound				= 0;
	mIncreasePopulation = false;
	mType				= CAPITALIST;

	initializeCapitalistWindow();
	initializeGuiFunctions();
}


Capitalist::~Capitalist()
{
}

void Capitalist::playMusic()
{
	CapitalistMusic["CapitalistMainTheme"]->play();
}

std::shared_ptr<President> Capitalist::getPresident()
{
	return mPresident;
}

void Capitalist::update()
{

	if( (mRound-1) % 4 == 0 ) 
	{
		chooseLeader();
	}
	mCurrentTax = mTaxes;
	mMaxTax = mCurrentTax + 5;
	mMinTax = mCurrentTax - 5;
}

void Capitalist::setTaxesCost(int tax)
{
	mTaxes = tax;
}

void Capitalist::setPresident(std::shared_ptr<President> president)
{

	mPresident = president;

	foodCost	+= president->getFoodPriceModifier();
	goodsCost	+= president->getGoodsPriceModifier();
	techCost	+= president->getTechPriceModifier();
}

//--------------------------------------------
/*Funktioner som ger medlemsvariabler nya värden*/
bool Capitalist::setFood(int value)
{	
	if(mCurrency >= value * foodCost)
	{
 		mFood		+= value;
		mCurrency	-= value * foodCost;
		mFoodText->setText(intToString(mFood));
		return true;
	}
	return false;
}

bool Capitalist::setGoods(int value)
{
	if(mCurrency >= value * goodsCost)
	{
		mGoods		+= value;
		mCurrency	-= value * goodsCost;
		mGoodsText->setText(intToString(mGoods));
		return true;
	}
	return false;
}

bool Capitalist::setTech(int value)
{
	if(mCurrency >= value * techCost)
	{
		mTech		+= value;
		mCurrency	-= value * techCost;
		mTechText->setText(intToString(mTech));
		return true;
	}
	return false;
}

//-----------------------------------------------------------
/*	
	Uppgraderar mNuclearWeapon med ett
	Kostar 10 mGoods och 5 mTech
										*/
bool Capitalist::upgradeNuclearWeapon(int value)
{
	int goodsNuclearPrice = 10 * mPresident->getNuclearPriceModifier() * value;
	int techNuclearPrice = 5  * mPresident->getNuclearPriceModifier() * value;
	if(mGoods >= goodsNuclearPrice && mTech >= techNuclearPrice)
	{
		mNuclearWeapon += value;
		mGoods -= goodsNuclearPrice;
		mTech -= techNuclearPrice;
		mNuclearText->setText(mNuclearWeapon);
		return true;
	}
	return false;
}

/*	
	Uppgraderar mSpaceProgram med ett
	Kostar 5 mGoods multiplicerat med den nuvarande nivån
	och 10 mTech multiplicerat med den nuvarande nivån
															*/
bool Capitalist::upgradeSpaceProgram(int value)
{
	int goodsSpaceProgramPrice = (mSpaceProgram == 0) ? 1 : mSpaceProgram * 5 * mPresident->getSpacePriceModifier() * value;
	int techSpaceProgramPrice = (mSpaceProgram == 0) ? 1 : mSpaceProgram * 10 * mPresident->getSpacePriceModifier() * value;
	if(mGoods >= goodsSpaceProgramPrice && mTech >= techSpaceProgramPrice)
	{
		mSpaceProgram += value;
		mGoods -= goodsSpaceProgramPrice;
		mTech -= techSpaceProgramPrice;
		mSpaceText->setText(mSpaceProgram);
		return true;
	}
	return false;
}

/*	
	Uppgraderar mSpyNetwork med ett
	Kostar 10 mTech multiplicerat med den nuvarande nivån
															*/
bool Capitalist::upgradeSpyNetwork(int value)
{
	int spyNetworkPrice = (mSpyNetwork == 0) ? 1 : mSpyNetwork * 10 * mPresident->getSpyPriceModifier() * value;

	if(mTech >= spyNetworkPrice)
	{
		mSpyNetwork += value;
		mTech -= spyNetworkPrice;
		mSpyText->setText(mSpyNetwork);
		return true;
	}
	return false;
}


void Capitalist::resetResourcesValues()
{
	mFoodCost->setText(0);
	mGoodsCost->setText(0);
	mTechCost->setText(0);
	mBuyFoodText->setText(0);
	mBuyGoodsText->setText(0);
	mBuyTechText->setText(0);
}

//------------------------------------------------------------------------------------------------------

 /*
	Ladda hem alla knapparnas positioner från XML dokument

	Av: Jon Wahlström 2013-01-31
 
 */
void Capitalist::loadButtonPosition()
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile("XML/CapitalistButtonPos.xml");

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

		CapitalistButtons[tempName].first.left = posX;
		CapitalistButtons[tempName].first.top = posY;
		CapitalistButtons[tempName].first.width = Width;
		CapitalistButtons[tempName].first.height = Height;
		CapitalistButtons[tempName].second = &ResourceHandler::getInstance()->getTexture(name);

		button = button->NextSiblingElement();
	}
}

 /*
	Laddar in fönstrernas positioner via XML dokument.

		Av: Jon Wahlström 2013-01-31
 */
void Capitalist::loadWindowPosition()
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile("XML/CapitalistWindowPos.xml");

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
				
		CapitalistWindows[tempName].first.left = posX;
		CapitalistWindows[tempName].first.top = posY;
		CapitalistWindows[tempName].first.width = Width;
		CapitalistWindows[tempName].first.height = Height;
		CapitalistWindows[tempName].second = &ResourceHandler::getInstance()->getTexture(name);
		
		window = window->NextSiblingElement();
	}	
}

	/*Laddar in kapitalisternas music och ljud via XML 
			
			Av: Jon Wahlström*/
void Capitalist::loadCapitalistMusic()
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile("XML/CapitalistSounds.xml");

	if(doc.Error())
		std::cout << "Fel! Capitalist::loadCapitalistMusic";
	
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
				
		CapitalistMusic[tempName] = ResourceHandler::getInstance()->getMusic(name);
	
		music = music->NextSiblingElement();	
	}
}



 /*
  Initierar huvudfönster med all GUI-information som behövs
 
 				Av: Jon Wahlström		2013-01-23
																*/
void Capitalist::initializeCapitalistWindow()
{
	loadButtonPosition();
	loadWindowPosition();
	loadCapitalistMusic();
	//playMusic();

	mCapitalistMainWindow				= GUIWindow::create(CapitalistWindows["CapitalistInterface"]);
	mCapitalistPresident				= GUIButton::create(CapitalistButtons["President"], mCapitalistMainWindow);
	mCapitalistTaxesButton				= GUIButton::create(CapitalistButtons["Taxes"], mCapitalistMainWindow);
	mCapitalistResourceButton			= GUIButton::create(CapitalistButtons["Resource"], mCapitalistMainWindow);
	mCapitalistUpgradeButton			= GUIButton::create(CapitalistButtons["Upgrade"], mCapitalistMainWindow);
	mCapitalistExportButton				= GUIButton::create(CapitalistButtons["Export"], mCapitalistMainWindow);
	mCapitalistEndTurnButton			= GUIButton::create(CapitalistButtons["EndTurn"], mCapitalistMainWindow);
	mCapitalistMainWindow->setVisible(false);

	/*GUI text för utskrift av värden på komunisternas interface*/
	mNuclearText						= GUIText::create(sf::FloatRect(815, 16, 40, 40), intToString(getNuclearWeapon()), mCapitalistMainWindow);
	mSpaceText							= GUIText::create(sf::FloatRect(815, 228, 40, 40), intToString(getSpaceProgram()), mCapitalistMainWindow);
	mSpyText							= GUIText::create(sf::FloatRect(815, 440, 40, 40), intToString(getSpyNetwork()), mCapitalistMainWindow);
	mFoodText							= GUIText::create(sf::FloatRect(10, 16, 40, 40), intToString(getFood()), mCapitalistMainWindow);
	mGoodsText							= GUIText::create(sf::FloatRect(10, 228, 40, 40), intToString(getGoods()), mCapitalistMainWindow);
	mTechText							= GUIText::create(sf::FloatRect(10, 440, 40, 40), intToString(getTech()), mCapitalistMainWindow);

	mTaxesWindow						= GUIWindow::create(CapitalistWindows["CapitalistTaxesWindow"], mCapitalistMainWindow);
	mLowerTaxesButton					= GUIButton::create(CapitalistButtons["LowerTaxes"], mTaxesWindow);
	mRaiseTaxesButton					= GUIButton::create(CapitalistButtons["RaiseTaxes"], mTaxesWindow);
	mTaxesCloseButton					= GUIButton::create(CapitalistButtons["CloseTaxes"], mTaxesWindow);
	mTaxesWindow->setVisible(false);
	/*Text för skatten*/
	mTaxesText							= GUIText::create(sf::FloatRect(300, 50, 40, 40), intToString(mTaxes), mCapitalistMainWindow);
	mChangeTaxesText					= GUIText::create(sf::FloatRect(93, 70, 40, 40), intToString(getTaxes()), mTaxesWindow);
	
	/*Resursfönster med knappar*/
	mResourceWindow						= GUIWindow::create(CapitalistWindows["CapitalistResourceWindow"], mCapitalistMainWindow);
	mLowerFoodByTenButton				= GUIButton::create(CapitalistButtons["LowerFoodByTen"], mResourceWindow);
	mLowerFoodByFiveButton				= GUIButton::create(CapitalistButtons["LowerFoodByFive"], mResourceWindow);
	mLowerFoodByOneButton				= GUIButton::create(CapitalistButtons["LowerFoodByOne"], mResourceWindow);
	mRaiseFoodByOneButton				= GUIButton::create(CapitalistButtons["RaiseFoodByOne"], mResourceWindow);
	mRaiseFoodByFiveButton				= GUIButton::create(CapitalistButtons["RaiseFoodByFive"], mResourceWindow);
	mRaiseFoodByTenButton				= GUIButton::create(CapitalistButtons["RaiseFoodByTen"], mResourceWindow);

	mLowerGoodsByTenButton				= GUIButton::create(CapitalistButtons["LowerGoodsByTen"], mResourceWindow);
	mLowerGoodsByFiveButton				= GUIButton::create(CapitalistButtons["LowerGoodsByFive"], mResourceWindow);
	mLowerGoodsByOneButton				= GUIButton::create(CapitalistButtons["LowerGoodsByOne"], mResourceWindow);
	mRaiseGoodsByOneButton				= GUIButton::create(CapitalistButtons["RaiseGoodsByOne"], mResourceWindow);
	mRaiseGoodsByFiveButton				= GUIButton::create(CapitalistButtons["RaiseGoodsByFive"], mResourceWindow);
	mRaiseGoodsByTenButton				= GUIButton::create(CapitalistButtons["RaiseGoodsByTen"], mResourceWindow);

	mLowerTechByTenButton				= GUIButton::create(CapitalistButtons["LowerTechByTen"], mResourceWindow);
	mLowerTechByFiveButton				= GUIButton::create(CapitalistButtons["LowerTechByFive"], mResourceWindow);
	mLowerTechByOneButton				= GUIButton::create(CapitalistButtons["LowerTechByOne"], mResourceWindow);
	mRaiseTechByOneButton				= GUIButton::create(CapitalistButtons["RaiseTechByOne"], mResourceWindow);
	mRaiseTechByFiveButton				= GUIButton::create(CapitalistButtons["RaiseTechByFive"], mResourceWindow);
	mRaiseTechByTenButton				= GUIButton::create(CapitalistButtons["RaiseTechByTen"], mResourceWindow);
	mResourceCloseButton				= GUIButton::create(CapitalistButtons["CloseResource"], mResourceWindow);
	/*Text för resurser*/
	mBuyFoodText						= GUIText::create(sf::FloatRect(89, 57, 40, 40), "0",mResourceWindow);
	mBuyGoodsText						= GUIText::create(sf::FloatRect(269, 57, 40, 40), "0", mResourceWindow);
	mBuyTechText						= GUIText::create(sf::FloatRect(449, 57, 40, 40), "0", mResourceWindow);
	mFoodCost							= GUIText::create(sf::FloatRect(30, 20, 40, 40), "0", mResourceWindow);
	mGoodsCost							= GUIText::create(sf::FloatRect(210, 20, 40, 40), "0", mResourceWindow);
	mTechCost							= GUIText::create(sf::FloatRect(390, 20, 40, 40), "0", mResourceWindow);
	//mTotalResourcesCost					= GUIText::create( sf::FloatRect(20, 30, 40, 40), "0", mResourceWindow);
	mResourceWindow->setVisible(false);

	/*Uppgraderingsfönster med knappar*/
	mUpgradeWindow						= GUIWindow::create(CapitalistWindows["CapitalistUpgradeWindow"], mCapitalistMainWindow);
	mUpgradeNuclearWeaponButton		    = GUIButton::create(CapitalistButtons["UpgradeNuclearWeapon"], mUpgradeWindow);
	mUpgradeSpaceProgramButton			= GUIButton::create(CapitalistButtons["UpgradeSpaceProgram"], mUpgradeWindow);
	mUpgradeSpyNetworkButton			= GUIButton::create(CapitalistButtons["UpgradeSpyNetwork"], mUpgradeWindow);
	mUpgradeCloseButton					= GUIButton::create(CapitalistButtons["CloseUpgrade"], mUpgradeWindow);
	/*Text för uppgradering*/
	mBuyNuclearText						= GUIText::create(sf::FloatRect(159, 145, 22, 22), "0", mUpgradeWindow);
	mBuySpaceProgramText				= GUIText::create(sf::FloatRect(337, 107, 22, 22), "0", mUpgradeWindow);
	mBuySpyNetworkText					= GUIText::create(sf::FloatRect(517, 78, 22, 22), "0", mUpgradeWindow);
	mUpgradeWindow->setVisible(false);

	mExportWindow						= GUIWindow::create(CapitalistWindows["CapitalistExportWindow"], mCapitalistMainWindow);
	mExportLowerFoodButton				= GUIButton::create(CapitalistButtons["LowerFood"], mExportWindow); 
	mExportRaiseFoodButton				= GUIButton::create(CapitalistButtons["RaiseFood"], mExportWindow);
	mExportLowerGoodsButton				= GUIButton::create(CapitalistButtons["LowerGoods"], mExportWindow);
	mExportRaiseGoodsButton				= GUIButton::create(CapitalistButtons["RaiseGoods"], mExportWindow);
	mExportLowerTechButton				= GUIButton::create(CapitalistButtons["LowerTech"], mExportWindow);
	mExportRaiseTechButton				= GUIButton::create(CapitalistButtons["RaiseTech"], mExportWindow);
	mExportCloseButton					= GUIButton::create(CapitalistButtons["CloseExport"], mExportWindow);
	mExportWindow->setVisible(false);
	

	mChoosePresidentWindow				= GUIWindow::create(CapitalistWindows["ChoosePresident"], mCapitalistMainWindow);
	mPickedPresidentWindow				= GUIWindow::create(CapitalistWindows["PickedPresident"], mCapitalistMainWindow);
	mFirstPresidentButton				= GUIButton::create(CapitalistButtons["FirstPresident"], mChoosePresidentWindow);
	mSecondPresidentButton				= GUIButton::create(CapitalistButtons["SecondPresident"], mChoosePresidentWindow);
	mPickedPresidentButton				= GUIButton::create(CapitalistButtons["PickedPresident"], mPickedPresidentWindow);
	mClosePresidentWindow				= GUIButton::create(CapitalistButtons["ClosePresident"], mChoosePresidentWindow);
	mPickedPresidentWindow->setVisible(false);
	
	chooseLeader();

	/*
	 	Lägger in föräldernoden i vektorn som finns i GUIManager
	 	och kommer automatiskt få med sig alla barnnoder till denna
	 	vilket är alla GUIElement som finns i denna klass som har 
	 	mCapitalistMainWindow som parent-argument i dess konstruktor
																		*/
	GUIManager::getInstance()->addGUIElement(mCapitalistMainWindow);
}



void Capitalist::chooseLeader()
{
	mCapitalistMainWindow->setEnabled(false, true);
	mChoosePresidentWindow->setEnabled(true, true);
	mChoosePresidentWindow->setVisible(true);
	if(mPresident == NULL)
		mFirstPresident = GameManager::getInstance()->getRandomPresident();
	else
	{
		if(mPresident->getYearsElected() < 2)
			mFirstPresident = mPresident;
		else
			mFirstPresident = GameManager::getInstance()->getRandomPresident();
	}
	mSecondPresident = GameManager::getInstance()->getRandomPresident();

	mFirstPresidentButton->setTexture(std::pair<sf::FloatRect, sf::Texture*>(mFirstPresidentButton->getRectangle(), mFirstPresident->getTexture()));
	mSecondPresidentButton->setTexture(std::pair<sf::FloatRect, sf::Texture*>(mSecondPresidentButton->getRectangle(), mSecondPresident->getTexture()));
}



 /**/
void Capitalist::initializeGuiFunctions()
{
	/*Taxes GUI-window knapparna*/
	mCapitalistTaxesButton->setOnClickFunction([=]()			
	{ 
		mCapitalistMainWindow->setEnabled(false, true);
		mTaxesWindow->setEnabled(true, true);
		mTaxesWindow->setVisible(true); 
		mCapitalistTaxesButton->setTexture(CapitalistButtons["TaxesIsPressed"]);
	});
	/*Sänker skatten med fem*/
	mLowerTaxesButton->setOnClickFunction([=]()
	{
		mTaxes -= 5;
		if(mTaxes <= mMinTax)
			mTaxes = mMinTax;
		if(mTaxes < 5)
			mTaxes = 5;
		mChangeTaxesText->setText(intToString(mTaxes));
		
	});
	/*Höjer skatten med fem*/
	mRaiseTaxesButton->setOnClickFunction([=]()
	{
		std::cout<<"mTaxes before: "<<mTaxes<<std::endl;
		mTaxes += 5;
		if(mTaxes >= mMaxTax)
			mTaxes = mMaxTax;
		if(mTaxes > 95)
			mTaxes = 95;
		std::cout<<"mTaxes: "<<mTaxes<<" max tax: "<<mMaxTax<<std::endl;
		mChangeTaxesText->setText(intToString(mTaxes));
	});

	/*Resources GUI-Window knappar*/
	mCapitalistResourceButton->setOnClickFunction([=]()			
	{ 
		mCapitalistMainWindow->setEnabled(false, true);
		mResourceWindow->setEnabled(true, true);
		mResourceWindow->setVisible(true); 
		mCapitalistResourceButton->setTexture(CapitalistButtons["ResourceIsPressed"]);
	});

	mLowerFoodByTenButton->setOnClickFunction([=]()
	{ 
		int amount = stringToInt(mBuyFoodText->getText()) - 10;
		int cost = stringToInt(mFoodCost->getText()) + foodCost * -10;
		if(amount >= 0)
		{
			mBuyFoodText->setText(amount);
			mFoodCost->setText(cost);
		}
	});			
	mLowerFoodByFiveButton->setOnClickFunction([=]()
	{ 
		int amount = stringToInt(mBuyFoodText->getText()) - 5;
		int cost = stringToInt(mFoodCost->getText()) + foodCost * -5;
		if(amount >= 0)
		{
			mBuyFoodText->setText(amount);
			mFoodCost->setText(cost);
		}
	});	
	mLowerFoodByOneButton->setOnClickFunction([=]()
	{ 
		int amount = stringToInt(mBuyFoodText->getText()) - 1;
		int cost = stringToInt(mFoodCost->getText()) + foodCost * -1;
		if(amount >= 0)
		{
			mBuyFoodText->setText(amount);
			mFoodCost->setText(cost);
		}
	});				
	mRaiseFoodByOneButton->setOnClickFunction([=]()
	{ 
		int amount = stringToInt(mBuyFoodText->getText()) + 1;
		int cost = stringToInt(mFoodCost->getText()) + foodCost * 1;
		mBuyFoodText->setText(amount);
		mFoodCost->setText(cost);
	});	
	mRaiseFoodByFiveButton->setOnClickFunction([=]()
	{ 
		int amount = stringToInt(mBuyFoodText->getText()) + 5;
		int cost = stringToInt(mFoodCost->getText()) + foodCost * 5;
		mBuyFoodText->setText(amount);
		mFoodCost->setText(cost);
	});	
	mRaiseFoodByTenButton->setOnClickFunction([=]()
	{ 
		int amount = stringToInt(mBuyFoodText->getText()) + 10;
		int cost = stringToInt(mFoodCost->getText()) + foodCost * 10;
		mBuyFoodText->setText(amount);
		mFoodCost->setText(cost);
	});	

	mLowerGoodsByTenButton->setOnClickFunction([=]()
	{ 
		int amount = stringToInt(mBuyGoodsText->getText()) - 10;
		int cost = stringToInt(mGoodsCost->getText()) + goodsCost * -10;
		if(amount >= 0)
		{
			mBuyGoodsText->setText(amount);
			mGoodsCost->setText(cost);
		}
	});	
	mLowerGoodsByFiveButton->setOnClickFunction([=]()
	{ 
		int amount = stringToInt(mBuyGoodsText->getText()) - 5;
		int cost = stringToInt(mGoodsCost->getText()) + goodsCost * -5;
		if(amount >= 0)
		{
			mBuyGoodsText->setText(amount);
			mGoodsCost->setText(cost);
		}
	});	
	mLowerGoodsByOneButton->setOnClickFunction([=]()
	{ 
		int amount = stringToInt(mBuyGoodsText->getText()) - 1;
		int cost = stringToInt(mGoodsCost->getText()) + goodsCost * -1;
		if(amount >= 0)
		{
			mBuyGoodsText->setText(amount);
			mGoodsCost->setText(cost);
		}
	});	
	mRaiseGoodsByOneButton->setOnClickFunction([=]()
	{ 
		int amount = stringToInt(mBuyGoodsText->getText()) + 1;
		int cost = stringToInt(mGoodsCost->getText()) + goodsCost * 1;
		mBuyGoodsText->setText(amount);
		mGoodsCost->setText(cost);
	});	
	mRaiseGoodsByFiveButton->setOnClickFunction([=]()
	{ 
		int amount = stringToInt(mBuyGoodsText->getText()) + 5;
		int cost = stringToInt(mGoodsCost->getText()) + goodsCost * 5;
		mBuyGoodsText->setText(amount);
		mGoodsCost->setText(cost);
	});	
	mRaiseGoodsByTenButton->setOnClickFunction([=]()
	{ 
		int amount = stringToInt(mBuyGoodsText->getText()) + 10;
		int cost = stringToInt(mGoodsCost->getText()) + goodsCost * 10;
		mBuyGoodsText->setText(amount);
		mGoodsCost->setText(cost);
	});	

	mLowerTechByTenButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mBuyTechText->getText()) - 10;
		int cost = stringToInt(mTechCost->getText()) + techCost * -10;
		if(amount >= 0)
		{
			mBuyTechText->setText(amount);
			mTechCost->setText(cost);
		}
	});
	mLowerTechByFiveButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mBuyTechText->getText()) - 5;
		int cost = stringToInt(mTechCost->getText()) + techCost * -5;
		if(amount >= 0)
		{
			mBuyTechText->setText(amount);
			mTechCost->setText(cost);
		}
	});
	mLowerTechByOneButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mBuyTechText->getText()) - 1;
		int cost = stringToInt(mTechCost->getText()) + techCost * -1;
		if(amount >= 0)
		{
			mBuyTechText->setText(amount);
			mTechCost->setText(cost);
		}
	});
	mRaiseTechByOneButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mBuyTechText->getText()) + 1;
		int cost = stringToInt(mTechCost->getText()) + techCost * 1;
		mBuyTechText->setText(amount);
		mTechCost->setText(cost);
	});
	mRaiseTechByFiveButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mBuyTechText->getText()) + 5;
		int cost = stringToInt(mTechCost->getText()) + techCost * 5;
		mBuyTechText->setText(amount);
		mTechCost->setText(cost);
	});
	mRaiseTechByTenButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mBuyTechText->getText()) + 10;
		int cost = stringToInt(mTechCost->getText()) + techCost * 10;
		mBuyTechText->setText(amount);
		mTechCost->setText(cost);
	});

	/*Upgrade GUI-Window med knappar*/
	mCapitalistUpgradeButton->setOnClickFunction([=]()	
	{ 
		mCapitalistMainWindow->setEnabled(false, true);
		mUpgradeWindow->setEnabled(true, true);

		mUpgradeWindow->setVisible(true);
		mCapitalistUpgradeButton->setTexture(CapitalistButtons["UpgradeIsPressed"]);
		mBuyNuclearText->setText(mNuclearText->getText());
		mBuySpaceProgramText->setText(mSpaceText->getText());
		mBuySpyNetworkText->setText(mSpyText->getText());

	});

	mUpgradeNuclearWeaponButton->setOnClickFunction([=]() 
	{ 
		int amount = stringToInt(mBuyNuclearText->getText());
		++amount;
		mBuyNuclearText->setText(amount);
	});		

	mUpgradeSpaceProgramButton->setOnClickFunction([=]()  
	{
		int amount = stringToInt(mBuySpaceProgramText->getText());
		++amount;
		mBuySpaceProgramText->setText(amount);
	});		

	mUpgradeSpyNetworkButton->setOnClickFunction([=]()	 
	{
		int amount = stringToInt(mBuySpyNetworkText->getText());
		++amount;
		mBuySpyNetworkText->setText(amount);
		
	});		

	/*Export GUI-Window med knapapr*/
	mCapitalistExportButton->setOnClickFunction([=]()	
	{ 
		mCapitalistMainWindow->setEnabled(false, true);
		mExportWindow->setEnabled(true, true);

		mExportWindow->setVisible(true); 
		mCapitalistExportButton->setTexture(CapitalistButtons["ExportIsPressed"]);

	});

	/*Stänger ner Taxes fönstret*/
	mTaxesCloseButton->setOnClickFunction([=]()					
	{ 
		mCapitalistMainWindow->setEnabled(true, true);
		mTaxesWindow->setVisible(false); 
		//ändrar textur till orginal
		mCapitalistTaxesButton->setTexture(CapitalistButtons["Taxes"]);
	});

	/*Stänger ner resources fönstret "Okay-knappen"*/
	mResourceCloseButton->setOnClickFunction([=]()				
	{ 
		int food = stringToInt(mFoodCost->getText());
		int goods = stringToInt(mGoodsCost->getText());
		int tech = stringToInt(mTechCost->getText());
		int totalCost = (food + goods + tech);
		if(mCurrency >= totalCost)
		{
			mCapitalistMainWindow->setEnabled(true, true);

			mResourceWindow->setVisible(false);
			//ändrar textur till orginal
			mCapitalistResourceButton->setTexture(CapitalistButtons["Resource"]);
			setFood(stringToInt(mBuyFoodText->getText()));
			setGoods(stringToInt(mBuyGoodsText->getText()));
			setTech(stringToInt(mBuyTechText->getText()));
			resetResourcesValues();
		}
		else
		{
			//Spela FAILSOUND!
		}		
	});

	/*Stänger ner upgrade fönstret "Okay-knappen"*/
	mUpgradeCloseButton->setOnClickFunction([=]()				
	{
		int nuclearDiff = stringToInt(mBuyNuclearText->getText()) - stringToInt(mNuclearText->getText()); 
		int spaceDiff = stringToInt(mBuySpaceProgramText->getText()) - stringToInt(mSpaceText->getText());
		int spyDiff = stringToInt(mBuySpyNetworkText->getText()) - stringToInt(mSpyText->getText());
		//if(upgradeNuclearWeapon(nuclearDiff) && upgradeSpaceProgram(spaceDiff) && upgradeSpyNetwork(spyDiff))
		//{
			mCapitalistMainWindow->setEnabled(true, true);

			mUpgradeWindow->setVisible(false); 	
			upgradeNuclearWeapon(nuclearDiff); 
			upgradeSpaceProgram(spaceDiff); 
			upgradeSpyNetwork(spyDiff);
			//ändrar textur till orginal
			mCapitalistUpgradeButton->setTexture(CapitalistButtons["Upgrade"]);
		//}
		//else
		//{
		//	//Spela bajsfailljud 
		//}
	});

	/*Stänger ner Export fönster "Okay-knappen"*/
	mExportCloseButton->setOnClickFunction([=]()				
	{ 
		mCapitalistMainWindow->setEnabled(true, true);

		mExportWindow->setVisible(false); 
		//ändrar textur till orginal
		mCapitalistExportButton->setTexture(CapitalistButtons["Export"]);
	});

	
	
	/*Val av president bild 1*/
	mFirstPresidentButton->setOnClickFunction([=]()				
	{ 

		//mChoosePresidentWindow->setVisible(false); 
		//mPickedPresidentWindow->setVisible(true); 
		setPresident(mFirstPresident);

		mPickedPresidentButton->setTexture(std::pair<sf::FloatRect, sf::Texture*>
			(mPickedPresidentButton->getRectangle(), mPresident->getTexture())); 
	});

	/*Val av president bild 2*/
	mSecondPresidentButton->setOnClickFunction([=]()			
	{ 
		//mChoosePresidentWindow->setVisible(false); 
		//mPickedPresidentWindow->setVisible(true); 
		setPresident(mSecondPresident);
		
		mPickedPresidentButton->setTexture(std::pair<sf::FloatRect, sf::Texture*>
			(mPickedPresidentButton->getRectangle(), mPresident->getTexture())); 
	});		

	/*När en president har blivit vald*/
	mClosePresidentWindow->setOnClickFunction([=]()				
	{ 
		/*om ingen president blivit vald går det inte att stänga fönstret*/
		if(mPresident != 0) 
		{
			mChoosePresidentWindow->setVisible(false);
			mPickedPresidentWindow->setVisible(true);
			int yearsElected = mPresident->getYearsElected();
			//std::cout<<"years elected: "<<yearsElected<<std::endl;
			mPresident->setYearsElected(yearsElected + 1);
			//std::vector<std::shared_ptr<void> > args;
			//args.push_back(mPickedPresidentWindow);

			std::shared_ptr<GUIElement> _test = mPickedPresidentWindow;
			std::shared_ptr<GUIButton> _presidentButton = mCapitalistPresident;
			std::shared_ptr<President> _president = mPresident;
			//timer för hur länge presidentval skall visas
			//när det är klart hamnar bilden i vänstra nedre hörn
			std::shared_ptr<GUIWindow> _mainWindow = mCapitalistMainWindow;
			Timer::setTimer([=]()
			{
				_test->setVisible(false);
				
				_presidentButton->setTexture(std::pair<sf::FloatRect, sf::Texture*>(_presidentButton->getRectangle(), _president->getTexture()));
				_presidentButton->setScale(0.53, 0.53);
				_mainWindow->setEnabled(true, true);
			}, 
				5000, 1);//antal millisekunder
		}
	});

	/*nästa runda*/
	mCapitalistEndTurnButton->setOnClickFunction([=]()	
	{ 
		if(mTaxes < mCurrentTax)
			setPatriotism(getPatriotism() + 2);
		else if(mTaxes > mCurrentTax)
			setPatriotism(getPatriotism() - 3);
		
		//mCapitalistEndTurnButton->setTexture(CapitalistButtons["EndTurnIsPressed"]);
		mTaxes = mCurrentTax;
		GameManager::getInstance()->nextRound();  
	});
}

void Capitalist::showGUI()
{
	//std::cout << "Capitalist show gui" << std::endl;
	mCapitalistMainWindow->setVisible(true);
	//mCapitalistEndTurnButton->setVisible(true);
}

void Capitalist::hideGUI()
{
	//std::cout << "Capitalist hide gui" << std::endl;
	mCapitalistMainWindow->setVisible(false);
	//mCapitalistEndTurnButton->setVisible(false);
}

