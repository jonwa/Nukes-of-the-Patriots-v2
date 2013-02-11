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

Capitalist::Capitalist() :
	mPresident(nullptr)
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
}

void Capitalist::setTaxesCost(int tax)
{
	mTaxDecreased = (tax < mTaxes);
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
		return true;
	}
	return false;
}

//-----------------------------------------------------------
/*	
	Uppgraderar mNuclearWeapon med ett
	Kostar 10 mGoods och 5 mTech
										*/
bool Capitalist::upgradeNuclearWeapon()
{
	int goodsNuclearPrice = 10 * mPresident->getNuclearPriceModifier();
	int techNuclearPrice = 5  * mPresident->getNuclearPriceModifier();
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
bool Capitalist::upgradeSpaceProgram()
{
	int goodsSpaceProgramPrice = (mSpaceProgram == 0) ? 1 : mSpaceProgram * 5 * mPresident->getSpacePriceModifier();
	int techSpaceProgramPrice = (mSpaceProgram == 0) ? 1 : mSpaceProgram * 10 * mPresident->getSpacePriceModifier();
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
bool Capitalist::upgradeSpyNetwork()
{
	int spyNetworkPrice = (mSpyNetwork == 0) ? 1 : mSpyNetwork * 10 * mPresident->getSpyPriceModifier();

	if(mTech >= spyNetworkPrice)
	{
		++mSpyNetwork;
		mTech -= spyNetworkPrice;
		return true;
	}
	return false;
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
	doc.LoadFile("XML/CapitalistMusic.xml");

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

	mCapitalistMainWindow				= GUIWindow::create(CapitalistWindows["CapitalistInterface"]);
	mCapitalistPresident				= GUIButton::create(CapitalistButtons["President"], mCapitalistMainWindow);
	mCapitalistTaxesButton				= GUIButton::create(CapitalistButtons["Taxes"], mCapitalistMainWindow);
	mCapitalistResourceButton			= GUIButton::create(CapitalistButtons["Resource"], mCapitalistMainWindow);
	mCapitalistUpgradeButton			= GUIButton::create(CapitalistButtons["Upgrade"], mCapitalistMainWindow);
	mCapitalistExportButton				= GUIButton::create(CapitalistButtons["Export"], mCapitalistMainWindow);
	mCapitalistEndTurnButton			= GUIButton::create(CapitalistButtons["EndTurn"], mCapitalistMainWindow);
	mCapitalistMainWindow->setVisible(false);

	/*GUI text för utskrift av värden på komunisternas interface*/
	mNuclearText						= GUIText::create(sf::FloatRect(962, 16, 40, 40), intToString(getNuclearWeapon()), mCapitalistMainWindow);
	mSpaceText							= GUIText::create(sf::FloatRect(962, 228, 40, 40), intToString(getSpaceProgram()), mCapitalistMainWindow);
	mSpyText							= GUIText::create(sf::FloatRect(962, 440, 40, 40), intToString(getSpyNetwork()), mCapitalistMainWindow);
	mFoodText							= GUIText::create(sf::FloatRect(160, 16, 40, 40), intToString(getFood()), mCapitalistMainWindow);
	mGoodsText							= GUIText::create(sf::FloatRect(160, 228, 40, 40), intToString(getGoods()), mCapitalistMainWindow);
	mTechText							= GUIText::create(sf::FloatRect(160, 440, 40, 40), intToString(getTech()), mCapitalistMainWindow);

	mTaxesWindow						= GUIWindow::create(CapitalistWindows["CapitalistTaxesWindow"], mCapitalistMainWindow);
	mLowerTaxesButton					= GUIButton::create(CapitalistButtons["LowerTaxes"], mTaxesWindow);
	mRaiseTaxesButton					= GUIButton::create(CapitalistButtons["RaiseTaxes"], mTaxesWindow);
	mTaxesCloseButton					= GUIButton::create(CapitalistButtons["CloseTaxes"], mTaxesWindow);
	mTaxesWindow->setVisible(false);

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

	
	mBuyFoodText						= GUIText::create(sf::FloatRect(89, 57, 40, 40), "0",mResourceWindow);
	mBuyGoodsText						= GUIText::create(sf::FloatRect(269, 57, 40, 40), "0", mResourceWindow);
	mBuyTechText						= GUIText::create(sf::FloatRect(449, 57, 40, 40), "0", mResourceWindow);
	mFoodCost							= GUIText::create(sf::FloatRect(30, 20, 40, 40), "0", mResourceWindow);
	mGoodsCost							= GUIText::create(sf::FloatRect(210, 20, 40, 40), "0", mResourceWindow);
	mTechCost							= GUIText::create(sf::FloatRect(390, 20, 40, 40), "0", mResourceWindow);
	//mTotalResourcesCost					= GUIText::create( sf::FloatRect(20, 30, 40, 40), "0", mResourceWindow);
	mResourceWindow->setVisible(false);

	mUpgradeWindow						= GUIWindow::create(CapitalistWindows["CapitalistUpgradeWindow"], mCapitalistMainWindow);
	mUpgradeNuclearWeaponButton		    = GUIButton::create(CapitalistButtons["UpgradeNuclearWeapon"], mUpgradeWindow);
	mUpgradeSpaceProgramButton			= GUIButton::create(CapitalistButtons["UpgradeSpaceProgram"], mUpgradeWindow);
	mUpgradeSpyNetworkButton			= GUIButton::create(CapitalistButtons["UpgradeSpyNetwork"], mUpgradeWindow);
	mUpgradeCloseButton					= GUIButton::create(CapitalistButtons["CloseUpgrade"], mUpgradeWindow);
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
	//mChoosePresidentWindow->setVisible(false);
	
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
	mFirstPresident = GameManager::getInstance()->getRandomPresident();
	mSecondPresident = GameManager::getInstance()->getRandomPresident();

	mFirstPresidentButton->setTexture(std::pair<sf::FloatRect, sf::Texture*>(mFirstPresidentButton->getRectangle(), mFirstPresident->getTexture()));
	mSecondPresidentButton->setTexture(std::pair<sf::FloatRect, sf::Texture*>(mSecondPresidentButton->getRectangle(), mSecondPresident->getTexture()));
}



 /**/
void Capitalist::initializeGuiFunctions()
{
	/*Taxes GUI-window knapparna*/
	mCapitalistTaxesButton->setOnClickFunction([=]()			{ mTaxesWindow->setVisible(true); });
	/*mLowerTaxesButton->setOnClickFunction([=]()					{ mTaxesUpdate = decreaseTaxCost(mTaxesUpdate); });
	mRaiseTaxesButton->setOnClickFunction([=]()					{ mTaxesUpdate = increaseTaxCost(mTaxesUpdate); });*/

	mCapitalistResourceButton->setOnClickFunction([=]()			{ mResourceWindow->setVisible(true); });
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

	mUpgradeNuclearWeaponButton->setOnClickFunction([=]() { upgradeNuclearWeapon(); });		
	mUpgradeSpaceProgramButton->setOnClickFunction([=]()  { upgradeSpaceProgram();  });		
	mUpgradeSpyNetworkButton->setOnClickFunction([=]()	  { upgradeSpyNetwork();    });		

	/**/
	mCapitalistUpgradeButton->setOnClickFunction([=]()	{ mUpgradeWindow->setVisible(true); });

	mCapitalistExportButton->setOnClickFunction([=]()	{ mExportWindow->setVisible(true); });

	mCapitalistEndTurnButton->setOnClickFunction([=]()	{ GameManager::getInstance()->nextRound();  });


	//mTaxesCloseButton->setOnClickFunction([=]()					
	//{ 
	//	mTaxesWindow->setVisible(false); 
	//	mTaxes = mTaxesUpdate; 
	//	std::cout << mTaxes << "\nGAY" << std::endl; 
	//});

	///*Stänger ner resources fönstret "Okay-knappen"*/
	//mResourceCloseButton->setOnClickFunction([=]()				
	//{ 
	//	mResourceWindow->setVisible(false);
	//	setFood(stringToInt(textField->getValue()));
	//	mFood = mFoodUpdate;													
	//	mGoods = mGoodsUpdate;														 
	//	mTech = mTechUpdate; 														
	//});

	///*Stänger ner upgrade fönstret "Okay-knappen"*/
	//mUpgradeCloseButton->setOnClickFunction([=]()				
	//{
	//	mUpgradeWindow->setVisible(false); 														 
	//	mNuclearWeapon = mNuclearWeaponUpdate;														  
	//	mSpaceProgram	 = mSpaceProgramUpdate;														  
	//	mSpyNetwork	 = mSpyNetworkUpdate;															  
	//});

	/*Stänger ner Export fönster "Okay-knappen"*/
	mExportCloseButton->setOnClickFunction([=]()				
	{ 
		mExportWindow->setVisible(false); 
	});

	mCapitalistPresident->setOnClickFunction([=](){});
	
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
	
	mClosePresidentWindow->setOnClickFunction([=]()				
	{ 
		mChoosePresidentWindow->setVisible(false);
		mPickedPresidentWindow->setVisible(true);
		//std::vector<std::shared_ptr<void> > args;
		//args.push_back(mPickedPresidentWindow);
		std::shared_ptr<GUIElement> _test = mPickedPresidentWindow;
		Timer::setTimer([=](){_test->setVisible(false);}, 5000, 1);
	});

	
}

void Capitalist::showGUI()
{
	std::cout << "Capitalist show gui" << std::endl;
	mCapitalistMainWindow->setVisible(true);
	mCapitalistEndTurnButton->setVisible(true);
}

void Capitalist::hideGUI()
{
	std::cout << "Capitalist hide gui" << std::endl;
	mCapitalistMainWindow->setVisible(false);
	mCapitalistEndTurnButton->setVisible(false);
}