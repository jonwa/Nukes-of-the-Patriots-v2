#include "Capitalist.h"
#include "GUIElement.h"
#include "GUIManager.h"
#include "GUIWindow.h"
#include "GUIButton.h"
#include "GUIText.h"
#include "GUIEditField.h"
#include "GUIImage.h"
#include "tinyxml2.h"
#include "President.h"
#include "ResourceHandler.h"
#include "Randomizer.h"
#include <iostream>
#include <functional>
#include "GameManager.h"
#include "Timer.h"
#include "TimerHandler.h"
#include "Communist.h"
#include <SFML\Window\Mouse.hpp>

static int foodCost		= 10;
static int goodsCost	= 20;
static int techCost		= 30;
static int taxChange	= 5;
static bool activateWindow = false;

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

void Capitalist::stopMusic()
{
	CapitalistMusic["CapitalistMainTheme"]->stop();
}

std::shared_ptr<President> Capitalist::getPresident()
{
	return mPresident;
}

void Capitalist::newYearStart()
{
	int foodPatriotismChange = 0;
	if(mFood == 0)
		foodPatriotismChange = -4;
	else if(mFood > 0 && mFood <= mPopulation/2)
		foodPatriotismChange = -2;

	mFood -= mPopulation;
	if(mFood < 0) mFood = 0;
	int taxPatriotismChange = 0;
	int taxChange = mTaxes - mTaxesPreviousRound;
	if(taxChange > 0)
		taxPatriotismChange = -3;
	else if(taxChange < 0)
		taxPatriotismChange = 2;

	std::shared_ptr<SuperPower> enemy = GameManager::getInstance()->getCommunist();

	int enemyNuclearWeapon = enemy->getNuclearWeapon();
	int enemySpaceProgram = enemy->getSpaceProgram();

	int nuclearDifference = mNuclearWeapon - enemyNuclearWeapon;
	int spaceProgramDifference = mSpaceProgram - enemySpaceProgram;

	int nuclearWeaponChange = 0;
	int spaceProgramChange = 0;
	int exportedChange = 0;
	if(nuclearDifference > 0)
		nuclearWeaponChange = (nuclearDifference > enemyNuclearWeapon*2) ? 2 : 1;
	if(spaceProgramDifference > enemySpaceProgram)
		spaceProgramChange += 1;
	
	// My exported resources
	int exportedFoodChange = mExportedFood - (mExportedFood - mExportedFoodPreviousRound);
	int exportedGoodsChange = mExportedGoods - (mExportedGoods - mExportedGoodsPreviousRound);
	int exportedTechChange = mExportedTech - (mExportedTech - mExportedTechPreviousRound);
	int exportedTotal = exportedFoodChange + exportedGoodsChange + exportedTechChange;

	// Enemy exported resources
	int enemyFoodExported = enemy->getExportedFood() - (enemy->getExportedFood() - enemy->getExportedFoodPreviousRound());
	int enemyGoodsExported = enemy->getExportedFood() - (enemy->getExportedGoods() - enemy->getExportedGoodsPreviousRound());
	int enemyTechExported = enemy->getExportedFood() - (enemy->getExportedTech() - enemy->getExportedTechPreviousRound());
	int enemyExportedTotal = enemyFoodExported + enemyGoodsExported + enemyTechExported;

	if(exportedTotal > enemyExportedTotal)
		exportedChange += 1;

	int totalPatriotismChange = foodPatriotismChange + taxPatriotismChange + nuclearWeaponChange + spaceProgramChange + exportedChange;

}

void Capitalist::update()
{
	// Set previous round values as current round values so we can get the difference at the start of the next round
	// Would've been better to use a vector
	mPatriotismPreviousRound = mPatriotism;
	mCurrencyPreviousRound = mCurrency;
	mPopulationPreviousRound = mPopulation;
	mFoodPreviousRound = mFood;
	mGoodsPreviousRound = mGoods;
	mTechPreviousRound = mTech;
	mExportedFoodPreviousRound = mExportedFood;
	mExportedGoodsPreviousRound = mExportedGoods;
	mExportedTechPreviousRound = mExportedTech;
	mTaxesPreviousRound = mTaxes;
	mSpyNetworkPreviousRound = mSpyNetwork;
	mNuclearWeaponPreviousRound = mNuclearWeapon;
	mSpaceProgramPreviousRound = mSpaceProgram;

	if((mRound-1) % 4 == 0 ) 
	{
		chooseLeader();
	}
}

void Capitalist::setTaxesCost(int tax)
{
	//mTaxDecreased = (tax < mTaxes);
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
	mCapitalistTradeButton				= GUIButton::create(CapitalistButtons["Export"], mCapitalistMainWindow);
	mCapitalistEndTurnButton			= GUIButton::create(CapitalistButtons["EndTurn"], mCapitalistMainWindow);
	mLeftPanel							= GUIButton::create(CapitalistButtons["LeftPanel"], mCapitalistMainWindow);
	mRightPanel							= GUIButton::create(CapitalistButtons["RightPanel"], mCapitalistMainWindow);
	mCapitalistMainWindow->setVisible(false);

	/*GUI text för utskrift av värden på komunisternas interface*/
	mNuclearText						= GUIText::create(sf::FloatRect(836, 12, 40, 40), intToString(getNuclearWeapon()), mCapitalistMainWindow);
	mSpaceText							= GUIText::create(sf::FloatRect(836, 224, 40, 40), intToString(getSpaceProgram()), mCapitalistMainWindow);
	mSpyText							= GUIText::create(sf::FloatRect(836, 436, 40, 40), intToString(getSpyNetwork()), mCapitalistMainWindow);
	mFoodText							= GUIText::create(sf::FloatRect(29, 12, 40, 40), intToString(getFood()), mCapitalistMainWindow);
	mGoodsText							= GUIText::create(sf::FloatRect(29, 224, 40, 40), intToString(getGoods()), mCapitalistMainWindow);
	mTechText							= GUIText::create(sf::FloatRect(29, 436, 40, 40), intToString(getTech()), mCapitalistMainWindow);

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

	
	mBuyFoodText						= GUIText::create(sf::FloatRect(93, 70, 40, 40), "0",mResourceWindow);
	mBuyGoodsText						= GUIText::create(sf::FloatRect(273, 70, 40, 40), "0", mResourceWindow);
	mBuyTechText						= GUIText::create(sf::FloatRect(453, 70, 40, 40), "0", mResourceWindow);
	mFoodCost							= GUIText::create(sf::FloatRect(30, 20, 40, 40), "0", mResourceWindow);
	mGoodsCost							= GUIText::create(sf::FloatRect(210, 20, 40, 40), "0", mResourceWindow);
	mTechCost							= GUIText::create(sf::FloatRect(390, 20, 40, 40), "0", mResourceWindow);
	//mTotalResourcesCost				= GUIText::create( sf::FloatRect(20, 30, 40, 40), "0", mResourceWindow);
	mResourceWindow->setVisible(false);

	mUpgradeWindow						= GUIWindow::create(CapitalistWindows["CapitalistUpgradeWindow"], mCapitalistMainWindow);
	mUpgradeNuclearWeaponButton		    = GUIButton::create(CapitalistButtons["UpgradeNuclearWeapon"], mUpgradeWindow);
	mUpgradeSpaceProgramButton			= GUIButton::create(CapitalistButtons["UpgradeSpaceProgram"], mUpgradeWindow);
	mUpgradeSpyNetworkButton			= GUIButton::create(CapitalistButtons["UpgradeSpyNetwork"], mUpgradeWindow);
	mCancelUpgradeNuclearWeaponButton	= GUIButton::create(CapitalistButtons["CancelUpgradeNuclearWeapon"], mUpgradeWindow);
	mCancelUpgradeSpaceProgramButton	= GUIButton::create(CapitalistButtons["CancelUpgradeSpaceProgram"], mUpgradeWindow);
	mCancelUpgradeSpyNetworkButton		= GUIButton::create(CapitalistButtons["CancelUpgradeSpyNetwork"], mUpgradeWindow);
	mUpgradeCloseButton					= GUIButton::create(CapitalistButtons["CloseUpgrade"], mUpgradeWindow);

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

	mExportFoodPriceEditField			= GUIEditField::create(sf::FloatRect(509, 51, 100, 50), "0", true, mExportWindow);
	mExportGoodsPriceEditField			= GUIEditField::create(sf::FloatRect(509, 110, 100, 50), "0", true, mExportWindow);
	mExportTechPriceEditField			= GUIEditField::create(sf::FloatRect(509, 169, 100, 50), "0", true, mExportWindow);

	mExportWindow->setVisible(false);

	mImportWindow						= GUIWindow::create(CapitalistWindows["CapitalistImportWindow"], mCapitalistMainWindow);
	
	mImportResourcesAvailableText[0]	= GUIText::create(sf::FloatRect(150, 51, 56, 31), "50", mImportWindow);
	mImportResourcesAvailableText[1]	= GUIText::create(sf::FloatRect(150, 110, 56, 31), "50", mImportWindow);
	mImportResourcesAvailableText[2]	= GUIText::create(sf::FloatRect(150, 169, 56, 31), "50", mImportWindow);

	mImportPriceText[0]					= GUIText::create(sf::FloatRect(221, 51, 56, 31), "1", mImportWindow);
	mImportPriceText[1]					= GUIText::create(sf::FloatRect(221, 110, 56, 31), "1", mImportWindow);
	mImportPriceText[2]					= GUIText::create(sf::FloatRect(221, 169, 56, 31), "1", mImportWindow);
	
	sf::Texture *buyField = &ResourceHandler::getInstance()->getTexture(std::string("Menu/Namnruta-aktiv"));

	mImportBuyQuantityBackground[0]		= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(329, 51, 56, 31), buyField), mImportWindow);
	mImportBuyQuantityBackground[1]		= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(329, 110, 56, 31), buyField), mImportWindow);
	mImportBuyQuantityBackground[2]		= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(329, 169, 56, 31), buyField), mImportWindow);
	
	for(int i = 0; i < sizeof(mImportBuyQuantityBackground)/sizeof(mImportBuyQuantityBackground[0]); i++)
	{
		mImportBuyQuantityText[i] = GUIText::create(
			sf::FloatRect(mImportBuyQuantityBackground[i]->getLocalX() + mImportBuyQuantityBackground[i]->getWidth()/2, 
			mImportBuyQuantityBackground[i]->getLocalY() + mImportBuyQuantityBackground[i]->getHeight()/2,
			100, 50), "0", mImportWindow);

	}

	mImportCostText[0]					= GUIText::create(sf::FloatRect(509, 51, 100, 50), "0", mImportWindow);
	mImportCostText[1]					= GUIText::create(sf::FloatRect(509, 110, 100, 50), "0", mImportWindow);
	mImportCostText[2]					= GUIText::create(sf::FloatRect(509, 169, 100, 50), "0", mImportWindow);

	mImportGotoExportButton				= GUIButton::create(CapitalistButtons["ImportGotoExport"], mImportWindow);
	mImportGotoExportButton->setSize(CapitalistButtons["ImportGotoExport"].first.width, CapitalistButtons["ImportGotoExport"].first.height);
	mImportWindow->setVisible(false);


	sf::FloatRect firstPresRect			= CapitalistButtons["FirstPresident"].first;
	sf::FloatRect secondPresRect		= CapitalistButtons["SecondPresident"].first;
	sf::FloatRect pickedPresRect		= CapitalistButtons["PickedPresident"].first;

	mChoosePresidentWindow				= GUIWindow::create(CapitalistWindows["ChoosePresident"], mCapitalistMainWindow);
	mPickedPresidentWindow				= GUIWindow::create(CapitalistWindows["PickedPresident"], mCapitalistMainWindow);
	mFirstPresidentButton				= GUIButton::create(CapitalistButtons["FirstPresident"], mChoosePresidentWindow);
	mFirstPresidentPlaque				= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>
		(sf::FloatRect(firstPresRect.left, firstPresRect.top + firstPresRect.height - 5, firstPresRect.width, firstPresRect.height),
		&GameManager::getInstance()->getPresidentPlaque(mFirstPresident)), mChoosePresidentWindow);

	mSecondPresidentButton				= GUIButton::create(CapitalistButtons["SecondPresident"], mChoosePresidentWindow);
	mSecondPresidentPlaque				= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>
		(sf::FloatRect(secondPresRect.left, secondPresRect.top + secondPresRect.height - 5, secondPresRect.width, secondPresRect.height),
		&GameManager::getInstance()->getPresidentPlaque(mSecondPresident)), mChoosePresidentWindow);

	mPickedPresidentButton				= GUIButton::create(CapitalistButtons["PickedPresident"], mPickedPresidentWindow);
	mPickedPresidentPlaque				= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>
		(sf::FloatRect(pickedPresRect.left, pickedPresRect.top + pickedPresRect.height - 5, pickedPresRect.width, pickedPresRect.height),
		&GameManager::getInstance()->getPresidentPlaque(mPresident)), mPickedPresidentWindow);

	mClosePresidentWindow				= GUIButton::create(CapitalistButtons["ClosePresident"], mChoosePresidentWindow);
	mClosePickedPresidentWindow			= GUIButton::create(CapitalistButtons["ClosePresident"], mPickedPresidentWindow);
	mPickedPresidentWindow->setVisible(false);
	
	//används för att i början av varje kapitalistrunda visa om någon av resurserna har ökat i pris
	mIncreasedResourcesWindow			= GUIWindow::create(CapitalistWindows["IncreasedResources"], mCapitalistMainWindow);
	mCloseIncreasedResourcesWindow		= GUIButton::create(CapitalistButtons["CloseIncreasedResources"], mIncreasedResourcesWindow);
	mIncreasedResourcesWindow->setVisible(false);
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
	mFirstPresidentPlaque->setTexture(std::pair<sf::FloatRect, sf::Texture*>
		(mFirstPresidentPlaque->getRectangle(), &GameManager::getInstance()->getPresidentPlaque(mFirstPresident)));

	mSecondPresidentButton->setTexture(std::pair<sf::FloatRect, sf::Texture*>(mSecondPresidentButton->getRectangle(), mSecondPresident->getTexture()));
	mSecondPresidentPlaque->setTexture(std::pair<sf::FloatRect, sf::Texture*>
		(mSecondPresidentPlaque->getRectangle(), &GameManager::getInstance()->getPresidentPlaque(mSecondPresident)));
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
	mCancelUpgradeNuclearWeaponButton->setOnClickFunction([=]() 
	{
		mBuyNuclearText->setText(mNuclearText->getText());
	});
			

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

	/*Export GUI-Window med knapapr*/
	mCapitalistTradeButton->setOnClickFunction([=]()	
	{ 
		mCapitalistMainWindow->setEnabled(false, true);
		mImportWindow->setEnabled(true, true);

		mImportWindow->setVisible(true); 
		mCapitalistTradeButton->setTexture(CapitalistButtons["ExportIsPressed"]);

	});

	sf::Texture *minusTexture = &ResourceHandler::getInstance()->getTexture(std::string("Capitalist/kap_texture_button_minus"));
	sf::Texture *plusTexture = &ResourceHandler::getInstance()->getTexture(std::string("Capitalist/kap_texture_button_plus"));
	for(int i = 0; i < sizeof(mImportBuyQuantityBackground)/sizeof(mImportBuyQuantityBackground[0]); i++)
	{
		float x = mImportBuyQuantityBackground[i]->getLocalX(), y = mImportBuyQuantityBackground[i]->getLocalY();
		for(int h = 0; h < 3; h++)
		{
			mImportBuyButtonMinus[i][h] = GUIButton::create(
				std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(x - (h+1)*minusTexture->getSize().x, y, minusTexture->getSize().x, minusTexture->getSize().y),
				minusTexture),
				mImportWindow);
			mImportBuyButtonMinus[i][h]->setOnClickFunction([=]()
			{
				int foodAvailable = stringToInt(mImportResourcesAvailableText[i]->getText());
				int foodPrice = stringToInt(mImportPriceText[i]->getText());
				int quantity = stringToInt(mImportBuyQuantityText[i]->getText());
				quantity -= (5 * (h+1));
				if(quantity < 0)
					quantity = 0;
				mImportBuyQuantityText[i]->setText(intToString(quantity));
				int cost = quantity * (foodAvailable * foodPrice);
				mImportCostText[i]->setText(intToString(cost));
			});

			mImportBuyButtonPlus[i][h] = GUIButton::create(
				std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(x + mImportBuyQuantityBackground[i]->getWidth() + h*plusTexture->getSize().x, y, plusTexture->getSize().x, plusTexture->getSize().y),
				plusTexture),
				mImportWindow);
			mImportBuyButtonPlus[i][h]->setOnClickFunction([=]()
			{
				int foodAvailable = stringToInt(mImportResourcesAvailableText[i]->getText());
				int foodPrice = stringToInt(mImportPriceText[i]->getText());
				int quantity = stringToInt(mImportBuyQuantityText[i]->getText());
				quantity += (5 * (h+1));
				if(quantity > foodAvailable)
					quantity = foodAvailable;
				mImportBuyQuantityText[i]->setText(intToString(quantity));
				int cost = quantity * (foodAvailable * foodPrice);
				mImportCostText[i]->setText(intToString(cost));
			});
		}
	}

	/*nästa runda*/
	mCapitalistEndTurnButton->setOnClickFunction([=]()	
	{ 
		GameManager::getInstance()->nextRound();  
	});

	/*Stänger ner Taxes fönstret*/
	mTaxesCloseButton->setOnClickFunction([=]()					
	{ 
		mCapitalistMainWindow->setEnabled(true, true);

		mTaxesWindow->setVisible(false); 
		mCapitalistTaxesButton->setTexture(CapitalistButtons["Taxes"]);//ändrar textur till orginal
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
			mCapitalistResourceButton->setTexture(CapitalistButtons["Resource"]);//ändrar textur till orginal
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
			mCapitalistUpgradeButton->setTexture(CapitalistButtons["Upgrade"]);//ändrar textur till orginal
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
		mCapitalistTradeButton->setTexture(CapitalistButtons["Export"]);
	});

	
	
	/*Val av president bild 1*/
	mFirstPresidentPlaque->setOnClickFunction([=]()				
	{ 

		//mChoosePresidentWindow->setVisible(false); 
		//mPickedPresidentWindow->setVisible(true); 
		setPresident(mFirstPresident);

		mPickedPresidentButton->setTexture(std::pair<sf::FloatRect, sf::Texture*>
			(mPickedPresidentButton->getRectangle(), mPresident->getTexture()));

		mPickedPresidentPlaque->setTexture(std::pair<sf::FloatRect, sf::Texture*>
			(mPickedPresidentPlaque->getRectangle(), &GameManager::getInstance()->getPresidentPlaque(mPresident)));
	});

	/*Val av president bild 2*/
	mSecondPresidentPlaque->setOnClickFunction([=]()			
	{ 
		//mChoosePresidentWindow->setVisible(false); 
		//mPickedPresidentWindow->setVisible(true); 
		setPresident(mSecondPresident);
		
		mPickedPresidentButton->setTexture(std::pair<sf::FloatRect, sf::Texture*>
			(mPickedPresidentButton->getRectangle(), mPresident->getTexture()));

		mPickedPresidentPlaque->setTexture(std::pair<sf::FloatRect, sf::Texture*>
			(mPickedPresidentPlaque->getRectangle(), &GameManager::getInstance()->getPresidentPlaque(mPresident)));
	});		

	/*När en president har blivit vald*/
	mClosePresidentWindow->setOnClickFunction([=]()				
	{ 
		/*om ingen president blivit vald går det inte att stänga fönstret*/
		if(mPresident != 0) 
		{
			mChoosePresidentWindow->setVisible(false);
			mPickedPresidentWindow->setVisible(true);

			mPickedPresidentWindow->setEnabled(true, true);
			int yearsElected = mPresident->getYearsElected();

			mPresident->setYearsElected(yearsElected + 1);
		}
	});

	/*Stänger ner fönster som visar vilken president som blivit vald*/
	mClosePickedPresidentWindow->setOnClickFunction([=]()				
	{ 
		mPickedPresidentWindow->setVisible(false);
				
		mCapitalistPresident->setTexture(std::pair<sf::FloatRect, sf::Texture*>(mCapitalistPresident->getRectangle(), mPresident->getTexture()));
		mCapitalistPresident->setScale(0.63, 0.68);
		mCapitalistMainWindow->setEnabled(true, true);

	});

	/*nästa runda*/
	mCapitalistEndTurnButton->setOnClickFunction([=]()	
	{ 
		/*if(mTaxes < mCurrentTax)
			setPatriotism(getPatriotism() + 2);
		else if(mTaxes > mCurrentTax)
			setPatriotism(getPatriotism() - 3);*/
		
		//mCapitalistEndTurnButton->setTexture(CapitalistButtons["EndTurnIsPressed"]);
		//mTaxes = mCurrentTax;
		GameManager::getInstance()->nextRound();  
	});

	mCloseIncreasedResourcesWindow->setOnClickFunction([=]()
	{
		mIncreasedResourcesWindow->setVisible(false);
	});
}

void Capitalist::showGUI()
{
	mCapitalistMainWindow->setVisible(true);
}

void Capitalist::hideGUI()
{
	mCapitalistMainWindow->setVisible(false);
}

