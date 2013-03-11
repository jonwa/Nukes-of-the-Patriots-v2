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
#include <sstream>
#include "Timer.h"
#include "GUIAnimation.h"

#include <SFML\Audio\Listener.hpp>

static int foodCost			= 10;
static int goodsCost		= 20;
static int techCost			= 30;
static int propagandaCost	= 100;
static int taxChange		= 5;
static int currentGoods		= 0;
static int currentTech		= 0;
static bool activateWindow	= false;
static float volumeChange	= 10.f;
static int generalCount = 0;

Communist::Communist() : 
	mCount(0),
	mUpdateGUIThread(nullptr)
{
	mRound				= 0;
	mIncreasePopulation = false;
	mType				= COMMUNIST;

	initializeCommunistWindow();
	initializeGuiFunctions();
	
	fiveYearInitialize();
	propagandaInitialize();
	mUpdateGUIThread = new sf::Thread(&Communist::updateGUI, this);
	mUpdateGUIThread->launch();
}

void Communist::reset()
{
	static int foodCost			= 10;
	static int goodsCost		= 20;
	static int techCost			= 30;
	static int propagandaCost	= 100;
	static int taxChange		= 5;
	static int currentGoods		= 0;
	static int currentTech		= 0;
	static bool activateWindow	= false;

	static int generalCount = 0;
	mRound = 0;
	mCount = 0;
	mIncreasePopulation = false;

	mGeneral = nullptr;
	mChooseGeneralWindow->setVisible(true);
	
	mCommunistFiveYearPlanButton->setTexture(CommunistButtons["FiveYearPlan"]);
	mCommunistFiveYearPlanButton->setSize(CommunistButtons["FiveYearPlan"].first.width, CommunistButtons["FiveYearPlan"].first.height);
	mCommunistPropagandaButton->setTexture(CommunistButtons["Propaganda"]);
	mCommunistPropagandaButton->setSize(CommunistButtons["Propaganda"].first.width, CommunistButtons["Propaganda"].first.height);
	mCommunistUpgradeButton->setTexture(CommunistButtons["Upgrade"]);
	mCommunistUpgradeButton->setSize(CommunistButtons["Upgrade"].first.width, CommunistButtons["Upgrade"].first.height);
	mCommunistTradeButton->setTexture(CommunistButtons["Export"]);
	mCommunistTradeButton->setSize(CommunistButtons["Export"].first.width, CommunistButtons["Export"].first.height);


	mCommunistGeneralButton->setTexture(CommunistButtons["General"]);
	mCommunistGeneralButton->setSize(CommunistButtons["General"].first.width, CommunistButtons["General"].first.height);
	SuperPower::reset();
	
}

void Communist::updateGUI()
{
	Timer::setTimer([=]()
	{
		int oldPopulation = stringToInt(mPopulationText->getText().substr(0, mPopulationText->getText().length() - 9));
		if(mPopulation != oldPopulation)
			mPopulationText->setText(intToString(mPopulation) + " million");
		int oldCurrency = stringToInt(mCurrencyText->getText());
		if(mCurrency != oldCurrency)
			mCurrencyText->setText(intToString(mCurrency));
		int oldPatriotism = stringToInt(mPatriotismText->getText());
		if(mPatriotism != oldPatriotism)
			mPatriotismText->setText(intToString(mPatriotism));

		/*GUI text f�r utskrift av v�rden p� komunisternas interface*/
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
	}, 50, 0);
}


Communist::~Communist()
{
	
}


//spelar upp musiken samt loopar den
void Communist::playMusic()
{
	mCommunistMainTheme->playSound(true);
	mCommunistMainTheme->setVolume(60);
}
//Stoppar musiken
void Communist::stopMusic()
{
	mCommunistMainTheme->stopSound();
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
	else
	{
		if(mRound != 1)
			resourceIncome();
	}
}

void Communist::changeCityImage()
{
	if((mRound-1) % 9 == 0 && mRound != 1)
	{
		mCount++;
		mChangeCityImage->setTexture(std::pair<sf::FloatRect, sf::Texture*>(mChangeCityImage->getRectangle(), CityImages[mCount])); 
		if(mCount > 5)
			mCount = 0;
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

void Communist::newYearStart()
{
	//communist title: 745, 90
	//capitalist title: 241, 90
	int statsPosX = 125, statsPosY = 177;   //communist 628, 177
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
		//mPopulation += 1;
		mPopulation = mPopulation;
	else
	{
		mFoodChange->setText("");
		mFoodChangeValue->setText("");
	}

	mFood -= mPopulation;
	if(mFood < 0) mFood = 0;
	int taxPatriotismChange = 0;
	int taxChange = (mTaxes - mTaxesPreviousRound) / 5;
	
	if(taxChange > 0)
	{
		taxPatriotismChange = -2 * std::abs(taxChange);
		mTaxChange->setText("Tax increased");
		mTaxChange->setY(statsPosY);
		mTaxChangeValue->setText(taxPatriotismChange);
		mTaxChangeValue->setY(statsPosY);
		statsPosY += mTaxChange->getHeight();
	}
	else if(taxChange < 0)
	{
		taxPatriotismChange = 1 * std::abs(taxChange);
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

	std::shared_ptr<SuperPower> enemy = GameManager::getInstance()->getCapitalist();

	int enemyNuclearWeapon = enemy->getNuclearWeapon();
	int enemySpaceProgram = enemy->getSpaceProgram();

	int nuclearDifference = mNuclearWeapon - enemyNuclearWeapon;
	int spaceProgramDifference = mSpaceProgram - enemySpaceProgram;
	bool spaceProgramIncreased = (mSpaceProgram > mSpaceProgramPreviousRound);
	int spaceProgramAmount = 0;
	if(spaceProgramIncreased)
	{
		spaceProgramAmount = mSpaceProgram - mSpaceProgramPreviousRound;
		mSpaceProgramIncreasedText->setText("Space program increased");
		mSpaceProgramIncreasedText->setY(statsPosY);
		mSpaceProgramIncreasedTextValue->setText(spaceProgramAmount);
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
		mNuclearWeaponChange->setText("Double enemies nuclear weapon");
		nuclearWeaponChange = 2;
		mNuclearWeaponChange->setY(statsPosY);
		mNuclearWeaponChangeValue->setText("+"+intToString(nuclearWeaponChange));
		mNuclearWeaponChangeValue->setY(statsPosY);
		statsPosY += mNuclearWeaponChange->getHeight();
	}
	else if(mNuclearWeapon > enemyNuclearWeapon)
	{
		mNuclearWeaponChange->setText("Nuclear weapon more than enemy");
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
	int exportedFoodChange = mExportedFoodPreviousRound - mExportedFood;
	int exportedGoodsChange = mExportedGoodsPreviousRound - mExportedGoods;
	int exportedTechChange = mExportedTechPreviousRound - mExportedTech;
	int exportedTotal = exportedFoodChange + exportedGoodsChange + exportedTechChange;

	// Enemy exported resources
	int enemyFoodExported = enemy->getExportedFoodPreviousRound() - enemy->getExportedFood();
	int enemyGoodsExported = enemy->getExportedGoodsPreviousRound() - enemy->getExportedGoods();
	int enemyTechExported = enemy->getExportedTechPreviousRound() - enemy->getExportedTech();
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

	//mCurrentPopulationText[1]->setText(mPopulation);
	//mCurrentTaxesText[1]->setText(mTaxes);
	//mTaxesIncomeText[1]->setText(intToString(mTaxesPreviousRound*mPopulationPreviousRound));
	//std::cout<<"income window tax: "<<mTaxesPreviousRound*mPopulationPreviousRound<<std::endl;
	mTaxesIncomeWindow->setVisible(true);

	int totalPatriotismChange = foodPatriotismChange + taxPatriotismChange + nuclearWeaponChange + spaceProgramChange + spaceProgramAmount + exportedChange;
	mPatriotism += totalPatriotismChange;
	if(totalPatriotismChange <= 0)
		mPatriotismChange->setText("Total patriotism change: " + intToString(totalPatriotismChange));
	else
		mPatriotismChange->setText("Total patriotism change: +" + intToString(totalPatriotismChange));

	mNewPatriotism->setText("Total patriotism: " + intToString(mPatriotism));
}

void Communist::update()
{
	if(mRound > 0)
	{
		std::shared_ptr<SuperPower> enemy = GameManager::getInstance()->getCapitalist();

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

	openFiveYearPlan();

	changeCityImage();
}

//-----------------------------------------------------------
/*	
	Uppgraderar mNuclearWeapon med ett
	Kostar 10 mGoods och 5 mTech
										*/
bool Communist::upgradeNuclearWeapon(int value)
{
	int goodsNuclearPrice = 10 * value;
	int techNuclearPrice = 5 * value;
	
	mNuclearWeapon += value;
	//mGoods -= goodsNuclearPrice;
	//mTech -= techNuclearPrice;
	mNuclearText->setText(mNuclearWeapon);
	
	return true;
}

/*	
	Uppgraderar mSpaceProgram med ett
	Kostar 5 mGoods multiplicerat med den nuvarande niv�n
	och 10 mTech multiplicerat med den nuvarande niv�n
															*/
bool Communist::upgradeSpaceProgram(int value)
{
	int goodsSpaceProgramPrice = 0;
	int techSpaceProgramPrice = 0;
	for(int i = 0; i < value; ++i)
	{
		goodsSpaceProgramPrice += (stringToInt(mSpaceText->getText()) + i + 1) * 5;
		techSpaceProgramPrice += (stringToInt(mSpaceText->getText()) + i + 1) * 10;
	}

	mSpaceProgram += value;
	//mGoods -= goodsSpaceProgramPrice;
	//mTech -= techSpaceProgramPrice;
	mSpaceText->setText(mSpaceProgram);
		
	return true;
}

/*	
	Uppgraderar mSpyNetwork med ett
	Kostar 10 mTech multiplicerat med den nuvarande niv�n
															*/
bool Communist::upgradeSpyNetwork(int value)
{
	int spyNetworkPrice = 0;
	for(int i = 0; i < value; ++i)
	{
		spyNetworkPrice = (stringToInt(mSpyText->getText()) + i + 1) * 10;
	}

	mSpyNetwork += value;
	//mTech -= spyNetworkPrice;
	mSpyText->setText(mSpyNetwork);
	
	return true;
}

//--------------------------------------------
/*Funktioner som ger medlemsvariabler nya v�rden*/
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
	//mTaxDecreased = (tax < mTaxes);
	mTaxes = tax;
}
/*  K�per en dos propaganda f�r 100 kr/dos som kan ge upp till 10 av en resurs, 
	antalet man f�r �r ==  10 rolls med en %chans baserat p� resursens andel av 
	det �rets planerade totala m�ngd resurser. (D�rav m�ste 5-�rsplanen komma f�re)
	Man kan inte med hj�lp av propaganda producera mer av en 
	resurs �n den m�ngd som st�r i fem�rsplanen f�r det �ret.
																			*/
void Communist::buyPropagandaFood(int round)
{
	int resourcesTotal = 0;
	resourcesTotal += getYearlyFood(round);
	resourcesTotal += getYearlyGoods(round);
	resourcesTotal += getYearlyTech(round);

	float percent = (float)getYearlyFood(round)/(float)resourcesTotal;
	percent*=100;
	int food = 0;
	for(int i=0;i<10;i++)
	{
		float randNr=Randomizer::getInstance()->randomNr(100,1);

		if(randNr<percent)
		{
			food++;
		}
	}
	mCurrency -= propagandaCost;
	mFood += food;
	mBoughtPropagandaText->setText("Your People got " + intToString(food) + " food");
}

void Communist::buyPropagandaGoods(int round)
{
	int resourcesTotal = 0;
	resourcesTotal += getYearlyFood(round);
	resourcesTotal += getYearlyGoods(round);
	resourcesTotal += getYearlyTech(round);

	float percent = (float)getYearlyGoods(round)/(float)resourcesTotal;
	percent*=100;
	int goods = 0;
	for(int i=0;i<10;i++)
	{
		float randNr=Randomizer::getInstance()->randomNr(100,1);

		if(randNr<percent)
		{
			goods++;
		}
	}
	mCurrency -= propagandaCost;
	mGoods += goods;
	mBoughtPropagandaText->setText("Your people got " + intToString(goods) + " goods");
}

void Communist::buyPropagandaTech(int round)
{
	int resourcesTotal = 0;
	resourcesTotal += getYearlyFood(round);
	resourcesTotal += getYearlyGoods(round);
	resourcesTotal += getYearlyTech(round);
	
	float percent = (float)getYearlyTech(round)/(float)resourcesTotal;
	percent*=100;
	int tech = 0;
	for(int i=0;i<10;i++)
	{
		float randNr=Randomizer::getInstance()->randomNr(100,1);

		if(randNr<percent)
		{
			tech++;
		}
	}
	mCurrency -= propagandaCost;
	mTech += tech;
	mBoughtPropagandaText->setText("Your people got " + intToString(tech) + " tech");
}

//------------------------------------------------------------------------------------------------------
	
//------------------------------------------------------------------------------------------------------

 /*
	Ladda hem alla knapparnas positioner fr�n XML dokument

	Av: Jon Wahlstr�m 2013-01-31
 
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
	Laddar in f�nstrernas positioner via XML dokument.

		Av: Jon Wahlstr�m 2013-01-31
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


 /*initierar kommunisternas f�nster respektive f�nster/knappar etc.*/

void Communist::initializeCommunistWindow()
{
	loadButtonPosition();
	loadWindowPosition();
	loadCommunistMusic();
	initializeCityImages();

	mCommunistMainTheme				= Sound::create(CommunistMusic["CommunistMainTheme"]); 

	mCommunistMainWindow			= GUIWindow::create(CommunistWindows["CommunistInterface"]);
	mCommunistButtonFrame			= GUIWindow::create(CommunistWindows["InterfaceButtonsFrame"], mCommunistMainWindow);
	mCommunistBorder				= GUIWindow::create(CommunistWindows["CommunistBorder"], mCommunistMainWindow);
	mCommunistBorderTop				= GUIWindow::create(CommunistWindows["CommunistBorderTop"], mCommunistMainWindow);
	mChangeCityImage				= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>
									  (CommunistButtons["CityImages"].first, CityImages[0]), mCommunistMainWindow);
	mCommunistGeneralButton			= GUIButton::create(CommunistButtons["General"], mCommunistMainWindow);
	mGeneralFrame					= GUIImage::create(CommunistButtons["GeneralFrame"], mCommunistMainWindow);
	mCommunistFiveYearPlanButton    = GUIButton::create(CommunistButtons["FiveYearPlan"], mCommunistMainWindow);
	mCommunistPropagandaButton		= GUIButton::create(CommunistButtons["Propaganda"], mCommunistMainWindow);
	mCommunistUpgradeButton			= GUIButton::create(CommunistButtons["Upgrade"], mCommunistMainWindow);
	mCommunistTradeButton			= GUIButton::create(CommunistButtons["Export"], mCommunistMainWindow);
	mEndTurnFrame					= GUIImage::create(CommunistButtons["EndTurnFrame"], mCommunistMainWindow);		
	mCommunistEndTurnButton			= GUIButton::create(CommunistButtons["EndTurn"], mCommunistMainWindow);
	mLeftPanel						= GUIImage::create(CommunistButtons["LeftPanel"], mCommunistMainWindow);
	mRightPanel						= GUIImage::create(CommunistButtons["RightPanel"], mCommunistMainWindow); 

	
	mPopulationText						= GUIText::create(sf::FloatRect(698, 4, 0, 0), intToString(mPopulation) + " million", mCommunistMainWindow);
	mPopulationText->setScale(0.7, 0.7);

	mPopulationText->setAlignment("middle");
	mCurrencyText						= GUIText::create(sf::FloatRect(362, 4, 0, 0), intToString(mCurrency), mCommunistMainWindow);
	mCurrencyText->setScale(0.7, 0.7);
	mCurrencyText->setAlignment("middle");
	mPatriotismText						= GUIText::create(sf::FloatRect(530, 42, 0, 0), intToString(mPatriotism), mCommunistMainWindow);
	mPatriotismText->setScale(0.7, 0.7);
	mPatriotismText->setAlignment("middle");

	mPopulationText->setColor(sf::Color::White);

	mCurrencyText->setColor(sf::Color::White);

	mPatriotismText->setColor(sf::Color::White);


	mCommunistMainWindow->setVisible(false);

	/*GUI text f�r utskrift av v�rden p� kapitalisternas interface*/

	mNuclearText						= GUIText::create(sf::FloatRect(843, 3, 0, 0), intToString(getNuclearWeapon()), mCommunistMainWindow);
	mNuclearText->setAlignment("middle");
	mNuclearText->setColor(sf::Color(255, 255, 255, 255));
	mNuclearText->setScale(0.9, 0.9);
	mSpaceText							= GUIText::create(sf::FloatRect(843, 215, 0, 0), intToString(getSpaceProgram()), mCommunistMainWindow);
	mSpaceText->setAlignment("middle");
	mSpaceText->setColor(sf::Color(255, 255, 255, 255));
	mSpaceText->setScale(0.9, 0.9);
	mSpyText							= GUIText::create(sf::FloatRect(843, 427, 0, 0), intToString(getSpyNetwork()), mCommunistMainWindow);
	mSpyText->setAlignment("middle");
	mSpyText->setColor(sf::Color(255, 255, 255, 255));
	mSpyText->setScale(0.9, 0.9);
	mFoodText							= GUIText::create(sf::FloatRect(31, 3, 0, 0), intToString(getFood()), mCommunistMainWindow);
	mFoodText->setAlignment("middle");
	mFoodText->setColor(sf::Color(255, 255, 255, 255));
	mFoodText->setScale(0.9, 0.9);
	mGoodsText							= GUIText::create(sf::FloatRect(31, 215, 0, 0), intToString(getGoods()), mCommunistMainWindow);
	mGoodsText->setAlignment("middle");
	mGoodsText->setColor(sf::Color(255, 255, 255, 255));
	mGoodsText->setScale(0.9, 0.9);
	mTechText							= GUIText::create(sf::FloatRect(31, 427, 0, 0), intToString(getTech()), mCommunistMainWindow);
	mTechText->setAlignment("middle");
	mTechText->setColor(sf::Color(255, 255, 255, 255));
	mTechText->setScale(0.9, 0.9);	
	

	/*Taxes f�nster med knappar*/
	mFiveYearPlanWindow				= GUIWindow::create(CommunistWindows["FiveYearPlanWindow"], mCommunistMainWindow);
	mFiveYearPlanWindow->setVisible(false);
	mYearOneLowerTaxesButton		= GUIButton::create(CommunistButtons["YearOneLowerTaxes"], mFiveYearPlanWindow);	
	mYearOneRaiseTaxesButton		= GUIButton::create(CommunistButtons["YearOneRaiseTaxes"], mFiveYearPlanWindow);	

	mFiveYearTotalCostText			= GUIText::create(sf::FloatRect(305, 17, 0, 0), "Cost", mFiveYearPlanWindow);
	mFiveYearTotalCostText->setScale(0.8, 0.8);
	//mFiveYearTotalCostText->setAlignment("middle");

	mFiveYearIncomeText				= GUIText::create(sf::FloatRect(427, 17, 0, 0), "Income", mFiveYearPlanWindow);
	mFiveYearIncomeText->setScale(0.8, 0.8);
	mFiveYearIncomeText->setAlignment("middle");

	mFiveYearTaxText				= GUIText::create(sf::FloatRect(506, 17, 0, 0), "Taxes", mFiveYearPlanWindow);
	mFiveYearTaxText->setScale(0.8, 0.8);
	mFiveYearTaxText->setAlignment("middle");
	
	mTaxesPatriotismChange			= GUIText::create(sf::FloatRect(390, 440, 0, 0), "Patriotism: 0", mFiveYearPlanWindow);
	mTaxesPatriotismChange->setScale(0.8, 0.8);

	mYearOneTaxesText				= GUIText::create(sf::FloatRect(506, 66, 20, 20), "30", mFiveYearPlanWindow);
	mYearOneTaxesText->setAlignment("middle");
	mYearTwoTaxesText				= GUIText::create(sf::FloatRect(506, 144, 20, 20), "30", mFiveYearPlanWindow);
	mYearTwoTaxesText->setAlignment("middle");
	mYearThreeTaxesText				= GUIText::create(sf::FloatRect(506, 222, 20, 20), "30", mFiveYearPlanWindow);
	mYearThreeTaxesText->setAlignment("middle");
	mYearFourTaxesText				= GUIText::create(sf::FloatRect(506, 300, 20, 20), "30", mFiveYearPlanWindow);
	mYearFourTaxesText->setAlignment("middle");
	mYearFiveTaxesText				= GUIText::create(sf::FloatRect(506, 378, 20, 20), "30", mFiveYearPlanWindow);
	mYearFiveTaxesText->setAlignment("middle");

	mIncomeYearOne					= GUIText::create(sf::FloatRect(427, 66, 20, 20), "0", mFiveYearPlanWindow);
	mIncomeYearOne->setScale(0.8, 0.8);
	mIncomeYearOne->setAlignment("middle");

	mIncomeYearTwo					= GUIText::create(sf::FloatRect(427, 144, 20, 20), "0", mFiveYearPlanWindow);
	mIncomeYearTwo->setScale(0.8, 0.8);
	mIncomeYearTwo->setAlignment("middle");

	mIncomeYearThree				= GUIText::create(sf::FloatRect(427, 222, 20, 20), "0", mFiveYearPlanWindow);
	mIncomeYearThree->setScale(0.8, 0.8);
	mIncomeYearThree->setAlignment("middle");

	mIncomeYearFour					= GUIText::create(sf::FloatRect(427, 300, 20, 20), "0", mFiveYearPlanWindow);
	mIncomeYearFour->setScale(0.8, 0.8);
	mIncomeYearFour->setAlignment("middle");

	mIncomeYearFive					= GUIText::create(sf::FloatRect(427, 378, 20, 20), "0", mFiveYearPlanWindow);
	mIncomeYearFive->setScale(0.8, 0.8);
	mIncomeYearFive->setAlignment("middle");

	mIncomeYearOne->setText(intToString(stringToInt(mYearOneTaxesText->getText()) * mPopulation) + " �");
	mIncomeYearTwo->setText(intToString(stringToInt(mYearTwoTaxesText->getText()) * mPopulation) + " �");
	mIncomeYearThree->setText(intToString(stringToInt(mYearThreeTaxesText->getText()) * mPopulation) + " �");
	mIncomeYearFour->setText(intToString(stringToInt(mYearFourTaxesText->getText()) * mPopulation) + " �");
	mIncomeYearFive->setText(intToString(stringToInt(mYearFiveTaxesText->getText()) * mPopulation) + " �");
	/*Food*/
	mYearOneFood					= GUIEditField::create(CommunistButtons["YearOneFoodEditField"].first, GUIEditField::COM, "0", true, mFiveYearPlanWindow);
	mYearTwoFood					= GUIEditField::create(CommunistButtons["YearTwoFoodEditField"].first, GUIEditField::COM, "0", true, mFiveYearPlanWindow);
	mYearThreeFood					= GUIEditField::create(CommunistButtons["YearThreeFoodEditField"].first, GUIEditField::COM, "0", true, mFiveYearPlanWindow);
	mYearFourFood					= GUIEditField::create(CommunistButtons["YearFourFoodEditField"].first, GUIEditField::COM, "0", true, mFiveYearPlanWindow);
	mYearFiveFood					= GUIEditField::create(CommunistButtons["YearFiveFoodEditField"].first, GUIEditField::COM, "0", true, mFiveYearPlanWindow);

	mYearOneGoods					= GUIEditField::create(CommunistButtons["YearOneGoodsEditField"].first, GUIEditField::COM, "0", true, mFiveYearPlanWindow);
	mYearTwoGoods					= GUIEditField::create(CommunistButtons["YearTwoGoodsEditField"].first, GUIEditField::COM, "0", true, mFiveYearPlanWindow);
	mYearThreeGoods					= GUIEditField::create(CommunistButtons["YearThreeGoodsEditField"].first, GUIEditField::COM, "0", true, mFiveYearPlanWindow);
	mYearFourGoods					= GUIEditField::create(CommunistButtons["YearFourGoodsEditField"].first, GUIEditField::COM, "0", true, mFiveYearPlanWindow);
	mYearFiveGoods					= GUIEditField::create(CommunistButtons["YearFiveGoodsEditField"].first, GUIEditField::COM, "0", true, mFiveYearPlanWindow);

	mYearOneTech					= GUIEditField::create(CommunistButtons["YearOneTechEditField"].first, GUIEditField::COM, "0", true, mFiveYearPlanWindow);
	mYearTwoTech					= GUIEditField::create(CommunistButtons["YearTwoTechEditField"].first, GUIEditField::COM, "0", true, mFiveYearPlanWindow);
	mYearThreeTech					= GUIEditField::create(CommunistButtons["YearThreeTechEditField"].first, GUIEditField::COM, "0", true, mFiveYearPlanWindow);
	mYearFourTech					= GUIEditField::create(CommunistButtons["YearFourTechEditField"].first, GUIEditField::COM, "0", true, mFiveYearPlanWindow);
	mYearFiveTech					= GUIEditField::create(CommunistButtons["YearFiveTechEditField"].first, GUIEditField::COM, "0", true, mFiveYearPlanWindow);

	mTotalCostYearOne				= GUIText::create(sf::FloatRect(305, 66, 20, 20), "0 �", mFiveYearPlanWindow);
	mTotalCostYearOne->setScale(0.8, 0.8);

	mTotalCostYearTwo				= GUIText::create(sf::FloatRect(305, 144, 20, 20), "0 �", mFiveYearPlanWindow);
	mTotalCostYearTwo->setScale(0.8, 0.8);

	mTotalCostYearThree				= GUIText::create(sf::FloatRect(305, 222, 20, 20), "0 �", mFiveYearPlanWindow);
	mTotalCostYearThree->setScale(0.8, 0.8);

	mTotalCostYearFour				= GUIText::create(sf::FloatRect(305, 300, 20, 20), "0 �", mFiveYearPlanWindow);
	mTotalCostYearFour->setScale(0.8, 0.8);

	mTotalCostYearFive				= GUIText::create(sf::FloatRect(305, 378, 20, 20), "0 �", mFiveYearPlanWindow);
	mTotalCostYearFive->setScale(0.8, 0.8);

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
	mPropagandaCostText					= GUIText::create(sf::FloatRect(285, 355, 0, 0), "Propaganda cost: 100 �", mPropagandaWindowFirst);
	mPropagandaCostText->setAlignment("middle");
	mBoughtPropagandaText				= GUIText::create(sf::FloatRect(225, 100, 0, 0), "0", mPropagandaWindowSecond);
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
	
	mSpyPanel							= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(359, 60, 200, 200), &ResourceHandler::getInstance()->getTexture(std::string("Communist/spy_panel"))), mUpgradeWindow);
	mSpacePanel							= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(184, 97, 200, 200), &ResourceHandler::getInstance()->getTexture(std::string("Communist/space_panel"))), mUpgradeWindow);
	mNuclearPanel						= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(9, 135, 200, 200), &ResourceHandler::getInstance()->getTexture(std::string("Communist/nuclear_panel"))), mUpgradeWindow);

	mBuyNuclearText						= GUIText::create(sf::FloatRect(39, 138, 22, 22), "0", mUpgradeWindow);
	mBuyNuclearText->setAlignment("middle");
	mBuyNuclearText->setColor(sf::Color(255, 255, 255, 255));
	mBuyNuclearText->setScale(0.9, 0.9);
	mNuclearTechCost					= GUIText::create(sf::FloatRect(35, 95, 20, 20), "0", mUpgradeWindow);
	mNuclearGoodsCost					= GUIText::create(sf::FloatRect(35, 60, 20, 20), "0", mUpgradeWindow);
	mBuySpaceProgramText				= GUIText::create(sf::FloatRect(214, 100, 22, 22), "0", mUpgradeWindow);
	mBuySpaceProgramText->setAlignment("middle");
	mBuySpaceProgramText->setColor(sf::Color(255, 255, 255, 255));
	mBuySpaceProgramText->setScale(0.9, 0.9);
	mSpaceProgramTechCost				= GUIText::create(sf::FloatRect(210, 60, 20, 20), "0", mUpgradeWindow);
	mSpaceProgramGoodsCost				= GUIText::create(sf::FloatRect(210, 25, 20, 20), "0", mUpgradeWindow);
	mBuySpyNetworkText					= GUIText::create(sf::FloatRect(389, 63, 22, 22), "0", mUpgradeWindow);
	mBuySpyNetworkText->setAlignment("middle");
	mBuySpyNetworkText->setColor(sf::Color(255, 255, 255, 255));
	mBuySpyNetworkText->setScale(0.9, 0.9);
	mSpyNetworkTechCost					= GUIText::create(sf::FloatRect(400, 10, 20, 20), "0", mUpgradeWindow);
	mUpgradeWindow->setVisible(false);

	/*Export GUI Window med knappar*/
	mExportWindow						= GUIWindow::create(CommunistWindows["CommunistExportWindow"], mCommunistMainWindow);

	mExportQuantityLabel				= GUIText::create(sf::FloatRect(164, 12, 200, 100), "Quantity", mExportWindow);
	mExportQuantityLabel->setScale(0.7, 0.7);
	mExportQuantityLabel->setAlignment("middle");
	mExportCostLabel					= GUIText::create(sf::FloatRect(348, 12, 200, 100), "Cost", mExportWindow);
	mExportCostLabel->setScale(0.7, 0.7);
	mExportCostLabel->setAlignment("middle");
	mExportPriceLabel					= GUIText::create(sf::FloatRect(502, 12, 200, 100), "Price", mExportWindow);
	mExportPriceLabel->setScale(0.7, 0.7);
	mExportPriceLabel->setAlignment("middle");
	mExportTotalPriceLabel				= GUIText::create(sf::FloatRect(362, 231, 200, 100), "Total price:", mExportWindow);
	mExportTotalPriceLabel->setScale(0.7, 0.7);
	mExportTotalPriceLabel->setAlignment("middle");

	mExportTotalPriceValue				= GUIText::create(sf::FloatRect(502, 231, 100, 31), "0", mExportWindow);
	mExportTotalPriceValue->setScale(0.7, 0.7);
	mExportTotalPriceValue->setAlignment("middle");
	
	sf::Texture *buyField = &ResourceHandler::getInstance()->getTexture(std::string("Communist/Communist_Edit_Field"));

	mExportQuantityBackground[0]		= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(137, 53, 57, 40), buyField), mExportWindow);
	mExportQuantityBackground[1]		= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(137, 111, 57, 40), buyField), mExportWindow);
	mExportQuantityBackground[2]		= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(137, 170, 57, 40), buyField), mExportWindow);
	for(int i = 0; i < 3; i++)
	{
		mExportQuantityBackground[i]->setSize(57, 40);		
	}

	mExportQuantityText[0] = GUIText::create(sf::FloatRect(mExportQuantityBackground[0]->getLocalX() + mExportQuantityBackground[0]->getWidth()/2, 62, 57, 40), "0", mExportWindow);
	mExportQuantityText[1] = GUIText::create(sf::FloatRect(mExportQuantityBackground[1]->getLocalX() + mExportQuantityBackground[1]->getWidth()/2, 117, 57, 40), "0", mExportWindow);
	mExportQuantityText[2] = GUIText::create(sf::FloatRect(mExportQuantityBackground[2]->getLocalX() + mExportQuantityBackground[2]->getWidth()/2, 176, 57, 40), "0", mExportWindow);
	for(int i = 0; i < 3; i++)
	{
		mExportQuantityText[i]->setScale(0.7, 0.7);
		mExportQuantityText[i]->setAlignment("middle");
	}

	mExportFoodCost						= GUIEditField::create(sf::FloatRect(280, 67, 139, 35), GUIEditField::CAP, "0", true, mExportWindow);
	mExportGoodsCost					= GUIEditField::create(sf::FloatRect(280, 112, 139, 35), GUIEditField::CAP, "0", true, mExportWindow);
	mExportTechCost						= GUIEditField::create(sf::FloatRect(280, 171, 139, 35), GUIEditField::CAP, "0", true, mExportWindow);

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

	mExportPriceText[0]			= GUIText::create(sf::FloatRect(502, 53, 100, 31), "0", mExportWindow);
	mExportPriceText[1]			= GUIText::create(sf::FloatRect(502, 111, 100, 31), "0", mExportWindow);
	mExportPriceText[2]			= GUIText::create(sf::FloatRect(502, 170, 100, 31), "0", mExportWindow);
	for(int i = 0; i < 3; i++)
	{
		mExportPriceText[i]->setScale(0.7, 0.7);
		mExportPriceText[i]->setAlignment("middle");
	}

	mExportConfirmButton				= GUIButton::create(CommunistButtons["ExportConfirm"], mExportWindow);
	mExportConfirmButton->setSize(CommunistButtons["ExportConfirm"].first.width, CommunistButtons["ExportConfirm"].first.height);
	mExportWindow->setVisible(false);
	
	mImportWindow						= GUIWindow::create(CommunistWindows["CommunistImportWindow"], mCommunistMainWindow);
	
	mImportResourceLabel				= GUIText::create(sf::FloatRect(32, 12, 200, 100), "Res.", mImportWindow);
	mImportResourceLabel->setScale(0.7, 0.7);
	mImportPriceLabel					= GUIText::create(sf::FloatRect(164, 12, 200, 100), "Price", mImportWindow);
	mImportPriceLabel->setScale(0.7, 0.7);
	mImportPriceLabel->setAlignment("middle");
	mImportQuantityLabel				= GUIText::create(sf::FloatRect(348, 12, 200, 100), "Quantity", mImportWindow);
	mImportQuantityLabel->setScale(0.7, 0.7);
	mImportQuantityLabel->setAlignment("middle");
	mImportCostLabel					= GUIText::create(sf::FloatRect(502, 12, 200, 100), "Cost", mImportWindow);
	mImportCostLabel->setScale(0.7, 0.7);
	mImportCostLabel->setAlignment("middle");
	mImportTotalCostLabel				= GUIText::create(sf::FloatRect(362, 231, 200, 100), "Total cost:", mImportWindow);
	mImportTotalCostLabel->setScale(0.7, 0.7);
	
	mImportTotalCostText				= GUIText::create(sf::FloatRect(507, 231, 200, 100), "0", mImportWindow);
	mImportTotalCostText->setScale(0.7, 0.7);
	mImportTotalCostText->setAlignment("middle");

	mImportResourcesAvailableText[0]	= GUIText::create(sf::FloatRect(80, 58, 56, 31), "0", mImportWindow);
	mImportResourcesAvailableText[1]	= GUIText::create(sf::FloatRect(80, 117, 56, 31), "0", mImportWindow);
	mImportResourcesAvailableText[2]	= GUIText::create(sf::FloatRect(80, 177, 56, 31), "0", mImportWindow);
	for(int i = 0; i < 3; i++)
	{
		mImportResourcesAvailableText[i]->setScale(0.7, 0.7);
	}

	mImportPriceText[0]					= GUIText::create(sf::FloatRect(168, 58, 56, 31), "N/A", mImportWindow);
	mImportPriceText[1]					= GUIText::create(sf::FloatRect(168, 117, 56, 31), "N/A", mImportWindow);
	mImportPriceText[2]					= GUIText::create(sf::FloatRect(168, 177, 56, 31), "N/A", mImportWindow);
	for(int i = 0; i < 3; i++)
	{
		mImportPriceText[i]->setScale(0.7, 0.7);
		mImportPriceText[i]->setAlignment("middle");
	}

	mImportBuyQuantityBackground[0]		= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(320, 53, 57, 40), buyField), mImportWindow);
	mImportBuyQuantityBackground[1]		= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(320, 111, 57, 40), buyField), mImportWindow);
	mImportBuyQuantityBackground[2]		= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(320, 170, 57, 40), buyField), mImportWindow);
	for(int i = 0; i < sizeof(mImportBuyQuantityBackground)/sizeof(mImportBuyQuantityBackground[0]); i++)
	{
		mImportBuyQuantityBackground[i]->setSize(57, 40);
	}
	mImportBuyQuantityText[0] = GUIText::create(sf::FloatRect(mImportBuyQuantityBackground[0]->getLocalX() + mImportBuyQuantityBackground[0]->getWidth()/2, 58, 57, 40), "0", mImportWindow);
	mImportBuyQuantityText[1] = GUIText::create(sf::FloatRect(mImportBuyQuantityBackground[1]->getLocalX() + mImportBuyQuantityBackground[1]->getWidth()/2, 117, 57, 40), "0", mImportWindow);
	mImportBuyQuantityText[2] = GUIText::create(sf::FloatRect(mImportBuyQuantityBackground[2]->getLocalX() + mImportBuyQuantityBackground[2]->getWidth()/2, 177, 57, 40), "0", mImportWindow);
	for(int i = 0; i < 3; i++)
	{
		mImportBuyQuantityText[i]->setScale(0.7, 0.7);
		mImportBuyQuantityText[i]->setAlignment("middle");
	}

	mImportTotalPriceText[0]					= GUIText::create(sf::FloatRect(507, 58, 100, 50), "0", mImportWindow);
	mImportTotalPriceText[1]					= GUIText::create(sf::FloatRect(507, 117, 100, 50), "0", mImportWindow);
	mImportTotalPriceText[2]					= GUIText::create(sf::FloatRect(507, 177, 100, 50), "0", mImportWindow);
	for(int i = 0; i < 3; i++)
	{
		mImportTotalPriceText[i]->setScale(0.7, 0.7);
		mImportTotalPriceText[i]->setAlignment("middle");
	}

	mImportGotoExportButton				= GUIButton::create(CommunistButtons["ImportGotoExport"], mImportWindow);
	mImportWindow->setVisible(false);

	mExportedResourcesWindow			= GUIWindow::create(CommunistWindows["ExportedResourcesWindow"], mCommunistMainWindow);
	mCloseExportedResourceWindow		= GUIButton::create(CommunistButtons["CloseExportedResourcesButton"], mExportedResourcesWindow);
	
	int y = 20;
	mResourcesExportedText[0]			= GUIText::create(sf::FloatRect(50, y, 0, 0), "0", mExportedResourcesWindow);
	mResourcesExportedText[0]->setScale(0.8, 0.8);
	y += mResourcesExportedText[0]->getHeight() + 5;
	mResourcesExportedText[1]			= GUIText::create(sf::FloatRect(50, y, 0, 0), "0", mExportedResourcesWindow);
	mResourcesExportedText[1]->setScale(0.8, 0.8);
	y += mResourcesExportedText[1]->getHeight() + 5;
	mResourcesExportedText[2]			= GUIText::create(sf::FloatRect(50, y, 0, 0), "0", mExportedResourcesWindow);
	mResourcesExportedText[2]->setScale(0.8, 0.8);
	y += mResourcesExportedText[2]->getHeight() + 5;
	mExportedIncomeText					= GUIText::create(sf::FloatRect(mResourcesExportedText[2]->getWidth() + 5, y, 0, 0), "0", mExportedResourcesWindow);
	mExportedIncomeText->setScale(0.5, 0.5);
	mExportedResourcesWindow->setVisible(false);

	mChooseGeneralWindow				= GUIWindow::create(CommunistWindows["ChooseGeneral"], mCommunistMainWindow);
	mPickedGeneralWindow				= GUIWindow::create(CommunistWindows["PickedGeneral"], mCommunistMainWindow);
	mPickedGeneralButton				= GUIImage::create(CommunistButtons["PickedGeneral"], mPickedGeneralWindow);
	sf::FloatRect pickedRect			= CommunistButtons["PickedGeneral"].first;
	mPickedGeneralPlaque				= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>
		(sf::FloatRect(pickedRect.left, pickedRect.top + pickedRect.height - 5, 0, 0),
		&GameManager::getInstance()->getGeneralPlaque(GameManager::getInstance()->getGeneral(generalCount))), mPickedGeneralWindow);

	mPickedGeneralWindow->setVisible(false);

	mFirstGeneralButton					= GUIImage::create(CommunistButtons["FirstGeneral"], mChooseGeneralWindow);
	sf::FloatRect generalRect			= CommunistButtons["FirstGeneral"].first;
	mFirstGeneralPlaque					= GUIButton::create(std::pair<sf::FloatRect, sf::Texture*>
		(sf::FloatRect(generalRect.left, generalRect.top + generalRect.height - 5, 0, 0),
		&GameManager::getInstance()->getGeneralPlaque(GameManager::getInstance()->getGeneral(generalCount))), mChooseGeneralWindow);

	
	mGoToNextPortraitButton				= GUIButton::create(CommunistButtons["GoToNextPortrait"], mChooseGeneralWindow);
	mGoToPreviousPortraitButton			= GUIButton::create(CommunistButtons["GoToPreviousPortrait"], mChooseGeneralWindow);
	mCloseGeneralWindow					= GUIButton::create(CommunistButtons["CloseGeneral"], mChooseGeneralWindow);
	mClosePickedGeneralWindow			= GUIButton::create(CommunistButtons["ClosePickedGeneral"], mPickedGeneralWindow);

	
	chooseLeader();

	
	std::shared_ptr<GUIWindow> statsWindow = GameManager::getInstance()->getStatsWindow();

	int statsPosY = 177;

	mCommunistHeadLine					= GUIText::create(sf::FloatRect(758, 130, 0, 0), "COMMUNIST", statsWindow);
	mCommunistHeadLine->setAlignment("middle");

	mNuclearWeaponChange				= GUIText::create(sf::FloatRect(585, statsPosY, 0, 0), "0", statsWindow);
	mNuclearWeaponChange->setAlignment("left");
	mNuclearWeaponChange->setScale(0.5, 0.5);
	mNuclearWeaponChangeValue			= GUIText::create(sf::FloatRect(855, statsPosY, 0, 0), "0", statsWindow);
	mNuclearWeaponChangeValue->setAlignment("left");
	mNuclearWeaponChangeValue->setScale(0.5, 0.5);
	statsPosY += mNuclearWeaponChange->getHeight();

	mSpaceProgramIncreasedText			= GUIText::create(sf::FloatRect(585, statsPosY, 0, 0), "0", statsWindow);
	mSpaceProgramIncreasedText->setAlignment("left");
	mSpaceProgramIncreasedText->setScale(0.5, 0.5);
	mSpaceProgramIncreasedTextValue		= GUIText::create(sf::FloatRect(855, statsPosY, 0, 0), "0", statsWindow);
	mSpaceProgramIncreasedTextValue->setAlignment("left");
	mSpaceProgramIncreasedTextValue->setScale(0.5, 0.5);
	statsPosY += mSpaceProgramIncreasedText->getHeight();

	mSpaceProgramMoreThanEnemyText		= GUIText::create(sf::FloatRect(585, statsPosY, 0, 0), "0", statsWindow);
	mSpaceProgramMoreThanEnemyText->setAlignment("left");
	mSpaceProgramMoreThanEnemyText->setScale(0.5, 0.5);
	mSpaceProgramMoreThanEnemyTextValue	= GUIText::create(sf::FloatRect(855, statsPosY, 0, 0), "0", statsWindow);
	mSpaceProgramMoreThanEnemyTextValue->setAlignment("left");
	mSpaceProgramMoreThanEnemyTextValue->setScale(0.5, 0.5);
	statsPosY += mSpaceProgramMoreThanEnemyText->getHeight();

	mExportedChange						= GUIText::create(sf::FloatRect(585, statsPosY, 0, 0), "0", statsWindow);
	mExportedChange->setAlignment("left");
	mExportedChange->setScale(0.5, 0.5);
	mExportedChangeValue	= GUIText::create(sf::FloatRect(855, statsPosY, 0, 0), "0", statsWindow);
	mExportedChangeValue->setAlignment("left");
	mExportedChangeValue->setScale(0.5, 0.5);
	statsPosY += mExportedChange->getHeight();

	mFoodChange							= GUIText::create(sf::FloatRect(585, statsPosY, 0, 0), "0", statsWindow);
	mFoodChange->setAlignment("left");
	mFoodChange->setScale(0.5, 0.5);
	mFoodChangeValue	= GUIText::create(sf::FloatRect(855, statsPosY, 0, 0), "0", statsWindow);
	mFoodChangeValue->setAlignment("left");
	mFoodChangeValue->setScale(0.5, 0.5);
	statsPosY += mFoodChange->getHeight();

	mTaxChange							= GUIText::create(sf::FloatRect(585, statsPosY, 0, 0), "0", statsWindow);
	mTaxChange->setScale(0.5, 0.5);
	mTaxChange->setAlignment("left");
	mTaxChangeValue	= GUIText::create(sf::FloatRect(855, statsPosY, 0, 0), "0", statsWindow);
	mTaxChangeValue->setAlignment("left");
	mTaxChangeValue->setScale(0.5, 0.5);
	statsPosY += mTaxChange->getHeight();

	mPatriotismChange					= GUIText::create(sf::FloatRect(585, statsPosY, 0, 0), "", statsWindow);
	mPatriotismChange->setScale(0.5, 0.5);
	statsPosY += mTaxChange->getHeight();
	
	mNewPatriotism						= GUIText::create(sf::FloatRect(585, statsPosY, 0, 0), "", statsWindow);
	mNewPatriotism->setScale(0.5, 0.5);

	mResourceIncomeWindow				= GUIWindow::create(CommunistWindows["ResourceIncome"], mCommunistMainWindow);
	mCloseResourceIncomeWindow			= GUIButton::create(CommunistButtons["CloseResourceIncome"], mResourceIncomeWindow);
	y = 20;
	mFoodIncome							= GUIText::create(sf::FloatRect(50, y, 0, 0), "0", mResourceIncomeWindow);
	mFoodIncome->setScale(0.8, 0.8);
	y +=  mFoodIncome->getHeight() + 5;
	mGoodsIncome						= GUIText::create(sf::FloatRect(50, y, 0, 0), "0", mResourceIncomeWindow);
	mGoodsIncome->setScale(0.8, 0.8);
	y += mGoodsIncome->getHeight() + 5;
	mTechIncome							= GUIText::create(sf::FloatRect(50, y, 0, 0), "0", mResourceIncomeWindow);
	mTechIncome->setScale(0.8, 0.8);
	mResourceIncomeWindow->setVisible(false);

	mTaxesIncomeWindow					= GUIWindow::create(CommunistWindows["TaxesIncome"], mCommunistMainWindow);
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
	mTaxesIncomeText[1]					= GUIText::create(sf::FloatRect(331, 74, 0, 0), intToString(mTaxesPreviousRound*mPopulationPreviousRound), mTaxesIncomeWindow);
	mTaxesIncomeText[1]->setScale(0.8, 0.8);
	mTaxesIncomeText[1]->setAlignment("left");

	mCloseTaxesIncomeWindow				= GUIButton::create(CommunistButtons["CloseTaxesIncome"], mTaxesIncomeWindow);
	mTaxesIncomeWindow->setVisible(false);

	mPopulationEatsFoodWindow			= GUIWindow::create(CommunistWindows["PopulationEatsFood"], mCommunistMainWindow);
	mPopulationEatsFoodText				= GUIText::create(sf::FloatRect(50, 50, 0, 0), "Example: Population eats 50 food \nPopulation grows to 51 million",  mPopulationEatsFoodWindow);
	mPopulationEatsFoodText->setScale(0.8, 0.8);
	mPopulationEatsFoodText->setAlignment("left");
	mClosePopulationEatsFoodWindow		= GUIButton::create(CommunistButtons["ClosePopulationEatsFood"], mPopulationEatsFoodWindow);
	mDoIncreasePopulation				= GUIButton::create(CommunistButtons["IncreasePopulation"], mPopulationEatsFoodWindow);
	mDoNotIncreasePopulation			= GUIButton::create(CommunistButtons["DoNotIncreasePopulation"], mPopulationEatsFoodWindow);
	mPopulationEatsFoodWindow->setVisible(false);

	mFoodImage[0]	= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(39, 17, 38, 38), &ResourceHandler::getInstance()->getTexture(std::string("Communist/food_image"))), mFiveYearPlanWindow);
	mFoodImage[1]	= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(31, 53, 35, 35), &ResourceHandler::getInstance()->getTexture(std::string("Communist/food_image"))), mImportWindow);
	mFoodImage[2]	= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(31, 53, 35, 35), &ResourceHandler::getInstance()->getTexture(std::string("Communist/food_image"))), mExportWindow);

	mGoodsImage[0]	= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(140, 17, 38, 38), &ResourceHandler::getInstance()->getTexture(std::string("Communist/goods_image"))), mFiveYearPlanWindow);
	mGoodsImage[1]	= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(31, 111, 35, 35), &ResourceHandler::getInstance()->getTexture(std::string("Communist/goods_image"))), mImportWindow);
	mGoodsImage[2]	= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(31, 111, 35, 35), &ResourceHandler::getInstance()->getTexture(std::string("Communist/goods_image"))), mExportWindow);

	mTechImage[0]	= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(241, 17, 38, 38), &ResourceHandler::getInstance()->getTexture(std::string("Communist/tech_image"))), mFiveYearPlanWindow);
	mTechImage[1]	= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(31, 170, 35, 35), &ResourceHandler::getInstance()->getTexture(std::string("Communist/tech_image"))), mImportWindow);
	mTechImage[2]	= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(31, 170, 35, 35), &ResourceHandler::getInstance()->getTexture(std::string("Communist/tech_image"))), mExportWindow);

	mGeneralBiography = GUIText::create(sf::FloatRect(40, 290, 0, 0), "", mPickedGeneralWindow);
	mGeneralBiography->setScale(0.6, 0.6);

	/*
	 	L�gger in f�r�ldernoden i vektorn som finns i GUIManager
	 	och kommer automatiskt f� med sig alla barnnoder till denna
	 	vilket �r alla GUIElement som finns i denna klass som har 
	 	mCapitalistMainWindow som parent-argument i dess konstruktor
																		*/

	GUIManager::getInstance()->addGUIElement(mCommunistMainWindow);
}



void Communist::initializeCityImages()
{
	CityImages.push_back(&ResourceHandler::getInstance()->getTexture(std::string("Communist/kom1")));
	CityImages.push_back(&ResourceHandler::getInstance()->getTexture(std::string("Communist/kom2")));
	CityImages.push_back(&ResourceHandler::getInstance()->getTexture(std::string("Communist/kom3")));
	CityImages.push_back(&ResourceHandler::getInstance()->getTexture(std::string("Communist/kom4")));
	CityImages.push_back(&ResourceHandler::getInstance()->getTexture(std::string("Communist/kom5")));
}

void Communist::chooseLeader()
{
	GameManager* manager = GameManager::getInstance();
	mCommunistMainWindow->setEnabled(false, true);
	mChooseGeneralWindow->setEnabled(true, true);
	mFirstGeneralButton->setTexture(std::pair<sf::FloatRect, sf::Texture*>(mFirstGeneralButton->getRectangle(), manager->getGeneral(generalCount)->getTexture())); 
}

/*Initierar funktionerna f�r fem�rsplansknapparna. Egen funktion f�r att inte g�ra allting s� gr�tigt.*/
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
			resourcesTotalCost[j + i*resources] = GUIText::create(sf::FloatRect(35 + 101 * i, 98 + 78 * j, 20, 20), "0 �", mFiveYearPlanWindow);
			resourcesTotalCost[j + i*resources]->setScale(0.7, 0.7);
		}
	}

	for(int i = 0; i < 5; ++i)
	{
		foodFields[i]->setMaxCharacters(4);
		goodsFields[i]->setMaxCharacters(4);
		techFields[i]->setMaxCharacters(4);
		foodFields[i]->setOnGuiChangeFunction([=]()
		{
			int foodAmount = stringToInt(foodFields[i]->getText());
			int goodsAmount = stringToInt(goodsFields[i]->getText());
			int techAmount = stringToInt(techFields[i]->getText());
			int amount = foodAmount * foodCost + goodsAmount * goodsCost + techAmount * techCost;
			resourcesTotalCost[i]->setText(intToString(foodAmount * foodCost) + " �");
			totalYearCost[i]->setText(intToString(amount) + " �");
		});
		goodsFields[i]->setOnGuiChangeFunction([=]()
		{
			int foodAmount = stringToInt(foodFields[i]->getText());
			int goodsAmount = stringToInt(goodsFields[i]->getText());
			int techAmount = stringToInt(techFields[i]->getText());
			int amount = foodAmount * foodCost + goodsAmount * goodsCost + techAmount * techCost;
			resourcesTotalCost[i + resources]->setText(intToString(goodsAmount * goodsCost) + " �");
			totalYearCost[i]->setText(intToString(amount) + " �");
		});
		techFields[i]->setOnGuiChangeFunction([=]()
		{
			int foodAmount = stringToInt(foodFields[i]->getText());
			int goodsAmount = stringToInt(goodsFields[i]->getText());
			int techAmount = stringToInt(techFields[i]->getText());
			int amount = foodAmount * foodCost + goodsAmount * goodsCost + techAmount * techCost;
			resourcesTotalCost[i + resources*2]->setText(intToString(techAmount * techCost) + " �");
			totalYearCost[i]->setText(intToString(amount) + " �");
		});
	}

	
	//--------------------------------------------------------------------------
	/*Taxes, �r ett*/
	mYearOneLowerTaxesButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearOneTaxesText->getText()) - taxChange;
		if(amount < 5)
			amount = 5;
		int difference = mTaxes - amount;
		difference = difference / 5;
		mYearOneTaxesText->setText(amount);
		mYearTwoTaxesText->setText(amount);
		mYearThreeTaxesText->setText(amount);
		mYearFourTaxesText->setText(amount);
		mYearFiveTaxesText->setText(amount);
		mIncomeYearOne->setText(intToString(amount * mPopulation) + " �");
		mIncomeYearTwo->setText(intToString(amount * mPopulation) + " �");
		mIncomeYearThree->setText(intToString(amount * mPopulation) + " �");
		mIncomeYearFour->setText(intToString(amount * mPopulation) + " �");
		mIncomeYearFive->setText(intToString(amount * mPopulation) + " �");
		if(amount < mTaxes)
			mTaxesPatriotismChange->setText("Patriotism: +" + intToString(difference * 1));
		else if(amount > mTaxes)
			mTaxesPatriotismChange->setText("Patriotism: -" + intToString(difference * -2));
		else
			mTaxesPatriotismChange->setText("Patriotism: 0");
	});
	mYearOneRaiseTaxesButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearOneTaxesText->getText()) + taxChange;
		if(amount > 95)
			amount = 95;
		int difference = mTaxes - amount;
		difference = difference / 5;
		mYearOneTaxesText->setText(amount);
		mYearTwoTaxesText->setText(amount);
		mYearThreeTaxesText->setText(amount);
		mYearFourTaxesText->setText(amount);
		mYearFiveTaxesText->setText(amount);
		mIncomeYearOne->setText(intToString(amount * mPopulation) + " �");
		mIncomeYearTwo->setText(intToString(amount * mPopulation) + " �");
		mIncomeYearThree->setText(intToString(amount * mPopulation) + " �");
		mIncomeYearFour->setText(intToString(amount * mPopulation) + " �");
		mIncomeYearFive->setText(intToString(amount * mPopulation) + " �");
		if(amount > mTaxes)
			mTaxesPatriotismChange->setText("Patriotism: -" + intToString(difference * -2));
		else if(amount < mTaxes)
			mTaxesPatriotismChange->setText("Patriotism: +" + intToString(difference * 1));
		else
			mTaxesPatriotismChange->setText("Patriotism: 0");
	});
}

 /**/
void Communist::initializeGuiFunctions()
{
	fiveYearGuiFunctions();
	/*Fem �rs plan knappen p� interface  */
	mCommunistFiveYearPlanButton->setOnClickFunction([=]()		
	{
		mCommunistMainWindow->setEnabled(false, true);
		mFiveYearPlanCloseButton->setEnabled(true, false);

		mFiveYearPlanWindow->setVisible(true);
		
		mCommunistFiveYearPlanButton->setTexture(CommunistButtons["FiveYearPlanIsPressed"]);

		float x = mFiveYearPlanWindow->getX() + mFiveYearPlanWindow->getRectangle().width/2;
		float y = mFiveYearPlanWindow->getY() + mFiveYearPlanWindow->getRectangle().height/2;
		GUIAnimation::move(mFiveYearPlanWindow, 100, sf::FloatRect(x, y, 0, 0), mFiveYearPlanWindow->getRectangle());
		for(std::vector<std::shared_ptr<GUIElement> >::size_type i = 0; i < mFiveYearPlanWindow->getChildVector().size(); ++i)
		{
			GUIAnimation::move(mFiveYearPlanWindow->getChildVector()[i], 100, sf::FloatRect(x, y, 0, 0), mFiveYearPlanWindow->getChildVector()[i]->getRectangle());
		}
	});
	/*Propaganda knappen p� interface*/
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

		float x = mPropagandaWindowFirst->getX() + mPropagandaWindowFirst->getRectangle().width/2;
		float y = mPropagandaWindowFirst->getY() + mPropagandaWindowFirst->getRectangle().height/2;
		GUIAnimation::move(mPropagandaWindowFirst, 100, sf::FloatRect(x, y, 0, 0), mPropagandaWindowFirst->getRectangle());
		for(std::vector<std::shared_ptr<GUIElement> >::size_type i = 0; i < mPropagandaWindowFirst->getChildVector().size(); ++i)
		{
			GUIAnimation::move(mPropagandaWindowFirst->getChildVector()[i], 100, sf::FloatRect(x, y, 0, 0), mPropagandaWindowFirst->getChildVector()[i]->getRectangle());
		}

	});
	/*Upgrade knappen p� interface*/
	mCommunistUpgradeButton->setOnClickFunction([=]()			
	{
		mCommunistMainWindow->setEnabled(false, true);
		mUpgradeWindow->setEnabled(true, true);

		mBuySpaceProgramText->setText(mSpaceText->getText());
		mBuyNuclearText->setText(mNuclearText->getText());
		mBuySpyNetworkText->setText(mSpyText->getText());
		mUpgradeWindow->setVisible(true); 
		mCommunistUpgradeButton->setTexture(CommunistButtons["UpgradeIsPressed"]);

		upgradeWindowText();

		float x = mUpgradeWindow->getX() + mUpgradeWindow->getRectangle().width/2;
		float y = mUpgradeWindow->getY() + mUpgradeWindow->getRectangle().height/2;
		GUIAnimation::move(mUpgradeWindow, 100, sf::FloatRect(x, y, 0, 0), mUpgradeWindow->getRectangle());
		for(std::vector<std::shared_ptr<GUIElement> >::size_type i = 0; i < mUpgradeWindow->getChildVector().size(); ++i)
		{
			GUIAnimation::move(mUpgradeWindow->getChildVector()[i], 100, sf::FloatRect(x, y, 0, 0), mUpgradeWindow->getChildVector()[i]->getRectangle());
		}
	});
	
	mFiveYearPlanCloseButton->setOnClickFunction([=]()					
	{		
		mFiveYearPlanWindow->setVisible(false);
		mCommunistFiveYearPlanButton->setTexture(CommunistButtons["FiveYearPlan"]);
		
		if(mFiveYearPlanWindow->isEnabled())
		{
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

			updateAllResources();
			resourceIncome();
			mCurrentPopulationText[1]->setText(mPopulation);
			mCurrentTaxesText[1]->setText(mTaxes);
			mTaxesIncomeText[1]->setText(intToString(mTaxes*mPopulation));
			mTaxesIncomeWindow->setVisible(true);
			mTaxesIncomeWindow->setEnabled(true, true);
			mFiveYearPlanWindow->setEnabled(false, true);
			mCurrentPopulationText[1]->setText(mPopulation);
			mCurrentTaxesText[1]->setText(mTaxes);
			mTaxesIncomeText[1]->setText(mTaxes*mPopulation);
		}
		mCommunistMainWindow->setEnabled(true, true);
	});
	/*St�nger propagandaf�nster nummer ett*/
	mPropagandaWindowFirstCloseButton->setOnClickFunction([=]()	
	{ 
		mCommunistMainWindow->setEnabled(true, true);

		mPropagandaWindowFirst->setVisible(false);
		mCommunistPropagandaButton->setTexture(CommunistButtons["Propaganda"]);
	});

	/*St�nger propagandaf�nster nummer tv�*/
	mPropagandaWindowSecondCloseButton->setOnClickFunction([=]()
	{
		
		mPropagandaWindowFirst->setEnabled(true, true);
		mPropagandaWindowSecond->setVisible(false);
		mCommunistPropagandaButton->setTexture(CommunistButtons["Propaganda"]);
	});

	mPropagandaBuyFoodButton->setOnClickFunction([=]()
	{
		if(mCurrency >= propagandaCost && getYearlyFood(mRound) != 0)
		{
			mPropagandaWindowSecond->setEnabled(true, true);
			buyPropagandaFood(getRound());

			mPropagandaWindowFirst->setEnabled(false, true);
			mPropagandaWindowSecond->setVisible(true);

			float x = mPropagandaBuyFoodButton->getX() + mPropagandaBuyFoodButton->getRectangle().width/2;
			float y = mPropagandaBuyFoodButton->getY() + mPropagandaBuyFoodButton->getRectangle().height/2;
			GUIAnimation::move(mPropagandaWindowSecond, 200, sf::FloatRect(x, y, 0, 0), mPropagandaWindowSecond->getRectangle());
			for(std::vector<std::shared_ptr<GUIElement> >::size_type i = 0; i < mPropagandaWindowSecond->getChildVector().size(); ++i)
			{
				GUIAnimation::move(mPropagandaWindowSecond->getChildVector()[i], 200, sf::FloatRect(x, y, 0, 0), mPropagandaWindowSecond->getChildVector()[i]->getRectangle());
			}
			
			mShowBoughtPropaganda->setTexture(std::pair<sf::FloatRect, sf::Texture*>
				(mShowBoughtPropaganda->getRectangle(), mPropagandaBuyFoodButton->getTexture()));
			//mShowBoughtPropaganda->setScale(0.8, 0.8);
		}
	});

	mPropagandaBuyGoodsButton->setOnClickFunction([=]()
	{
		if(mCurrency >= propagandaCost && getYearlyGoods(mRound) != 0)
		{
			mPropagandaWindowSecond->setEnabled(true, true);
			buyPropagandaGoods(getRound());

			mPropagandaWindowFirst->setEnabled(false, true);
			mPropagandaWindowSecond->setVisible(true);

			float x = mPropagandaBuyGoodsButton->getX() + mPropagandaBuyGoodsButton->getRectangle().width/2;
			float y = mPropagandaBuyGoodsButton->getY() + mPropagandaBuyGoodsButton->getRectangle().height/2;
			GUIAnimation::move(mPropagandaWindowSecond, 200, sf::FloatRect(x, y, 0, 0), mPropagandaWindowSecond->getRectangle());
			for(std::vector<std::shared_ptr<GUIElement> >::size_type i = 0; i < mPropagandaWindowSecond->getChildVector().size(); ++i)
			{
				GUIAnimation::move(mPropagandaWindowSecond->getChildVector()[i], 200, sf::FloatRect(x, y, 0, 0), mPropagandaWindowSecond->getChildVector()[i]->getRectangle());
			}
			
			mShowBoughtPropaganda->setTexture(std::pair<sf::FloatRect, sf::Texture*>
				(mShowBoughtPropaganda->getRectangle(), mPropagandaBuyGoodsButton->getTexture()));
			//mShowBoughtPropaganda->setScale(0.8, 0.8);
		}
	});
	mPropagandaBuyTechButton->setOnClickFunction([=]()
	{
		if(mCurrency >= propagandaCost && getYearlyTech(mRound) != 0)
		{
			mPropagandaWindowSecond->setEnabled(true, true);
			buyPropagandaTech(getRound());

			mPropagandaWindowFirst->setEnabled(false, true);
			mPropagandaWindowSecond->setVisible(true);

			float x = mPropagandaBuyTechButton->getX() + mPropagandaBuyTechButton->getRectangle().width/2;
			float y = mPropagandaBuyTechButton->getY() + mPropagandaBuyTechButton->getRectangle().height/2;
			GUIAnimation::move(mPropagandaWindowSecond, 200, sf::FloatRect(x, y, 0, 0), mPropagandaWindowSecond->getRectangle());
			for(std::vector<std::shared_ptr<GUIElement> >::size_type i = 0; i < mPropagandaWindowSecond->getChildVector().size(); ++i)
			{
				GUIAnimation::move(mPropagandaWindowSecond->getChildVector()[i], 200, sf::FloatRect(x, y, 0, 0), mPropagandaWindowSecond->getChildVector()[i]->getRectangle());
			}
			
			mShowBoughtPropaganda->setTexture(std::pair<sf::FloatRect, sf::Texture*>
				(mShowBoughtPropaganda->getRectangle(), mPropagandaBuyTechButton->getTexture()));
			//mShowBoughtPropaganda->setScale(0.8, 0.8);
		}
	});

	/*Upgrade och cancel f�r NuclearWeapon*/
	mUpgradeNuclearWeaponButton->setOnClickFunction([=]() 
	{
		int nuclearGoodsPrice	= 10;
		int nuclearTechPrice	= 5;
		int amount = stringToInt(mBuyNuclearText->getText());
		
		if(mGoods >= nuclearGoodsPrice && mTech >= nuclearTechPrice)
		{
			mUpgradeNuclearWeaponButton->canClick(true);
			++amount;
			mBuyNuclearText->setText(amount);
			upgradeWindowText();
			mGoods -= nuclearGoodsPrice;
			mTech  -= nuclearTechPrice;
		}
		else
			mUpgradeNuclearWeaponButton->canClick(false);
	});		
	mCancelUpgradeNuclearWeaponButton->setOnClickFunction([=]() 
	{
		int nuclearGoodsPrice	= 10;
		int nuclearTechPrice	= 5;
		int difference = stringToInt(mBuyNuclearText->getText()) - stringToInt(mNuclearText->getText());
		mGoods += nuclearGoodsPrice * difference;
		mTech  += nuclearTechPrice * difference;
		mBuyNuclearText->setText(mNuclearText->getText());
		upgradeWindowText();
		
	});
			
	/*Upgrade och cancel f�r SpaceProgram*/
	mUpgradeSpaceProgramButton->setOnClickFunction([=]()  
	{
		int spaceProgramGoodsPrice  = (stringToInt(mBuySpaceProgramText->getText()) + 1) * 5;
		int spaceProgramTechPrice	= (stringToInt(mBuySpaceProgramText->getText()) + 1) * 10;
		int amount = stringToInt(mBuySpaceProgramText->getText());
		if(mGoods >= spaceProgramGoodsPrice && mTech >= spaceProgramTechPrice)
		{
			mUpgradeSpaceProgramButton->canClick(true);
			++amount;
			mBuySpaceProgramText->setText(amount);
			upgradeWindowText();
			mGoods -= spaceProgramGoodsPrice;
			mTech  -= spaceProgramTechPrice;
		}
		else
			mUpgradeSpaceProgramButton->canClick(false);
	});
	mCancelUpgradeSpaceProgramButton->setOnClickFunction([=]() 
	{
		int spaceProgramGoodsPrice  = 5;
		int spaceProgramTechPrice	= 10;
		int difference = stringToInt(mBuySpaceProgramText->getText()) - stringToInt(mSpaceText->getText());
		for(int i = 0; i < difference; ++i)
		{
			mGoods += spaceProgramGoodsPrice * (i + 1);
			mTech  += spaceProgramTechPrice * (i + 1);
		}
		mBuySpaceProgramText->setText(mSpaceText->getText());
		upgradeWindowText();
	});

	/*Upgrade och cancel f�r SpyNetwork*/
	mUpgradeSpyNetworkButton->setOnClickFunction([=]()	 
	{
		int spyNetworkTechPrice = (stringToInt(mBuySpyNetworkText->getText()) + 1) * 10;
		int amount = stringToInt(mBuySpyNetworkText->getText());
		if(mTech >= spyNetworkTechPrice)
		{
			mUpgradeSpyNetworkButton->canClick(true);
			++amount;
			mTech -= spyNetworkTechPrice;
			mBuySpyNetworkText->setText(amount);
			upgradeWindowText();
		}
		else
			mUpgradeSpyNetworkButton->canClick(false);
		
	});		
	mCancelUpgradeSpyNetworkButton->setOnClickFunction([=]() 
	{
		int spyNetworkTechPrice = 10;
		int difference = stringToInt(mBuySpyNetworkText->getText()) - stringToInt(mSpyText->getText());
		for(int i = 0; i < difference; ++i)
		{
			mTech += spyNetworkTechPrice * (i + 1);
		}
		mBuySpyNetworkText->setText(mSpyText->getText());
		upgradeWindowText();
	});

	/*St�nger ned Upgradef�nstret*/
	mUpgradeCloseButton->setOnClickFunction([=]()				
	{ 
		int nuclearDiff = stringToInt(mBuyNuclearText->getText()) - stringToInt(mNuclearText->getText()); 
		int spaceDiff = stringToInt(mBuySpaceProgramText->getText()) - stringToInt(mSpaceText->getText());
		int spyDiff = stringToInt(mBuySpyNetworkText->getText()) - stringToInt(mSpyText->getText());

		mCommunistMainWindow->setEnabled(true, true);

		mUpgradeWindow->setVisible(false);
		upgradeNuclearWeapon(nuclearDiff); 
		upgradeSpaceProgram(spaceDiff); 
		upgradeSpyNetwork(spyDiff);
		mCommunistUpgradeButton->setTexture(CommunistButtons["Upgrade"]);
	});

	/*GUI hantering f�r valet av general*/
	/*Bl�ddra mellan generalerna f�r att v�lja mellan de fem som finns*/
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

	/*N�r en general har blivit vald*/
	mCloseGeneralWindow->setOnClickFunction([=]()
	{
		std::shared_ptr<GUIWindow> _chooseWindow = mChooseGeneralWindow;
		std::shared_ptr<GUIWindow> _pickedWindow = mPickedGeneralWindow;
		/*GUIAnimation::fadeToColor(_chooseWindow, 1000, _chooseWindow->getColor(), sf::Color(255, 255, 255, 0));
		for(std::vector<std::shared_ptr<GUIElement>>::iterator it = _pickedWindow->getChildVector().begin(); it != _pickedWindow->getChildVector().end(); it++)
		{
			sf::Color color = (*it)->getColor();
			color.a = 255;
			GUIAnimation::fadeToColor(*it, 1000, sf::Color(color.r, color.g, color.b, 0), color);
		}*/
		sf::Color color = mGeneralBiography->getColor();
		color.a = 255;
		GUIAnimation::fadeToColor(mGeneralBiography, 1000, sf::Color(color.r, color.g, color.b, 0), color);
		_chooseWindow->setEnabled(false, true);
		_pickedWindow->setEnabled(false, true);
		mPickedGeneralWindow->setVisible(true);
		Timer::setTimer([=]()
		{
			_chooseWindow->setVisible(false);
			_chooseWindow->setColor(sf::Color(255, 255, 255, 255));
			_pickedWindow->setEnabled(true, true);
		}, 1000, 1);
			

		mGeneral = GameManager::getInstance()->getGeneral(generalCount);

		mPickedGeneralButton->setTexture(std::pair<sf::FloatRect, sf::Texture*>
			(mPickedGeneralButton->getRectangle(), mGeneral->getTexture()));

		mPickedGeneralPlaque->setTexture(std::pair<sf::FloatRect, sf::Texture*>
			(mPickedGeneralPlaque->getRectangle(), mFirstGeneralPlaque->getTexture()));

		mGeneralBiography->setText(mGeneral->getBiography());
		mGeneral->playSlogan();

	});
	/*St�nger ner f�nster som visar vilken general som blivit vald*/
	mClosePickedGeneralWindow->setOnClickFunction([=]()
	{
		mPickedGeneralWindow->setVisible(false);
		mCommunistGeneralButton->setTexture(std::pair<sf::FloatRect, sf::Texture*>(mCommunistGeneralButton->getRectangle(), mGeneral->getTexture()));
		mCommunistGeneralButton->setScale(0.63, 0.68);
		mFiveYearPlanWindow->setVisible(true);
		mFiveYearPlanWindow->setEnabled(true, true);
	});

	/*Export knappen p� interface*/
	mCommunistTradeButton->setOnClickFunction([=]()
	{ 
		mCommunistMainWindow->setEnabled(false, true);
		mImportWindow->setEnabled(true, true);

		mImportWindow->setVisible(true); 
		mCommunistTradeButton->setTexture(CommunistButtons["ExportIsPressed"]);

		float x = mImportWindow->getX() + mImportWindow->getRectangle().width/2;
		float y = mImportWindow->getY() + mImportWindow->getRectangle().height/2;
		GUIAnimation::move(mImportWindow, 100, sf::FloatRect(x, y, 0, 0), mImportWindow->getRectangle());
		for(std::vector<std::shared_ptr<GUIElement> >::size_type i = 0; i < mImportWindow->getChildVector().size(); ++i)
		{
			
			GUIAnimation::move(mImportWindow->getChildVector()[i], 100, sf::FloatRect(x, y, 0, 0), mImportWindow->getChildVector()[i]->getRectangle());
		}
	});

	sf::Texture *minusTexture[3];
	minusTexture[0] = &ResourceHandler::getInstance()->getTexture(std::string("Communist/kom_texture_button_minus"));
	minusTexture[1] = &ResourceHandler::getInstance()->getTexture(std::string("Communist/kom_texture_button_minus5"));
	minusTexture[2] = &ResourceHandler::getInstance()->getTexture(std::string("Communist/kom_texture_button_minus10"));

	sf::Texture *plusTexture[3];
	plusTexture[0] = &ResourceHandler::getInstance()->getTexture(std::string("Communist/kom_texture_button_plus"));
	plusTexture[1] = &ResourceHandler::getInstance()->getTexture(std::string("Communist/kom_texture_button_plus5"));
	plusTexture[2] = &ResourceHandler::getInstance()->getTexture(std::string("Communist/kom_texture_button_plus10"));
	
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

		std::shared_ptr<SuperPower> enemy = GameManager::getInstance()->getCapitalist();
		enemy->setExportedFood(enemy->getExportedFood() - _importedFoodQuantity);
		enemy->setExportedGoods(enemy->getExportedGoods() - _importedGoodsQuantity);
		enemy->setExportedTech(enemy->getExportedTech() - _importedTechQuantity);
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
		mExportWindow->setVisible(false);
		mCommunistMainWindow->setEnabled(true, true);
		mCommunistTradeButton->setTexture(CommunistButtons["Export"]);

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
	});

	mCloseTaxesIncomeWindow->setOnClickFunction([=]()
	{
		mTaxesIncomeWindow->setVisible(false);
		mTaxesIncomeWindow->setEnabled(false, true);
		if(mRound != 1)
		{
			std::shared_ptr<SuperPower> enemy = GameManager::getInstance()->getCapitalist();
			int moneyIntFood = mPopulation + enemy->getPopulation();
			int moneyIntGoods = mPopulation + enemy->getPopulation();
			int moneyIntTech = mPopulation + enemy->getPopulation();
			// Money internationally should be equal to everybodies money together
			int foodBought = 0;
			int goodsBought = 0;
			int techBought = 0;
			int exports = 0;
			// if nobody bought my exports - then it will be sold internationally
			foodBought = (getExportedFood() == 0 || getExportedFoodPrice() == 0) ? 0 : moneyIntFood / getExportedFoodPrice();
			goodsBought = (getExportedGoods() == 0 || getExportedGoodsPrice() == 0) ? 0 : moneyIntGoods / getExportedGoodsPrice();
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
			mResourcesExportedText[0]->setText("You exported "+intToString(_exportedFood)+" food.");
			mResourcesExportedText[1]->setText("You exported "+intToString(_exportedGoods)+" goods.");
			mResourcesExportedText[2]->setText("You exported "+intToString(_exportedTech)+" tech.");
			int exportedTotal = _exportedFood*mExportedFoodPrice + _exportedGoods*mExportedGoodsPrice + _exportedTech*mExportedTechPrice;
			mExportedIncomeText->setText("You got " + intToString(exportedTotal) + " � from exports.");
			mExportedResourcesWindow->setVisible(true);
			mExportedResourcesWindow->setEnabled(true, true);
		}
		else
		{
			mResourceIncomeWindow->setVisible(true);
			mResourceIncomeWindow->setEnabled(true, true);
		}
	});

	mCloseExportedResourceWindow->setOnClickFunction([=]()
	{
		mExportedResourcesWindow->setVisible(false);
		mExportedResourcesWindow->setEnabled(false, true);
		mResourceIncomeWindow->setVisible(true);
		mResourceIncomeWindow->setEnabled(true, true);
	});

	mCloseResourceIncomeWindow->setOnClickFunction([=]()
	{
		mResourceIncomeWindow->setVisible(false);
		mResourceIncomeWindow->setEnabled(false, true);
		mCommunistMainWindow->setEnabled(true, true);
	});

	/*n�sta runda*/
	mCommunistEndTurnButton->setOnClickFunction([=]()	
	{
		updateFood(mPopulationEatsFoodText);

		mPopulationEatsFoodWindow->setVisible(true);
		mCommunistEndTurnButton->setTexture(CommunistButtons["EndTurnIsPressed"]);
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
		mCommunistMainTheme->fadeToVolume(CommunistMusic["CommunistMainTheme"], 2000, CommunistMusic["CommunistMainTheme"]->getVolume(), 0);
		mPopulationEatsFoodWindow->setVisible(false);
		std::shared_ptr<GUIButton> endTurn = mCommunistEndTurnButton;
		sf::FloatRect rect = sf::FloatRect(CommunistButtons["EndTurn"].first);
		sf::Texture* texture = CommunistButtons["EndTurn"].second;
		std::shared_ptr<Sound> music = mCommunistMainTheme;
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
}

void Communist::upgradeWindowText()
{
	int spaceProgram			= stringToInt(mBuySpaceProgramText->getText());
	int spyNetwork				= stringToInt(mBuySpyNetworkText->getText());
	int nuclearGoodsPrice		= 10;
	int nuclearTechPrice		= 5;
	int spaceProgramGoodsPrice  = (spaceProgram + 1) * 5;
	int spaceProgramTechPrice	= 0;
	if(GameManager::getInstance()->getCapitalist()->getSpaceProgram() > mSpaceProgram)
		spaceProgramTechPrice	= (spaceProgram + 1) * 10 - (5 * spyNetwork);
	else
		spaceProgramTechPrice	= (spaceProgram + 1) * 10;
	if(spaceProgramTechPrice < 5)
		spaceProgramTechPrice = 5;
	int spyNetworkTechPrice		= (spyNetwork + 1) * 10;

	mNuclearGoodsCost->setText("Goods " +intToString(nuclearGoodsPrice));
	mNuclearTechCost->setText("Tech " +intToString(nuclearTechPrice));
	mSpaceProgramGoodsCost->setText("Goods " +intToString(spaceProgramGoodsPrice));
	mSpaceProgramTechCost->setText("Tech " +intToString(spaceProgramTechPrice));
	mSpyNetworkTechCost->setText("Tech " +intToString(spyNetworkTechPrice));
}

void Communist::updateAllResources()
{
	/*�r ett*/
	setYearlyResources(1, "food", stringToInt(mYearOneFood->getText()));
	setYearlyResources(1, "goods", stringToInt(mYearOneGoods->getText()));
	setYearlyResources(1, "tech", stringToInt(mYearOneTech->getText()));
	setYearlyResources(1, "taxes", stringToInt(mYearOneTaxesText->getText()));
	/*�r tv�*/
	setYearlyResources(2, "food", stringToInt(mYearTwoFood->getText()));
	setYearlyResources(2, "goods", stringToInt(mYearTwoGoods->getText()));
	setYearlyResources(2, "tech", stringToInt(mYearTwoTech->getText()));
	setYearlyResources(2, "taxes", stringToInt(mYearTwoTaxesText->getText()));
	/*�r tre*/
	setYearlyResources(3, "food", stringToInt(mYearThreeFood->getText()));
	setYearlyResources(3, "goods", stringToInt(mYearThreeGoods->getText()));
	setYearlyResources(3, "tech", stringToInt(mYearThreeTech->getText()));
	setYearlyResources(3, "taxes", stringToInt(mYearThreeTaxesText->getText()));
	/*�r fyra*/
	setYearlyResources(4, "food", stringToInt(mYearFourFood->getText()));
	setYearlyResources(4, "goods", stringToInt(mYearFourGoods->getText()));
	setYearlyResources(4, "tech", stringToInt(mYearFourTech->getText()));
	setYearlyResources(4, "taxes", stringToInt(mYearFourTaxesText->getText()));
	/*�r fem*/
	setYearlyResources(5, "food", stringToInt(mYearFiveFood->getText()));
	setYearlyResources(5, "goods", stringToInt(mYearFiveGoods->getText()));
	setYearlyResources(5, "tech", stringToInt(mYearFiveTech->getText()));
	setYearlyResources(5, "taxes", stringToInt(mYearFiveTaxesText->getText()));
}

void Communist::resourceIncome()
{
	mTaxes  = getYearlyTaxes(mRound);
	getTaxIncome();
	int totalCost = getYearlyFood(mRound) * foodCost;
	totalCost += getYearlyGoods(mRound) * goodsCost;
	totalCost += getYearlyTech(mRound) * techCost;
	
	int moneyDifference = mCurrency - totalCost;
	if(mCurrency >= totalCost)
	{
		mCurrency -= totalCost;
		mFood  += getYearlyFood(mRound);
		mGoods += getYearlyGoods(mRound);
		mTech  += getYearlyTech(mRound);
		mFoodIncome->setText("You get " + intToString(getYearlyFood(mRound)) + " food");
		mGoodsIncome->setText("You get " + intToString(getYearlyGoods(mRound)) + " goods");
		mTechIncome->setText("You get " + intToString(getYearlyTech(mRound)) + " tech");
	}
	else
	{
		int food = 0, goods = 0, tech = 0;
		while(moneyDifference < 0)
		{
			std::vector<int> resourceCosts;
			std::vector<std::string> resourceType;
			if(mCurrency >= foodCost)
			{
				resourceCosts.push_back(foodCost);
				resourceType.push_back("food");
			}
			if(mCurrency >= goodsCost)
			{
				resourceCosts.push_back(goodsCost);
				resourceType.push_back("goods");
			}
			if(mCurrency >= techCost)
			{
				resourceCosts.push_back(techCost);
				resourceType.push_back("tech");
			}
			if(resourceCosts.empty())
				break;
			int rand = Randomizer::getInstance()->randomNr(resourceCosts.size(), 0);
			mCurrency -= resourceCosts[rand];

			if(resourceType[rand] == "food")
			{
				mFood++;
				food++;
				moneyDifference += foodCost;
			}
			else if(resourceType[rand] == "goods")
			{
				mGoods++;
				goods++;
				moneyDifference += goodsCost;
			}
			else if(resourceType[rand] == "tech")
			{
				mTech++;
				tech++;
				moneyDifference += techCost;
			}
		}
		mFoodIncome->setText("You get " + intToString(food) + " food");
		mGoodsIncome->setText("You get " + intToString(goods) + " goods");
		mTechIncome->setText("You get " + intToString(tech) + " tech");
	}
}

void Communist::showGUI()
{
	mCommunistMainWindow->setVisible(true);
	playMusic();
}

void Communist::hideGUI()
{
	mCommunistMainWindow->setVisible(false);
} 