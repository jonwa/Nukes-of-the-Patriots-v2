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
static int currentGoods = 0;
static int currentTech  = 0;
static bool activateWindow = false;

Capitalist::Capitalist() :
	mPresident(nullptr),
	mCount(0),
	mUpdateGUIThread(nullptr)
{
	mRound				= 0;
	mIncreasePopulation = false;
	mType				= CAPITALIST;

	initializeCapitalistWindow();
	initializeGuiFunctions();
	initializeCityImages();
	mUpdateGUIThread = new sf::Thread(&Capitalist::updateGUI, this);
	mUpdateGUIThread->launch();
}

void Capitalist::updateGUI()
{
	Timer::setTimer([=]()
	{
		int oldPopulation = stringToInt(mPopulationText->getText().substr(0, mPopulationText->getText().length() - 9));
		if(mPopulation != oldPopulation)
			mPopulationText->setText(intToString(mPopulation) + " millions");
		int oldCurrency = stringToInt(mCurrencyText->getText());
		if(mCurrency != oldCurrency)
			mCurrencyText->setText(intToString(mCurrency));
		int oldPatriotism = stringToInt(mPatriotismText->getText());
		if(mPatriotism != oldPatriotism)
			mPatriotismText->setText(intToString(mPatriotism));

		/*GUI text för utskrift av värden på komunisternas interface*/
		int oldNuclear = stringToInt(mNuclearText->getText());
		if(getNuclearWeapon() != oldNuclear)
			mNuclearText->setText(intToString(getNuclearWeapon()));

		int oldSpaceProgram = stringToInt(mSpaceText->getText());
		if(getSpaceProgram() != oldSpaceProgram)
			mSpaceText->setText(intToString(getSpaceProgram()));

		int oldSpyNetwork = stringToInt(mSpyText->getText());
		if(getSpyNetwork() != oldSpyNetwork)
			mSpyText->setText(intToString(getSpyNetwork()));

		int oldFood = stringToInt(mFoodText->getText());
		if(getFood() != oldFood)
			mFoodText->setText(intToString(getFood()));

		int oldGoods = stringToInt(mGoodsText->getText());
		if(getGoods() != oldGoods)
			mGoodsText->setText(intToString(getGoods()));

		int oldTech = stringToInt(mTechText->getText());
		if(getTech() != oldTech)
			mTechText->setText(intToString(getTech()));


		mResourceFoodPriceText->setText(foodCost);
		mResourceGoodsPriceText->setText(goodsCost);
		mResourceTechPriceText->setText(techCost);
	}, 50, 0);
}

Capitalist::~Capitalist()
{
}

void Capitalist::playMusic()
{
	std::shared_ptr<sf::Music> music = CapitalistMusic["CapitalistMainTheme"];
	music->setVolume(60);
	music->play();
}

void Capitalist::stopMusic()
{
	CapitalistMusic["CapitalistMainTheme"]->stop();
}

std::shared_ptr<President> Capitalist::getPresident()
{
	return mPresident;
}

void Capitalist::changeCityImage()
{
	if((mRound-1) % 9 == 0 && mRound != 1)
	{
		mCount++;
		mChangeCityImage->setTexture(std::pair<sf::FloatRect, sf::Texture*>(mChangeCityImage->getRectangle(), CityImages[mCount])); 
		if(mCount > 5)
			mCount = 0;
	}
}

void Capitalist::newYearStart()
{
	int statsPosX = 125, statsPosY = 177;   //communist 628, 177
	mTaxesIncomeWindow->setVisible(true);
	getTaxIncome();
	//communist title: 745, 90
	//capitalist title: 241, 90
	int statsPosY = 177;   //communist 628, 177
	int foodPatriotismChange = 0;
	if(mFood == 0)
	{
		foodPatriotismChange = -4;
		mFoodChange->setText("No food for the population");
		mFoodChange->setY(statsPosY);
		mFoodChangeValue->setText(foodPatriotismChange);
		mFoodChangeValue->setY(statsPosY);
		statsPosY += mFoodChange->getHeight();
	}
	else if(mFood > 0 && mFood <= mPopulation/2)
	{
		foodPatriotismChange = -2;
		mFoodChange->setText("Not as much food as we expected");
		mFoodChange->setY(statsPosY);
		mFoodChangeValue->setText(foodPatriotismChange);
		mFoodChangeValue->setY(statsPosY);
		statsPosY += mFoodChange->getHeight();
	}
	else if(mFood >= mPopulation)
		mPopulation += 1;
	else
	{
		mFoodChange->setText("");
		mFoodChangeValue->setText("");
	}

	mFood -= mPopulation;
	if(mFood < 0) mFood = 0;
	int taxPatriotismChange = 0;
	int taxChange = mTaxes - mTaxesPreviousRound;
	if(taxChange > 0)
	{
		taxPatriotismChange = -2;
		mTaxChange->setText("Tax increased");
		mTaxChange->setY(statsPosY);
		mTaxChangeValue->setText(taxPatriotismChange);
		mTaxChangeValue->setY(statsPosY);
		statsPosY += mTaxChange->getHeight();
	}
	else if(taxChange < 0)
	{
		taxPatriotismChange = 1;
		mTaxChange->setText("Tax decreased");
		mTaxChange->setY(statsPosY);
		mTaxChangeValue->setText("+"+intToString(taxPatriotismChange));
		mTaxChangeValue->setY(statsPosY);
		statsPosY += mTaxChange->getHeight();
	}
	else
	{
		mTaxChange->setText("");
		mTaxChangeValue->setText("");
	}

	std::shared_ptr<SuperPower> enemy = GameManager::getInstance()->getCommunist();

	int enemyNuclearWeapon = enemy->getNuclearWeapon();
	int enemySpaceProgram = enemy->getSpaceProgram();

	int nuclearDifference = mNuclearWeapon - enemyNuclearWeapon;
	int spaceProgramDifference = mSpaceProgram - enemySpaceProgram;
	bool spaceProgramIncreased = (mSpaceProgram > mSpaceProgramPreviousRound);
	if(spaceProgramIncreased)
	{
		mSpaceProgramIncreasedText->setText("Space program increased");
		mSpaceProgramIncreasedText->setY(statsPosY);
		mSpaceProgramIncreasedTextValue->setText("+1");
		mSpaceProgramIncreasedTextValue->setY(statsPosY);
		statsPosY += mSpaceProgramIncreasedText->getHeight();
	}
	else
	{
		mSpaceProgramIncreasedText->setText("");
		mSpaceProgramIncreasedTextValue->setText("");
	}

	int nuclearWeaponChange = 0;
	int spaceProgramChange = 0;
	int exportedChange = 0;
	if(nuclearDifference > enemyNuclearWeapon*2)
	{
		mNuclearWeaponChange->setText("Nuclear weapon double as much as the enemy");
		nuclearWeaponChange = 2;
		mNuclearWeaponChange->setY(statsPosY);
		mNuclearWeaponChangeValue->setText("+"+intToString(nuclearWeaponChange));
		mNuclearWeaponChangeValue->setY(nuclearWeaponChange);
		statsPosY += mNuclearWeaponChange->getHeight();
	}
	else if(nuclearDifference > enemyNuclearWeapon)
	{
		mNuclearWeaponChange->setText("Nuclear weapon more than enemy");
		nuclearWeaponChange = 1;
		mNuclearWeaponChange->setY(statsPosY);
		mNuclearWeaponChangeValue->setText("+"+intToString(nuclearWeaponChange));
		mNuclearWeaponChangeValue->setY(nuclearWeaponChange);
		statsPosY += mNuclearWeaponChange->getHeight();
	}
	else
	{
		mNuclearWeaponChange->setText("");
		mNuclearWeaponChangeValue->setText("");
	}

	if(spaceProgramDifference > enemySpaceProgram)
	{
		mSpaceProgramMoreThanEnemyText->setText("Best upgraded Space program");
		spaceProgramChange = 1;
		mSpaceProgramMoreThanEnemyText->setY(statsPosY);
		mSpaceProgramMoreThanEnemyTextValue->setText("+"+intToString(spaceProgramChange));
		mSpaceProgramMoreThanEnemyTextValue->setY(statsPosY);
		statsPosY += mSpaceProgramMoreThanEnemyText->getHeight();
	}
	else
	{
		mSpaceProgramMoreThanEnemyText->setText("");
		mSpaceProgramMoreThanEnemyTextValue->setText("");
	}
	
	// My exported resources
	int exportedFoodChange = mExportedFood;
	int exportedGoodsChange = mExportedGoods;
	int exportedTechChange = mExportedTech;
	int exportedTotal = exportedFoodChange + exportedGoodsChange + exportedTechChange;

	// Enemy exported resources
	int enemyFoodExported = enemy->getExportedFood();
	int enemyGoodsExported = enemy->getExportedFood();
	int enemyTechExported = enemy->getExportedFood();
	int enemyExportedTotal = enemyFoodExported + enemyGoodsExported + enemyTechExported;

	if(exportedTotal > enemyExportedTotal)
	{
		mExportedChange->setText("Exported more resources than enemy");
		exportedChange += 1;
		mExportedChange->setY(statsPosY);
		mExportedChangeValue->setText("+"+intToString(exportedChange));
		mExportedChangeValue->setY(statsPosY);
		statsPosY += mExportedChange->getHeight();
	}
	else
	{
		mExportedChange->setText("");
		mExportedChangeValue->setText("");
	}


	mCurrentPopulationText[1]->setText(mPopulation);
	mCurrentTaxesText[1]->setText(mTaxes);
	mTaxesIncomeText[1]->setText(intToString(mTaxesPreviousRound*mPopulationPreviousRound));
	std::cout<<"income window tax: "<<mTaxesPreviousRound*mPopulationPreviousRound<<std::endl;

	int totalPatriotismChange = foodPatriotismChange + taxPatriotismChange + nuclearWeaponChange + spaceProgramChange + exportedChange + (spaceProgramIncreased ? 1 : 0);
	mPatriotism += totalPatriotismChange;
}

void Capitalist::update()
{
	if(mRound > 1)
	{
		std::shared_ptr<SuperPower> enemy = GameManager::getInstance()->getCommunist();

		mImportResourcesAvailableText[0]->setText(enemy->getExportedFood());
		mImportResourcesAvailableText[1]->setText(enemy->getExportedGoods());
		mImportResourcesAvailableText[2]->setText(enemy->getExportedTech());

		mImportBuyQuantityText[0]->setText("0");
		mImportBuyQuantityText[1]->setText("0");
		mImportBuyQuantityText[2]->setText("0");

		mImportCostText[0]->setText("0");
		mImportCostText[1]->setText("0");
		mImportCostText[2]->setText("0");

		if(enemy->getExportedFood() == 0)
			mImportPriceText[0]->setText("N/A");
		else
			mImportPriceText[0]->setText(enemy->getExportedFoodPrice());

		if(enemy->getExportedGoods() == 0)
			mImportPriceText[1]->setText("N/A");
		else
			mImportPriceText[1]->setText(enemy->getExportedGoodsPrice());

		if(enemy->getExportedTech() == 0)
			mImportPriceText[2]->setText("N/A");
		else
			mImportPriceText[2]->setText(enemy->getExportedTechPrice());
	}
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
	std::cout<<"tax previous round: "<<mTaxesPreviousRound<<std::endl;
	std::cout<<"population previous round: "<<mPopulationPreviousRound<<std::endl;

	if((mRound-1) % 4 == 0 ) 
	{
		chooseLeader();
	}

	changeCityImage();
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
	if(foodCost < 1)foodCost = 1;
	if(goodsCost < 1)goodsCost = 1;
	if(techCost < 1)techCost = 1;
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
	int techNuclearPrice = 5 * mPresident->getNuclearPriceModifier() * value;
	
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
	int goodsSpaceProgramPrice = 0;
	int techSpaceProgramPrice = 0;
	for(int i = 0; i < value; ++i)
	{
		goodsSpaceProgramPrice += (stringToInt(mSpaceText->getText()) + i + 1) * 5 * mPresident->getSpacePriceModifier();
		techSpaceProgramPrice += (stringToInt(mSpaceText->getText()) + i + 1) * 10 * mPresident->getSpacePriceModifier();
	}
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
	int spyNetworkPrice = 0;
	for(int i = 0; i < value; ++i)
	{
		spyNetworkPrice = (stringToInt(mSpyText->getText()) + i + 1) * 10 * mPresident->getSpyPriceModifier();
	}

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
	mCapitalistBorder					= GUIWindow::create(CapitalistWindows["CapitalistBorder"], mCapitalistMainWindow);
	mCapitalistBorder					= GUIWindow::create(CapitalistWindows["CapitalistBorderTop"], mCapitalistMainWindow);
	mChangeCityImage					= GUIButton::create(CapitalistButtons["CityImages"], mCapitalistMainWindow);
	mCapitalistPresident				= GUIButton::create(CapitalistButtons["President"], mCapitalistMainWindow);
	mCapitalistTaxesButton				= GUIButton::create(CapitalistButtons["Taxes"], mCapitalistMainWindow);
	mCapitalistResourceButton			= GUIButton::create(CapitalistButtons["Resource"], mCapitalistMainWindow);
	mCapitalistUpgradeButton			= GUIButton::create(CapitalistButtons["Upgrade"], mCapitalistMainWindow);
	mCapitalistTradeButton				= GUIButton::create(CapitalistButtons["Export"], mCapitalistMainWindow);
	mCapitalistEndTurnButton			= GUIButton::create(CapitalistButtons["EndTurn"], mCapitalistMainWindow);
	mLeftPanel							= GUIButton::create(CapitalistButtons["LeftPanel"], mCapitalistMainWindow);
	mRightPanel							= GUIButton::create(CapitalistButtons["RightPanel"], mCapitalistMainWindow);

	mPopulationText						= GUIText::create(sf::FloatRect(697, 14, 228, 36), intToString(mPopulation) + " million", mCapitalistMainWindow);
	mPopulationText->setScale(0.5, 0.5);
	mPopulationText->setAlignment("middle");
	mCurrencyText						= GUIText::create(sf::FloatRect(361, 14, 228, 36), intToString(mCurrency), mCapitalistMainWindow);
	mCurrencyText->setScale(0.5, 0.5);
	mCurrencyText->setAlignment("middle");
	mPatriotismText						= GUIText::create(sf::FloatRect(520, 50, 156, 36), intToString(mPatriotism), mCapitalistMainWindow);
	mPatriotismText->setScale(0.5, 0.5);
	mPatriotismText->setAlignment("middle");
	
	mPopulationText->setColor(sf::Color::White);

	mCurrencyText->setColor(sf::Color::White);

	mPatriotismText->setColor(sf::Color::White);

	mCapitalistMainWindow->setVisible(false);

	/*GUI text för utskrift av värden på komunisternas interface*/

	mNuclearText						= GUIText::create(sf::FloatRect(843, 7, 0, 0), intToString(getNuclearWeapon()), mCapitalistMainWindow);
	mNuclearText->setAlignment("middle");
	mSpaceText							= GUIText::create(sf::FloatRect(843, 219, 0, 0), intToString(getSpaceProgram()), mCapitalistMainWindow);
	mSpaceText->setAlignment("middle");
	mSpyText							= GUIText::create(sf::FloatRect(843, 431, 0, 0), intToString(getSpyNetwork()), mCapitalistMainWindow);
	mSpyText->setAlignment("middle");
	mFoodText							= GUIText::create(sf::FloatRect(31, 7, 0, 0), intToString(getFood()), mCapitalistMainWindow);
	mFoodText->setAlignment("middle");
	mGoodsText							= GUIText::create(sf::FloatRect(31, 219, 0, 0), intToString(getGoods()), mCapitalistMainWindow);
	mGoodsText->setAlignment("middle");
	mTechText							= GUIText::create(sf::FloatRect(31, 431, 0, 0), intToString(getTech()), mCapitalistMainWindow);
	mTechText->setAlignment("middle");

	mTaxesWindow						= GUIWindow::create(CapitalistWindows["CapitalistTaxesWindow"], mCapitalistMainWindow);
	mLowerTaxesButton					= GUIButton::create(CapitalistButtons["LowerTaxes"], mTaxesWindow);
	mTaxValueText						= GUIText::create(sf::FloatRect(105, 78, 0, 0), intToString(mTaxes), mTaxesWindow);
	mTaxValueText->setAlignment("middle");

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
	mResourceFoodPriceText				= GUIText::create(sf::FloatRect(30, 100, 40, 40), intToString(foodCost), mResourceWindow);

	mLowerGoodsByTenButton				= GUIButton::create(CapitalistButtons["LowerGoodsByTen"], mResourceWindow);
	mLowerGoodsByFiveButton				= GUIButton::create(CapitalistButtons["LowerGoodsByFive"], mResourceWindow);
	mLowerGoodsByOneButton				= GUIButton::create(CapitalistButtons["LowerGoodsByOne"], mResourceWindow);
	mRaiseGoodsByOneButton				= GUIButton::create(CapitalistButtons["RaiseGoodsByOne"], mResourceWindow);
	mRaiseGoodsByFiveButton				= GUIButton::create(CapitalistButtons["RaiseGoodsByFive"], mResourceWindow);
	mRaiseGoodsByTenButton				= GUIButton::create(CapitalistButtons["RaiseGoodsByTen"], mResourceWindow);
	mResourceGoodsPriceText				= GUIText::create(sf::FloatRect(210, 100, 40, 40), intToString(goodsCost), mResourceWindow);

	mLowerTechByTenButton				= GUIButton::create(CapitalistButtons["LowerTechByTen"], mResourceWindow);
	mLowerTechByFiveButton				= GUIButton::create(CapitalistButtons["LowerTechByFive"], mResourceWindow);
	mLowerTechByOneButton				= GUIButton::create(CapitalistButtons["LowerTechByOne"], mResourceWindow);
	mRaiseTechByOneButton				= GUIButton::create(CapitalistButtons["RaiseTechByOne"], mResourceWindow);
	mRaiseTechByFiveButton				= GUIButton::create(CapitalistButtons["RaiseTechByFive"], mResourceWindow);
	mRaiseTechByTenButton				= GUIButton::create(CapitalistButtons["RaiseTechByTen"], mResourceWindow);
	mResourceCloseButton				= GUIButton::create(CapitalistButtons["CloseResource"], mResourceWindow);
	mResourceTechPriceText				= GUIText::create(sf::FloatRect(390, 100, 40, 40), intToString(techCost), mResourceWindow);

	
	mBuyFoodText						= GUIText::create(sf::FloatRect(104, 70, 0, 0), "0",mResourceWindow);
	mBuyFoodText->setAlignment("middle");
	mBuyGoodsText						= GUIText::create(sf::FloatRect(285, 70, 40, 40), "0", mResourceWindow);
	mBuyGoodsText->setAlignment("middle");
	mBuyTechText						= GUIText::create(sf::FloatRect(465, 70, 40, 40), "0", mResourceWindow);
	mBuyTechText->setAlignment("middle");
	mFoodCost							= GUIText::create(sf::FloatRect(55, 30, 40, 40), "0 §", mResourceWindow);
	mFoodCost->setAlignment("middle");
	mGoodsCost							= GUIText::create(sf::FloatRect(236, 30, 40, 40), "0 §", mResourceWindow);
	mGoodsCost->setAlignment("middle");
	mTechCost							= GUIText::create(sf::FloatRect(416, 30, 40, 40), "0 §", mResourceWindow);
	mTechCost->setAlignment("middle");
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
	mNuclearTechCost					= GUIText::create(sf::FloatRect(35, 70, 20, 20), "0", mUpgradeWindow);
	mNuclearGoodsCost					= GUIText::create(sf::FloatRect(35, 105, 20, 20), "0", mUpgradeWindow);
	mBuySpaceProgramText				= GUIText::create(sf::FloatRect(337, 107, 22, 22), "0", mUpgradeWindow);
	mSpaceProgramTechCost				= GUIText::create(sf::FloatRect(210, 35, 20, 20), "0", mUpgradeWindow);
	mSpaceProgramGoodsCost				= GUIText::create(sf::FloatRect(210, 70, 20, 20), "0", mUpgradeWindow);
	mBuySpyNetworkText					= GUIText::create(sf::FloatRect(517, 78, 22, 22), "0", mUpgradeWindow);
	mSpyNetworkTechCost					= GUIText::create(sf::FloatRect(400, 20, 20, 20), "0", mUpgradeWindow);
	mUpgradeWindow->setVisible(false);

	mExportWindow						= GUIWindow::create(CapitalistWindows["CapitalistExportWindow"], mCapitalistMainWindow);

	mExportTotalPriceText[0]			= GUIText::create(sf::FloatRect(221, 51, 56, 31), "1", mImportWindow);
	mExportTotalPriceText[1]			= GUIText::create(sf::FloatRect(221, 110, 56, 31), "1", mImportWindow);
	mExportTotalPriceText[2]			= GUIText::create(sf::FloatRect(221, 169, 56, 31), "1", mImportWindow);
	
	sf::Texture *buyField = &ResourceHandler::getInstance()->getTexture(std::string(""));

	mExportQuantityBackground[0]		= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(130, 56, 73, 27), buyField), mExportWindow);
	mExportQuantityBackground[1]		= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(130, 111, 73, 27), buyField), mExportWindow);
	mExportQuantityBackground[2]		= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(130, 170, 73, 27), buyField), mExportWindow);

	mExportFoodCost						= GUIEditField::create(sf::FloatRect(263, 60, 157, 28), GUIEditField::CAP, "0", true, mExportWindow);
	mExportGoodsCost					= GUIEditField::create(sf::FloatRect(263, 115, 157, 28), GUIEditField::CAP, "0", true, mExportWindow);
	mExportTechCost						= GUIEditField::create(sf::FloatRect(263, 170, 157, 28), GUIEditField::CAP, "0", true, mExportWindow);

	for(int i = 0; i < sizeof(mExportQuantityBackground)/sizeof(mExportQuantityBackground[0]); i++)
	{
		mExportQuantityText[i] = GUIText::create(
			sf::FloatRect(mExportQuantityBackground[i]->getLocalX() + mExportQuantityBackground[i]->getWidth()/2 - 5, 
			mExportQuantityBackground[i]->getLocalY() + mExportQuantityBackground[i]->getHeight()/2,
			100, 50), "0", mExportWindow);
		mExportQuantityText[i]->setAlignment("middle");

	}


	mExportConfirmButton				= GUIButton::create(CapitalistButtons["ExportConfirm"], mExportWindow);
	mExportConfirmButton->setSize(CapitalistButtons["ExportConfirm"].first.width, CapitalistButtons["ExportConfirm"].first.height);
	mExportWindow->setVisible(false);
	
	mImportWindow						= GUIWindow::create(CapitalistWindows["CapitalistImportWindow"], mCapitalistMainWindow);
	
	mImportResourcesAvailableText[0]	= GUIText::create(sf::FloatRect(150, 51, 56, 31), "0", mImportWindow);
	mImportResourcesAvailableText[1]	= GUIText::create(sf::FloatRect(150, 110, 56, 31), "0", mImportWindow);
	mImportResourcesAvailableText[2]	= GUIText::create(sf::FloatRect(150, 169, 56, 31), "0", mImportWindow);

	mImportPriceText[0]					= GUIText::create(sf::FloatRect(221, 51, 56, 31), "N/A", mImportWindow);
	mImportPriceText[1]					= GUIText::create(sf::FloatRect(221, 110, 56, 31), "N/A", mImportWindow);
	mImportPriceText[2]					= GUIText::create(sf::FloatRect(221, 169, 56, 31), "N/A", mImportWindow);

	mImportBuyQuantityBackground[0]		= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(329, 51, 56, 31), buyField), mImportWindow);
	mImportBuyQuantityBackground[1]		= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(329, 110, 56, 31), buyField), mImportWindow);
	mImportBuyQuantityBackground[2]		= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(329, 169, 56, 31), buyField), mImportWindow);
	
	for(int i = 0; i < sizeof(mImportBuyQuantityBackground)/sizeof(mImportBuyQuantityBackground[0]); i++)
	{
		mImportBuyQuantityText[i] = GUIText::create(
			sf::FloatRect(mImportBuyQuantityBackground[i]->getLocalX() + mImportBuyQuantityBackground[i]->getWidth()/2 - 5, 
			mImportBuyQuantityBackground[i]->getLocalY() + mImportBuyQuantityBackground[i]->getHeight()/2,
			100, 50), "0", mImportWindow);
		mImportBuyQuantityText[i]->setAlignment("middle");

	}

	mImportCostText[0]					= GUIText::create(sf::FloatRect(470, 56, 100, 50), "0", mImportWindow);
	mImportCostText[1]					= GUIText::create(sf::FloatRect(470, 116, 100, 50), "0", mImportWindow);
	mImportCostText[2]					= GUIText::create(sf::FloatRect(470, 174, 100, 50), "0", mImportWindow);

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
		(sf::FloatRect(firstPresRect.left, firstPresRect.top + firstPresRect.height - 5, 0, 0),
		&GameManager::getInstance()->getPresidentPlaque(mFirstPresident)), mChoosePresidentWindow);

	mSecondPresidentButton				= GUIButton::create(CapitalistButtons["SecondPresident"], mChoosePresidentWindow);
	mSecondPresidentPlaque				= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>
		(sf::FloatRect(secondPresRect.left, secondPresRect.top + secondPresRect.height - 5, 0, 0),
		&GameManager::getInstance()->getPresidentPlaque(mSecondPresident)), mChoosePresidentWindow);

	mPickedPresidentButton				= GUIButton::create(CapitalistButtons["PickedPresident"], mPickedPresidentWindow);
	mPickedPresidentPlaque				= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>
		(sf::FloatRect(pickedPresRect.left, pickedPresRect.top + pickedPresRect.height - 5, 0, 0),
		&GameManager::getInstance()->getPresidentPlaque(mPresident)), mPickedPresidentWindow);

	mClosePresidentWindow				= GUIButton::create(CapitalistButtons["ClosePresident"], mChoosePresidentWindow);
	mClosePickedPresidentWindow			= GUIButton::create(CapitalistButtons["ClosePickedPresident"], mPickedPresidentWindow);
	mPickedPresidentWindow->setVisible(false);
	
	//används för att i början av varje kapitalistrunda visa om någon av resurserna har ökat i pris
	mIncreasedResourcesWindow			= GUIWindow::create(CapitalistWindows["IncreasedResources"], mCapitalistMainWindow);
	mCloseIncreasedResourcesWindow		= GUIButton::create(CapitalistButtons["CloseIncreasedResources"], mIncreasedResourcesWindow);
	mIncreasedResourcesWindow->setVisible(false);

	std::shared_ptr<GUIWindow> statsWindow = GameManager::getInstance()->getStatsWindow();

	int statsPosY = 177;

	mCapitalistHeadLine					= GUIText::create(sf::FloatRect(253, 130, 0, 0), "CAPITALIST", statsWindow);
	mCapitalistHeadLine->setAlignment("middle");

	mNuclearWeaponChange				= GUIText::create(sf::FloatRect(80, statsPosY, 0, 0), "0", statsWindow);
	mNuclearWeaponChange->setAlignment("left");
	mNuclearWeaponChange->setScale(0.5, 0.5);
	mNuclearWeaponChangeValue			= GUIText::create(sf::FloatRect(350, statsPosY, 0, 0), "0", statsWindow);
	mNuclearWeaponChangeValue->setAlignment("left");
	mNuclearWeaponChangeValue->setScale(0.5, 0.5);
	statsPosY += mNuclearWeaponChange->getHeight();

	mSpaceProgramIncreasedText			= GUIText::create(sf::FloatRect(80, statsPosY, 0, 0), "0", statsWindow);
	mSpaceProgramIncreasedText->setAlignment("left");
	mSpaceProgramIncreasedText->setScale(0.5, 0.5);
	mSpaceProgramIncreasedTextValue		= GUIText::create(sf::FloatRect(350, statsPosY, 0, 0), "0", statsWindow);
	mSpaceProgramIncreasedTextValue->setAlignment("left");
	mSpaceProgramIncreasedTextValue->setScale(0.5, 0.5);
	statsPosY += mSpaceProgramIncreasedText->getHeight();

	mSpaceProgramMoreThanEnemyText		= GUIText::create(sf::FloatRect(80, statsPosY, 0, 0), "0", statsWindow);
	mSpaceProgramMoreThanEnemyText->setAlignment("left");
	mSpaceProgramMoreThanEnemyText->setScale(0.5, 0.5);
	mSpaceProgramMoreThanEnemyTextValue	= GUIText::create(sf::FloatRect(350, statsPosY, 0, 0), "0", statsWindow);
	mSpaceProgramMoreThanEnemyTextValue->setAlignment("left");
	mSpaceProgramMoreThanEnemyTextValue->setScale(0.5, 0.5);
	statsPosY += mSpaceProgramMoreThanEnemyText->getHeight();

	mExportedChange						= GUIText::create(sf::FloatRect(80, statsPosY, 0, 0), "0", statsWindow);
	mExportedChange->setAlignment("left");
	mExportedChange->setScale(0.5, 0.5);
	mExportedChangeValue	= GUIText::create(sf::FloatRect(350, statsPosY, 0, 0), "0", statsWindow);
	mExportedChangeValue->setAlignment("left");
	mExportedChangeValue->setScale(0.5, 0.5);
	statsPosY += mExportedChange->getHeight();

	mFoodChange							= GUIText::create(sf::FloatRect(80, statsPosY, 0, 0), "0", statsWindow);
	mFoodChange->setAlignment("left");
	mFoodChange->setScale(0.5, 0.5);
	mFoodChangeValue	= GUIText::create(sf::FloatRect(350, statsPosY, 0, 0), "0", statsWindow);
	mFoodChangeValue->setAlignment("left");
	mFoodChangeValue->setScale(0.5, 0.5);
	statsPosY += mFoodChange->getHeight();

	mTaxChange							= GUIText::create(sf::FloatRect(80, statsPosY, 0, 0), "0", statsWindow);
	mTaxChange->setScale(0.5, 0.5);
	mTaxChange->setAlignment("left");
	mTaxChangeValue	= GUIText::create(sf::FloatRect(350, statsPosY, 0, 0), "0", statsWindow);
	mTaxChangeValue->setAlignment("left");
	mTaxChangeValue->setScale(0.5, 0.5);

	mPresidentBiography					= GUIText::create(sf::FloatRect(40, 260, 0, 0), "", mPickedPresidentWindow);
	mPresidentBiography->setScale(0.6, 0.6);
	mPresidentPositiveText[0]			= GUIText::create(sf::FloatRect(0, 0, 0, 0), "", mPickedPresidentWindow);
	mPresidentPositiveText[1]			= GUIText::create(sf::FloatRect(0, 0, 0, 0), "", mPickedPresidentWindow);
	mPresidentNegativeText				= GUIText::create(sf::FloatRect(0, 0, 0, 0), "", mPickedPresidentWindow);
	mFirstPositiveStat[0]				= GUIText::create(sf::FloatRect(65, 290, 0, 0), "", mChoosePresidentWindow);
	mFirstPositiveStat[0]->setScale(0.6, 0.6);
	mSecondPositiveStat[0]				= GUIText::create(sf::FloatRect(65, 340, 0, 0), "", mChoosePresidentWindow);
	mSecondPositiveStat[0]->setScale(0.6, 0.6);
	mFirstNegativeStat					= GUIText::create(sf::FloatRect(65, 390, 0, 0), "", mChoosePresidentWindow);
	mFirstNegativeStat->setScale(0.6, 0.6);
	mFirstPositiveStat[1]				= GUIText::create(sf::FloatRect(329, 290, 0, 0), "", mChoosePresidentWindow);
	mFirstPositiveStat[1]->setScale(0.6, 0.6);
	mSecondPositiveStat[1]				= GUIText::create(sf::FloatRect(329, 340, 0, 0), "", mChoosePresidentWindow);
	mSecondPositiveStat[1]->setScale(0.6, 0.6);
	mSecondNegativeStat					= GUIText::create(sf::FloatRect(329, 390, 0, 0), "", mChoosePresidentWindow);
	mSecondNegativeStat->setScale(0.6, 0.6);


	mTaxesIncomeWindow					= GUIWindow::create(CapitalistWindows["TaxesIncome"], mCapitalistMainWindow);
	mCurrentPopulationText[0]			= GUIText::create(sf::FloatRect(50, 26, 0, 0), "Population ", mTaxesIncomeWindow);
	mCurrentPopulationText[0]->setScale(0.8, 0.8);
	mCurrentPopulationText[0]->setAlignment("left");
	mCurrentPopulationText[1]			= GUIText::create(sf::FloatRect(331, 26, 0, 0), intToString(getPopulation()) + " million", mTaxesIncomeWindow);
	mCurrentPopulationText[1]->setScale(0.8, 0.8);
	mCurrentPopulationText[1]->setAlignment("left");

    mCurrentTaxesText[0]		        = GUIText::create(sf::FloatRect(50, 50, 0, 0), "Current tax ", mTaxesIncomeWindow);
	mCurrentTaxesText[0]->setScale(0.8, 0.8);
	mCurrentTaxesText[0]->setAlignment("left");
	mCurrentTaxesText[1]		        = GUIText::create(sf::FloatRect(331, 50, 0, 0), intToString(getTaxes()), mTaxesIncomeWindow);
	mCurrentTaxesText[1]->setScale(0.8, 0.8);
	mCurrentTaxesText[1]->setAlignment("left");

    mTaxesIncomeText[0]					= GUIText::create(sf::FloatRect(50, 74, 0, 0), "Tax income ", mTaxesIncomeWindow);
	mTaxesIncomeText[0]->setScale(0.8, 0.8);
	mTaxesIncomeText[0]->setAlignment("left");
	mTaxesIncomeText[1]					= GUIText::create(sf::FloatRect(331, 74, 0, 0), "0", mTaxesIncomeWindow);
	mTaxesIncomeText[1]->setScale(0.8, 0.8);
	mTaxesIncomeText[1]->setAlignment("left");

	mCloseTaxesIncomeWindow				= GUIButton::create(CapitalistButtons["CloseTaxesIncome"], mTaxesIncomeWindow);
	mTaxesIncomeWindow->setVisible(false);

	mIncreasedResourcesPriceWindow		= GUIWindow::create(CapitalistWindows["IncreasedResources"], mCapitalistMainWindow);
	mIncreasedResourcesText				= GUIText::create(sf::FloatRect(50, 50, 0, 0), "Example: The price of goods is now 21", mIncreasedResourcesPriceWindow);
	mIncreasedResourcesText->setScale(0.8, 0.8);
	mIncreasedResourcesText->setAlignment("left");
	mCloseIncreasedResourcesPriceWindow	= GUIButton::create(CapitalistButtons["CloseIncreasedResources"], mIncreasedResourcesPriceWindow);
	mIncreasedResourcesPriceWindow->setVisible(false);
	
	mPopulationEatsFoodWindow			= GUIWindow::create(CapitalistWindows["PopulationEatsFood"], mCapitalistMainWindow);
	mPopulationEatsFoodText				= GUIText::create(sf::FloatRect(50, 50, 0, 0), "Example: Population eats 50 food \nPopulation grows to 51 million",  mPopulationEatsFoodWindow);
	mPopulationEatsFoodText->setScale(0.8, 0.8);
	mPopulationEatsFoodText->setAlignment("left");
	mClosePopulationEatsFoodWindow		= GUIButton::create(CapitalistButtons["ClosePopulationEatsFood"], mPopulationEatsFoodWindow);
	mPopulationEatsFoodWindow->setVisible(false);

	mFoodImage[0]	= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(135, 20, 35, 35), &ResourceHandler::getInstance()->getTexture(std::string("Capitalist/food_image"))), mResourceWindow);
	mFoodImage[1]	= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(70, 51, 35, 35), &ResourceHandler::getInstance()->getTexture(std::string("Capitalist/food_image"))), mImportWindow);
	mFoodImage[2]	= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(25, 58, 35, 35), &ResourceHandler::getInstance()->getTexture(std::string("Capitalist/food_image"))), mExportWindow);

	mGoodsImage[0]	= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(315, 20, 35, 35), &ResourceHandler::getInstance()->getTexture(std::string("Capitalist/goods_image"))), mResourceWindow);
	mGoodsImage[1]	= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(70, 110, 35, 35), &ResourceHandler::getInstance()->getTexture(std::string("Capitalist/goods_image"))), mImportWindow);
	mGoodsImage[2]	= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(25, 113, 35, 35), &ResourceHandler::getInstance()->getTexture(std::string("Capitalist/goods_image"))), mExportWindow);

	mTechImage[0]	= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(495, 20, 35, 35), &ResourceHandler::getInstance()->getTexture(std::string("Capitalist/tech_image"))), mResourceWindow);
	mTechImage[1]	= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(70, 169, 35, 35), &ResourceHandler::getInstance()->getTexture(std::string("Capitalist/tech_image"))), mImportWindow);
	mTechImage[2]	= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(25, 173, 35, 35), &ResourceHandler::getInstance()->getTexture(std::string("Capitalist/tech_image"))), mExportWindow);
	
	/*
	 	Lägger in föräldernoden i vektorn som finns i GUIManager
	 	och kommer automatiskt få med sig alla barnnoder till denna
	 	vilket är alla GUIElement som finns i denna klass som har 
	 	mCapitalistMainWindow som parent-argument i dess konstruktor
																		*/
	chooseLeader();
	GUIManager::getInstance()->addGUIElement(mCapitalistMainWindow);
}

void Capitalist::initializeCityImages()
{
	CityImages.push_back(&ResourceHandler::getInstance()->getTexture(std::string("Capitalist/kap_city2")));
	CityImages.push_back(&ResourceHandler::getInstance()->getTexture(std::string("Capitalist/kap_city3")));
	CityImages.push_back(&ResourceHandler::getInstance()->getTexture(std::string("Capitalist/kap_city4")));
	CityImages.push_back(&ResourceHandler::getInstance()->getTexture(std::string("Capitalist/kap_city5")));
	mChangeCityImage->setTexture(std::pair<sf::FloatRect, sf::Texture*>(mChangeCityImage->getRectangle(), CityImages[0])); 
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

	mFirstPositiveStat[0]->setText(mFirstPresident->getFirstPositiveStat());
	mSecondPositiveStat[0]->setText(mFirstPresident->getSecondPositiveStat());
	mFirstNegativeStat->setText(mFirstPresident->getNegativeStat());
	
	mFirstPositiveStat[1]->setText(mSecondPresident->getFirstPositiveStat());
	mSecondPositiveStat[1]->setText(mSecondPresident->getSecondPositiveStat());
	mSecondNegativeStat->setText(mSecondPresident->getNegativeStat());
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

	mLowerTaxesButton->setOnClickFunction([=]()
	{
		int newTax = stringToInt(mTaxValueText->getText()) - 5;
		if(newTax < mTaxesPreviousRound - 5)
			newTax = mTaxesPreviousRound - 5;
		if(newTax < 0)
			newTax = 0;
		mTaxValueText->setText(newTax);
	});

	mRaiseTaxesButton->setOnClickFunction([=]()
	{
		int newTax = stringToInt(mTaxValueText->getText()) + 5;
		if(newTax > mTaxesPreviousRound + 5)
			newTax = mTaxesPreviousRound + 5;
		if(newTax > 95)
			newTax = 95;
		mTaxValueText->setText(newTax);
	});

 	mLowerFoodByTenButton->setOnClickFunction([=]()
 	{ 
		int foodAmount = stringToInt(mBuyFoodText->getText()) - 10;
		int goodsAmount = stringToInt(mBuyGoodsText->getText());
		int techAmount = stringToInt(mBuyTechText->getText());
		if(foodAmount < 0)
			foodAmount = 0;
		int totalCost = foodCost*foodAmount + goodsCost*goodsAmount + techCost*techAmount;
		int moneyDifference = mCurrency - totalCost;
		if(moneyDifference < 0)
		{
			foodAmount -= std::ceilf((float)abs(moneyDifference)/(float)foodCost);
			totalCost = foodCost*foodAmount + goodsCost*goodsAmount + techCost*techAmount;
		}
		mBuyFoodText->setText(foodAmount);
		mFoodCost->setText(intToString(foodCost*foodAmount) + " §");
		//mBuyTotalCostText->setText(totalCost);
	});		
	mLowerFoodByFiveButton->setOnClickFunction([=]()
	{ 
		int foodAmount = stringToInt(mBuyFoodText->getText()) - 5;
		int goodsAmount = stringToInt(mBuyGoodsText->getText());
		int techAmount = stringToInt(mBuyTechText->getText());
		if(foodAmount < 0)
			foodAmount = 0;
		int totalCost = foodCost*foodAmount + goodsCost*goodsAmount + techCost*techAmount;
		int moneyDifference = mCurrency - totalCost;
		if(moneyDifference < 0)
		{
			foodAmount -= std::ceilf((float)abs(moneyDifference)/(float)foodCost);
			totalCost = foodCost*foodAmount + goodsCost*goodsAmount + techCost*techAmount;
		}
		mBuyFoodText->setText(foodAmount);
		mFoodCost->setText(intToString(foodCost*foodAmount) + " §");
		//mBuyTotalCostText->setText(totalCost);
	});	
	mLowerFoodByOneButton->setOnClickFunction([=]()
	{ 
		int foodAmount = stringToInt(mBuyFoodText->getText()) - 1;
		int goodsAmount = stringToInt(mBuyGoodsText->getText());
		int techAmount = stringToInt(mBuyTechText->getText());
		if(foodAmount < 0)
			foodAmount = 0;
		int totalCost = foodCost*foodAmount + goodsCost*goodsAmount + techCost*techAmount;
		int moneyDifference = mCurrency - totalCost;
		if(moneyDifference < 0)
		{
			foodAmount -= std::ceilf((float)abs(moneyDifference)/(float)foodCost);
			totalCost = foodCost*foodAmount + goodsCost*goodsAmount + techCost*techAmount;
		}
		mBuyFoodText->setText(foodAmount);
		mFoodCost->setText(intToString(foodCost*foodAmount) + " §");
		//mBuyTotalCostText->setText(totalCost);
	});				
	mRaiseFoodByOneButton->setOnClickFunction([=]()
	{ 
		int foodAmount = stringToInt(mBuyFoodText->getText()) + 1;
		int goodsAmount = stringToInt(mBuyGoodsText->getText());
		int techAmount = stringToInt(mBuyTechText->getText());
		int totalCost = foodCost*foodAmount + goodsCost*goodsAmount + techCost*techAmount;
		int moneyDifference = mCurrency - totalCost;
		if(moneyDifference < 0)
		{
			foodAmount -= std::ceilf((float)abs(moneyDifference)/(float)foodCost);
			totalCost = foodCost*foodAmount + goodsCost*goodsAmount + techCost*techAmount;
		}
		mBuyFoodText->setText(foodAmount);
		mFoodCost->setText(intToString(foodCost*foodAmount) + " §");
		//mBuyTotalCostText->setText(totalCost);
	});	
	mRaiseFoodByFiveButton->setOnClickFunction([=]()
	{ 
		int foodAmount = stringToInt(mBuyFoodText->getText()) + 5;
		int goodsAmount = stringToInt(mBuyGoodsText->getText());
		int techAmount = stringToInt(mBuyTechText->getText());
		int totalCost = foodCost*foodAmount + goodsCost*goodsAmount + techCost*techAmount;
		int moneyDifference = mCurrency - totalCost;
		if(moneyDifference < 0)
		{
			foodAmount -= std::ceilf((float)abs(moneyDifference)/(float)foodCost);
			totalCost = foodCost*foodAmount + goodsCost*goodsAmount + techCost*techAmount;
		}
		mBuyFoodText->setText(foodAmount);
		mFoodCost->setText(intToString(foodCost*foodAmount) + " §");
		//mBuyTotalCostText->setText(totalCost);
	});	
	mRaiseFoodByTenButton->setOnClickFunction([=]()
	{ 
		int foodAmount = stringToInt(mBuyFoodText->getText()) + 10;
		int goodsAmount = stringToInt(mBuyGoodsText->getText());
		int techAmount = stringToInt(mBuyTechText->getText());
		int totalCost = foodCost*foodAmount + goodsCost*goodsAmount + techCost*techAmount;
		int moneyDifference = mCurrency - totalCost;
		if(moneyDifference < 0)
		{
			foodAmount -= std::ceilf((float)abs(moneyDifference)/(float)foodCost);
			totalCost = foodCost*foodAmount + goodsCost*goodsAmount + techCost*techAmount;
		}
		mBuyFoodText->setText(foodAmount);
		mFoodCost->setText(intToString(foodCost*foodAmount) + " §");
		//mBuyTotalCostText->setText(totalCost);
	});	

	mLowerGoodsByTenButton->setOnClickFunction([=]()
	{ 
		int foodAmount = stringToInt(mBuyFoodText->getText());
		int goodsAmount = stringToInt(mBuyGoodsText->getText()) - 10;
		int techAmount = stringToInt(mBuyTechText->getText());
		if(goodsAmount < 0)
			goodsAmount = 0;
		int totalCost = foodCost*foodAmount + goodsCost*goodsAmount + techCost*techAmount;
		int moneyDifference = mCurrency - totalCost;
		if(moneyDifference < 0)
		{
			goodsAmount -= std::ceilf((float)abs(moneyDifference)/(float)goodsCost);
			totalCost = foodCost*foodAmount + goodsCost*goodsAmount + techCost*techAmount;
		}
		mBuyGoodsText->setText(goodsAmount);
		mGoodsCost->setText(intToString(goodsCost*goodsAmount) + " §");
		//mBuyTotalCostText->setText(totalCost);
	});	
	mLowerGoodsByFiveButton->setOnClickFunction([=]()
	{ 
		int foodAmount = stringToInt(mBuyFoodText->getText());
		int goodsAmount = stringToInt(mBuyGoodsText->getText()) - 5;
		int techAmount = stringToInt(mBuyTechText->getText());
		if(goodsAmount < 0)
			goodsAmount = 0;
		int totalCost = foodCost*foodAmount + goodsCost*goodsAmount + techCost*techAmount;
		int moneyDifference = mCurrency - totalCost;
		if(moneyDifference < 0)
		{
			goodsAmount -= std::ceilf((float)abs(moneyDifference)/(float)goodsCost);
			totalCost = foodCost*foodAmount + goodsCost*goodsAmount + techCost*techAmount;
		}
		mBuyGoodsText->setText(goodsAmount);
		mGoodsCost->setText(intToString(goodsCost*goodsAmount) + " §");
		//mBuyTotalCostText->setText(totalCost);
	});	
	mLowerGoodsByOneButton->setOnClickFunction([=]()
	{ 
		int foodAmount = stringToInt(mBuyFoodText->getText());
		int goodsAmount = stringToInt(mBuyGoodsText->getText()) - 1;
		int techAmount = stringToInt(mBuyTechText->getText());
		if(goodsAmount < 0)
			goodsAmount = 0;
		int totalCost = foodCost*foodAmount + goodsCost*goodsAmount + techCost*techAmount;
		int moneyDifference = mCurrency - totalCost;
		if(moneyDifference < 0)
		{
			goodsAmount -= std::ceilf((float)abs(moneyDifference)/(float)goodsCost);
			totalCost = foodCost*foodAmount + goodsCost*goodsAmount + techCost*techAmount;
		}
		mBuyGoodsText->setText(goodsAmount);
		mGoodsCost->setText(intToString(goodsCost*goodsAmount) + " §");
		//mBuyTotalCostText->setText(totalCost);
	});	
	mRaiseGoodsByOneButton->setOnClickFunction([=]()
	{ 
		int foodAmount = stringToInt(mBuyFoodText->getText());
		int goodsAmount = stringToInt(mBuyGoodsText->getText()) + 1;
		int techAmount = stringToInt(mBuyTechText->getText());
		int totalCost = foodCost*foodAmount + goodsCost*goodsAmount + techCost*techAmount;
		int moneyDifference = mCurrency - totalCost;
		if(moneyDifference < 0)
		{
			goodsAmount -= std::ceilf((float)abs(moneyDifference)/(float)goodsCost);
			totalCost = foodCost*foodAmount + goodsCost*goodsAmount + techCost*techAmount;
		}
		mBuyGoodsText->setText(goodsAmount);
		mGoodsCost->setText(intToString(goodsCost*goodsAmount) + " §");
		//mBuyTotalCostText->setText(totalCost);
	});	
	mRaiseGoodsByFiveButton->setOnClickFunction([=]()
	{ 
		int foodAmount = stringToInt(mBuyFoodText->getText());
		int goodsAmount = stringToInt(mBuyGoodsText->getText()) + 5;
		int techAmount = stringToInt(mBuyTechText->getText());
		int totalCost = foodCost*foodAmount + goodsCost*goodsAmount + techCost*techAmount;
		int moneyDifference = mCurrency - totalCost;
		if(moneyDifference < 0)
		{
			goodsAmount -= std::ceilf((float)abs(moneyDifference)/(float)goodsCost);
			totalCost = foodCost*foodAmount + goodsCost*goodsAmount + techCost*techAmount;
		}
		mBuyGoodsText->setText(goodsAmount);
		mGoodsCost->setText(intToString(goodsCost*goodsAmount) + " §");
		//mBuyTotalCostText->setText(totalCost);
	});	
	mRaiseGoodsByTenButton->setOnClickFunction([=]()
	{ 
		int foodAmount = stringToInt(mBuyFoodText->getText());
		int goodsAmount = stringToInt(mBuyGoodsText->getText()) + 10;
		int techAmount = stringToInt(mBuyTechText->getText());
		int totalCost = foodCost*foodAmount + goodsCost*goodsAmount + techCost*techAmount;
		int moneyDifference = mCurrency - totalCost;
		if(moneyDifference < 0)
		{
			goodsAmount -= std::ceilf((float)abs(moneyDifference)/(float)goodsCost);
			totalCost = foodCost*foodAmount + goodsCost*goodsAmount + techCost*techAmount;
		}
		mBuyGoodsText->setText(goodsAmount);
		mGoodsCost->setText(intToString(goodsCost*goodsAmount) + " §");
		//mBuyTotalCostText->setText(totalCost);
	});	

	mLowerTechByTenButton->setOnClickFunction([=]()
	{
		int foodAmount = stringToInt(mBuyFoodText->getText());
		int goodsAmount = stringToInt(mBuyGoodsText->getText());
		int techAmount = stringToInt(mBuyTechText->getText()) - 10;
		if(techAmount < 0)
			techAmount = 0;
		int totalCost = foodCost*foodAmount + goodsCost*goodsAmount + techCost*techAmount;
		int moneyDifference = mCurrency - totalCost;
		if(moneyDifference < 0)
		{
			techAmount -= std::ceilf((float)abs(moneyDifference)/(float)techCost);
			totalCost = foodCost*foodAmount + goodsCost*goodsAmount + techCost*techAmount;
		}
		mBuyTechText->setText(techAmount);
		mTechCost->setText(intToString(techCost*techAmount) + " §");
		//mBuyTotalCostText->setText(totalCost);
	});
	mLowerTechByFiveButton->setOnClickFunction([=]()
	{
		int foodAmount = stringToInt(mBuyFoodText->getText());
		int goodsAmount = stringToInt(mBuyGoodsText->getText());
		int techAmount = stringToInt(mBuyTechText->getText()) - 5;
		if(techAmount < 0)
			techAmount = 0;
		int totalCost = foodCost*foodAmount + goodsCost*goodsAmount + techCost*techAmount;
		int moneyDifference = mCurrency - totalCost;
		if(moneyDifference < 0)
		{
			techAmount -= std::ceilf((float)abs(moneyDifference)/(float)techCost);
			totalCost = foodCost*foodAmount + goodsCost*goodsAmount + techCost*techAmount;
		}
		mBuyTechText->setText(techAmount);
		mTechCost->setText(intToString(techCost*techAmount) + " §");
		//mBuyTotalCostText->setText(totalCost);
	});
	mLowerTechByOneButton->setOnClickFunction([=]()
	{
		int foodAmount = stringToInt(mBuyFoodText->getText());
		int goodsAmount = stringToInt(mBuyGoodsText->getText());
		int techAmount = stringToInt(mBuyTechText->getText()) - 1;
		if(techAmount < 0)
			techAmount = 0;
		int totalCost = foodCost*foodAmount + goodsCost*goodsAmount + techCost*techAmount;
		int moneyDifference = mCurrency - totalCost;
		if(moneyDifference < 0)
		{
			techAmount -= std::ceilf((float)abs(moneyDifference)/(float)techCost);
			totalCost = foodCost*foodAmount + goodsCost*goodsAmount + techCost*techAmount;
		}
		mBuyTechText->setText(techAmount);
		mTechCost->setText(intToString(techCost*techAmount) + " §");
		//mBuyTotalCostText->setText(totalCost);
	});
	mRaiseTechByOneButton->setOnClickFunction([=]()
	{
		int foodAmount = stringToInt(mBuyFoodText->getText());
		int goodsAmount = stringToInt(mBuyGoodsText->getText());
		int techAmount = stringToInt(mBuyTechText->getText()) + 1;
		int totalCost = foodCost*foodAmount + goodsCost*goodsAmount + techCost*techAmount;
		int moneyDifference = mCurrency - totalCost;
		if(moneyDifference < 0)
		{
			techAmount -= std::ceilf((float)abs(moneyDifference)/(float)techCost);
			totalCost = foodCost*foodAmount + goodsCost*goodsAmount + techCost*techAmount;
		}
		mBuyTechText->setText(techAmount);
		mTechCost->setText(intToString(techCost*techAmount) + " §");
		//mBuyTotalCostText->setText(totalCost);
	});
	mRaiseTechByFiveButton->setOnClickFunction([=]()
	{
		int foodAmount = stringToInt(mBuyFoodText->getText());
		int goodsAmount = stringToInt(mBuyGoodsText->getText());
		int techAmount = stringToInt(mBuyTechText->getText()) + 5;
		int totalCost = foodCost*foodAmount + goodsCost*goodsAmount + techCost*techAmount;
		int moneyDifference = mCurrency - totalCost;
		if(moneyDifference < 0)
		{
			techAmount -= std::ceilf((float)abs(moneyDifference)/(float)techCost);
			totalCost = foodCost*foodAmount + goodsCost*goodsAmount + techCost*techAmount;
		}
		mBuyTechText->setText(techAmount);
		mTechCost->setText(intToString(techCost*techAmount) + " §");
		//mBuyTotalCostText->setText(totalCost);
	});
	mRaiseTechByTenButton->setOnClickFunction([=]()
	{
		int foodAmount = stringToInt(mBuyFoodText->getText());
		int goodsAmount = stringToInt(mBuyGoodsText->getText());
		int techAmount = stringToInt(mBuyTechText->getText()) + 10;
		int totalCost = foodCost*foodAmount + goodsCost*goodsAmount + techCost*techAmount;
		int moneyDifference = mCurrency - totalCost;
		if(moneyDifference < 0)
		{
			techAmount -= std::ceilf((float)abs(moneyDifference)/(float)techCost);
			totalCost = foodCost*foodAmount + goodsCost*goodsAmount + techCost*techAmount;
		}
		mBuyTechText->setText(techAmount);
		mTechCost->setText(intToString(techCost*techAmount) + " §");
		//mBuyTotalCostText->setText(totalCost);
	});


	/*Upgrade GUI-Window med knappar*/
	mCapitalistUpgradeButton->setOnClickFunction([=]()	
	{
		currentGoods				= mGoods;
		currentTech					= mTech;

		mCapitalistMainWindow->setEnabled(false, true);
		mUpgradeWindow->setEnabled(true, true);

		mUpgradeWindow->setVisible(true);
		mCapitalistUpgradeButton->setTexture(CapitalistButtons["UpgradeIsPressed"]);
		mBuyNuclearText->setText(mNuclearText->getText());
		mBuySpaceProgramText->setText(mSpaceText->getText());
		mBuySpyNetworkText->setText(mSpyText->getText());

		upgradeWindowText();

	});

	mUpgradeNuclearWeaponButton->setOnClickFunction([=]() 
	{
		int nuclearGoodsPrice	= 10 * mPresident->getNuclearPriceModifier();
		int nuclearTechPrice	= 5  * mPresident->getNuclearPriceModifier();
		int amount = stringToInt(mBuyNuclearText->getText());
		
		if(currentGoods >= nuclearGoodsPrice && currentTech >= nuclearTechPrice)
		{
			++amount;
			mBuyNuclearText->setText(amount);
			upgradeWindowText();
			currentGoods -= nuclearGoodsPrice;
			currentTech  -= nuclearTechPrice;
		}
	});		
	mCancelUpgradeNuclearWeaponButton->setOnClickFunction([=]() 
	{
		int difference = stringToInt(mBuyNuclearText->getText()) - stringToInt(mNuclearText->getText());
		currentGoods += stringToInt(mNuclearGoodsCost->getText()) * difference;
		currentTech	 += stringToInt(mNuclearTechCost->getText()) * difference;
		mBuyNuclearText->setText(mNuclearText->getText());
		upgradeWindowText();
		
	});
			

	mUpgradeSpaceProgramButton->setOnClickFunction([=]()  
	{
		int spaceProgramGoodsPrice  = (stringToInt(mBuySpaceProgramText->getText()) + 1) * 5 * mPresident->getSpacePriceModifier();
		int spaceProgramTechPrice	= (stringToInt(mBuySpaceProgramText->getText()) + 1) * 10 * mPresident->getSpacePriceModifier();
		int amount = stringToInt(mBuySpaceProgramText->getText());
		if(currentGoods >= spaceProgramGoodsPrice && currentTech >= spaceProgramTechPrice)
		{
			++amount;
			mBuySpaceProgramText->setText(amount);
			upgradeWindowText();
			currentGoods -= spaceProgramGoodsPrice;
			currentTech -= spaceProgramTechPrice;
		}
	});
	mCancelUpgradeSpaceProgramButton->setOnClickFunction([=]() 
	{
		int difference = stringToInt(mBuySpaceProgramText->getText()) - stringToInt(mSpaceText->getText());
		for(int i = 0; i < difference; ++i)
		{
			currentGoods += (stringToInt(mSpaceText->getText()) + i + 1) * 5 * mPresident->getSpacePriceModifier();
			currentTech += (stringToInt(mSpaceText->getText()) + i + 1) * 10 * mPresident->getSpacePriceModifier();
		}
		mBuySpaceProgramText->setText(mSpaceText->getText());
		upgradeWindowText();
	});

	mUpgradeSpyNetworkButton->setOnClickFunction([=]()	 
	{
		int spyNetworkTechPrice = (stringToInt(mBuySpyNetworkText->getText()) + 1) * 10 * mPresident->getSpyPriceModifier();
		int amount = stringToInt(mBuySpyNetworkText->getText());
		if(currentTech >= spyNetworkTechPrice)
		{
			++amount;
			currentTech -= spyNetworkTechPrice;
			mBuySpyNetworkText->setText(amount);
			upgradeWindowText();
		}
		
	});		
	mCancelUpgradeSpyNetworkButton->setOnClickFunction([=]() 
	{
		int difference = stringToInt(mBuySpyNetworkText->getText()) - stringToInt(mSpyText->getText());
		for(int i = 0; i < difference; ++i)
		{
			currentTech += (stringToInt(mSpyText->getText()) + i + 1) * 10 * mPresident->getSpyPriceModifier();
		}
		mBuySpyNetworkText->setText(mSpyText->getText());
		upgradeWindowText();
	});

	/*Export GUI-Window med knappar*/
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
				quantity -= (h == 0) ? 1 : (h*5);
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
				quantity += (h == 0) ? 1 : (h*5);
				if(quantity > foodAvailable)
					quantity = foodAvailable;
				mImportBuyQuantityText[i]->setText(intToString(quantity));
				int cost = quantity * (foodAvailable * foodPrice);
				mImportCostText[i]->setText(intToString(cost));
			});
		}
	}

	mImportGotoExportButton->setOnClickFunction(
		[=]()
	{
		mImportWindow->setVisible(false);
		mExportWindow->setVisible(true);
		mExportWindow->setEnabled(true, true);
	});

	std::vector<int*> resourcesAvailable;
	resourcesAvailable.push_back(&mFood);
	resourcesAvailable.push_back(&mGoods);
	resourcesAvailable.push_back(&mTech);

	for(int i = 0; i < sizeof(mExportQuantityBackground)/sizeof(mExportQuantityBackground[0]); i++)
	{
		float x = mExportQuantityBackground[i]->getLocalX(), y = mExportQuantityBackground[i]->getLocalY();
		for(int h = 0; h < 3; h++)
		{
			mExportButtonMinus[i][h] = GUIButton::create(
				std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(x - (h+1)*minusTexture->getSize().x, y, minusTexture->getSize().x, minusTexture->getSize().y),
				minusTexture),
				mExportWindow);
			mExportButtonMinus[i][h]->setOnClickFunction([=]()
			{
				int quantity = stringToInt(mExportQuantityText[i]->getText());
				quantity -= (h == 0) ? 1 : (h*5);
				if(quantity < 0)
					quantity = 0;
				mExportQuantityText[i]->setText(intToString(quantity));
				//int cost = quantity * (foodAvailable * foodPrice);
				//mImportCostText[i]->setText(intToString(cost));
			});

			mExportButtonPlus[i][h] = GUIButton::create(
				std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(x + mImportBuyQuantityBackground[i]->getWidth() + h*plusTexture->getSize().x, y, plusTexture->getSize().x, plusTexture->getSize().y),
				plusTexture),
				mExportWindow);
			mExportButtonPlus[i][h]->setOnClickFunction([=]()
			{
				int foodAvailable = mFood;
				int quantity = stringToInt(mExportQuantityText[i]->getText());
				quantity += (h == 0) ? 1 : (h*5);
				if(quantity > *resourcesAvailable[i])
					quantity = *resourcesAvailable[i];
				mExportQuantityText[i]->setText(intToString(quantity));
				//int cost = quantity * (foodAvailable * foodPrice);
				//mImportCostText[i]->setText(intToString(cost));
			});
		}
	}

	mExportConfirmButton->setOnClickFunction(
		[=]()
	{
		mExportWindow->setVisible(false);
		mCapitalistMainWindow->setEnabled(true, true);
		mCapitalistTradeButton->setTexture(CapitalistButtons["Export"]);
		//mCurrency -= stringToInt(mImportTotalCostText->getText());

		mExportedFood = stringToInt(mExportQuantityText[0]->getText());
		mExportedGoods = stringToInt(mExportQuantityText[1]->getText());
		mExportedTech = stringToInt(mExportQuantityText[2]->getText());

		mExportedFoodPrice = stringToInt(mExportFoodCost->getText());
		mExportedGoodsPrice = stringToInt(mExportGoodsCost->getText());
		mExportedTechPrice = stringToInt(mExportTechCost->getText());
	});

	/*nästa runda*/
	mCapitalistEndTurnButton->setOnClickFunction([=]()	
	{ 
		int foodBought = mFood - mFoodPreviousRound;
		int goodsBought = mGoods - mGoodsPreviousRound;
		int techBought = mTech - mTechPreviousRound;
		if(foodBought > goodsBought && foodBought > techBought)
			foodCost += 1;
		else if(goodsBought > foodBought && goodsBought > techBought)
			foodCost += 1;
		else if(techBought > foodBought && techBought > goodsBought)
			foodCost += 1;
		GameManager::getInstance()->nextRound();  
	});

	/*Stänger ner Taxes fönstret*/
	mTaxesCloseButton->setOnClickFunction([=]()					
	{ 
		mCapitalistMainWindow->setEnabled(true, true);
		mTaxes = stringToInt(mTaxValueText->getText());
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

		mCapitalistMainWindow->setEnabled(true, true);
		
		mUpgradeWindow->setVisible(false); 	
		upgradeNuclearWeapon(nuclearDiff); 
		upgradeSpaceProgram(spaceDiff); 
		upgradeSpyNetwork(spyDiff);
		mCapitalistUpgradeButton->setTexture(CapitalistButtons["Upgrade"]);//ändrar textur till orginal
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

		mPresidentBiography->setText(mPresident->getBiography());
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

		mPresidentBiography->setText(mPresident->getBiography());
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
		mIncreasedResourcesPriceWindow->setVisible(true);
	});
	
	mCloseIncreasedResourcesPriceWindow->setOnClickFunction([=]()
	{
		mIncreasedResourcesPriceWindow->setVisible(false);
		mPopulationEatsFoodWindow->setVisible(true);
	});

	mClosePopulationEatsFoodWindow->setOnClickFunction([=]()
	{
		mPopulationEatsFoodWindow->setVisible(false);
		/*if(mTaxes < mCurrentTax)
			setPatriotism(getPatriotism() + 2);
		else if(mTaxes > mCurrentTax)
			setPatriotism(getPatriotism() - 3);*/
		
		//mCapitalistEndTurnButton->setTexture(CapitalistButtons["EndTurnIsPressed"]);
		//mTaxes = mCurrentTax;
		GameManager::getInstance()->nextRound();  
		stopMusic();
	});
	
	mCloseTaxesIncomeWindow->setOnClickFunction([=]()
	{
		mTaxesIncomeWindow->setVisible(false);
	});


	mCloseIncreasedResourcesWindow->setOnClickFunction([=]()
	{
		mIncreasedResourcesWindow->setVisible(false);
	});
}

void Capitalist::upgradeWindowText()
{
	int spaceProgram			= stringToInt(mBuySpaceProgramText->getText());
	int spyNetwork				= stringToInt(mBuySpyNetworkText->getText());
	int nuclearGoodsPrice		= 10 * mPresident->getNuclearPriceModifier();
	int nuclearTechPrice		= 5  * mPresident->getNuclearPriceModifier();
	int spaceProgramGoodsPrice  = (spaceProgram + 1) * 5 * mPresident->getSpacePriceModifier();
	int spaceProgramTechPrice	= (spaceProgram + 1) * 10 * mPresident->getSpacePriceModifier();
	int spyNetworkTechPrice		= (spyNetwork + 1) * 10 * mPresident->getSpyPriceModifier();

	mNuclearGoodsCost->setText(nuclearGoodsPrice);
	mNuclearTechCost->setText(nuclearTechPrice);
	mSpaceProgramGoodsCost->setText(spaceProgramGoodsPrice);
	mSpaceProgramTechCost->setText(spaceProgramTechPrice);
	mSpyNetworkTechCost->setText(spyNetworkTechPrice);
	
}

void Capitalist::showGUI()
{
	mCapitalistMainWindow->setVisible(true);
	//playMusic();
}

void Capitalist::hideGUI()
{
	mCapitalistMainWindow->setVisible(false);
}

