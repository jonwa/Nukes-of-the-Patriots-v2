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

static int foodCost			= 10;
static int goodsCost		= 20;
static int techCost			= 30;
static int propagandaCost	= 100;
static int taxChange		= 5;
static int currentGoods		= 0;
static int currentTech		= 0;
static bool activateWindow	= false;

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
	}, 50, 0);
}


Communist::~Communist()
{
	
}


//spelar upp musiken samt loopar den
void Communist::playMusic()
{
	std::shared_ptr<sf::Music> music = CommunistMusic["CommunistMainTheme"];
	music->setVolume(100);
	//music->play();
	//music->setLoop(true);
}
//Stoppar musiken
void Communist::stopMusic()
{
	CommunistMusic["CommunistMainTheme"]->stop();
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
	mTaxesIncomeWindow->setVisible(true);
	getTaxIncome();
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

	std::shared_ptr<SuperPower> enemy = GameManager::getInstance()->getCapitalist();

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

void Communist::update()
{
	
	if(mRound > 1)
	{
		std::shared_ptr<SuperPower> enemy = GameManager::getInstance()->getCapitalist();

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

	/*openFiveYearPlan();*/

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
bool Communist::upgradeSpaceProgram(int value)
{
	int goodsSpaceProgramPrice = 0;
	int techSpaceProgramPrice = 0;
	for(int i = 0; i < value; ++i)
	{
		goodsSpaceProgramPrice += (stringToInt(mSpaceText->getText()) + i + 1) * 5;
		techSpaceProgramPrice += (stringToInt(mSpaceText->getText()) + i + 1) * 10;
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
bool Communist::upgradeSpyNetwork(int value)
{
	int spyNetworkPrice = 0;
	for(int i = 0; i < value; ++i)
	{
		spyNetworkPrice = (stringToInt(mSpyText->getText()) + i + 1) * 10;
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
	//mTaxDecreased = (tax < mTaxes);
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

	float percent = mGoods/resourcesTotal;
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
	
	float percent = mTech/resourcesTotal;
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


 /*initierar kommunisternas fönster respektive fönster/knappar etc.*/

void Communist::initializeCommunistWindow()
{
	loadButtonPosition();
	loadWindowPosition();
	loadCommunistMusic();
	initializeCityImages();

	mCommunistMainWindow			= GUIWindow::create(CommunistWindows["CommunistInterface"]);
	mCommunistBorder				= GUIWindow::create(CommunistWindows["CommunistBorder"], mCommunistMainWindow);
	mCommunistBorderTop				= GUIWindow::create(CommunistWindows["CommunistBorderTop"], mCommunistMainWindow);
	mChangeCityImage				= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>
									  (CommunistButtons["CityImages"].first, CityImages[0]), mCommunistMainWindow);
	mCommunistGeneralButton			= GUIButton::create(CommunistButtons["General"], mCommunistMainWindow);
	mCommunistFiveYearPlanButton    = GUIButton::create(CommunistButtons["FiveYearPlan"], mCommunistMainWindow);
	mCommunistPropagandaButton		= GUIButton::create(CommunistButtons["Propaganda"], mCommunistMainWindow);
	mCommunistUpgradeButton			= GUIButton::create(CommunistButtons["Upgrade"], mCommunistMainWindow);
	mCommunistTradeButton			= GUIButton::create(CommunistButtons["Export"], mCommunistMainWindow);
	mCommunistEndTurnButton			= GUIButton::create(CommunistButtons["EndTurn"], mCommunistMainWindow);
	mLeftPanel						= GUIImage::create(CommunistButtons["LeftPanel"], mCommunistMainWindow);
	mRightPanel						= GUIImage::create(CommunistButtons["RightPanel"], mCommunistMainWindow); 

	
	mPopulationText						= GUIText::create(sf::FloatRect(698, 20, 0, 0), intToString(mPopulation) + " million", mCommunistMainWindow);
	mPopulationText->setScale(0.7, 0.7);

	mPopulationText->setAlignment("middle");
	mCurrencyText						= GUIText::create(sf::FloatRect(362, 14, 0, 0), intToString(mCurrency), mCommunistMainWindow);
	mCurrencyText->setScale(0.7, 0.7);
	mCurrencyText->setAlignment("middle");
	mPatriotismText						= GUIText::create(sf::FloatRect(530, 53, 0, 0), intToString(mPatriotism), mCommunistMainWindow);
	mPatriotismText->setScale(0.7, 0.7);
	mPatriotismText->setAlignment("middle");

	mPopulationText->setColor(sf::Color::White);

	mCurrencyText->setColor(sf::Color::White);

	mPatriotismText->setColor(sf::Color::White);


	mCommunistMainWindow->setVisible(false);

	/*GUI text för utskrift av värden på kapitalisternas interface*/

	mNuclearText						= GUIText::create(sf::FloatRect(843, 17, 0, 0), intToString(getNuclearWeapon()), mCommunistMainWindow);
	mNuclearText->setAlignment("middle");
	mNuclearText->setScale(0.9, 0.9);
	mSpaceText							= GUIText::create(sf::FloatRect(843, 229, 0, 0), intToString(getSpaceProgram()), mCommunistMainWindow);
	mSpaceText->setAlignment("middle");
	mSpaceText->setScale(0.9, 0.9);
	mSpyText							= GUIText::create(sf::FloatRect(843, 441, 0, 0), intToString(getSpyNetwork()), mCommunistMainWindow);
	mSpyText->setAlignment("middle");
	mSpyText->setScale(0.9, 0.9);
	mFoodText							= GUIText::create(sf::FloatRect(31, 17, 0, 0), intToString(getFood()), mCommunistMainWindow);
	mFoodText->setAlignment("middle");
	mFoodText->setScale(0.9, 0.9);
	mGoodsText							= GUIText::create(sf::FloatRect(31, 229, 0, 0), intToString(getGoods()), mCommunistMainWindow);
	mGoodsText->setAlignment("middle");
	mGoodsText->setScale(0.9, 0.9);
	mTechText							= GUIText::create(sf::FloatRect(31, 441, 0, 0), intToString(getTech()), mCommunistMainWindow);
	mTechText->setAlignment("middle");
	mTechText->setScale(0.9, 0.9);	
	

	/*Taxes fönster med knappar*/
	mFiveYearPlanWindow				= GUIWindow::create(CommunistWindows["FiveYearPlanWindow"], mCommunistMainWindow);	
	mYearOneLowerTaxesButton		= GUIButton::create(CommunistButtons["YearOneLowerTaxes"], mFiveYearPlanWindow);	
	mYearOneRaiseTaxesButton		= GUIButton::create(CommunistButtons["YearOneRaiseTaxes"], mFiveYearPlanWindow);	
	/*mYearTwoLowerTaxesButton		= GUIButton::create(CommunistButtons["YearTwoLowerTaxes"], mFiveYearPlanWindow);	
	mYearTwoRaiseTaxesButton		= GUIButton::create(CommunistButtons["YearTwoRaiseTaxes"], mFiveYearPlanWindow);	
	mYearThreeLowerTaxesButton		= GUIButton::create(CommunistButtons["YearThreeLowerTaxes"], mFiveYearPlanWindow);	
	mYearThreeRaiseTaxesButton		= GUIButton::create(CommunistButtons["YearThreeRaiseTaxes"], mFiveYearPlanWindow);	
	mYearFourLowerTaxesButton		= GUIButton::create(CommunistButtons["YearFourLowerTaxes"], mFiveYearPlanWindow);	
	mYearFourRaiseTaxesButton		= GUIButton::create(CommunistButtons["YearFourRaiseTaxes"], mFiveYearPlanWindow);	
	mYearFiveLowerTaxesButton		= GUIButton::create(CommunistButtons["YearFiveLowerTaxes"], mFiveYearPlanWindow);	
	mYearFiveRaiseTaxesButton		= GUIButton::create(CommunistButtons["YearFiveRaiseTaxes"], mFiveYearPlanWindow);*/

	mFiveYearTotalCostText			= GUIText::create(sf::FloatRect(305, 17, 0, 0), "Cost", mFiveYearPlanWindow);
	mFiveYearTotalCostText->setScale(0.8, 0.8);
	//mFiveYearTotalCostText->setAlignment("middle");

	mFiveYearIncomeText				= GUIText::create(sf::FloatRect(427, 35, 0, 0), "Income", mFiveYearPlanWindow);
	mFiveYearIncomeText->setScale(0.8, 0.8);
	mFiveYearIncomeText->setAlignment("middle");

	mFiveYearTaxText				= GUIText::create(sf::FloatRect(506, 35, 0, 0), "Taxes", mFiveYearPlanWindow);
	mFiveYearTaxText->setScale(0.8, 0.8);
	mFiveYearTaxText->setAlignment("middle");

	mYearOneTaxesText				= GUIText::create(sf::FloatRect(506, 84, 20, 20), "30", mFiveYearPlanWindow);
	mYearOneTaxesText->setAlignment("middle");
	mYearTwoTaxesText				= GUIText::create(sf::FloatRect(506, 162, 20, 20), "30", mFiveYearPlanWindow);
	mYearTwoTaxesText->setAlignment("middle");
	mYearThreeTaxesText				= GUIText::create(sf::FloatRect(506, 240, 20, 20), "30", mFiveYearPlanWindow);
	mYearThreeTaxesText->setAlignment("middle");
	mYearFourTaxesText				= GUIText::create(sf::FloatRect(506, 318, 20, 20), "30", mFiveYearPlanWindow);
	mYearFourTaxesText->setAlignment("middle");
	mYearFiveTaxesText				= GUIText::create(sf::FloatRect(506, 396, 20, 20), "30", mFiveYearPlanWindow);
	mYearFiveTaxesText->setAlignment("middle");

	mIncomeYearOne					= GUIText::create(sf::FloatRect(427, 86, 20, 20), "0", mFiveYearPlanWindow);
	mIncomeYearOne->setScale(0.8, 0.8);
	mIncomeYearOne->setAlignment("middle");

	mIncomeYearTwo					= GUIText::create(sf::FloatRect(427, 164, 20, 20), "0", mFiveYearPlanWindow);
	mIncomeYearTwo->setScale(0.8, 0.8);
	mIncomeYearTwo->setAlignment("middle");

	mIncomeYearThree				= GUIText::create(sf::FloatRect(427, 242, 20, 20), "0", mFiveYearPlanWindow);
	mIncomeYearThree->setScale(0.8, 0.8);
	mIncomeYearThree->setAlignment("middle");

	mIncomeYearFour					= GUIText::create(sf::FloatRect(427, 320, 20, 20), "0", mFiveYearPlanWindow);
	mIncomeYearFour->setScale(0.8, 0.8);
	mIncomeYearFour->setAlignment("middle");

	mIncomeYearFive					= GUIText::create(sf::FloatRect(427, 398, 20, 20), "0", mFiveYearPlanWindow);
	mIncomeYearFive->setScale(0.8, 0.8);
	mIncomeYearFive->setAlignment("middle");

	mIncomeYearOne->setText(intToString(stringToInt(mYearOneTaxesText->getText()) * mPopulation) + " §");
	mIncomeYearTwo->setText(intToString(stringToInt(mYearTwoTaxesText->getText()) * mPopulation) + " §");
	mIncomeYearThree->setText(intToString(stringToInt(mYearThreeTaxesText->getText()) * mPopulation) + " §");
	mIncomeYearFour->setText(intToString(stringToInt(mYearFourTaxesText->getText()) * mPopulation) + " §");
	mIncomeYearFive->setText(intToString(stringToInt(mYearFiveTaxesText->getText()) * mPopulation) + " §");
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

	mTotalCostYearOne				= GUIText::create(sf::FloatRect(305, 66, 20, 20), "0 §", mFiveYearPlanWindow);
	mTotalCostYearOne->setScale(0.8, 0.8);

	mTotalCostYearTwo				= GUIText::create(sf::FloatRect(305, 144, 20, 20), "0 §", mFiveYearPlanWindow);
	mTotalCostYearTwo->setScale(0.8, 0.8);

	mTotalCostYearThree				= GUIText::create(sf::FloatRect(305, 222, 20, 20), "0 §", mFiveYearPlanWindow);
	mTotalCostYearThree->setScale(0.8, 0.8);

	mTotalCostYearFour				= GUIText::create(sf::FloatRect(305, 300, 20, 20), "0 §", mFiveYearPlanWindow);
	mTotalCostYearFour->setScale(0.8, 0.8);

	mTotalCostYearFive				= GUIText::create(sf::FloatRect(305, 378, 20, 20), "0 §", mFiveYearPlanWindow);
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
	mPropagandaCostText					= GUIText::create(sf::FloatRect(285, 355, 0, 0), "Propaganda cost: 100 §", mPropagandaWindowFirst);
	mPropagandaCostText->setAlignment("middle");
	mBoughtPropagandaText				= GUIText::create(sf::FloatRect(225, 100, 0, 0), "", mPropagandaWindowSecond);
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
	
	mSpyPanel		= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(358, 60, 212, 212), &ResourceHandler::getInstance()->getTexture(std::string("Communist/spy_panel"))), mUpgradeWindow);
	mSpacePanel		= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(179, 97, 212, 212), &ResourceHandler::getInstance()->getTexture(std::string("Communist/space_panel"))), mUpgradeWindow);
	mNuclearPanel	= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(0, 135, 212, 212), &ResourceHandler::getInstance()->getTexture(std::string("Communist/nuclear_panel"))), mUpgradeWindow);

	mBuyNuclearText						= GUIText::create(sf::FloatRect(31, 152, 22, 22), "0", mUpgradeWindow);
	mBuyNuclearText->setAlignment("middle");
	mBuyNuclearText->setScale(0.9, 0.9);
	mNuclearTechCost					= GUIText::create(sf::FloatRect(35, 95, 20, 20), "0", mUpgradeWindow);
	mNuclearGoodsCost					= GUIText::create(sf::FloatRect(35, 60, 20, 20), "0", mUpgradeWindow);
	mBuySpaceProgramText				= GUIText::create(sf::FloatRect(210, 114, 22, 22), "0", mUpgradeWindow);
	mBuySpaceProgramText->setAlignment("middle");
	mBuySpaceProgramText->setScale(0.9, 0.9);
	mSpaceProgramTechCost				= GUIText::create(sf::FloatRect(210, 60, 20, 20), "0", mUpgradeWindow);
	mSpaceProgramGoodsCost				= GUIText::create(sf::FloatRect(210, 25, 20, 20), "0", mUpgradeWindow);
	mBuySpyNetworkText					= GUIText::create(sf::FloatRect(389, 77, 22, 22), "0", mUpgradeWindow);
	mBuySpyNetworkText->setAlignment("middle");
	mBuySpyNetworkText->setScale(0.9, 0.9);
	mSpyNetworkTechCost					= GUIText::create(sf::FloatRect(400, 10, 20, 20), "0", mUpgradeWindow);
	mUpgradeWindow->setVisible(false);

	/*Export GUI Window med knappar*/
	mExportWindow						= GUIWindow::create(CommunistWindows["CommunistExportWindow"], mCommunistMainWindow);

	mExportTotalPriceText[0]			= GUIText::create(sf::FloatRect(221, 51, 56, 31), "1", mImportWindow);
	mExportTotalPriceText[1]			= GUIText::create(sf::FloatRect(221, 110, 56, 31), "1", mImportWindow);
	mExportTotalPriceText[2]			= GUIText::create(sf::FloatRect(221, 169, 56, 31), "1", mImportWindow);
	
	sf::Texture *buyField = &ResourceHandler::getInstance()->getTexture(std::string(""));

	mExportQuantityBackground[0]		= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(130, 56, 73, 27), buyField), mExportWindow);
	mExportQuantityBackground[1]		= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(130, 111, 73, 27), buyField), mExportWindow);
	mExportQuantityBackground[2]		= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(130, 170, 73, 27), buyField), mExportWindow);

	mExportFoodCost						= GUIEditField::create(sf::FloatRect(263, 52, 157, 28), GUIEditField::COM, "0", true, mExportWindow);
	mExportGoodsCost					= GUIEditField::create(sf::FloatRect(263, 107, 157, 28), GUIEditField::COM, "0", true, mExportWindow);
	mExportTechCost						= GUIEditField::create(sf::FloatRect(263, 166, 157, 28), GUIEditField::COM, "0", true, mExportWindow);

	for(int i = 0; i < sizeof(mExportQuantityBackground)/sizeof(mExportQuantityBackground[0]); i++)
	{
		mExportQuantityText[i] = GUIText::create(
			sf::FloatRect(mExportQuantityBackground[i]->getLocalX() + mExportQuantityBackground[i]->getWidth()/2, 
			mExportQuantityBackground[i]->getLocalY() + mExportQuantityBackground[i]->getHeight()/2,
			100, 50), "0", mExportWindow);
		mExportQuantityText[i]->setAlignment("middle");

	}


	mExportConfirmButton				= GUIButton::create(CommunistButtons["ExportConfirm"], mExportWindow);
	mExportConfirmButton->setSize(CommunistButtons["ExportConfirm"].first.width, CommunistButtons["ExportConfirm"].first.height);
	mExportWindow->setVisible(false);
	
	mImportWindow						= GUIWindow::create(CommunistWindows["CommunistImportWindow"], mCommunistMainWindow);
	
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
			sf::FloatRect(mImportBuyQuantityBackground[i]->getLocalX() + mImportBuyQuantityBackground[i]->getWidth()/2, 
			mImportBuyQuantityBackground[i]->getLocalY() + mImportBuyQuantityBackground[i]->getHeight()/2,
			100, 50), "0", mImportWindow);
		mImportBuyQuantityText[i]->setAlignment("middle");

	}

	mImportCostText[0]					= GUIText::create(sf::FloatRect(509, 51, 100, 50), "0", mImportWindow);
	mImportCostText[1]					= GUIText::create(sf::FloatRect(509, 110, 100, 50), "0", mImportWindow);
	mImportCostText[2]					= GUIText::create(sf::FloatRect(509, 169, 100, 50), "0", mImportWindow);

	mImportGotoExportButton				= GUIButton::create(CommunistButtons["ImportGotoExport"], mImportWindow);
	mImportGotoExportButton->setSize(CommunistButtons["ImportGotoExport"].first.width, CommunistButtons["ImportGotoExport"].first.height);
	mImportWindow->setVisible(false);


	mChooseGeneralWindow				= GUIWindow::create(CommunistWindows["ChooseGeneral"], mCommunistMainWindow);
	mPickedGeneralWindow				= GUIWindow::create(CommunistWindows["PickedGeneral"], mCommunistMainWindow);
	mPickedGeneralButton				= GUIButton::create(CommunistButtons["PickedGeneral"], mPickedGeneralWindow);
	sf::FloatRect pickedRect			= CommunistButtons["PickedGeneral"].first;
	mPickedGeneralPlaque				= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>
		(sf::FloatRect(pickedRect.left, pickedRect.top + pickedRect.height - 5, 0, 0),
		&GameManager::getInstance()->getGeneralPlaque(GameManager::getInstance()->getGeneral(generalCount))), mPickedGeneralWindow);

	mPickedGeneralWindow->setVisible(false);

	mFirstGeneralButton					= GUIButton::create(CommunistButtons["FirstGeneral"], mChooseGeneralWindow);
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

	mResourceIncomeWindow				= GUIWindow::create(CommunistWindows["ResourceIncome"], mCommunistMainWindow);
	mCloseResourceIncomeWindow			= GUIButton::create(CommunistWindows["CloseResourceIncome"], mResourceIncomeWindow);
	mFoodIncome							= GUIText::create(sf::FloatRect(50, 31, 0, 0), "", mResourceIncomeWindow);
	mGoodsIncome						= GUIText::create(sf::FloatRect(50, 70, 0, 0), "", mResourceIncomeWindow);
	mTechIncome							= GUIText::create(sf::FloatRect(50, 109, 0, 0), "", mResourceIncomeWindow);
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
	mTaxesIncomeText[1]					= GUIText::create(sf::FloatRect(331, 74, 0, 0), "0", mTaxesIncomeWindow);
	mTaxesIncomeText[1]->setScale(0.8, 0.8);
	mTaxesIncomeText[1]->setAlignment("left");

	mCloseTaxesIncomeWindow				= GUIButton::create(CommunistButtons["CloseTaxesIncome"], mTaxesIncomeWindow);
	mTaxesIncomeWindow->setVisible(false);

	mPopulationEatsFoodWindow			= GUIWindow::create(CommunistWindows["PopulationEatsFood"], mCommunistMainWindow);
	mPopulationEatsFoodText				= GUIText::create(sf::FloatRect(50, 50, 0, 0), "Example: Population eats 50 food \nPopulation grows to 51 million",  mPopulationEatsFoodWindow);
	mPopulationEatsFoodText->setScale(0.8, 0.8);
	mPopulationEatsFoodText->setAlignment("left");
	mClosePopulationEatsFoodWindow		= GUIButton::create(CommunistButtons["ClosePopulationEatsFood"], mPopulationEatsFoodWindow);
	mPopulationEatsFoodWindow->setVisible(false);

	mFoodImage[0]	= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(39, 17, 38, 38), &ResourceHandler::getInstance()->getTexture(std::string("Communist/food_image"))), mFiveYearPlanWindow);
	mFoodImage[1]	= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(70, 51, 35, 35), &ResourceHandler::getInstance()->getTexture(std::string("Capitalist/food_image"))), mImportWindow);
	mFoodImage[2]	= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(25, 58, 35, 35), &ResourceHandler::getInstance()->getTexture(std::string("Capitalist/food_image"))), mExportWindow);

	mGoodsImage[0]	= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(140, 17, 38, 38), &ResourceHandler::getInstance()->getTexture(std::string("Communist/goods_image"))), mFiveYearPlanWindow);
	mGoodsImage[1]	= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(70, 110, 35, 35), &ResourceHandler::getInstance()->getTexture(std::string("Capitalist/goods_image"))), mImportWindow);
	mGoodsImage[2]	= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(25, 113, 35, 35), &ResourceHandler::getInstance()->getTexture(std::string("Capitalist/goods_image"))), mExportWindow);

	mTechImage[0]	= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(241, 17, 38, 38), &ResourceHandler::getInstance()->getTexture(std::string("Communist/tech_image"))), mFiveYearPlanWindow);
	mTechImage[1]	= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(70, 169, 35, 35), &ResourceHandler::getInstance()->getTexture(std::string("Capitalist/tech_image"))), mImportWindow);
	mTechImage[2]	= GUIImage::create(std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(25, 173, 35, 35), &ResourceHandler::getInstance()->getTexture(std::string("Capitalist/tech_image"))), mExportWindow);

	mGeneralBiography = GUIText::create(sf::FloatRect(40, 290, 0, 0), "", mPickedGeneralWindow);
	mGeneralBiography->setScale(0.6, 0.6);

	/*
	 	Lägger in föräldernoden i vektorn som finns i GUIManager
	 	och kommer automatiskt få med sig alla barnnoder till denna
	 	vilket är alla GUIElement som finns i denna klass som har 
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
	//mChangeCityImage->setTexture(std::pair<sf::FloatRect, sf::Texture*>(mChangeCityImage->getRectangle(), CityImages[0])); 
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
			resourcesTotalCost[j + i*resources] = GUIText::create(sf::FloatRect(35 + 101 * i, 98 + 78 * j, 20, 20), "0 §", mFiveYearPlanWindow);
			resourcesTotalCost[j + i*resources]->setScale(0.7, 0.7);
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
			resourcesTotalCost[i]->setText(intToString(foodAmount * foodCost) + " §");
			totalYearCost[i]->setText(intToString(amount) + " §");
		});
		goodsFields[i]->setOnGuiChangeFunction([=]()
		{
			int foodAmount = stringToInt(foodFields[i]->getText());
			int goodsAmount = stringToInt(goodsFields[i]->getText());
			int techAmount = stringToInt(techFields[i]->getText());
			int amount = foodAmount * foodCost + goodsAmount * goodsCost + techAmount * techCost;
			resourcesTotalCost[i + resources]->setText(intToString(goodsAmount * goodsCost) + " §");
			totalYearCost[i]->setText(intToString(amount) + " §");
		});
		techFields[i]->setOnGuiChangeFunction([=]()
		{
			int foodAmount = stringToInt(foodFields[i]->getText());
			int goodsAmount = stringToInt(goodsFields[i]->getText());
			int techAmount = stringToInt(techFields[i]->getText());
			int amount = foodAmount * foodCost + goodsAmount * goodsCost + techAmount * techCost;
			resourcesTotalCost[i + resources*2]->setText(intToString(techAmount * techCost) + " §");
			totalYearCost[i]->setText(intToString(amount) + " §");
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
		mYearTwoTaxesText->setText(amount);
		mYearThreeTaxesText->setText(amount);
		mYearFourTaxesText->setText(amount);
		mYearFiveTaxesText->setText(amount);
		mIncomeYearOne->setText(intToString(amount * mPopulation) + " §");
		mIncomeYearTwo->setText(intToString(amount * mPopulation) + " §");
		mIncomeYearThree->setText(intToString(amount * mPopulation) + " §");
		mIncomeYearFour->setText(intToString(amount * mPopulation) + " §");
		mIncomeYearFive->setText(intToString(amount * mPopulation) + " §");
	});
	mYearOneRaiseTaxesButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearOneTaxesText->getText()) + taxChange;
		if(amount > 95)
			amount = 95;
		mYearOneTaxesText->setText(amount);
		mYearTwoTaxesText->setText(amount);
		mYearThreeTaxesText->setText(amount);
		mYearFourTaxesText->setText(amount);
		mYearFiveTaxesText->setText(amount);
		mIncomeYearOne->setText(intToString(amount * mPopulation) + " §");
		mIncomeYearTwo->setText(intToString(amount * mPopulation) + " §");
		mIncomeYearThree->setText(intToString(amount * mPopulation) + " §");
		mIncomeYearFour->setText(intToString(amount * mPopulation) + " §");
		mIncomeYearFive->setText(intToString(amount * mPopulation) + " §");

	});
	/*mYearTwoLowerTaxesButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearTwoTaxesText->getText()) - taxChange;
		if(amount < 5)
			amount = 5;
		mYearTwoTaxesText->setText(amount);
		mIncomeYearTwo->setText(intToString(amount * mPopulation) + " §");
	});
	mYearTwoRaiseTaxesButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearTwoTaxesText->getText()) + taxChange;
		if(amount > 95)
			amount = 95;
		mYearTwoTaxesText->setText(amount);
		mIncomeYearTwo->setText(intToString(amount * mPopulation) + " §");
	});
	mYearThreeLowerTaxesButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearThreeTaxesText->getText()) - taxChange;
		if(amount < 5)
			amount = 5;
		mYearThreeTaxesText->setText(amount);
		mIncomeYearThree->setText(intToString(amount * mPopulation) + " §");
	});
	mYearThreeRaiseTaxesButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearThreeTaxesText->getText()) + taxChange;
		if(amount > 95)
			amount = 95;
		mYearThreeTaxesText->setText(amount);
		mIncomeYearThree->setText(intToString(amount * mPopulation) + " §");
	});
	mYearFourLowerTaxesButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFourTaxesText->getText()) - taxChange;
		if(amount < 5)
			amount = 5;
		mYearFourTaxesText->setText(amount);
		mIncomeYearFour->setText(intToString(amount * mPopulation) + " §");
	});
	mYearFourRaiseTaxesButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFourTaxesText->getText()) + taxChange;
		if(amount > 95)
			amount = 95;
		mYearFourTaxesText->setText(amount);
		mIncomeYearFour->setText(intToString(amount * mPopulation) + " §");
	});
	mYearFiveLowerTaxesButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFiveTaxesText->getText()) - taxChange;
		if(amount < 5)
			amount = 5;
		mYearFiveTaxesText->setText(amount);
		mIncomeYearFive->setText(intToString(amount * mPopulation) + " §");
	});
	mYearFiveRaiseTaxesButton->setOnClickFunction([=]()
	{
		int amount = stringToInt(mYearFiveTaxesText->getText()) + taxChange;
		if(amount > 95)
			amount = 95;
		mYearFiveTaxesText->setText(amount);
		mIncomeYearFive->setText(intToString(amount * mPopulation) + " §");
	});*/
}

 /**/
void Communist::initializeGuiFunctions()
{
	fiveYearGuiFunctions();
	/*Fem års plan knappen på interface  */
	mCommunistFiveYearPlanButton->setOnClickFunction([=]()		
	{
		mCommunistMainWindow->setEnabled(false, true);

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

		float x = mPropagandaWindowFirst->getX() + mPropagandaWindowFirst->getRectangle().width/2;
		float y = mPropagandaWindowFirst->getY() + mPropagandaWindowFirst->getRectangle().height/2;
		GUIAnimation::move(mPropagandaWindowFirst, 100, sf::FloatRect(x, y, 0, 0), mPropagandaWindowFirst->getRectangle());
		for(std::vector<std::shared_ptr<GUIElement> >::size_type i = 0; i < mPropagandaWindowFirst->getChildVector().size(); ++i)
		{
			
			GUIAnimation::move(mPropagandaWindowFirst->getChildVector()[i], 100, sf::FloatRect(x, y, 0, 0), mPropagandaWindowFirst->getChildVector()[i]->getRectangle());
		}

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
		mCommunistMainWindow->setEnabled(true, true);
		
		mFiveYearPlanWindow->setVisible(false);
		mCommunistFiveYearPlanButton->setTexture(CommunistButtons["FiveYearPlan"]);
		if(mFiveYearPlanWindow->isEnabled())
		{
			updateAllResources();
			mTaxesIncomeWindow->setVisible(true);
			mTaxesIncomeWindow->setEnabled(true, true);
			mFiveYearPlanWindow->setEnabled(false);
		}
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
	});

	mPropagandaBuyGoodsButton->setOnClickFunction([=]()
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
	});
	mPropagandaBuyTechButton->setOnClickFunction([=]()
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
	});

	/*Upgrade och cancel för NuclearWeapon*/
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
			
	/*Upgrade och cancel för SpaceProgram*/
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

	/*Upgrade och cancel för SpyNetwork*/
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

	/*Stänger ned Upgradefönstret*/
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
		//GUIAnimation::fadeToColor(mChooseGeneralWindow, 1000, mChooseGeneralWindow->getColor(), sf::Color(255, 255, 255, 0));
		std::shared_ptr<GUIWindow> _chooseWindow = mChooseGeneralWindow;
		std::shared_ptr<GUIWindow> _pickedWindow = mPickedGeneralWindow;
		Timer::setTimer([=]()
		{
			_chooseWindow->setVisible(false);
			_pickedWindow->setEnabled(true, true);
		}, 1000, 1);
			_pickedWindow->setVisible(true);
			
			GUIAnimation::fadeToColor(_pickedWindow, 1000, sf::Color(255, 255, 255, 0), _pickedWindow->getColor());
		//}, 500, 1);

		mGeneral = GameManager::getInstance()->getGeneral(generalCount);

		mPickedGeneralButton->setTexture(std::pair<sf::FloatRect, sf::Texture*>
			(mPickedGeneralButton->getRectangle(), mGeneral->getTexture()));

		mPickedGeneralPlaque->setTexture(std::pair<sf::FloatRect, sf::Texture*>
			(mPickedGeneralPlaque->getRectangle(), mFirstGeneralPlaque->getTexture()));

		mGeneralBiography->setText(mGeneral->getBiography());
		mGeneral->playSlogan();

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

	/*Export knappen på interface*/
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
	minusTexture[0] = &ResourceHandler::getInstance()->getTexture(std::string("Communist/kom_texture_button_minus1"));
	minusTexture[1] = &ResourceHandler::getInstance()->getTexture(std::string("Communist/kom_texture_button_minus5"));
	minusTexture[2] = &ResourceHandler::getInstance()->getTexture(std::string("Communist/kom_texture_button_minus10"));

	sf::Texture *plusTexture[3];
	plusTexture[0] = &ResourceHandler::getInstance()->getTexture(std::string("Communist/kom_texture_button_plus1"));
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
				std::pair<sf::FloatRect, sf::Texture*>(sf::FloatRect(x + mImportBuyQuantityBackground[i]->getWidth() + h*plusTexture[h]->getSize().x, y, plusTexture[h]->getSize().x, plusTexture[h]->getSize().y),
				plusTexture[h]),
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
				//int cost = quantity * (foodAvailable * foodPrice);
				//mImportCostText[i]->setText(intToString(cost));
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
		mCommunistMainWindow->setEnabled(true, true);
		mCommunistTradeButton->setTexture(CommunistButtons["Export"]);
		//mCurrency -= stringToInt(mImportTotalCostText->getText());

		mExportedFood = stringToInt(mExportQuantityText[0]->getText());
		mExportedGoods = stringToInt(mExportQuantityText[1]->getText());
		mExportedTech = stringToInt(mExportQuantityText[2]->getText());
		mFood = mFoodPreviousRound - mExportedFood;
		mGoods = mGoodsPreviousRound - mExportedGoods;
		mTech = mTechPreviousRound - mExportedTech;

		mExportedFoodPrice = stringToInt(mExportFoodCost->getText());
		mExportedGoodsPrice = stringToInt(mExportGoodsCost->getText());
		mExportedTechPrice = stringToInt(mExportTechCost->getText());

		std::cout<<"communist Exported goods: "<<mExportedGoods<<std::endl;
	});

	mCloseTaxesIncomeWindow->setOnClickFunction([=]()
	{
		mTaxesIncomeWindow->setVisible(false);
		mTaxesIncomeWindow->setEnabled(false, true);
		mResourceIncomeWindow->setVisible(true);
		mResourceIncomeWindow->setEnabled(true, true);
	});

	mCloseResourceIncomeWindow->setOnClickFunction([=]()
	{
		mResourceIncomeWindow->setVisible(false);
		mResourceIncomeWindow->setEnabled(false, true);
		mCommunistMainWindow->setEnabled(true, true);
	});

	/*nästa runda*/
	mCommunistEndTurnButton->setOnClickFunction([=]()	
	{
		updateFood(mPopulationEatsFoodText);

		mPopulationEatsFoodWindow->setVisible(true);
		mCommunistEndTurnButton->setTexture(CommunistButtons["EndTurnIsPressed"]);
	});
	
	mClosePopulationEatsFoodWindow->setOnClickFunction([=]()
	{
		mPopulationEatsFoodWindow->setVisible(false);
		/*if(mTaxes < mCurrentTax)
			setPatriotism(getPatriotism() + 2);
		else if(mTaxes > mCurrentTax)
			setPatriotism(getPatriotism() - 3);*/
		
		//mCommunistEndTurnButton->setTexture(CommunistButtons["EndTurnIsPressed"]);
		//mTaxes = mCurrentTax;
		GameManager::getInstance()->nextRound();  
		stopMusic();
	});

}

void Communist::upgradeWindowText()
{
	int spaceProgram			= stringToInt(mBuySpaceProgramText->getText());
	int spyNetwork				= stringToInt(mBuySpyNetworkText->getText());
	int nuclearGoodsPrice		= 10;
	int nuclearTechPrice		= 5;
	int spaceProgramGoodsPrice  = (spaceProgram + 1) * 5;
	int spaceProgramTechPrice	= (spaceProgram + 1) * 10;
	int spyNetworkTechPrice		= (spyNetwork + 1) * 10;

	mNuclearGoodsCost->setText("Goods " +intToString(nuclearGoodsPrice));
	mNuclearTechCost->setText("Tech " +intToString(nuclearTechPrice));
	mSpaceProgramGoodsCost->setText("Goods " +intToString(spaceProgramGoodsPrice));
	mSpaceProgramTechCost->setText("Tech " +intToString(spaceProgramTechPrice));
	mSpyNetworkTechCost->setText("Tech " +intToString(spyNetworkTechPrice));
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

void Communist::resourceIncome()
{
	mFood  += getYearlyFood(mRound);
	mGoods += getYearlyGoods(mRound);
	mTech  += getYearlyTech(mRound);
	mFoodIncome->setText("You get " + intToString(getYearlyFood(mRound)) + " food");
	mGoodsIncome->setText("You get " + intToString(getYearlyGoods(mRound)) + " goods");
	mTechIncome->setText("You get " + intToString(getYearlyTech(mRound)) + " tech");
}

void Communist::showGUI()
{
	mCommunistMainWindow->setVisible(true);
	//playMusic();
}

void Communist::hideGUI()
{
	mCommunistMainWindow->setVisible(false);
} 