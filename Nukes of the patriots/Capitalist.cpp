#include "Capitalist.h"
#include "GUIElement.h"
#include "GUIManager.h"
#include "GUIWindow.h"
#include "GUIButton.h"
#include "GUIText.h"
#include "GUIEditField.h"
#include "GUIImage.h"
#include "President.h"
#include "ResourceHandler.h"
#include "Randomizer.h"
#include <iostream>
#include <functional>
#include "GameManager.h"
#include "Timer.h"
#include "TimerHandler.h"
#include "Communist.h"
#include "GUIAnimation.h"
#include <SFML\Window\Mouse.hpp>
#include <SFML\Window\Keyboard.hpp>
#include "Menu.h"

static int foodCost		= 10;
static int goodsCost	= 20;
static int techCost		= 30;
static int taxChange	= 5;
static int taxPatriotismChange = 0;
static int currentGoods = 0;
static int currentTech  = 0;
static int maxCurrencyChange = 0;
static bool activateWindow = false;

Capitalist::Capitalist() :
	mPresident(nullptr),
	mCount(0),
	mUpdateGUIThread(nullptr),
	mPickedPresident(0)
	
{
	mRound				= 0;
	mIncreasePopulation = false;
	mType				= CAPITALIST;
	initializeCapitalistWindow();
	initializeGuiFunctions();

	mUpdateGUIThread = new sf::Thread(&Capitalist::updateGUI, this);
	mUpdateGUIThread->launch();
}
Capitalist::~Capitalist()
{
	
}

//saving capitalist information
void Capitalist::saveGame(tinyxml2::XMLDocument &doc)
{
	tinyxml2::XMLElement *capitalist = doc.NewElement("Capitalist");

	//saving activateWindow
	tinyxml2::XMLElement *ActivateWindow = doc.NewElement("ActivateWindow");
	ActivateWindow->SetAttribute("value", activateWindow);
	capitalist->InsertEndChild(ActivateWindow);

	//saving round
	tinyxml2::XMLElement *round = doc.NewElement("Round");
	round->SetAttribute("value", mRound);
	capitalist->InsertEndChild(round);

	//saving city_image_counter
	tinyxml2::XMLElement *count = doc.NewElement("Count");
	count->SetAttribute("value", mCount);
	capitalist->InsertEndChild(count);

	//saving overall information (population, patriotism, currency, taxes)
	tinyxml2::XMLElement *overallInformation = doc.NewElement("OverallInformation");
	overallInformation->SetAttribute("Population", getPopulation());
	overallInformation->SetAttribute("Patriotism", getPatriotism());
	overallInformation->SetAttribute("Currency", getCurrency());
	overallInformation->SetAttribute("Taxes", getTaxes());
	//overallInformation->SetAttribute("TaxDecreased", mTaxDecreased);
	capitalist->InsertEndChild(overallInformation);

	//saving overall information from previous round (population, patriotism, currency, taxes)
	tinyxml2::XMLElement *overallInformationPreviousRound = doc.NewElement("OverallInformationPreviousRound");
	overallInformationPreviousRound->SetAttribute("Population_previous_round", getPopulationPreviousRound());
	overallInformationPreviousRound->SetAttribute("Patriotism_previous_round", getPatriotismPreviousRound());
	overallInformationPreviousRound->SetAttribute("Currency_previous_round", getCurrencyPreviousRound());
	overallInformationPreviousRound->SetAttribute("Taxes_previous_round", getTaxPreviousRound());
	capitalist->InsertEndChild(overallInformationPreviousRound);

	//saving President information
	tinyxml2::XMLElement *president = doc.NewElement("PresidentInformation");
	president->SetAttribute("President", mPresident);
	president->SetAttribute("Picked_president", mPickedPresident);
	capitalist->InsertEndChild(president);

	//saving resources information (amount)
	tinyxml2::XMLElement *resourcesAmount = doc.NewElement("ResourcesAmount");
	resourcesAmount->SetAttribute("food_amount", getFood());
	resourcesAmount->SetAttribute("goods_amount", getGoods());
	resourcesAmount->SetAttribute("tech_amount", getTech()); 
	capitalist->InsertEndChild(resourcesAmount);

	//saving resources information (prices)
	tinyxml2::XMLElement *resourcesPrices = doc.NewElement("ResourcesPrices");
	resourcesPrices->SetAttribute("food_price", foodCost);
	resourcesPrices->SetAttribute("goods_price", goodsCost);
	resourcesPrices->SetAttribute("tech_price", techCost);
	capitalist->InsertEndChild(resourcesPrices);

	//saving resources information from previous round
	tinyxml2::XMLElement *previousRoundResources = doc.NewElement("PreviousRoundResources");
	previousRoundResources->SetAttribute("Food_previous_round", getFoodPreviousRound());
	previousRoundResources->SetAttribute("Goods_previous_round", getGoodsPreviousRound());
	previousRoundResources->SetAttribute("Tech_previous_round", getTaxPreviousRound());
	capitalist->InsertEndChild(previousRoundResources);

	//saving exported resources information (amount)
	tinyxml2::XMLElement *exportedResources = doc.NewElement("ExportedResourcesAmount");
	exportedResources->SetAttribute("food_amount", getExportedFood());
	exportedResources->SetAttribute("goods_amount", getExportedGoods());
	exportedResources->SetAttribute("tech_amount", getExportedTech());
	capitalist->InsertEndChild(exportedResources);

	//saving exported resources information (prices)
	tinyxml2::XMLElement *exportedResourcesPrices = doc.NewElement("ExportedResourcesPrices");
	exportedResourcesPrices->SetAttribute("food_price", getExportedFoodPrice());
	exportedResourcesPrices->SetAttribute("goods_price", getExportedGoodsPrice());
	exportedResourcesPrices->SetAttribute("tech_price", getExportedTechPrice());
	capitalist->InsertEndChild(exportedResourcesPrices);

	//saving exported resources information (sold)
	tinyxml2::XMLElement *exportedResourcesSold = doc.NewElement("ExportedResourcesSold");
	exportedResourcesSold->SetAttribute("food_sold", getExportedFoodSold());
	exportedResourcesSold->SetAttribute("goods_sold", getExportedGoodsSold());
	exportedResourcesSold->SetAttribute("tech_sold", getExportedTechSold());

	//saving exported resources information from previous round
	tinyxml2::XMLElement *previousRoundExportedResources = doc.NewElement("PreviousRoundExportedResources");
	previousRoundExportedResources->SetAttribute("Exported_food_previous_round", getExportedFoodPreviousRound());
	previousRoundExportedResources->SetAttribute("Exported_goods_previous_round", getExportedGoodsPreviousRound());
	previousRoundExportedResources->SetAttribute("Exported_tech_previous_round", getExportedTechPreviousRound());
	capitalist->InsertEndChild(previousRoundExportedResources);

	//saving upgrades (nuclear, spaceprogram, spynetwork)
	tinyxml2::XMLElement *upgrades = doc.NewElement("Upgrades");
	upgrades->SetAttribute("Nuclear_weapon", getNuclearWeapon());
	upgrades->SetAttribute("Space_program", getSpaceProgram());
	upgrades->SetAttribute("Spy_network", getSpyNetwork());
	capitalist->InsertEndChild(upgrades);

	//saving upgrades from previous round
	tinyxml2::XMLElement *previousRoundUpgrades = doc.NewElement("PreviousRoundUpgrades");
	previousRoundUpgrades->SetAttribute("Nuclear_weapon_previous_round", getNuclearWeaponPreviousRound());
	previousRoundUpgrades->SetAttribute("Space_program_previous_round", getSpaceProgramPreviousRound());
	previousRoundUpgrades->SetAttribute("Spy_network_previous_round", getSpyNetworkPreviousRound());
	capitalist->InsertEndChild(previousRoundUpgrades);

	doc.InsertEndChild(capitalist);
}

//loading capitalist information
void Capitalist::loadGame(tinyxml2::XMLDocument &doc)
{
	tinyxml2::XMLElement *capitalist = doc.FirstChildElement("Capitalist");
	
	//loading activate window
	tinyxml2::XMLElement *ActivateWindow = capitalist->FirstChildElement("ActivateWindow");
	activateWindow = atoi(ActivateWindow->Attribute("value"));

	//loading round
	tinyxml2::XMLElement *round = capitalist->FirstChildElement("Round");
	mRound = atoi(round->Attribute("value"));

	//loading city_image_count
	tinyxml2::XMLElement *count = capitalist->FirstChildElement("Count");
	mCount = atoi(count->Attribute("value"));

	//loading overall information
	tinyxml2::XMLElement *overallInformation = capitalist->FirstChildElement("OverallInformation");
	mPopulation = atoi(overallInformation->Attribute("Population"));
	mPatriotism = atoi(overallInformation->Attribute("Patriotism"));
	mCurrency	= atoi(overallInformation->Attribute("Currency"));
	mTaxes		= atoi(overallInformation->Attribute("Taxes"));

	//loading overall information from previous round
	tinyxml2::XMLElement *overallInformationPreviousRound = capitalist->FirstChildElement("OverallInformationPreviousRound");
	mPopulationPreviousRound = atoi(overallInformationPreviousRound->Attribute("Population_previous_round"));
	mPatriotismPreviousRound = atoi(overallInformationPreviousRound->Attribute("Patriotism_previous_round"));
	mCurrencyPreviousRound   = atoi(overallInformationPreviousRound->Attribute("Currency_previous_round"));
	mTaxesPreviousRound		 = atoi(overallInformationPreviousRound->Attribute("Taxes_previous_round"));

	//loading General information
	tinyxml2::XMLElement *president = capitalist->FirstChildElement("PresidentInformation");
	//ADD THE INFORMATION ABOUT WHICH PRESIDENT THAT WAS PICKED
	mPickedPresident = atoi(president->Attribute("Picked_president"));

	//loading resources (amount)
	tinyxml2::XMLElement *resourcesAmount = capitalist->FirstChildElement("ResouresAmount");
	mFood	= atoi(resourcesAmount->Attribute("food_amount"));
	mGoods	= atoi(resourcesAmount->Attribute("goods_amount"));
	mTech	= atoi(resourcesAmount->Attribute("tech_amount"));

	//loading resources (prices)
	tinyxml2::XMLElement *resourcesPrices = capitalist->FirstChildElement("ResourcesPrices");
	foodCost	= atoi(resourcesPrices->Attribute("food_price"));
	goodsCost	= atoi(resourcesPrices->Attribute("goods_price"));
	techCost	= atoi(resourcesPrices->Attribute("tech_price"));

	//loading resources information from previous round
	tinyxml2::XMLElement *previousRoundResources = capitalist->FirstChildElement("PreviousRoundResources");
	mFoodPreviousRound  = atoi(previousRoundResources->Attribute("Food_previous_round"));
	mGoodsPreviousRound = atoi(previousRoundResources->Attribute("Goods_previous_round"));
	mTaxesPreviousRound = atoi(previousRoundResources->Attribute("Tech_previous_round"));

	//loading exported resource information (amount)
	tinyxml2::XMLElement *exportedResources = capitalist->FirstChildElement("ExportedResourcesAmount");
	mExportedFood  = atoi(exportedResources->Attribute("food_amount"));
	mExportedGoods = atoi(exportedResources->Attribute("goods_amount"));
	mExportedTech  = atoi(exportedResources->Attribute("tech_amount"));

	//loading exported resources information (prices)
	tinyxml2::XMLElement *exportedResourcesPrices = capitalist->FirstChildElement("ExportedResourcesPrices");
	mExportedFoodPrice  = atoi(exportedResourcesPrices->Attribute("food_price"));
	mExportedGoodsPrice = atoi(exportedResourcesPrices->Attribute("goods_price"));
	mExportedTechPrice  = atoi(exportedResourcesPrices->Attribute("tech_price"));

	//loading exported resources information from previous round
	tinyxml2::XMLElement *previousRoundExportedResources = capitalist->FirstChildElement("PreviousRoundExportedResources");
	mExportedFoodPreviousRound  = atoi(previousRoundExportedResources->Attribute("Exported_food_previous_round"));
	mExportedGoodsPreviousRound = atoi(previousRoundExportedResources->Attribute("Exported_goods_previous_round"));
	mExportedTechPreviousRound  = atoi(previousRoundExportedResources->Attribute("Exported_tech_previous_round"));

	//loading upgrades (nuclear, spaceprogram, spynetwork)
	tinyxml2::XMLElement *upgrades = capitalist->FirstChildElement("Upgrades");
	mNuclearWeapon = atoi(upgrades->Attribute("Nuclear_weapon"));
	mSpaceProgram  = atoi(upgrades->Attribute("Space_program"));
	mSpyNetwork    = atoi(upgrades->Attribute("Spy_network"));

	//loading upgrades from previoud round
	tinyxml2::XMLElement *previousRoundUpgrades = capitalist->FirstChildElement("PreviousRoundUpgrades");
	mNuclearWeaponPreviousRound  = atoi(previousRoundUpgrades->Attribute("Nuclear_weapon_previous_round"));
	mSpaceProgramPreviousRound	 = atoi(previousRoundUpgrades->Attribute("Space_program_previous_round"));
	mSpyNetworkPreviousRound	 = atoi(previousRoundUpgrades->Attribute("Spy_network_previous_round"));
}

void Capitalist::reset()
{
	static int foodCost		= 10;
	static int goodsCost	= 20;
	static int techCost		= 30;
	static int taxChange	= 5;
	static int currentGoods = 0;
	static int currentTech  = 0;
	static bool activateWindow = false;
	mPresident = nullptr;
	mCount = 0;
	mIncreasePopulation = false;
	mRound = 0;
	hideGUI();
	mTaxesIncomeWindow->setVisible(false);

	mCapitalistTaxesButton->setTexture(CapitalistButtons["Taxes"]);
	mCapitalistTaxesButton->setSize(CapitalistButtons["Taxes"].first.width, CapitalistButtons["Taxes"].first.height);
	mCapitalistResourceButton->setTexture(CapitalistButtons["Resource"]);
	mCapitalistResourceButton->setSize(CapitalistButtons["Resource"].first.width, CapitalistButtons["Resource"].first.height);
	mCapitalistUpgradeButton->setTexture(CapitalistButtons["Upgrade"]);
	mCapitalistUpgradeButton->setSize(CapitalistButtons["Upgrade"].first.width, CapitalistButtons["Upgrade"].first.height);
	mCapitalistTradeButton->setTexture(CapitalistButtons["Export"]);
	mCapitalistTradeButton->setSize(CapitalistButtons["Export"].first.width, CapitalistButtons["Export"].first.height);
	
	mChoosePresidentWindow->setVisible(true);
	mChoosePresidentWindow->setEnabled(true, true );
	mCapitalistPresident->setTexture(CapitalistButtons["President"]);
	mCapitalistPresident->setSize(CapitalistButtons["President"].first.width, CapitalistButtons["President"].first.height);
	
	SuperPower::reset();
}

void Capitalist::updateGUI()
{
	//while(true)
	//{
		//if(mCapitalistPresident != nullptr && mPresident != NULL)
		//{
			//mPresident->presidentAnimation();
			//mCapitalistPresident->setTexture(std::pair<sf::FloatRect, sf::Texture*>(mCapitalistPresident->getRectangle(), mPresident->getCurrentAnimationFrame()));
			//mCapitalistPresident->setScale(0.55, 0.60);
		//}
	Timer::setTimer([=]()
	{
		int oldPopulation = stringToInt(mPopulationText->getText().substr(0, mPopulationText->getText().length() - 9));
		if(mPopulation != oldPopulation)
			mPopulationText->setText("Population: " + intToString(mPopulation) + " million");

		int oldCurrency = stringToInt(mCurrencyText->getText());
		if(mCurrency != oldCurrency && stringToInt(mCurrencyText->getText()) != mCurrency)
		{
			int difference = mCurrency - oldCurrency;
			int currencyChange = 1;
			maxCurrencyChange += currencyChange;
			if(difference < 0)
			{
				if(maxCurrencyChange > 10)
				{
					maxCurrencyChange = 0;
					currencyChange = 100;
				}
				mCurrencyText->setText(intToString(oldCurrency - currencyChange) + " §");
				if(stringToInt(mCurrencyText->getText()) < mCurrency)
					mCurrencyText->setText(intToString(mCurrency) + " §");
			}
			else
			{
				if(maxCurrencyChange > 10)
				{
					maxCurrencyChange = 0;
					currencyChange = 100;
				}
				mCurrencyText->setText(intToString(oldCurrency + currencyChange) + " §");
				if(stringToInt(mCurrencyText->getText()) > mCurrency)
					mCurrencyText->setText(intToString(mCurrency) + " §");
			}
		}

		int oldPatriotism = stringToInt(mPatriotismText->getText());
		if(mPatriotism != oldPatriotism)
			mPatriotismText->setText("Patriotism: " + intToString(mPatriotism));

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


		mResourceFoodPriceText->setText("Price: " + intToString(foodCost) + " §");
		mResourceGoodsPriceText->setText("Price: " + intToString(goodsCost) + " §");
		mResourceTechPriceText->setText("Price: " + intToString(techCost) + " §");
	}, 2, 0);
}


void Capitalist::playMusic()
{
	mCapitalistMainTheme->playSound(true);
	mCapitalistMainTheme->setVolume(60);
}

void Capitalist::stopMusic()
{
	mCapitalistMainTheme->stopSound();
}

void Capitalist::pauseMusic()
{
	mCapitalistMainTheme->pauseSound();
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
	//communist title: 745, 90
	//capitalist title: 241, 90
	int statsPosY = 177;   //communist 628, 177
	int foodPatriotismChange = 0;

	if(mFood == 0)
	{
		foodPatriotismChange = -4; 
		mFoodChange->setText("No food for the people");
		mFoodChange->setY(statsPosY);
		mFoodChangeValue->setText(foodPatriotismChange);
		mFoodChangeValue->setY(statsPosY);
		statsPosY += mFoodChange->getHeight();
	}
	else if(mFood > 0 && mFood <= mPopulation/2)
	{
		foodPatriotismChange = -2;
		mFoodChange->setText("Too little food for the people");
		mFoodChange->setY(statsPosY);
		mFoodChangeValue->setText(foodPatriotismChange);
		mFoodChangeValue->setY(statsPosY);
		statsPosY += mFoodChange->getHeight();
	}
	else if(mFood >= mPopulation)
		mPopulation = mPopulation;
	else
	{
		mFoodChange->setText("");
		mFoodChangeValue->setText("");
	}
	
	mFood -= (float)mPopulationPreviousRound*mPresident->getPopEatsMore(); ////// FIXA DETTA SÅ ATT DET STÄMMER
	if(mFood < 0) mFood = 0;
	int taxPatriotismChange = 0;
	int taxChange = mTaxes - mTaxesPreviousRound;
	if(taxChange > 0)
	{
		taxPatriotismChange = -3;
		mTaxChange->setText("Tax increased");
		mTaxChange->setY(statsPosY);
		mTaxChangeValue->setText(taxPatriotismChange);
		mTaxChangeValue->setY(statsPosY);
		statsPosY += mTaxChange->getHeight();
	}
	else if(taxChange < 0)
	{
		taxPatriotismChange = 2 + mPresident->getPatriotismTaxModifier();
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
	int spaceProgramAmount = 0;
	if(spaceProgramIncreased)
	{
		spaceProgramAmount = mSpaceProgram - mSpaceProgramPreviousRound;
		mSpaceProgramIncreasedText->setText("Space program upgraded");
		mSpaceProgramIncreasedText->setY(statsPosY);
		mSpaceProgramIncreasedTextValue->setText("+" + intToString(spaceProgramAmount));
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
	if(mNuclearWeapon >= enemyNuclearWeapon*2)
	{
		mNuclearWeaponChange->setText("Twice as many nuclear weapons");
		nuclearWeaponChange = 2;
		mNuclearWeaponChange->setY(statsPosY);
		mNuclearWeaponChangeValue->setText("+"+intToString(nuclearWeaponChange));
		mNuclearWeaponChangeValue->setY(statsPosY);
		statsPosY += mNuclearWeaponChange->getHeight();
	}
	else if(mNuclearWeapon > enemyNuclearWeapon)
	{
		mNuclearWeaponChange->setText("More nuclear weapons");
		nuclearWeaponChange = 1;
		mNuclearWeaponChange->setY(statsPosY);
		mNuclearWeaponChangeValue->setText("+"+intToString(nuclearWeaponChange));
		mNuclearWeaponChangeValue->setY(statsPosY);
		statsPosY += mNuclearWeaponChange->getHeight();
	}
	else
	{
		mNuclearWeaponChange->setText("");
		mNuclearWeaponChangeValue->setText("");
	}

	if(mSpaceProgram > enemySpaceProgram)
	{
		mSpaceProgramMoreThanEnemyText->setText("More advanced space program");
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
	//int exportedFoodChange = mExportedFoodPreviousRound - mExportedFood;
	//int exportedGoodsChange = mExportedGoodsPreviousRound - mExportedGoods;
	//int exportedTechChange = mExportedTechPreviousRound - mExportedTech;
	int exportedTotal = getExportedFoodSold() + getExportedGoodsSold() + getExportedTechSold();

	// Enemy exported resources
	//int enemyFoodExported = enemy->getExportedFoodPreviousRound() - enemy->getExportedFood();
	//int enemyGoodsExported = enemy->getExportedGoodsPreviousRound() - enemy->getExportedGoods();
	//int enemyTechExported = enemy->getExportedTechPreviousRound() - enemy->getExportedTech();
	int enemyExportedTotal = enemy->getExportedFoodSold() + enemy->getExportedGoodsSold() + enemy->getExportedTechSold();

	if(exportedTotal > enemyExportedTotal)
	{
		mExportedChange->setText("Exported more resources");
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


	//mCurrentPopulationText[1]->setText(mPopulation);
	//mCurrentTaxesText[1]->setText(mTaxes);
	//mTaxesIncomeText[1]->setText(intToString(mTaxesPreviousRound*mPopulationPreviousRound));
	//std::cout<<"income window tax: "<<mTaxesPreviousRound*mPopulationPreviousRound<<std::endl;

	int totalPatriotismChange = foodPatriotismChange + taxPatriotismChange + nuclearWeaponChange + spaceProgramChange + spaceProgramAmount + exportedChange;
	mPatriotism += totalPatriotismChange;
	
	mPatriotismChange->setText("Total patriotism change: " + intToString(totalPatriotismChange));
	mNewPatriotism->setText("Total patriotism: " + intToString(mPatriotism));
}

void Capitalist::update()
{
	if(GameManager::getInstance()->getGameType() == LAN && GameManager::getInstance()->getRemoteClient()->getSuperPower() == CAPITALIST)
		GameManager::getInstance()->setEnemyTurn();
	else
		GameManager::getInstance()->setMyTurn();

	mWindowHeadlines[0]->setText(intToString(GameManager::getInstance()->getYear()) + " Presidential Elections ");
	mImportHeadliner->setText("Import From " + Menu::getInstance()->getEditField("CommunistNameField")->getText());
	
	//std::cout<<"capitalist mRound: "<<mRound<<std::endl;
	if(mRound > 1)
	{
		if((mRound-1) % 4 == 0 ) 
		{
			chooseLeader();
		}
		else
		{
			getTaxIncome();

			mTaxesIncomeWindow->setVisible(true);
			mCapitalistMainWindow->setEnabled(false, true);
			mTaxesIncomeWindow->setEnabled(true, true);
		}
	}
	mPopulationEatsFoodHeadliner->setText("Population Report " + intToString(GameManager::getInstance()->getYear()));
		
	mCurrentPopulationText[1]->setText(intToString(getPopulation()) + " million");
	mCurrentTaxesText[1]->setText(intToString(mTaxes));
	mTaxesIncomeText[1]->setText(intToString(getTaxes()*getPopulation()) + " §");

	std::shared_ptr<SuperPower> enemy = GameManager::getInstance()->getCommunist();

	mImportResourcesAvailableText[0]->setText(enemy->getExportedFood());
	mImportResourcesAvailableText[1]->setText(enemy->getExportedGoods());
	mImportResourcesAvailableText[2]->setText(enemy->getExportedTech());

	mImportPriceText[0]->setText(enemy->getExportedFoodPrice());
	mImportPriceText[1]->setText(enemy->getExportedGoodsPrice());
	mImportPriceText[2]->setText(enemy->getExportedTechPrice());

	mImportBuyQuantityText[0]->setText("0");
	mImportBuyQuantityText[1]->setText("0");
	mImportBuyQuantityText[2]->setText("0");

	mImportTotalPriceText[0]->setText("0");
	mImportTotalPriceText[1]->setText("0");
	mImportTotalPriceText[2]->setText("0");

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

	mExportQuantityText[0]->setText(mExportedFood);
	mExportQuantityText[1]->setText(mExportedGoods);
	mExportQuantityText[2]->setText(mExportedTech);

	mExportFoodCost->setText(intToString(mExportedFoodPrice));
	mExportGoodsCost->setText(intToString(mExportedGoodsPrice));
	mExportTechCost->setText(intToString(mExportedTechPrice));

	mExportPriceText[0]->setText(mExportedFood * mExportedFoodPrice);
	mExportPriceText[1]->setText(mExportedGoods * mExportedGoodsPrice);
	mExportPriceText[2]->setText(mExportedTech * mExportedTechPrice);
	mExportTotalPriceValue->setText(mExportedFood * mExportedFoodPrice + mExportedGoods * mExportedGoodsPrice + mExportedTech * mExportedTechPrice);
	// Set previous round values as current round values so we can get the difference at the start of the next round
	// Would've been better to use a vector
	mPatriotismPreviousRound = mPatriotism;
	mCurrencyPreviousRound = mCurrency;
	mPopulationPreviousRound = mPopulation;
	mFoodPreviousRound = mFood;
	mGoodsPreviousRound = mGoods;
	mTechPreviousRound = mTech;
	mTaxesPreviousRound = mTaxes;
	mSpyNetworkPreviousRound = mSpyNetwork;
	mNuclearWeaponPreviousRound = mNuclearWeapon;
	mSpaceProgramPreviousRound = mSpaceProgram;
	//setExportedFoodSold(0);
	//setExportedGoodsSold(0);
	//setExportedTechSold(0);
	//std::cout<<"tax previous round: "<<mTaxesPreviousRound<<std::endl;
	//std::cout<<"population previous round: "<<mPopulationPreviousRound<<std::endl;
	mTaxesPatriotismChange->setText(0);

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
	taxPatriotismChange = president->getPatriotismTaxModifier();
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
	
	mNuclearWeapon += value;
	//mGoods -= goodsNuclearPrice;
	//mTech -= techNuclearPrice;
	mNuclearText->setText(mNuclearWeapon);
	
	return true;
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
	
	mSpaceProgram += value;
	//mGoods -= goodsSpaceProgramPrice;
	//mTech -= techSpaceProgramPrice;
	mSpaceText->setText(mSpaceProgram);
	
	return true;
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

	mSpyNetwork += value;
	//mTech -= spyNetworkPrice;
	mSpyText->setText(mSpyNetwork);
	
	return true;
}

/*Kontrollerar ifall det finns nog med mat till hela befolkningen*/
bool Capitalist::enoughFood()
{
	return mFood >= mPopulation * mPresident->getPopEatsMore();
}

void Capitalist::resetResourcesValues()
{
	mFoodCost->setText("Cost: 0 §");
	mGoodsCost->setText("Cost: 0 §");
	mTechCost->setText("Cost: 0 §");
	mTotalResourcesCost->setText("Total cost: 0 §");
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
	initializeCityImages();

	mCapitalistMainTheme				= Sound::create(CapitalistMusic["CapitalistMainTheme"]);
	mIncreasedResoucesSound				= Sound::create();
	mUpgradeSound						= Sound::create();


	mCapitalistMainWindow				= GUIWindow::create(CapitalistWindows["CapitalistInterface"]);
	mCapitalistButtonFrame				= GUIWindow::create(CapitalistWindows["InterfaceButtonsFrame"], mCapitalistMainWindow);
	mCapitalistBorder					= GUIWindow::create(CapitalistWindows["CapitalistBorder"], mCapitalistMainWindow);
	mCapitalistBorder					= GUIWindow::create(CapitalistWindows["CapitalistBorderTop"], mCapitalistMainWindow);
	mChangeCityImage					= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>
										  (CapitalistButtons["CityImages"].first, CityImages[0]), mCapitalistMainWindow);
	mCapitalistPresident				= GUIButton::create(CapitalistButtons["President"], mCapitalistMainWindow);
	mPresidentFrame						= GUIImage::create(CapitalistButtons["PresidentFrame"], mCapitalistMainWindow);
	mCapitalistPresident->setX(mPresidentFrame->getX() + 7); mCapitalistPresident->setY(mPresidentFrame->getY() + 9);
	mCapitalistPresident->setScale(0.90, 0.80);
	mCapitalistTaxesButton				= GUIButton::create(CapitalistButtons["Taxes"], mCapitalistMainWindow);
	mCapitalistResourceButton			= GUIButton::create(CapitalistButtons["Resource"], mCapitalistMainWindow);
	mCapitalistUpgradeButton			= GUIButton::create(CapitalistButtons["Upgrade"], mCapitalistMainWindow);
	mCapitalistTradeButton				= GUIButton::create(CapitalistButtons["Export"], mCapitalistMainWindow);
	mEndTurnFrame						= GUIImage::create(CapitalistButtons["EndTurnFrame"], mCapitalistMainWindow);
	mCapitalistEndTurnButton			= GUIButton::create(CapitalistButtons["EndTurn"], mCapitalistMainWindow);
	mLeftPanel							= GUIImage::create(CapitalistButtons["LeftPanel"], mCapitalistMainWindow);
	mRightPanel							= GUIImage::create(CapitalistButtons["RightPanel"], mCapitalistMainWindow);

	mPopulationText						= GUIText::create(sf::FloatRect(698, 4, 0, 0), intToString(mPopulation) + " million", mCapitalistMainWindow);
	mPopulationText->setScale(0.7, 0.7);
	mPopulationText->setColor(sf::Color::White);
	mPopulationText->setAlignment("middle");
	mCurrencyText						= GUIText::create(sf::FloatRect(362, 4, 0, 0), intToString(mCurrency) + "§", mCapitalistMainWindow);
	mCurrencyText->setScale(0.7, 0.7);
	mCurrencyText->setColor(sf::Color::White);
	mCurrencyText->setAlignment("middle");
	mPatriotismText						= GUIText::create(sf::FloatRect(530, 42, 0, 0), "Patriotism: " + intToString(mPatriotism), mCapitalistMainWindow);
	mPatriotismText->setScale(0.7, 0.7);
	mPatriotismText->setColor(sf::Color::White);
	mPatriotismText->setAlignment("middle");
	mCapitalistMainWindow->setVisible(false);

	/*GUI text för utskrift av värden på komunisternas interface*/

	mNuclearText						= GUIText::create(sf::FloatRect(843, 3, 0, 0), intToString(getNuclearWeapon()), mCapitalistMainWindow);
	mNuclearText->setAlignment("middle");
	mNuclearText->setScale(0.9, 0.9);
	mSpaceText							= GUIText::create(sf::FloatRect(843, 215, 0, 0), intToString(getSpaceProgram()), mCapitalistMainWindow);
	mSpaceText->setAlignment("middle");
	mSpaceText->setScale(0.9, 0.9);
	mSpyText							= GUIText::create(sf::FloatRect(843, 427, 0, 0), intToString(getSpyNetwork()), mCapitalistMainWindow);
	mSpyText->setAlignment("middle");
	mSpyText->setScale(0.9, 0.9);
	mFoodText							= GUIText::create(sf::FloatRect(31, 3, 0, 0), intToString(getFood()), mCapitalistMainWindow);
	mFoodText->setAlignment("middle");
	mFoodText->setScale(0.9, 0.9);
	mGoodsText							= GUIText::create(sf::FloatRect(31, 215, 0, 0), intToString(getGoods()), mCapitalistMainWindow);
	mGoodsText->setAlignment("middle");
	mGoodsText->setScale(0.9, 0.9);
	mTechText							= GUIText::create(sf::FloatRect(31, 427, 0, 0), intToString(getTech()), mCapitalistMainWindow);
	mTechText->setAlignment("middle");
	mTechText->setScale(0.9, 0.9);

	mTaxesWindow						= GUIWindow::create(CapitalistWindows["CapitalistTaxesWindow"], mCapitalistMainWindow);
	mLowerTaxesButton					= GUIButton::create(CapitalistButtons["LowerTaxes"], mTaxesWindow);
	mTaxValueText						= GUIText::create(sf::FloatRect(105, 58, 0, 0), intToString(mTaxes), mTaxesWindow);
	mTaxValueText->setAlignment("middle");
	mTaxText							= GUIText::create(sf::FloatRect(285, 9, 0, 0), "Set Tax Rate", mTaxesWindow);
	mTaxText->setAlignment("middle");
	mTaxesPatriotismChange				= GUIText::create(sf::FloatRect(230, 50, 0, 0), "Patriotism: 0", mTaxesWindow);
	mTaxesPatriotismChange->setScale(0.8, 0.8);

	mRaiseTaxesButton					= GUIButton::create(CapitalistButtons["RaiseTaxes"], mTaxesWindow);
	mTaxesCloseButton					= GUIButton::create(CapitalistButtons["CloseTaxes"], mTaxesWindow);
	mTaxesWindow->setVisible(false);

	mResourceWindow						= GUIWindow::create(CapitalistWindows["CapitalistResourceWindow"], mCapitalistMainWindow);
	mWindowHeadlines[2]					= GUIText::create(sf::FloatRect(285, 9, 0, 0), "Buy Resources", mResourceWindow);
	mWindowHeadlines[2]->setAlignment("middle");
	mLowerFoodByTenButton				= GUIButton::create(CapitalistButtons["LowerFoodByTen"], mResourceWindow);
	mLowerFoodByFiveButton				= GUIButton::create(CapitalistButtons["LowerFoodByFive"], mResourceWindow);
	mLowerFoodByOneButton				= GUIButton::create(CapitalistButtons["LowerFoodByOne"], mResourceWindow);
	mRaiseFoodByOneButton				= GUIButton::create(CapitalistButtons["RaiseFoodByOne"], mResourceWindow);
	mRaiseFoodByFiveButton				= GUIButton::create(CapitalistButtons["RaiseFoodByFive"], mResourceWindow);
	mRaiseFoodByTenButton				= GUIButton::create(CapitalistButtons["RaiseFoodByTen"], mResourceWindow);
	mResourceFoodPriceText				= GUIText::create(sf::FloatRect(20, 103, 40, 40), "Price: " + intToString(foodCost) + " §", mResourceWindow);
	mResourceFoodPriceText->setScale(0.8, 0.8);

	mLowerGoodsByTenButton				= GUIButton::create(CapitalistButtons["LowerGoodsByTen"], mResourceWindow);
	mLowerGoodsByFiveButton				= GUIButton::create(CapitalistButtons["LowerGoodsByFive"], mResourceWindow);
	mLowerGoodsByOneButton				= GUIButton::create(CapitalistButtons["LowerGoodsByOne"], mResourceWindow);
	mRaiseGoodsByOneButton				= GUIButton::create(CapitalistButtons["RaiseGoodsByOne"], mResourceWindow);
	mRaiseGoodsByFiveButton				= GUIButton::create(CapitalistButtons["RaiseGoodsByFive"], mResourceWindow);
	mRaiseGoodsByTenButton				= GUIButton::create(CapitalistButtons["RaiseGoodsByTen"], mResourceWindow);
	mResourceGoodsPriceText				= GUIText::create(sf::FloatRect(200, 103, 40, 40), "Price: " + intToString(goodsCost) + " §", mResourceWindow);
	mResourceGoodsPriceText->setScale(0.8, 0.8);

	mLowerTechByTenButton				= GUIButton::create(CapitalistButtons["LowerTechByTen"], mResourceWindow);
	mLowerTechByFiveButton				= GUIButton::create(CapitalistButtons["LowerTechByFive"], mResourceWindow);
	mLowerTechByOneButton				= GUIButton::create(CapitalistButtons["LowerTechByOne"], mResourceWindow);
	mRaiseTechByOneButton				= GUIButton::create(CapitalistButtons["RaiseTechByOne"], mResourceWindow);
	mRaiseTechByFiveButton				= GUIButton::create(CapitalistButtons["RaiseTechByFive"], mResourceWindow);
	mRaiseTechByTenButton				= GUIButton::create(CapitalistButtons["RaiseTechByTen"], mResourceWindow);
	mResourceCloseButton				= GUIButton::create(CapitalistButtons["CloseResource"], mResourceWindow);
	mResourceTechPriceText				= GUIText::create(sf::FloatRect(380, 103, 40, 40), "Price: " + intToString(techCost) + " §", mResourceWindow);
	mResourceTechPriceText->setScale(0.8, 0.8);
	
	mBuyFoodText						= GUIText::create(sf::FloatRect(104, 141, 0, 0), "0",mResourceWindow);
	mBuyFoodText->setAlignment("middle");
	mBuyGoodsText						= GUIText::create(sf::FloatRect(285, 141, 40, 40), "0", mResourceWindow);
	mBuyGoodsText->setAlignment("middle");
	mBuyTechText						= GUIText::create(sf::FloatRect(465, 141, 40, 40), "0", mResourceWindow);
	mBuyTechText->setAlignment("middle");
	mFoodCost							= GUIText::create(sf::FloatRect(20, 183, 40, 40), "Cost: 0 §", mResourceWindow);
	mFoodCost->setScale(0.8, 0.8);
	//mFoodCost->setAlignment("middle");
	mGoodsCost							= GUIText::create(sf::FloatRect(200, 183, 40, 40), "Cost: 0 §", mResourceWindow);
	mGoodsCost->setScale(0.8, 0.8);
	//mGoodsCost->setAlignment("middle");
	mTechCost							= GUIText::create(sf::FloatRect(380, 183, 40, 40), "Cost: 0 §", mResourceWindow);
	mTechCost->setScale(0.8, 0.8);
	//mTechCost->setAlignment("middle");

	mTotalResourcesCost				= GUIText::create( sf::FloatRect(285, 230, 40, 40), "Total cost: 0 §", mResourceWindow);
	mTotalResourcesCost->setScale(0.8, 0.8);
	mTotalResourcesCost->setAlignment("middle");
	mResourceWindow->setVisible(false);

	mUpgradeWindow						= GUIWindow::create(CapitalistWindows["CapitalistUpgradeWindow"], mCapitalistMainWindow);
	mUpgradeHeadliner					= GUIText::create(sf::FloatRect(285, 9, 0, 0), "Technological Advancements", mUpgradeWindow);
	mUpgradeHeadliner->setAlignment("middle");
	mUpgradeNuclearWeaponButton		    = GUIButton::create(CapitalistButtons["UpgradeNuclearWeapon"], mUpgradeWindow);
	mUpgradeSpaceProgramButton			= GUIButton::create(CapitalistButtons["UpgradeSpaceProgram"], mUpgradeWindow);
	mUpgradeSpyNetworkButton			= GUIButton::create(CapitalistButtons["UpgradeSpyNetwork"], mUpgradeWindow);
	mCancelUpgradeNuclearWeaponButton	= GUIButton::create(CapitalistButtons["CancelUpgradeNuclearWeapon"], mUpgradeWindow);
	mCancelUpgradeSpaceProgramButton	= GUIButton::create(CapitalistButtons["CancelUpgradeSpaceProgram"], mUpgradeWindow);
	mCancelUpgradeSpyNetworkButton		= GUIButton::create(CapitalistButtons["CancelUpgradeSpyNetwork"], mUpgradeWindow);
	mUpgradeCloseButton					= GUIButton::create(CapitalistButtons["CloseUpgrade"], mUpgradeWindow);

	mSpyPanel		= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(359, 90, 200, 200), &ResourceHandler::getInstance()->getTexture(std::string("Capitalist/kap_spy_upgrade"))), mUpgradeWindow);
	mSpacePanel		= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(184, 127, 200, 200), &ResourceHandler::getInstance()->getTexture(std::string("Capitalist/kap_space_upgrade"))), mUpgradeWindow);
	mNuclearPanel	= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(9, 165, 200, 200), &ResourceHandler::getInstance()->getTexture(std::string("Capitalist/kap_nuke_upgrade"))), mUpgradeWindow);

	mBuyNuclearText						= GUIText::create(sf::FloatRect(39, 168, 22, 22), "0", mUpgradeWindow);
	mBuyNuclearText->setAlignment("middle");
	mBuyNuclearText->setScale(0.9, 0.9);
	mNuclearTechCost					= GUIText::create(sf::FloatRect(45, 125, 20, 20), "0", mUpgradeWindow);
	mNuclearTechCost->setScale(0.9, 0.9);
	mNuclearGoodsCost					= GUIText::create(sf::FloatRect(45, 90, 20, 20), "0", mUpgradeWindow);
	mNuclearGoodsCost->setScale(0.9, 0.9);
	mBuySpaceProgramText				= GUIText::create(sf::FloatRect(214, 130, 22, 22), "0", mUpgradeWindow);
	mBuySpaceProgramText->setAlignment("middle");
	mBuySpaceProgramText->setScale(0.9, 0.9);
	mSpaceProgramTechCost				= GUIText::create(sf::FloatRect(220, 90, 20, 20), "0", mUpgradeWindow);
	mSpaceProgramTechCost->setScale(0.9, 0.9);
	mSpaceProgramGoodsCost				= GUIText::create(sf::FloatRect(220, 55, 20, 20), "0", mUpgradeWindow);
	mSpaceProgramGoodsCost->setScale(0.9, 0.9);
	mBuySpyNetworkText					= GUIText::create(sf::FloatRect(389, 93, 22, 22), "0", mUpgradeWindow);
	mBuySpyNetworkText->setAlignment("middle");
	mBuySpyNetworkText->setScale(0.9, 0.9);
	mSpyNetworkTechCost					= GUIText::create(sf::FloatRect(410, 50, 20, 20), "0", mUpgradeWindow);
	mSpyNetworkTechCost->setScale(0.9, 0.9);
	mUpgradeWindow->setVisible(false);

	mExportWindow						= GUIWindow::create(CapitalistWindows["CapitalistExportWindow"], mCapitalistMainWindow);
	mExportHeadliner					= GUIText::create(sf::FloatRect(285, 9, 0, 0), "Export Resources", mExportWindow);
	mExportHeadliner->setAlignment("middle");
	mExportQuantityLabel				= GUIText::create(sf::FloatRect(164, 57, 200, 100), "Quantity", mExportWindow);
	mExportQuantityLabel->setScale(0.7, 0.7);
	mExportQuantityLabel->setAlignment("middle");
	mExportCostLabel					= GUIText::create(sf::FloatRect(348, 57, 200, 100), "Cost", mExportWindow);
	mExportCostLabel->setScale(0.7, 0.7);
	mExportCostLabel->setAlignment("middle");
	mExportPriceLabel					= GUIText::create(sf::FloatRect(502, 57, 200, 100), "Price", mExportWindow);
	mExportPriceLabel->setScale(0.7, 0.7);
	mExportPriceLabel->setAlignment("middle");
	mExportTotalPriceLabel				= GUIText::create(sf::FloatRect(392, 271, 200, 100), "Total price:", mExportWindow);
	mExportTotalPriceLabel->setScale(0.7, 0.7);
	mExportTotalPriceLabel->setAlignment("middle");

	mExportTotalPriceValue				= GUIText::create(sf::FloatRect(502, 271, 100, 31), "0", mExportWindow);
	mExportTotalPriceValue->setScale(0.7, 0.7);
	mExportTotalPriceValue->setAlignment("middle");
	
	sf::Texture *buyField = &ResourceHandler::getInstance()->getTexture(std::string("Capitalist/Capitalist_Edit_Field"));

	mExportQuantityBackground[0]		= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(137, 93/*53*/, 57, 40), buyField), mExportWindow);
	mExportQuantityBackground[1]		= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(137, 151/*111*/, 57, 40), buyField), mExportWindow);
	mExportQuantityBackground[2]		= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(137, 210/*170*/, 57, 40), buyField), mExportWindow);
	for(int i = 0; i < 3; i++)
	{
		mExportQuantityBackground[i]->setSize(57, 40);		
	}

	mExportQuantityText[0] = GUIText::create(sf::FloatRect(mExportQuantityBackground[0]->getLocalX() + mExportQuantityBackground[0]->getWidth()/2, 102/*62*/, 57, 40), "0", mExportWindow);
	mExportQuantityText[1] = GUIText::create(sf::FloatRect(mExportQuantityBackground[1]->getLocalX() + mExportQuantityBackground[1]->getWidth()/2, 157/*117*/, 57, 40), "0", mExportWindow);
	mExportQuantityText[2] = GUIText::create(sf::FloatRect(mExportQuantityBackground[2]->getLocalX() + mExportQuantityBackground[2]->getWidth()/2, 216/*176*/, 57, 40), "0", mExportWindow);
	for(int i = 0; i < 3; i++)
	{
		mExportQuantityText[i]->setScale(0.7, 0.7);
		mExportQuantityText[i]->setAlignment("middle");
	}

	mExportFoodCost						= GUIEditField::create(sf::FloatRect(280, 97/*67*/, 139, 35), GUIEditField::CAP, "0", true, mExportWindow);
	mExportGoodsCost					= GUIEditField::create(sf::FloatRect(280, 152/*112*/, 139, 35), GUIEditField::CAP, "0", true, mExportWindow);
	mExportTechCost						= GUIEditField::create(sf::FloatRect(280, 211/*171*/, 139, 35), GUIEditField::CAP, "0", true, mExportWindow);

	mExportFoodCost->setMaxCharacters(4);
	mExportGoodsCost->setMaxCharacters(4);
	mExportTechCost->setMaxCharacters(4);

	mExportFoodCost->setOnGuiChangeFunction([=]()
	{
		int _foodCost = stringToInt(mExportFoodCost->getText());
		int _foodQuantity = stringToInt(mExportQuantityText[0]->getText());
		mExportPriceText[0]->setText(_foodCost * _foodQuantity);
		int totalCost = 0;
		for(int i = 0; i < 3; i++)
		{
			totalCost += stringToInt(mExportPriceText[i]->getText());
		}
		mExportTotalPriceValue->setText(totalCost);
	});
	mExportGoodsCost->setOnGuiChangeFunction([=]()
	{
		int _goodsCost = stringToInt(mExportGoodsCost->getText());
		int _goodsQuantity = stringToInt(mExportQuantityText[1]->getText());
		mExportPriceText[1]->setText(_goodsCost * _goodsQuantity);
		int totalCost = 0;
		for(int i = 0; i < 3; i++)
		{
			totalCost += stringToInt(mExportPriceText[i]->getText());
		}
		mExportTotalPriceValue->setText(totalCost);
	});
	mExportTechCost->setOnGuiChangeFunction([=]()
	{
		int _techCost = stringToInt(mExportTechCost->getText());
		int _techQuantity = stringToInt(mExportQuantityText[2]->getText());
		mExportPriceText[2]->setText(_techCost * _techQuantity);
		int totalCost = 0;
		for(int i = 0; i < 3; i++)
		{
			totalCost += stringToInt(mExportPriceText[i]->getText());
		}
		mExportTotalPriceValue->setText(totalCost);
	});

	mExportPriceText[0]			= GUIText::create(sf::FloatRect(502, 93/*53*/, 100, 31), "0", mExportWindow);
	mExportPriceText[1]			= GUIText::create(sf::FloatRect(502, 151/*111*/, 100, 31), "0", mExportWindow);
	mExportPriceText[2]			= GUIText::create(sf::FloatRect(502, 210/*170*/, 100, 31), "0", mExportWindow);
	for(int i = 0; i < 3; i++)
	{
		mExportPriceText[i]->setScale(0.7, 0.7);
		mExportPriceText[i]->setAlignment("middle");
	}

	mExportConfirmButton				= GUIButton::create(CapitalistButtons["ExportConfirm"], mExportWindow);
	mExportConfirmButton->setSize(CapitalistButtons["ExportConfirm"].first.width, CapitalistButtons["ExportConfirm"].first.height);
	mExportWindow->setVisible(false);

	mExportedWithoutPriceWindow			= GUIWindow::create(CapitalistWindows["ExportedWithoutPrice"], mCapitalistMainWindow);
	mExportedWithoutPriceText			= GUIText::create(sf::FloatRect(285, 60, 0, 0), "You must set a price for your export!", mExportedWithoutPriceWindow);
	mExportedWithoutPriceText->setAlignment("middle");
	mExportedWithoutPriceWindow->setVisible(false);
	
	mImportWindow						= GUIWindow::create(CapitalistWindows["CapitalistImportWindow"], mCapitalistMainWindow);
	mImportHeadliner					= GUIText::create(sf::FloatRect(285, 9, 0, 0), "Import From " + Menu::getInstance()->getEditField("CommunistNameField")->getText(), mImportWindow);
	mImportHeadliner->setAlignment("middle");
	mImportResourceLabel				= GUIText::create(sf::FloatRect(32, 52, 200, 100), "Res.", mImportWindow);
	mImportResourceLabel->setScale(0.7, 0.7);
	mImportPriceLabel					= GUIText::create(sf::FloatRect(164, 52, 200, 100), "Price", mImportWindow);
	mImportPriceLabel->setScale(0.7, 0.7);
	mImportPriceLabel->setAlignment("middle");
	mImportQuantityLabel				= GUIText::create(sf::FloatRect(348, 52, 200, 100), "Quantity", mImportWindow);
	mImportQuantityLabel->setScale(0.7, 0.7);
	mImportQuantityLabel->setAlignment("middle");
	mImportCostLabel					= GUIText::create(sf::FloatRect(502, 52, 200, 100), "Cost", mImportWindow);
	mImportCostLabel->setScale(0.7, 0.7);
	mImportCostLabel->setAlignment("middle");
	mImportTotalCostLabel				= GUIText::create(sf::FloatRect(392, 271, 200, 100), "Total cost:", mImportWindow);
	mImportTotalCostLabel->setScale(0.7, 0.7);
	mImportTotalCostLabel->setAlignment("middle");
	mImportTotalCostText				= GUIText::create(sf::FloatRect(502, 271, 100, 31), "0", mImportWindow);
	mImportTotalCostText->setScale(0.7, 0.7);
	mImportTotalCostText->setAlignment("middle");
	
	mImportResourcesAvailableText[0]	= GUIText::create(sf::FloatRect(80, 98, 56, 31), "0", mImportWindow);
	mImportResourcesAvailableText[1]	= GUIText::create(sf::FloatRect(80, 157, 56, 31), "0", mImportWindow);
	mImportResourcesAvailableText[2]	= GUIText::create(sf::FloatRect(80, 217, 56, 31), "0", mImportWindow);
	for(int i = 0; i < 3; i++)
	{
		mImportResourcesAvailableText[i]->setScale(0.7, 0.7);
	}

	mImportPriceText[0]					= GUIText::create(sf::FloatRect(168, 98, 56, 31), "N/A", mImportWindow);
	mImportPriceText[1]					= GUIText::create(sf::FloatRect(168, 157, 56, 31), "N/A", mImportWindow);
	mImportPriceText[2]					= GUIText::create(sf::FloatRect(168, 217, 56, 31), "N/A", mImportWindow);
	for(int i = 0; i < 3; i++)
	{
		mImportPriceText[i]->setScale(0.7, 0.7);
		mImportPriceText[i]->setAlignment("middle");
	}

	mImportBuyQuantityBackground[0]		= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(320, 93, 57, 40), buyField), mImportWindow);
	mImportBuyQuantityBackground[1]		= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(320, 151, 57, 40), buyField), mImportWindow);
	mImportBuyQuantityBackground[2]		= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(320, 210, 57, 40), buyField), mImportWindow);
	for(int i = 0; i < sizeof(mImportBuyQuantityBackground)/sizeof(mImportBuyQuantityBackground[0]); i++)
	{
		mImportBuyQuantityBackground[i]->setSize(57, 40);
	}
	mImportBuyQuantityText[0] = GUIText::create(sf::FloatRect(mImportBuyQuantityBackground[0]->getLocalX() + mImportBuyQuantityBackground[0]->getWidth()/2, 98, 57, 40), "0", mImportWindow);
	mImportBuyQuantityText[1] = GUIText::create(sf::FloatRect(mImportBuyQuantityBackground[1]->getLocalX() + mImportBuyQuantityBackground[1]->getWidth()/2, 157, 57, 40), "0", mImportWindow);
	mImportBuyQuantityText[2] = GUIText::create(sf::FloatRect(mImportBuyQuantityBackground[2]->getLocalX() + mImportBuyQuantityBackground[2]->getWidth()/2, 217, 57, 40), "0", mImportWindow);
	for(int i = 0; i < 3; i++)
	{
		mImportBuyQuantityText[i]->setScale(0.7, 0.7);
		mImportBuyQuantityText[i]->setAlignment("middle");
	}

	mImportTotalPriceText[0]					= GUIText::create(sf::FloatRect(507, 98, 100, 50), "0", mImportWindow);
	mImportTotalPriceText[1]					= GUIText::create(sf::FloatRect(507, 157, 100, 50), "0", mImportWindow);
	mImportTotalPriceText[2]					= GUIText::create(sf::FloatRect(507, 217, 100, 50), "0", mImportWindow);
	for(int i = 0; i < 3; i++)
	{
		mImportTotalPriceText[i]->setScale(0.7, 0.7);
		mImportTotalPriceText[i]->setAlignment("middle");
	}

	mImportGotoExportButton				= GUIButton::create(CapitalistButtons["ImportGotoExport"], mImportWindow);
	mImportWindow->setVisible(false);

	mExportedResourcesWindow			= GUIWindow::create(CapitalistWindows["ExportedResourcesIncome"], mCapitalistMainWindow);
	mExportIncomeHeadliner				= GUIText::create(sf::FloatRect(285, 9, 0, 0), "International Export", mExportedResourcesWindow);
	mExportIncomeHeadliner->setAlignment("middle");
	mCloseExportedResourceWindow		= GUIButton::create(CapitalistButtons["CloseExportedResourcesIncome"], mExportedResourcesWindow);

	int y = 100/*20*/;
	mResourcesExportedText[0]			= GUIText::create(sf::FloatRect(50, y, 0, 0), "0", mExportedResourcesWindow);
	mResourcesExportedText[0]->setScale(0.8, 0.8);
	y += mResourcesExportedText[0]->getHeight() + 5;
	mResourcesExportedText[1]			= GUIText::create(sf::FloatRect(50, y, 0, 0), "0", mExportedResourcesWindow);
	mResourcesExportedText[1]->setScale(0.8, 0.8);
	y += mResourcesExportedText[1]->getHeight() + 5;
	mResourcesExportedText[2]			= GUIText::create(sf::FloatRect(50, y, 0, 0), "0", mExportedResourcesWindow);
	mResourcesExportedText[2]->setScale(0.8, 0.8);
	y += mResourcesExportedText[2]->getHeight() + 5;
	
	mExportedIncomeText					= GUIText::create(sf::FloatRect(mResourcesExportedText[2]->getWidth() + 37, y, 0, 0), "0", mExportedResourcesWindow);
	mExportedIncomeText->setScale(0.5, 0.5);
	
	mExportedResourcesWindow->setVisible(false);

	sf::FloatRect firstPresRect			= CapitalistButtons["FirstPresident"].first;
	sf::FloatRect secondPresRect		= CapitalistButtons["SecondPresident"].first;
	sf::FloatRect pickedPresRect		= CapitalistButtons["PickedPresident"].first;

	mChoosePresidentWindow				= GUIWindow::create(CapitalistWindows["ChoosePresident"], mCapitalistMainWindow);
	mPickedPresidentWindow				= GUIWindow::create(CapitalistWindows["PickedPresident"], mCapitalistMainWindow);
	mFirstPresidentButton				= GUIImage::create(CapitalistButtons["FirstPresident"], mChoosePresidentWindow);
	mFirstPresidentPlaque				= GUIButton::create(std::pair<sf::FloatRect, sf::Texture*>
		(sf::FloatRect(firstPresRect.left, firstPresRect.top + firstPresRect.height - 5, 180, 65),
		&GameManager::getInstance()->getPresidentPlaque(mFirstPresident)), mChoosePresidentWindow);

	mSecondPresidentButton				= GUIImage::create(CapitalistButtons["SecondPresident"], mChoosePresidentWindow);
	mSecondPresidentPlaque				= GUIButton::create(std::pair<sf::FloatRect, sf::Texture*>
		(sf::FloatRect(secondPresRect.left, secondPresRect.top + secondPresRect.height - 5, 180, 65),
		&GameManager::getInstance()->getPresidentPlaque(mSecondPresident)), mChoosePresidentWindow);

	mPickedPresidentButton				= GUIImage::create(CapitalistButtons["PickedPresident"], mPickedPresidentWindow);
	mPickedPresidentPlaque				= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>
		(sf::FloatRect(pickedPresRect.left, pickedPresRect.top + pickedPresRect.height - 5, 0, 0),
		&GameManager::getInstance()->getPresidentPlaque(mPresident)), mPickedPresidentWindow);

	mClosePresidentWindow				= GUIButton::create(CapitalistButtons["ClosePresident"], mChoosePresidentWindow);
	mClosePickedPresidentWindow			= GUIButton::create(CapitalistButtons["ClosePickedPresident"], mPickedPresidentWindow);
	mPickedPresidentWindow->setVisible(false);
	mChoosePresidentWindow->setVisible(false);
	
	//används för att i början av varje kapitalistrunda visa om någon av resurserna har ökat i pris
	mIncreasedResourcesWindow			= GUIWindow::create(CapitalistWindows["IncreasedResources"], mCapitalistMainWindow);	
	mCloseIncreasedResourcesWindow		= GUIButton::create(CapitalistButtons["CloseIncreasedResources"], mIncreasedResourcesWindow);
	mIncreasedResourcesWindow->setVisible(false);

	std::shared_ptr<GUIWindow> statsWindow = GameManager::getInstance()->getStatsWindow();

	int statsPosY = 177;

	mCapitalistHeadLine					= GUIText::create(sf::FloatRect(253, 130, 0, 0), Menu::getInstance()->getEditField("CapitalistNameField")->getText()/*"CAPITALIST"*/, statsWindow);
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
	statsPosY += mTaxChange->getHeight();

	mPatriotismChange					= GUIText::create(sf::FloatRect(80, statsPosY, 0, 0), "0", statsWindow);
	mPatriotismChange->setScale(0.5, 0.5);
	statsPosY += mPatriotismChange->getHeight();

	mNewPatriotism						= GUIText::create(sf::FloatRect(80, statsPosY, 0, 0), "", statsWindow);
	mNewPatriotism->setScale(0.5, 0.5);

	mPresidentBiography					= GUIText::create(sf::FloatRect(40, 330, 0, 0), "", mPickedPresidentWindow);
	mPresidentBiography->setScale(0.6, 0.6);
	mPresidentPositiveText[0]			= GUIText::create(sf::FloatRect(0, 0, 0, 0), "", mPickedPresidentWindow);
	mPresidentPositiveText[1]			= GUIText::create(sf::FloatRect(0, 0, 0, 0), "", mPickedPresidentWindow);
	mPresidentNegativeText				= GUIText::create(sf::FloatRect(0, 0, 0, 0), "", mPickedPresidentWindow);
	mFirstPositiveStat[0]				= GUIText::create(sf::FloatRect(65, 330, 0, 0), "", mChoosePresidentWindow);
	mFirstPositiveStat[0]->setScale(0.6, 0.6);
	mSecondPositiveStat[0]				= GUIText::create(sf::FloatRect(65, 380, 0, 0), "", mChoosePresidentWindow);
	mSecondPositiveStat[0]->setScale(0.6, 0.6);
	mFirstNegativeStat					= GUIText::create(sf::FloatRect(65, 430, 0, 0), "", mChoosePresidentWindow);
	mFirstNegativeStat->setScale(0.6, 0.6);
	mFirstPositiveStat[1]				= GUIText::create(sf::FloatRect(329, 330, 0, 0), "", mChoosePresidentWindow);
	mFirstPositiveStat[1]->setScale(0.6, 0.6);
	mSecondPositiveStat[1]				= GUIText::create(sf::FloatRect(329, 380, 0, 0), "", mChoosePresidentWindow);
	mSecondPositiveStat[1]->setScale(0.6, 0.6);
	mSecondNegativeStat					= GUIText::create(sf::FloatRect(329, 430, 0, 0), "", mChoosePresidentWindow);
	mSecondNegativeStat->setScale(0.6, 0.6);


	mTaxesIncomeWindow					= GUIWindow::create(CapitalistWindows["TaxesIncome"], mCapitalistMainWindow);
	mTaxesIncomeHeadLiner				= GUIText::create(sf::FloatRect(285, 9, 0, 0), "Tax Income", mTaxesIncomeWindow);
	mTaxesIncomeHeadLiner->setAlignment("middle");
	mCurrentPopulationText[0]			= GUIText::create(sf::FloatRect(50, 107, 0, 0), "Population ", mTaxesIncomeWindow);
	mCurrentPopulationText[0]->setScale(0.8, 0.8);
	mCurrentPopulationText[0]->setAlignment("left");
	mCurrentPopulationText[1]			= GUIText::create(sf::FloatRect(331, 107, 0, 0), intToString(getPopulation()) + " million", mTaxesIncomeWindow);
	mCurrentPopulationText[1]->setScale(0.8, 0.8);
	mCurrentPopulationText[1]->setAlignment("left");

    mCurrentTaxesText[0]		        = GUIText::create(sf::FloatRect(50, 137, 0, 0), "Current tax ", mTaxesIncomeWindow);
	mCurrentTaxesText[0]->setScale(0.8, 0.8);
	mCurrentTaxesText[0]->setAlignment("left");
	mCurrentTaxesText[1]		        = GUIText::create(sf::FloatRect(331, 137, 0, 0), intToString(getTaxes()), mTaxesIncomeWindow);
	mCurrentTaxesText[1]->setScale(0.8, 0.8);
	mCurrentTaxesText[1]->setAlignment("left");

    mTaxesIncomeText[0]					= GUIText::create(sf::FloatRect(50, 167, 0, 0), "Tax income " , mTaxesIncomeWindow);
	mTaxesIncomeText[0]->setScale(0.8, 0.8);
	mTaxesIncomeText[0]->setAlignment("left");
	mTaxesIncomeText[1]					= GUIText::create(sf::FloatRect(331, 167, 0, 0), intToString(mTaxesPreviousRound*mPopulationPreviousRound) + "§", mTaxesIncomeWindow);
	mTaxesIncomeText[1]->setScale(0.8, 0.8);
	mTaxesIncomeText[1]->setAlignment("left");

	mCloseTaxesIncomeWindow				= GUIButton::create(CapitalistButtons["CloseTaxesIncome"], mTaxesIncomeWindow);
	mTaxesIncomeWindow->setVisible(false);

	mIncreasedResourcesPriceWindow		= GUIWindow::create(CapitalistWindows["IncreasedResources"], mCapitalistMainWindow);
	mIncreasedResourcesPriceHeadliner	= GUIText::create(sf::FloatRect(285, 9, 0, 0), "Supply & Demand", mIncreasedResourcesPriceWindow);
	mIncreasedResourcesPriceHeadliner->setAlignment("middle");
	mIncreasedResourcesText				= GUIText::create(sf::FloatRect(50, 50, 0, 0), "", mIncreasedResourcesPriceWindow);
	mIncreasedResourcesText->setScale(0.8, 0.8);
	mIncreasedResourcesText->setAlignment("left");
	mCloseIncreasedResourcesPriceWindow	= GUIButton::create(CapitalistButtons["CloseIncreasedResources"], mIncreasedResourcesPriceWindow);
	mIncreasedResourcesPriceWindow->setVisible(false);
	
	mPopulationEatsFoodWindow			= GUIWindow::create(CapitalistWindows["PopulationEatsFood"], mCapitalistMainWindow);
	mPopulationEatsFoodHeadliner		= GUIText::create(sf::FloatRect(285, 9, 0, 0), "Population Report " + intToString(GameManager::getInstance()->getYear()), mPopulationEatsFoodWindow);
	mPopulationEatsFoodHeadliner->setAlignment("middle");
	mPopulationEatsFoodText				= GUIText::create(sf::FloatRect(50, 100, 0, 0), "",  mPopulationEatsFoodWindow);
	mPopulationEatsFoodText->setScale(0.8, 0.8);
	mPopulationEatsFoodText->setAlignment("left");
	mClosePopulationEatsFoodWindow		= GUIButton::create(CapitalistButtons["ClosePopulationEatsFood"], mPopulationEatsFoodWindow);
	mDoIncreasePopulation				= GUIButton::create(CapitalistButtons["IncreasePopulation"], mPopulationEatsFoodWindow);
	mDoNotIncreasePopulation			= GUIButton::create(CapitalistButtons["DoNotIncreasePopulation"], mPopulationEatsFoodWindow);
	mPopulationEatsFoodWindow->setVisible(false);

	mFoodImage[0]	= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(23, 60, 119, 44), &ResourceHandler::getInstance()->getTexture(std::string("Capitalist/res_food_icon"))), mResourceWindow);
	mFoodImage[1]	= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(31, 93/*53*/, 35, 35), &ResourceHandler::getInstance()->getTexture(std::string("Capitalist/food_image"))), mImportWindow);
	mFoodImage[2]	= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(31, 93/*53*/, 35, 35), &ResourceHandler::getInstance()->getTexture(std::string("Capitalist/food_image"))), mExportWindow);

	mGoodsImage[0]	= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(203, 60, 119, 44), &ResourceHandler::getInstance()->getTexture(std::string("Capitalist/res_goods_icon"))), mResourceWindow);
	mGoodsImage[1]	= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(31, 151/*111*/, 35, 35), &ResourceHandler::getInstance()->getTexture(std::string("Capitalist/goods_image"))), mImportWindow);
	mGoodsImage[2]	= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(31, 151/*111*/, 35, 35), &ResourceHandler::getInstance()->getTexture(std::string("Capitalist/goods_image"))), mExportWindow);

	mTechImage[0]	= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(383, 60, 119, 44), &ResourceHandler::getInstance()->getTexture(std::string("Capitalist/res_tech_icon"))), mResourceWindow);
	mTechImage[1]	= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(31, 210/*170*/, 35, 35), &ResourceHandler::getInstance()->getTexture(std::string("Capitalist/tech_image"))), mImportWindow);
	mTechImage[2]	= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(31, 210/*170*/, 35, 35), &ResourceHandler::getInstance()->getTexture(std::string("Capitalist/tech_image"))), mExportWindow);
	
	mWindowHeadlines[0] = GUIText::create(sf::FloatRect(285, 9, 0, 0), intToString(GameManager::getInstance()->getYear()) + " Presidential Elections ", mChoosePresidentWindow);
	mWindowHeadlines[0]->setAlignment("middle");
	mWindowHeadlines[1] = GUIText::create(sf::FloatRect(285, 9, 0, 0), "President of " + Menu::getInstance()->getEditField("CapitalistNameField")->getText(), mPickedPresidentWindow);
	mWindowHeadlines[1]->setAlignment("middle");

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
	CityImages.push_back(&ResourceHandler::getInstance()->getTexture(std::string("Capitalist/kap1")));
	CityImages.push_back(&ResourceHandler::getInstance()->getTexture(std::string("Capitalist/kap2")));
	CityImages.push_back(&ResourceHandler::getInstance()->getTexture(std::string("Capitalist/kap3")));
	CityImages.push_back(&ResourceHandler::getInstance()->getTexture(std::string("Capitalist/kap4")));
	CityImages.push_back(&ResourceHandler::getInstance()->getTexture(std::string("Capitalist/kap5")));
	//mChangeCityImage->setTexture(std::pair<sf::FloatRect, sf::Texture*>(mChangeCityImage->getRectangle(), CityImages[0])); 
}

void Capitalist::sendPresidentDataToOtherPlayer()
{
	sf::Packet packet;
	packet<<mFirstPresident->getName()<<mFirstPresident->getFirstPositiveStat()<<mFirstPresident->getSecondPositiveStat()<<mFirstPresident->getNegativeStat()
		<<mFirstPresident->getFoodPriceModifier()<<mFirstPresident->getGoodsPriceModifier()<<mFirstPresident->getTechPriceModifier()
		<<mFirstPresident->getNuclearPriceModifier()<<mFirstPresident->getSpacePriceModifier()<<mFirstPresident->getSpyPriceModifier()
		<<mFirstPresident->getPatriotismTaxModifier()<<mFirstPresident->getPopEatsMore();

	packet<<mSecondPresident->getName()<<mSecondPresident->getFirstPositiveStat()<<mSecondPresident->getSecondPositiveStat()<<mSecondPresident->getNegativeStat()
		<<mSecondPresident->getFoodPriceModifier()<<mSecondPresident->getGoodsPriceModifier()<<mSecondPresident->getTechPriceModifier()
		<<mSecondPresident->getNuclearPriceModifier()<<mSecondPresident->getSpacePriceModifier()<<mSecondPresident->getSpyPriceModifier()
		<<mSecondPresident->getPatriotismTaxModifier()<<mSecondPresident->getPopEatsMore();
	GameManager::getInstance()->triggerOtherPlayersEvent("syncRandomPresident", packet);
}

void Capitalist::chooseLeader()
{
	if(GameManager::getInstance()->getGameType() == LAN && GameManager::getInstance()->getRemoteClient()->getSuperPower() == COMMUNIST)
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

		if(GameManager::getInstance()->getRemoteClient()->isReady())
			sendPresidentDataToOtherPlayer();
	}
	else if(GameManager::getInstance()->getGameType() == VERSUS)
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
}

void Capitalist::LANChooseLeader(std::shared_ptr<President> firstPresident, std::shared_ptr<President> secondPresident)
{
	if(mPresident == NULL)
	{
		mFirstPresident = firstPresident;
		GameManager::getInstance()->removePresidentFromList(mFirstPresident);
	}
	else
	{
		if(mPresident->getYearsElected() < 2)
			mFirstPresident = mPresident;
		else
		{
			mFirstPresident = firstPresident;
			GameManager::getInstance()->removePresidentFromList(mFirstPresident);
		}
	}
	mSecondPresident = secondPresident;
	GameManager::getInstance()->removePresidentFromList(mSecondPresident);

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

	mCapitalistMainWindow->setEnabled(false, true);
	mChoosePresidentWindow->setEnabled(true, true);
	mChoosePresidentWindow->setVisible(true);
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

		float x = mTaxesWindow->getX() + mTaxesWindow->getRectangle().width/2;
		float y = mTaxesWindow->getY() + mTaxesWindow->getRectangle().height/2;
		GUIAnimation::move(mTaxesWindow, 100, sf::FloatRect(x, y, 0, 0), mTaxesWindow->getRectangle());
		for(std::vector<std::shared_ptr<GUIElement> >::size_type i = 0; i < mTaxesWindow->getChildVector().size(); ++i)
		{
			GUIAnimation::move(mTaxesWindow->getChildVector()[i], 100, sf::FloatRect(x, y, 0, 0), mTaxesWindow->getChildVector()[i]->getRectangle());
		}
	});

	/*Resources GUI-Window knappar*/
	mCapitalistResourceButton->setOnClickFunction([=]()			
	{ 
		mCapitalistMainWindow->setEnabled(false, true);
		mResourceWindow->setEnabled(true, true);
		mResourceWindow->setVisible(true); 
		mCapitalistResourceButton->setTexture(CapitalistButtons["ResourceIsPressed"]);
		float x = mResourceWindow->getX() + mResourceWindow->getRectangle().width/2;
		float y = mResourceWindow->getY() + mResourceWindow->getRectangle().height/2;
		GUIAnimation::move(mResourceWindow, 100, sf::FloatRect(x, y, 0, 0), mResourceWindow->getRectangle());
		for(std::vector<std::shared_ptr<GUIElement> >::size_type i = 0; i < mResourceWindow->getChildVector().size(); ++i)
		{
			GUIAnimation::move(mResourceWindow->getChildVector()[i], 100, sf::FloatRect(x, y, 0, 0), mResourceWindow->getChildVector()[i]->getRectangle());
		}
	});

	mLowerTaxesButton->setOnClickFunction([=]()
	{
		int newTax = stringToInt(mTaxValueText->getText()) - 5;
		if(newTax < mTaxesPreviousRound - 5)
		{
			newTax = mTaxesPreviousRound - 5;
		}

		if(newTax < mTaxesPreviousRound)
			mTaxesPatriotismChange->setText("Patriotism: +" + intToString(2 + taxPatriotismChange));
		else
			mTaxesPatriotismChange->setText("Patriotism: 0");
		if(newTax < 0)
			newTax = 0;
		mTaxValueText->setText(newTax);
	});

	mRaiseTaxesButton->setOnClickFunction([=]()
	{
		int newTax = stringToInt(mTaxValueText->getText()) + 5;
		if(newTax > mTaxesPreviousRound + 5)
		{
			newTax = mTaxesPreviousRound + 5;
		}
		
		if(newTax > mTaxesPreviousRound)
			mTaxesPatriotismChange->setText("Patriotism: -3");
		else
			mTaxesPatriotismChange->setText("Patriotism: 0");
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
		mFoodCost->setText("Cost: " + intToString(foodCost*foodAmount) + " §");
		mTotalResourcesCost->setText("Total cost: " + intToString(totalCost) + " §");
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
		mFoodCost->setText("Cost: " + intToString(foodCost*foodAmount) + " §");
		mTotalResourcesCost->setText("Total cost: " + intToString(totalCost) + " §");;
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
		mFoodCost->setText("Cost: " + intToString(foodCost*foodAmount) + " §");
		mTotalResourcesCost->setText("Total cost: " + intToString(totalCost) + " §");;
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
		mFoodCost->setText("Cost: " + intToString(foodCost*foodAmount) + " §");
		mTotalResourcesCost->setText("Total cost: " + intToString(totalCost) + " §");;
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
		mFoodCost->setText("Cost: " + intToString(foodCost*foodAmount) + " §");
		mTotalResourcesCost->setText("Total cost: " + intToString(totalCost) + " §");;
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
		mFoodCost->setText("Cost: " + intToString(foodCost*foodAmount) + " §");
		mTotalResourcesCost->setText("Total cost: " + intToString(totalCost) + " §");;
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
		mGoodsCost->setText("Cost: " + intToString(goodsCost*goodsAmount) + " §");
		mTotalResourcesCost->setText("Total cost: " + intToString(totalCost) + " §");;
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
		mGoodsCost->setText("Cost: " + intToString(goodsCost*goodsAmount) + " §");
		mTotalResourcesCost->setText("Total cost: " + intToString(totalCost) + " §");;
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
		mGoodsCost->setText("Cost: " + intToString(goodsCost*goodsAmount) + " §");
		mTotalResourcesCost->setText("Total cost: " + intToString(totalCost) + " §");;
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
		mGoodsCost->setText("Cost: " + intToString(goodsCost*goodsAmount) + " §");
		mTotalResourcesCost->setText("Total cost: " + intToString(totalCost) + " §");;
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
		mGoodsCost->setText("Cost: " + intToString(goodsCost*goodsAmount) + " §");
		mTotalResourcesCost->setText("Total cost: " + intToString(totalCost) + " §");;
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
		mGoodsCost->setText("Cost: " + intToString(goodsCost*goodsAmount) + " §");
		mTotalResourcesCost->setText("Total cost: " + intToString(totalCost) + " §");;
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
		mTechCost->setText("Cost: " + intToString(techCost*techAmount) + " §");
		mTotalResourcesCost->setText("Total cost: " + intToString(totalCost) + " §");;
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
		mTechCost->setText("Cost: " + intToString(techCost*techAmount) + " §");
		mTotalResourcesCost->setText("Total cost: " + intToString(totalCost) + " §");;
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
		mTechCost->setText("Cost: " + intToString(techCost*techAmount) + " §");
		mTotalResourcesCost->setText("Total cost: " + intToString(totalCost) + " §");;
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
		mTechCost->setText("Cost: " + intToString(techCost*techAmount) + " §");
		mTotalResourcesCost->setText("Total cost: " + intToString(totalCost) + " §");;
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
		mTechCost->setText("Cost: " + intToString(techCost*techAmount) + " §");
		mTotalResourcesCost->setText("Total cost: " + intToString(totalCost) + " §");;
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
		mTechCost->setText("Cost: " + intToString(techCost*techAmount) + " §");
		mTotalResourcesCost->setText("Total cost: " + intToString(totalCost) + " §");;
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

		float x = mUpgradeWindow->getX() + mUpgradeWindow->getRectangle().width/2;
		float y = mUpgradeWindow->getY() + mUpgradeWindow->getRectangle().height/2;
		GUIAnimation::move(mUpgradeWindow, 100, sf::FloatRect(x, y, 0, 0), mUpgradeWindow->getRectangle());
		for(std::vector<std::shared_ptr<GUIElement> >::size_type i = 0; i < mUpgradeWindow->getChildVector().size(); ++i)
		{
			
			GUIAnimation::move(mUpgradeWindow->getChildVector()[i], 100, sf::FloatRect(x, y, 0, 0), mUpgradeWindow->getChildVector()[i]->getRectangle());
		}

		upgradeWindowText();

	});

	mUpgradeNuclearWeaponButton->setOnClickFunction([=]() 
	{
		int nuclearGoodsPrice	= 10 * mPresident->getNuclearPriceModifier();
		int nuclearTechPrice	= 5  * mPresident->getNuclearPriceModifier();
		int amount = stringToInt(mBuyNuclearText->getText());
		
		if(mGoods >= nuclearGoodsPrice && mTech >= nuclearTechPrice)
		{
			if(mUpgradeSound != nullptr)
			{
				if(mUpgradeSound->getSound()->getStatus() == sf::Music::Stopped)
				{
					mUpgradeSound->setSound(getSoundEffect("Buttons/Nuclear"));
					mUpgradeSound->playSound();
				}
			}
			else
			{
				mUpgradeSound->setSound(getSoundEffect("Buttons/Nuclear"));
				mUpgradeSound->playSound();
			}
			++amount;
			mBuyNuclearText->setText(amount);
			upgradeWindowText();
			mGoods -= nuclearGoodsPrice;
			mTech  -= nuclearTechPrice;
		}
	});
	mCancelUpgradeNuclearWeaponButton->setOnClickFunction([=]() 
	{
		int nuclearGoodsPrice	= 10 * mPresident->getNuclearPriceModifier();
		int nuclearTechPrice	= 5  * mPresident->getNuclearPriceModifier();
		int difference = stringToInt(mBuyNuclearText->getText()) - stringToInt(mNuclearText->getText());
		mGoods += nuclearGoodsPrice * difference;
		mTech  += nuclearTechPrice * difference;
		mBuyNuclearText->setText(mNuclearText->getText());

		upgradeWindowText();
	});
			

	mUpgradeSpaceProgramButton->setOnClickFunction([=]()  
	{
		int spaceProgramGoodsPrice  = (stringToInt(mBuySpaceProgramText->getText()) + 1) * 5 * mPresident->getSpacePriceModifier();
		int spaceProgramTechPrice	= 0;
		if(GameManager::getInstance()->getCommunist()->getSpaceProgram() > mSpaceProgram)
			spaceProgramTechPrice	= (stringToInt(mBuySpaceProgramText->getText()) + 1) * 10 * mPresident->getSpacePriceModifier() - (5 * stringToInt(mBuySpyNetworkText->getText()));
		else
			spaceProgramTechPrice	= (stringToInt(mBuySpaceProgramText->getText()) + 1) * 10 * mPresident->getSpacePriceModifier();
		int amount = stringToInt(mBuySpaceProgramText->getText());
		if(mGoods >= spaceProgramGoodsPrice && mTech >= spaceProgramTechPrice)
		{
			if(mUpgradeSound->getSound() != nullptr)
			{
				if(mUpgradeSound->getSound()->getStatus() == sf::Music::Stopped)
				{
					mUpgradeSound->setSound(getSoundEffect("Buttons/Space"));
					mUpgradeSound->playSound();
				}
			}
			else
			{
				mUpgradeSound->setSound(getSoundEffect("Buttons/Space"));
				mUpgradeSound->playSound();
			}
			++amount;
			mBuySpaceProgramText->setText(amount);
			upgradeWindowText();
			mGoods -= spaceProgramGoodsPrice;
			mTech  -= spaceProgramTechPrice;
		}
	});
	mCancelUpgradeSpaceProgramButton->setOnClickFunction([=]() 
	{
		int spaceProgramGoodsPrice  = 5 * mPresident->getSpacePriceModifier();
		int spaceProgramTechPrice	= 0;
		if(GameManager::getInstance()->getCommunist()->getSpaceProgram() > mSpaceProgram)
			spaceProgramTechPrice	= 10 * mPresident->getSpacePriceModifier() - (5 * stringToInt(mBuySpyNetworkText->getText()));
		else
			spaceProgramTechPrice	= 10 * mPresident->getSpacePriceModifier();
		int difference = stringToInt(mBuySpaceProgramText->getText()) - stringToInt(mSpaceText->getText());
		for(int i = 0; i < difference; ++i)
		{
			mGoods += spaceProgramGoodsPrice * (i + 1);
			mTech  += spaceProgramTechPrice * (i + 1);
		}
		mBuySpaceProgramText->setText(mSpaceText->getText());
		upgradeWindowText();
	});

	mUpgradeSpyNetworkButton->setOnClickFunction([=]()	 
	{
		int spyNetworkTechPrice = (stringToInt(mBuySpyNetworkText->getText()) + 1) * 10 * mPresident->getSpyPriceModifier();
		int amount = stringToInt(mBuySpyNetworkText->getText());
		if(mTech >= spyNetworkTechPrice)
		{
			//if(getSoundEffect("Buttons/Spy")->getStatus() == sf::Music::Stopped)
				//playSoundEffect("Buttons/Spy");
			++amount;
			mTech -= spyNetworkTechPrice;
			mBuySpyNetworkText->setText(amount);
			upgradeWindowText();
		}
		
	});		
	mCancelUpgradeSpyNetworkButton->setOnClickFunction([=]() 
	{
		int spyNetworkTechPrice = 10 * mPresident->getSpyPriceModifier();
		int difference = stringToInt(mBuySpyNetworkText->getText()) - stringToInt(mSpyText->getText());
		for(int i = 0; i < difference; ++i)
		{
			mTech += spyNetworkTechPrice * (i + 1);
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

		float x = mImportWindow->getX() + mImportWindow->getRectangle().width/2;
		float y = mImportWindow->getY() + mImportWindow->getRectangle().height/2;
		GUIAnimation::move(mImportWindow, 100, sf::FloatRect(x, y, 0, 0), mImportWindow->getRectangle());
		for(std::vector<std::shared_ptr<GUIElement> >::size_type i = 0; i < mImportWindow->getChildVector().size(); ++i)
		{
			GUIAnimation::move(mImportWindow->getChildVector()[i], 100, sf::FloatRect(x, y, 0, 0), mImportWindow->getChildVector()[i]->getRectangle());
		}

	});
	
	sf::Texture *minusTexture[3];
	minusTexture[0] = &ResourceHandler::getInstance()->getTexture(std::string("Capitalist/kap_texture_button_minus"));
	minusTexture[1] = &ResourceHandler::getInstance()->getTexture(std::string("Capitalist/kap_texture_button_minus5"));
	minusTexture[2] = &ResourceHandler::getInstance()->getTexture(std::string("Capitalist/kap_texture_button_minus10"));

	sf::Texture *plusTexture[3];
	plusTexture[0] = &ResourceHandler::getInstance()->getTexture(std::string("Capitalist/kap_texture_button_plus"));
	plusTexture[1] = &ResourceHandler::getInstance()->getTexture(std::string("Capitalist/kap_texture_button_plus5"));
	plusTexture[2] = &ResourceHandler::getInstance()->getTexture(std::string("Capitalist/kap_texture_button_plus10"));
	
	for(int i = 0; i < sizeof(mImportBuyQuantityBackground)/sizeof(mImportBuyQuantityBackground[0]); i++)
	{
		float x = mImportBuyQuantityBackground[i]->getLocalX(), y = mImportBuyQuantityBackground[i]->getLocalY();
		for(int h = 0; h < 3; h++)
		{
			mImportBuyButtonMinus[i][h] = GUIButton::create(
				std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(x - (h+1)*minusTexture[h]->getSize().x, y, minusTexture[h]->getSize().x, minusTexture[h]->getSize().y),
				minusTexture[h]),
				mImportWindow);
			mImportBuyButtonMinus[i][h]->setOnClickFunction([=]()
			{
				int _resourcesAvailable = stringToInt(mImportResourcesAvailableText[i]->getText());
				int _resourcePrice = stringToInt(mImportPriceText[i]->getText());
				int quantity = stringToInt(mImportBuyQuantityText[i]->getText());
				quantity -= (h == 0) ? 1 : (h*5);
				if(quantity < 0)
					quantity = 0;
				mImportBuyQuantityText[i]->setText(quantity);

				int totalCost = 0;
				int _foodCost = stringToInt(mImportPriceText[0]->getText());
				int _foodQuantity = stringToInt(mImportBuyQuantityText[0]->getText());

				int _goodsCost = stringToInt(mImportPriceText[1]->getText());
				int _goodsQuantity = stringToInt(mImportBuyQuantityText[1]->getText());

				int _techCost = stringToInt(mImportPriceText[2]->getText());
				int _techQuantity = stringToInt(mImportBuyQuantityText[2]->getText());

				totalCost = _foodCost*_foodQuantity + _goodsCost*_goodsQuantity + _techCost*_techQuantity;

				mImportBuyQuantityText[i]->setText(intToString(quantity));
				int _resourceCost = quantity * _resourcePrice;
				mImportTotalPriceText[i]->setText(_resourceCost);

				mImportTotalCostText->setText(totalCost);
			});

			mImportBuyButtonPlus[i][h] = GUIButton::create(
				std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(x + mImportBuyQuantityBackground[i]->getWidth() + h*plusTexture[h]->getSize().x, y, plusTexture[h]->getSize().x, plusTexture[h]->getSize().y),
				plusTexture[h]),
				mImportWindow);
			mImportBuyButtonPlus[i][h]->setOnClickFunction([=]()
			{
				int _resourcesAvailable = stringToInt(mImportResourcesAvailableText[i]->getText());
				int _resourcePrice = stringToInt(mImportPriceText[i]->getText());
				int quantity = stringToInt(mImportBuyQuantityText[i]->getText());
				quantity += (h == 0) ? 1 : (h*5);
				if(quantity > _resourcesAvailable)
					quantity = _resourcesAvailable;
				mImportBuyQuantityText[i]->setText(quantity);

				int totalCost = 0;
				int _foodCost = stringToInt(mImportPriceText[0]->getText());
				int _foodQuantity = stringToInt(mImportBuyQuantityText[0]->getText());

				int _goodsCost = stringToInt(mImportPriceText[1]->getText());
				int _goodsQuantity = stringToInt(mImportBuyQuantityText[1]->getText());

				int _techCost = stringToInt(mImportPriceText[2]->getText());
				int _techQuantity = stringToInt(mImportBuyQuantityText[2]->getText());

				int _resourceQuantity[3];
				for(int c = 0; c < 3; c++)
				{
					_resourceQuantity[c] = stringToInt(mImportBuyQuantityText[c]->getText());
				}

				totalCost = _foodCost*_foodQuantity + _goodsCost*_goodsQuantity + _techCost*_techQuantity;
				int moneyDifference = mCurrency - totalCost;
				if(moneyDifference < 0)
				{
					_resourceQuantity[i] -= std::ceilf((float)abs(moneyDifference)/(float)_resourcePrice);
					totalCost = _foodCost*_resourceQuantity[0] + _goodsCost*_resourceQuantity[1] + _techCost*_resourceQuantity[2];
				}
				quantity = _resourceQuantity[i];

				mImportBuyQuantityText[i]->setText(intToString(quantity));
				int _resourceCost = quantity * _resourcePrice;
				mImportTotalPriceText[i]->setText(_resourceCost);

				mImportTotalCostText->setText(totalCost);
			});
		}
	}

	mImportGotoExportButton->setOnClickFunction(
		[=]()
	{
		mImportWindow->setVisible(false);
		mExportWindow->setVisible(true);
		mExportWindow->setEnabled(true, true);
		int _importedFoodQuantity = stringToInt(mImportBuyQuantityText[0]->getText());
		int _importedGoodsQuantity = stringToInt(mImportBuyQuantityText[1]->getText());
		int _importedTechQuantity = stringToInt(mImportBuyQuantityText[2]->getText());
		mFood += _importedFoodQuantity;
		mGoods += _importedGoodsQuantity;
		mTech += _importedTechQuantity;

		std::shared_ptr<SuperPower> enemy = GameManager::getInstance()->getCommunist();
		enemy->setExportedFood(enemy->getExportedFood() - _importedFoodQuantity);
		enemy->setExportedGoods(enemy->getExportedGoods() - _importedGoodsQuantity);
		enemy->setExportedTech(enemy->getExportedTech() - _importedTechQuantity);

		enemy->setExportedFoodSold(enemy->getExportedFoodSold() + _importedFoodQuantity);
		enemy->setExportedGoodsSold(enemy->getExportedGoodsSold() + _importedGoodsQuantity);
		enemy->setExportedTechSold(enemy->getExportedTechSold() + _importedTechQuantity);

		enemy->setCurrency(enemy->getCurrency() + _importedFoodQuantity*enemy->getExportedFoodPrice() + _importedGoodsQuantity*enemy->getExportedGoodsPrice() + _importedTechQuantity*enemy->getExportedTechPrice());

		mCurrency -= stringToInt(mImportTotalPriceText[0]->getText());
		mCurrency -= stringToInt(mImportTotalPriceText[1]->getText());
		mCurrency -= stringToInt(mImportTotalPriceText[2]->getText());

		mImportResourcesAvailableText[0]->setText(enemy->getExportedFood());
		mImportResourcesAvailableText[1]->setText(enemy->getExportedGoods());
		mImportResourcesAvailableText[2]->setText(enemy->getExportedTech());

		for(int i = 0; i < 3; i++)
		{
			mImportBuyQuantityText[i]->setText("0");
			mImportTotalPriceText[i]->setText("0");
			if(stringToInt(mImportResourcesAvailableText[i]->getText()) == 0)
				mImportPriceText[i]->setText("N/A");
		}
		mImportTotalCostText->setText("0");
	});

	std::vector<int*> resourcesAvailable;
	resourcesAvailable.push_back(&mFood);
	resourcesAvailable.push_back(&mGoods);
	resourcesAvailable.push_back(&mTech);

	std::vector<int*> exportValues;
	exportValues.push_back(&mExportedFood);
	exportValues.push_back(&mExportedGoods);
	exportValues.push_back(&mExportedTech);

	std::vector<std::shared_ptr<GUIEditField>> exportResourceCost;
	exportResourceCost.push_back(mExportFoodCost);
	exportResourceCost.push_back(mExportGoodsCost);
	exportResourceCost.push_back(mExportTechCost);

	for(int i = 0; i < sizeof(mExportQuantityBackground)/sizeof(mExportQuantityBackground[0]); i++)
	{
		float x = mExportQuantityBackground[i]->getLocalX(), y = mExportQuantityBackground[i]->getLocalY();
		for(int h = 0; h < 3; h++)
		{
			mExportButtonMinus[i][h] = GUIButton::create(
				std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(x - (h+1)*minusTexture[h]->getSize().x, y, minusTexture[h]->getSize().x, minusTexture[h]->getSize().y),
				minusTexture[h]),
				mExportWindow);
			mExportButtonMinus[i][h]->setOnClickFunction([=]()
			{
				int quantity = stringToInt(mExportQuantityText[i]->getText());
				quantity -= (h == 0) ? 1 : (h*5);
				if(quantity < 0)
					quantity = 0;
				mExportQuantityText[i]->setText(intToString(quantity));
				int cost = quantity * stringToInt(exportResourceCost[i]->getText());
				mExportPriceText[i]->setText(cost);

				int totalCost = 0;
				for(int c = 0; c < 3; c++)
				{
					totalCost += stringToInt(mExportPriceText[c]->getText());
				}
				mExportTotalPriceValue->setText(totalCost);
			});

			mExportButtonPlus[i][h] = GUIButton::create(
				std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(x + mImportBuyQuantityBackground[i]->getWidth() + h*plusTexture[h]->getSize().x, y, plusTexture[h]->getSize().x, plusTexture[h]->getSize().y),
				plusTexture[h]),
				mExportWindow);
			mExportButtonPlus[i][h]->setOnClickFunction([=]()
			{
				int foodAvailable = mFood;
				int quantity = stringToInt(mExportQuantityText[i]->getText());
				quantity += (h == 0) ? 1 : (h*5);
				if(quantity > *resourcesAvailable[i] + *exportValues[i])
					quantity = *resourcesAvailable[i] + *exportValues[i];
				mExportQuantityText[i]->setText(intToString(quantity));
				int cost = quantity * stringToInt(exportResourceCost[i]->getText());
				mExportPriceText[i]->setText(cost);

				int totalCost = 0;
				for(int c = 0; c < 3; c++)
				{
					totalCost += stringToInt(mExportPriceText[c]->getText());
				}
				mExportTotalPriceValue->setText(totalCost);
			});
		}
	}

	mExportConfirmButton->setOnClickFunction([=]()
	{
		if(stringToInt(mExportQuantityText[0]->getText()) > 0 && stringToInt(mExportFoodCost->getText()) == 0 || stringToInt(mExportQuantityText[1]->getText()) > 0 && stringToInt(mExportGoodsCost->getText()) == 0 || stringToInt(mExportQuantityText[2]->getText()) > 0 && stringToInt(mExportTechCost->getText()) == 0)
		{
			mExportedWithoutPriceWindow->setColor(sf::Color(255, 255, 255, 255));
			mExportedWithoutPriceText->setColor(sf::Color(0, 0, 0, 255));
			mExportedWithoutPriceWindow->setVisible(true);
			mExportWindow->setEnabled(false, true);
			std::shared_ptr<GUIWindow> _window = mExportedWithoutPriceWindow;
			std::shared_ptr<GUIText> _text = mExportedWithoutPriceText;
			std::shared_ptr<GUIWindow> _exportWindow = mExportWindow;
			Timer::setTimer([=]()
			{
				std::shared_ptr<GUIWindow> _window2 = _window;
				std::shared_ptr<GUIWindow> _exportWindow2 = _exportWindow;
				GUIAnimation::fadeToColor(_window, 500, sf::Color(255, 255, 255, 255), sf::Color(255, 255, 255, 0));
				GUIAnimation::fadeToColor(_text, 500, sf::Color(0, 0, 0, 255), sf::Color(0, 0, 0, 0));
				Timer::setTimer([=]()
				{
					_window2->setVisible(false);
					_exportWindow2->setEnabled(true, true);
				}, 500, 1);
			}, 750, 1);
		}
		else
		{
			mExportWindow->setVisible(false);
			mCapitalistMainWindow->setEnabled(true, true);
			mCapitalistTradeButton->setTexture(CapitalistButtons["Export"]);
			int _exportedFoodPreviousValue = mExportedFood;
			int _exportedGoodsPreviousValue = mExportedGoods;
			int _exportedTechPreviousValue = mExportedTech;
			mExportedFood = stringToInt(mExportQuantityText[0]->getText());
			mExportedGoods = stringToInt(mExportQuantityText[1]->getText());
			mExportedTech = stringToInt(mExportQuantityText[2]->getText());

			int _exportedFoodDiff = mExportedFood - _exportedFoodPreviousValue;
			int _exportedGoodsDiff = mExportedGoods - _exportedGoodsPreviousValue;
			int _exportedTechDiff = mExportedTech - _exportedTechPreviousValue;

			mFood -= _exportedFoodDiff;
			mGoods -= _exportedGoodsDiff;
			mTech -= _exportedTechDiff;

			mExportedFoodPrice = stringToInt(mExportFoodCost->getText());
			mExportedGoodsPrice = stringToInt(mExportGoodsCost->getText());
			mExportedTechPrice = stringToInt(mExportTechCost->getText());
		}
	});

	/*Stänger ner Taxes fönstret*/
	mTaxesCloseButton->setOnClickFunction([&]()					
	{ 
		mCapitalistMainWindow->setEnabled(true, true);
		mTaxesWindow->setEnabled(false, true);
		mTaxesWindow->setVisible(false);
		mTaxes = stringToInt(mTaxValueText->getText());
		
		mCapitalistTaxesButton->setTexture(CapitalistButtons["Taxes"]);//ändrar textur till orginal

		/*std::shared_ptr<GUIWindow> _window = mTaxesWindow;
		sf::FloatRect rect = mTaxesWindow->getRectangle();
		float x = mTaxesWindow->getX() + mTaxesWindow->getRectangle().width/2;
		float y = mTaxesWindow->getY() + mTaxesWindow->getRectangle().height/2;
		GUIAnimation::move(mTaxesWindow, 100, mTaxesWindow->getRectangle(), sf::FloatRect(x, y, 0, 0));
		for(std::vector<std::shared_ptr<GUIElement> >::size_type i = 0; i < mTaxesWindow->getChildVector().size(); ++i)
		{
			std::shared_ptr<GUIElement> element = mTaxesWindow->getChildVector()[i];
			GUIAnimation::move(element, 100, element->getRectangle(), sf::FloatRect(x, y, 0, 0));
		}
		Timer::setTimer([=]()
		{ 
			_window->setVisible(false);
			_window->setRectangle(rect);
		}, 500, 1);*/
	});

	/*Stänger ner resources fönstret "Okay-knappen"*/
	mResourceCloseButton->setOnClickFunction([=]()				
	{ 
		int food = stringToInt(mFoodCost->getText());
		int goods = stringToInt(mGoodsCost->getText());
		int tech = stringToInt(mTechCost->getText());
		int foodBought = stringToInt(mBuyFoodText->getText()) + mFood;
		int goodsBought = stringToInt(mBuyGoodsText->getText()) + mGoods;
		int techBought  = stringToInt(mBuyTechText->getText()) + mTech;
		int totalCost  = (food + goods + tech);
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
	});

	/*Stänger ner upgrade fönstret "Okay-knappen"*/
	mUpgradeCloseButton->setOnClickFunction([=]()				
	{
		int nuclearDiff = stringToInt(mBuyNuclearText->getText()) - stringToInt(mNuclearText->getText()); 
		int spaceDiff = stringToInt(mBuySpaceProgramText->getText()) - stringToInt(mSpaceText->getText());
		int spyDiff = stringToInt(mBuySpyNetworkText->getText()) - stringToInt(mSpyText->getText());

		mCapitalistMainWindow->setEnabled(true, true);
		mUpgradeWindow->setEnabled(false, true);
	
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
		mPickedPresident = 1;

		mSecondPresidentButton->setTexture(std::pair<sf::FloatRect, sf::Texture*>(mSecondPresidentButton->getRectangle(), mSecondPresident->getTexture()));
		mFirstPresidentButton->setTexture(std::pair<sf::FloatRect, sf::Texture*>(mFirstPresidentButton->getRectangle(), mFirstPresident->getClickedTexture()));

		mPickedPresidentButton->setTexture(std::pair<sf::FloatRect, sf::Texture*>
			(mPickedPresidentButton->getRectangle(), mFirstPresident->getTexture()));

		mPickedPresidentPlaque->setTexture(std::pair<sf::FloatRect, sf::Texture*>
			(mPickedPresidentPlaque->getRectangle(), &GameManager::getInstance()->getPresidentPlaque(mFirstPresident)));

		mPresidentBiography->setText(mFirstPresident->getBiography());
	});

	/*Val av president bild 2*/
	mSecondPresidentPlaque->setOnClickFunction([=]()			
	{ 
		//mChoosePresidentWindow->setVisible(false); 
		//mPickedPresidentWindow->setVisible(true); 
		mPickedPresident = 2;
		
		mFirstPresidentButton->setTexture(std::pair<sf::FloatRect, sf::Texture*>(mFirstPresidentButton->getRectangle(), mFirstPresident->getTexture()));
		mSecondPresidentButton->setTexture(std::pair<sf::FloatRect, sf::Texture*>(mSecondPresidentButton->getRectangle(), mSecondPresident->getClickedTexture()));

		mPickedPresidentButton->setTexture(std::pair<sf::FloatRect, sf::Texture*>
			(mPickedPresidentButton->getRectangle(), mSecondPresident->getTexture()));

		mPickedPresidentPlaque->setTexture(std::pair<sf::FloatRect, sf::Texture*>
			(mPickedPresidentPlaque->getRectangle(), &GameManager::getInstance()->getPresidentPlaque(mSecondPresident)));

		mPresidentBiography->setText(mSecondPresident->getBiography());
	});		

	/*När en president har blivit vald*/
	mClosePresidentWindow->setOnClickFunction([=]()				
	{ 
		if(mPickedPresident == 1)
			setPresident(mFirstPresident);
		else if(mPickedPresident == 2)
			setPresident(mSecondPresident);
		/*om ingen president blivit vald går det inte att stänga fönstret*/
		if(mPresident != 0 && mPickedPresident > 0) 
		{
			std::shared_ptr<GUIWindow> _chooseWindow = mChoosePresidentWindow;
			std::shared_ptr<GUIWindow> _pickedWindow = mPickedPresidentWindow;
			/*GUIAnimation::fadeToColor(_chooseWindow, 1000, _chooseWindow->getColor(), sf::Color(255, 255, 255, 0));
			for(std::vector<std::shared_ptr<GUIElement>>::iterator it = _pickedWindow->getChildVector().begin(); it != _pickedWindow->getChildVector().end(); it++)
			{
				sf::Color color = (*it)->getColor();
				color.a = 255;
				GUIAnimation::fadeToColor(*it, 1000, sf::Color(color.r, color.g, color.b, 0), color);
			}*/
			sf::Color color = mPresidentBiography->getColor();
			color.a = 255;
			GUIAnimation::fadeToColor(mPresidentBiography, 1000, sf::Color(color.r, color.g, color.b, 0), color);
			color = mWindowHeadlines[1]->getColor();
			color.a = 255;
			GUIAnimation::fadeToColor(mWindowHeadlines[1], 1000, sf::Color(color.r, color.g, color.b, 0), color);
			_chooseWindow->setEnabled(false, true);
			_pickedWindow->setEnabled(false, true);
			_pickedWindow->setVisible(true);
			Timer::setTimer([=]()
			{
				_chooseWindow->setVisible(false);
				_chooseWindow->setColor(sf::Color(255, 255, 255, 255));
				_pickedWindow->setEnabled(true, true);
			}, 1000, 1);
			
			//mPresident->playSlogan();
			int yearsElected = mPresident->getYearsElected();

			mPresident->setYearsElected(yearsElected + 1);
			mPickedPresident = 0;
		}
	});

	/*Stänger ner fönster som visar vilken president som blivit vald*/
	mClosePickedPresidentWindow->setOnClickFunction([=]()				
	{ 
		mPickedPresidentWindow->setVisible(false);
		getTaxIncome();
		mCurrentPopulationText[1]->setText(intToString(mPopulation) + " million");
		mCurrentTaxesText[1]->setText(intToString(mTaxes));
		mTaxesIncomeText[1]->setText(intToString(mTaxesPreviousRound*mPopulationPreviousRound) + " §");
		mCapitalistPresident->setTexture(std::pair<sf::FloatRect, sf::Texture*>(mCapitalistPresident->getRectangle(), mPresident->getTexture()));
		mCapitalistPresident->setX(mPresidentFrame->getX() + 8); mCapitalistPresident->setY(mPresidentFrame->getY() + 9);
		mCapitalistPresident->setScale(0.55, 0.60);
		mPresident->resetAnimation();
		mTaxesIncomeWindow->setVisible(true);
		mTaxesIncomeWindow->setEnabled(true, true);

	});

	/*nästa runda*/
	mCapitalistEndTurnButton->setOnClickFunction([=]()	
	{
		mCapitalistMainTheme->fadeToVolume(1000, mCapitalistMainTheme->getVolume(), 25);
		mExportedFoodPreviousRound = mExportedFood;
		mExportedGoodsPreviousRound = mExportedGoods;
		mExportedTechPreviousRound = mExportedTech;
		int foodBought = mFood - mFoodPreviousRound;
		
		int goodsBought = mGoods - mGoodsPreviousRound;
		int techBought = mTech - mTechPreviousRound;
		int totalBought = (foodBought*foodCost) + (goodsBought*goodsCost) + (techBought*techCost);

		int foodTotalCost = foodBought * foodCost;
		int goodsTotalCost = goodsBought * goodsCost;
		int techTotalCost = techBought * techCost;
		if(foodTotalCost > goodsTotalCost && foodTotalCost > techTotalCost)
		{
			foodCost += 1;
			mIncreasedResourcesText->setText("The price of food is now " + intToString(foodCost) + " §");
			mIncreasedResourcesPriceWindow->setVisible(true);
		}
		else if(goodsTotalCost > foodTotalCost && goodsTotalCost > techTotalCost)
		{
			goodsCost += 1;
			mIncreasedResourcesText->setText("The price of goods is now " + intToString(goodsCost) + " §");
			mIncreasedResoucesSound->setSound(getSoundEffect("Buttons/Goods"));
			
			mIncreasedResoucesSound->playSound();

			mIncreasedResourcesPriceWindow->setVisible(true);
		}
		else if(techTotalCost > foodTotalCost && techTotalCost > goodsTotalCost)
		{
			techCost += 1;

			mIncreasedResourcesText->setText("The price of tech is now " + intToString(techCost) + " §");
			mIncreasedResoucesSound->setSound(getSoundEffect("Buttons/Factory"));
			mIncreasedResoucesSound->playSound();

			mIncreasedResourcesPriceWindow->setVisible(true);
		}
		else if(totalBought != 0)
		{
			if(GameManager::getInstance()->getGameType() == LAN && GameManager::getInstance()->isMyTurnToPlay())
			{
				std::vector<int> cost;
				std::vector<int> resourceType;
				std::vector<std::string> resourceLink;
				resourceLink.push_back("food");
				resourceLink.push_back("goods");
				resourceLink.push_back("tech");
				int highestCost = 0;
				cost.push_back(foodTotalCost);
				cost.push_back(goodsTotalCost);
				cost.push_back(techTotalCost);
				for(std::vector<int>::size_type i = 0; i < cost.size(); ++i)
				{
					if(cost[i] > highestCost)
					{
						highestCost = cost[i];
					}
				}
				for(std::vector<int>::size_type i = 0; i < cost.size(); ++i)
				{
					if(cost[i] == highestCost)
					{
						resourceType.push_back(i);
					}
				}
				int rand = Randomizer::getInstance()->randomNr(resourceType.size(), 0);
				sf::Packet packet;
				packet<<rand;
				GameManager::getInstance()->triggerOtherPlayersEvent("capitalistResourcesRandomIncrease", packet);
				if(resourceLink[resourceType[rand]] == "food")
				{
					foodCost += 1;
					mIncreasedResourcesText->setText("The price of food is now " + intToString(foodCost)  + " §");
				}
				else if(resourceLink[resourceType[rand]] == "goods")
				{
					goodsCost += 1;
					mIncreasedResourcesText->setText("The price of goods is now " + intToString(goodsCost) + " §");
				}
				else
				{
					techCost += 1;
					mIncreasedResourcesText->setText("The price of tech is now " + intToString(techCost) + " §");
				}
				mIncreasedResourcesPriceWindow->setVisible(true);
			}
			else if(GameManager::getInstance()->getGameType() == VERSUS)
			{
				std::vector<int> cost;
				std::vector<int> resourceType;
				std::vector<std::string> resourceLink;
				resourceLink.push_back("food");
				resourceLink.push_back("goods");
				resourceLink.push_back("tech");
				int highestCost = 0;
				cost.push_back(foodTotalCost);
				cost.push_back(goodsTotalCost);
				cost.push_back(techTotalCost);
				for(std::vector<int>::size_type i = 0; i < cost.size(); ++i)
				{
					if(cost[i] > highestCost)
					{
						highestCost = cost[i];
					}
				}
				for(std::vector<int>::size_type i = 0; i < cost.size(); ++i)
				{
					if(cost[i] == highestCost)
					{
						resourceType.push_back(i);
					}
				}
				int rand = Randomizer::getInstance()->randomNr(resourceType.size(), 0);
				if(resourceLink[resourceType[rand]] == "food")
				{
					foodCost += 1;
					mIncreasedResourcesText->setText("The price of food is now " + intToString(foodCost)  + " §");
				}
				else if(resourceLink[resourceType[rand]] == "goods")
				{
					goodsCost += 1;
					mIncreasedResourcesText->setText("The price of goods is now " + intToString(goodsCost) + " §");
				}
				else
				{
					techCost += 1;
					mIncreasedResourcesText->setText("The price of tech is now " + intToString(techCost) + " §");
				}
				mIncreasedResourcesPriceWindow->setVisible(true);
			}
		}
		else
		{
			updateFood(mPopulationEatsFoodText);

			mPopulationEatsFoodWindow->setVisible(true);
			if(mIncreasePopulation)
			{
				mClosePopulationEatsFoodWindow->setVisible(false);
			}
			else
			{
				mDoIncreasePopulation->setVisible(false);
				mDoNotIncreasePopulation->setVisible(false);
			}
		}
		mCapitalistEndTurnButton->setTexture(CapitalistButtons["EndTurnIsPressed"]);
	});
	
	mCloseIncreasedResourcesPriceWindow->setOnClickFunction([=]()
	{
		//mIncreasedResoucesSound->fadeToVolume(500, mIncreasedResoucesSound->getVolume(), 0);
		mIncreasedResourcesPriceWindow->setVisible(false);

		updateFood(mPopulationEatsFoodText);

		mPopulationEatsFoodWindow->setVisible(true);
		mClosePopulationEatsFoodWindow->setVisible(true);
		mDoIncreasePopulation->setVisible(true);
		mDoNotIncreasePopulation->setVisible(true);
		if(mIncreasePopulation)
		{
			mClosePopulationEatsFoodWindow->setVisible(false);
			mIncreasePopulation = false;
		}
		else
		{
			mDoIncreasePopulation->setVisible(false);
			mDoNotIncreasePopulation->setVisible(false);
		}
	});

	mDoIncreasePopulation->setOnClickFunction([=]()
	{
		mCurrency -= mPopulation;
		mPopulation += 1;
		mPopulationEatsFoodText->setText("Population increased from " + intToString(mPopulation - 1) + " to " + intToString(mPopulation));
		mDoIncreasePopulation->setVisible(false);
		mDoNotIncreasePopulation->setVisible(false);
		mClosePopulationEatsFoodWindow->setVisible(true);
	});

	mDoNotIncreasePopulation->setOnClickFunction([=]()
	{
		mPopulationEatsFoodText->setText("The population stays the same");
		mDoIncreasePopulation->setVisible(false);
		mDoNotIncreasePopulation->setVisible(false);
		mClosePopulationEatsFoodWindow->setVisible(true);
	});
	
	mClosePopulationEatsFoodWindow->setOnClickFunction([=]()
	{
		mCapitalistMainTheme->fadeToVolume(2000, CapitalistMusic["CapitalistMainTheme"]->getVolume(), 0);
		mPopulationEatsSound->fadeToVolume(1000, mPopulationEatsSound->getVolume(), 0);
		mPopulationEatsFoodWindow->setVisible(false);
		std::shared_ptr<Sound> music = mCapitalistMainTheme;
		std::shared_ptr<GUIButton> endTurn = mCapitalistEndTurnButton;
		sf::FloatRect rect = sf::FloatRect(CapitalistButtons["EndTurn"].first);
		sf::Texture* texture = CapitalistButtons["EndTurn"].second;
		Timer::setTimer([=]()
		{	
			/*if(mTaxes < mCurrentTax)
				setPatriotism(getPatriotism() + 2);
			else if(mTaxes > mCurrentTax)
				setPatriotism(getPatriotism() - 3);*/
		
			//mCommunistEndTurnButton->setTexture(CommunistButtons["EndTurnIsPressed"]);
			//mTaxes = mCurrentTax;

			GameManager::getInstance()->nextRound(); 
			music->stopSound();
			endTurn->setTexture(std::pair<sf::FloatRect, sf::Texture*>(rect, texture));
				//	//mTaxes = mCurrentTax;	

			}, 2000, 1);
	});
	
	mCloseTaxesIncomeWindow->setOnClickFunction([=]()
	{
		mTaxesIncomeWindow->setVisible(false);
		mTaxesIncomeWindow->setEnabled(false, true);
		if(mRound != 1)
		{
			std::shared_ptr<SuperPower> enemy = GameManager::getInstance()->getCommunist();
			int moneyIntFood = mPopulation + enemy->getPopulation();
			int moneyIntGoods = mPopulation + enemy->getPopulation();
			int moneyIntTech = mPopulation + enemy->getPopulation();
			// Money internationally should be equal to everybodies money together
			int foodBought = 0;
			int goodsBought = 0;
			int techBought = 0;
			int exports = 0;
			// if nobody bought my exports - then it will be sold internationally
			foodBought = (getExportedFood() == 0 || getExportedFoodPrice() == 0) ? getExportedFood() : moneyIntFood / getExportedFoodPrice();
			goodsBought = (getExportedGoods() == 0 || getExportedGoodsPrice() == 0) ? getExportedTech() : moneyIntGoods / getExportedGoodsPrice();
			techBought =(getExportedTech() == 0 || getExportedTechPrice() == 0) ? 0 :  moneyIntTech / getExportedTechPrice();
			//// if international market tries to buy more resources than you have
			if(foodBought > getExportedFood()) foodBought = getExportedFood();
			if(goodsBought > getExportedGoods()) goodsBought = getExportedGoods();
			if(techBought > getExportedTech()) techBought = getExportedTech();
			exports += (foodBought * getExportedFoodPrice()) + (goodsBought * getExportedGoodsPrice()) + (techBought * getExportedTechPrice());
			setExportedFood(getExportedFood() - foodBought);
			setExportedGoods(getExportedGoods() - goodsBought);
			setExportedTech(getExportedTech() - techBought);
			setCurrency(getCurrency() + exports);
			int _exportedFood = mExportedFoodPreviousRound-mExportedFood;
			int _exportedGoods = mExportedGoodsPreviousRound-mExportedGoods;
			int _exportedTech = mExportedTechPreviousRound-mExportedTech;
			mResourcesExportedText[0]->setText("You exported "+intToString(foodBought)+" food.");
			mResourcesExportedText[1]->setText("You exported "+intToString(goodsBought)+" goods.");
			mResourcesExportedText[2]->setText("You exported "+intToString(techBought)+" tech.");
			setExportedFoodSold(getExportedFoodSold() + foodBought);
			setExportedGoodsSold(getExportedGoodsSold() + goodsBought);
			setExportedTechSold(getExportedTechSold() + techBought);
			int exportedTotal = foodBought*mExportedFoodPrice + goodsBought*mExportedGoodsPrice + techBought*mExportedTechPrice;
			mExportedIncomeText->setText("You got " + intToString(exportedTotal) + " § from exports.");
			mExportedResourcesWindow->setVisible(true);
			mExportedResourcesWindow->setEnabled(true, true);

			mImportResourcesAvailableText[0]->setText(enemy->getExportedFood());
			mImportResourcesAvailableText[1]->setText(enemy->getExportedGoods());
			mImportResourcesAvailableText[2]->setText(enemy->getExportedTech());

			mImportPriceText[0]->setText(enemy->getExportedFoodPrice());
			mImportPriceText[1]->setText(enemy->getExportedGoodsPrice());
			mImportPriceText[2]->setText(enemy->getExportedTechPrice());

			mImportBuyQuantityText[0]->setText("0");
			mImportBuyQuantityText[1]->setText("0");
			mImportBuyQuantityText[2]->setText("0");

			mImportTotalPriceText[0]->setText("0");
			mImportTotalPriceText[1]->setText("0");
			mImportTotalPriceText[2]->setText("0");

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

			mExportQuantityText[0]->setText(mExportedFood);
			mExportQuantityText[1]->setText(mExportedGoods);
			mExportQuantityText[2]->setText(mExportedTech);

			mExportFoodCost->setText(intToString(mExportedFoodPrice));
			mExportGoodsCost->setText(intToString(mExportedGoodsPrice));
			mExportTechCost->setText(intToString(mExportedTechPrice));

			mExportPriceText[0]->setText(mExportedFood * mExportedFoodPrice);
			mExportPriceText[1]->setText(mExportedGoods * mExportedGoodsPrice);
			mExportPriceText[2]->setText(mExportedTech * mExportedTechPrice);
			mExportTotalPriceValue->setText(mExportedFood * mExportedFoodPrice + mExportedGoods * mExportedGoodsPrice + mExportedTech * mExportedTechPrice);
		}
		else
		{
			mCapitalistMainWindow->setEnabled(true, true);
		}
		mCapitalistMainWindow->setEnabled(true, true);
	});

	mCloseExportedResourceWindow->setOnClickFunction([=]()
	{
		mExportedResourcesWindow->setVisible(false);
		mExportedResourcesWindow->setEnabled(false, true);
		mCapitalistMainWindow->setEnabled(true, true);
	});

	mCloseIncreasedResourcesWindow->setOnClickFunction([=]()
	{
		mIncreasedResourcesWindow->setVisible(false);
	});

	mCapitalistPresident->setMouseEnterFunction([=]()
	{
		mPickedPresidentWindow->setVisible(true);
		mClosePickedPresidentWindow->setVisible(false);
	});
	mCapitalistPresident->setMouseLeaveFunction([=]()
	{
		mPickedPresidentWindow->setVisible(false);
		mClosePickedPresidentWindow->setVisible(true);
	});
}

void Capitalist::upgradeWindowText()
{
	int spaceProgram			= stringToInt(mBuySpaceProgramText->getText());
	int spyNetwork				= stringToInt(mBuySpyNetworkText->getText());
	int nuclearGoodsPrice		= 10 * mPresident->getNuclearPriceModifier();
	int nuclearTechPrice		= 5  * mPresident->getNuclearPriceModifier();
	int spaceProgramGoodsPrice  = (spaceProgram + 1) * 5 * mPresident->getSpacePriceModifier();
	int spaceProgramTechPrice	= 0;
	if(GameManager::getInstance()->getCommunist()->getSpaceProgram() > mSpaceProgram)
		spaceProgramTechPrice	= (spaceProgram + 1) * 10 * mPresident->getSpacePriceModifier() - (5 * spyNetwork);
	else
		spaceProgramTechPrice	= (spaceProgram + 1) * 10 * mPresident->getSpacePriceModifier();
	if(spaceProgramTechPrice < 5)
		spaceProgramTechPrice = 5;
	int spyNetworkTechPrice		= (spyNetwork + 1) * 10 * mPresident->getSpyPriceModifier();

	mNuclearGoodsCost->setText("Goods " +intToString(nuclearGoodsPrice));
	mNuclearTechCost->setText("Tech " +intToString(nuclearTechPrice));
	mSpaceProgramGoodsCost->setText("Goods " +intToString(spaceProgramGoodsPrice));
	mSpaceProgramTechCost->setText("Tech " +intToString(spaceProgramTechPrice));
	mSpyNetworkTechCost->setText("Tech " +intToString(spyNetworkTechPrice));
	
}

void Capitalist::showGUI()
{
	mCapitalistMainWindow->setVisible(true);
	playMusic();
}

void Capitalist::hideGUI()
{
	mCapitalistMainWindow->setVisible(false);
}

void Capitalist::LANRandomIncreasedResource(int random)
{
	int foodBought = mFood - mFoodPreviousRound;
	int goodsBought = mGoods - mGoodsPreviousRound;
	int techBought = mTech - mTechPreviousRound;
	int totalBought = (foodBought*foodCost) + (goodsBought*goodsCost) + (techBought*techCost);

	int foodTotalCost = foodBought * foodCost;
	int goodsTotalCost = goodsBought * goodsCost;
	int techTotalCost = techBought * techCost;
	std::vector<int> cost;
	std::vector<int> resourceType;
	std::vector<std::string> resourceLink;
	resourceLink.push_back("food");
	resourceLink.push_back("goods");
	resourceLink.push_back("tech");
	int highestCost = 0;
	cost.push_back(foodTotalCost);
	cost.push_back(goodsTotalCost);
	cost.push_back(techTotalCost);
	for(std::vector<int>::size_type i = 0; i < cost.size(); ++i)
	{
		if(cost[i] > highestCost)
		{
			highestCost = cost[i];
		}
	}
	for(std::vector<int>::size_type i = 0; i < cost.size(); ++i)
	{
		if(cost[i] == highestCost)
		{
			resourceType.push_back(i);
		}
	}
	int rand = random;
	if(resourceLink[resourceType[rand]] == "food")
	{
		foodCost += 1;
		mIncreasedResourcesText->setText("The price of food is now " + intToString(foodCost)  + " §");
	}
	else if(resourceLink[resourceType[rand]] == "goods")
	{
		goodsCost += 1;
		mIncreasedResourcesText->setText("The price of goods is now " + intToString(goodsCost) + " §");
	}
	else
	{
		techCost += 1;
		mIncreasedResourcesText->setText("The price of tech is now " + intToString(techCost) + " §");
	}
	mIncreasedResourcesPriceWindow->setVisible(true);
}