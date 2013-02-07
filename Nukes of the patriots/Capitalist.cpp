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
	//CapitalistMusic["CapitalistMainTheme"]->play();
}

std::shared_ptr<President> Capitalist::getPresident()
{
	return mPresident;
}

int Capitalist::increaseTaxCost(int currentTax)
{						
	currentTax += taxChange; //  + mPresident->getPatriotismTaxModifier();

	return currentTax;
}

int Capitalist::decreaseTaxCost(int currentTax)
{
	currentTax -= taxChange; // + mPresident->getPatriotismTaxModifier();

	return currentTax;
}

void Capitalist::setPresident(std::shared_ptr<President> president)
{
	mPresident = president;
	
	foodCost	+= president->getFoodPriceModifier();
	goodsCost	+= president->getGoodsPriceModifier();
	techCost	+= president->getTechPriceModifier();
}

void Capitalist::setFood(int amount)
{
	mFoodUpdate += amount;
	mCurrencyUpdate -= amount * foodCost;
}

void Capitalist::setGoods(int amount)
{
	mGoodsUpdate += amount;
	mCurrencyUpdate -= amount * goodsCost;
}

void Capitalist::setTech(int amount)
{
	mTechUpdate += amount;
	mCurrencyUpdate -= amount * techCost;
}

//-----------------------------------------------------------
/*	
	Uppgraderar mNuclearWeapon med ett
	Kostar 10 mGoods och 5 mTech
										*/
void Capitalist::upgradeNuclearWeapon()
{
	mGoodsUpdate	-= 10 * mPresident->getNuclearPriceModifier();
	mTechUpdate		-= 5  * mPresident->getNuclearPriceModifier();

	++mNuclearWeaponUpdate;
}

/*	
	Uppgraderar mSpaceProgram med ett
	Kostar 5 mGoods multiplicerat med den nuvarande nivån
	och 10 mTech multiplicerat med den nuvarande nivån
															*/
void Capitalist::upgradeSpaceProgram()
{
	if(mSpaceProgramUpdate > 0)
	{
		mGoodsUpdate	-= 5 * mSpaceProgramUpdate * mPresident->getSpacePriceModifier();
		mTechUpdate		-= 10 * mSpaceProgramUpdate * mPresident->getSpacePriceModifier();
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
void Capitalist::upgradeSpyNetwork()
{
	if(mSpyNetworkUpdate > 0)
	{
		mTechUpdate -= 10 * mSpyNetworkUpdate * mPresident->getSpyPriceModifier();
	}
	else
	{
		mTechUpdate -= 10;
	}
	++mSpyNetworkUpdate;
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

	mCapitalistMainWindow				= std::make_shared<GUIWindow>(CapitalistWindows["CapitalistInterface"]);
	mCapitalistPresident				= std::make_shared<GUIButton>(CapitalistButtons["President"], mCapitalistMainWindow);
	mCapitalistTaxesButton				= std::make_shared<GUIButton>(CapitalistButtons["Taxes"], mCapitalistMainWindow);
	mCapitalistResourceButton			= std::make_shared<GUIButton>(CapitalistButtons["Resource"], mCapitalistMainWindow);
	mCapitalistUpgradeButton			= std::make_shared<GUIButton>(CapitalistButtons["Upgrade"], mCapitalistMainWindow);
	mCapitalistExportButton				= std::make_shared<GUIButton>(CapitalistButtons["Export"], mCapitalistMainWindow);
	mCapitalistEndTurnButton			= std::make_shared<GUIButton>(CapitalistButtons["EndTurn"], mCapitalistMainWindow);

	/*GUI text för utskrift av värden på komunisternas interface*/
	mNuclearText						= std::make_shared<GUIText>(sf::FloatRect(962, 16, 40, 40), intToString(getNuclearWeapon()), mCapitalistMainWindow);
	mSpaceText							= std::make_shared<GUIText>(sf::FloatRect(962, 228, 40, 40), intToString(getSpaceProgram()), mCapitalistMainWindow);
	mSpyText							= std::make_shared<GUIText>(sf::FloatRect(962, 440, 40, 40), intToString(getSpyNetwork()), mCapitalistMainWindow);
	mFoodText							= std::make_shared<GUIText>(sf::FloatRect(160, 16, 40, 40), intToString(getFood()), mCapitalistMainWindow);
	mGoodsText							= std::make_shared<GUIText>(sf::FloatRect(160, 228, 40, 40), intToString(getGoods()), mCapitalistMainWindow);
	mTechText							= std::make_shared<GUIText>(sf::FloatRect(160, 440, 40, 40), intToString(getTech()), mCapitalistMainWindow);

	mTaxesWindow						= std::make_shared<GUIWindow>(CapitalistWindows["CapitalistTaxesWindow"], mCapitalistMainWindow);
	mLowerTaxesButton					= std::make_shared<GUIButton>(CapitalistButtons["LowerTaxes"], mTaxesWindow);
	mRaiseTaxesButton					= std::make_shared<GUIButton>(CapitalistButtons["RaiseTaxes"], mTaxesWindow);
	mTaxesCloseButton					= std::make_shared<GUIButton>(CapitalistButtons["CloseTaxes"], mTaxesWindow);
	mTaxesWindow->setVisible(false);

	mResourceWindow						= std::make_shared<GUIWindow>(CapitalistWindows["CapitalistResourceWindow"], mCapitalistMainWindow);
	mLowerFoodByTenButton				= std::make_shared<GUIButton>(CapitalistButtons["LowerFoodByTen"], mResourceWindow);
	mLowerFoodByFiveButton				= std::make_shared<GUIButton>(CapitalistButtons["LowerFoodByFive"], mResourceWindow);
	mLowerFoodByOneButton				= std::make_shared<GUIButton>(CapitalistButtons["LowerFoodByOne"], mResourceWindow);
	mRaiseFoodByOneButton				= std::make_shared<GUIButton>(CapitalistButtons["RaiseFoodByOne"], mResourceWindow);
	mRaiseFoodByFiveButton				= std::make_shared<GUIButton>(CapitalistButtons["RaiseFoodByFive"], mResourceWindow);
	mRaiseFoodByTenButton				= std::make_shared<GUIButton>(CapitalistButtons["RaiseFoodByTen"], mResourceWindow);

	mLowerGoodsByTenButton				= std::make_shared<GUIButton>(CapitalistButtons["LowerGoodsByTen"], mResourceWindow);
	mLowerGoodsByFiveButton				= std::make_shared<GUIButton>(CapitalistButtons["LowerGoodsByFive"], mResourceWindow);
	mLowerGoodsByOneButton				= std::make_shared<GUIButton>(CapitalistButtons["LowerGoodsByOne"], mResourceWindow);
	mRaiseGoodsByOneButton				= std::make_shared<GUIButton>(CapitalistButtons["RaiseGoodsByOne"], mResourceWindow);
	mRaiseGoodsByFiveButton				= std::make_shared<GUIButton>(CapitalistButtons["RaiseGoodsByFive"], mResourceWindow);
	mRaiseGoodsByTenButton				= std::make_shared<GUIButton>(CapitalistButtons["RaiseGoodsByTen"], mResourceWindow);

	mLowerTechByTenButton				= std::make_shared<GUIButton>(CapitalistButtons["LowerTechByTen"], mResourceWindow);
	mLowerTechByFiveButton				= std::make_shared<GUIButton>(CapitalistButtons["LowerTechByFive"], mResourceWindow);
	mLowerTechByOneButton				= std::make_shared<GUIButton>(CapitalistButtons["LowerTechByOne"], mResourceWindow);
	mRaiseTechByOneButton				= std::make_shared<GUIButton>(CapitalistButtons["RaiseTechByOne"], mResourceWindow);
	mRaiseTechByFiveButton				= std::make_shared<GUIButton>(CapitalistButtons["RaiseTechByFive"], mResourceWindow);
	mRaiseTechByTenButton				= std::make_shared<GUIButton>(CapitalistButtons["RaiseTechByTen"], mResourceWindow);
	mResourceCloseButton				= std::make_shared<GUIButton>(CapitalistButtons["CloseResource"], mResourceWindow);
	mResourceWindow->setVisible(false);

	mUpgradeWindow						= std::make_shared<GUIWindow>(CapitalistWindows["CapitalistUpgradeWindow"], mCapitalistMainWindow);
	mUpgradeNuclearWeaponButton		    = std::make_shared<GUIButton>(CapitalistButtons["UpgradeNuclearWeapon"], mUpgradeWindow);
	mUpgradeSpaceProgramButton			= std::make_shared<GUIButton>(CapitalistButtons["UpgradeSpaceProgram"], mUpgradeWindow);
	mUpgradeSpyNetworkButton			= std::make_shared<GUIButton>(CapitalistButtons["UpgradeSpyNetwork"], mUpgradeWindow);
	mUpgradeCloseButton					= std::make_shared<GUIButton>(CapitalistButtons["CloseUpgrade"], mUpgradeWindow);
	mUpgradeWindow->setVisible(false);

	mExportWindow						= std::make_shared<GUIWindow>(CapitalistWindows["CapitalistExportWindow"], mCapitalistMainWindow);
	mExportLowerFoodButton				= std::make_shared<GUIButton>(CapitalistButtons["LowerFood"], mExportWindow); 
	mExportRaiseFoodButton				= std::make_shared<GUIButton>(CapitalistButtons["RaiseFood"], mExportWindow);
	mExportLowerGoodsButton				= std::make_shared<GUIButton>(CapitalistButtons["LowerGoods"], mExportWindow);
	mExportRaiseGoodsButton				= std::make_shared<GUIButton>(CapitalistButtons["RaiseGoods"], mExportWindow);
	mExportLowerTechButton				= std::make_shared<GUIButton>(CapitalistButtons["LowerTech"], mExportWindow);
	mExportRaiseTechButton				= std::make_shared<GUIButton>(CapitalistButtons["RaiseTech"], mExportWindow);
	mExportCloseButton					= std::make_shared<GUIButton>(CapitalistButtons["CloseExport"], mExportWindow);
	mExportWindow->setVisible(false);
	

	mChoosePresidentWindow				= std::make_shared<GUIWindow>(CapitalistWindows["ChoosePresident"], mCapitalistMainWindow);
	mPickedPresidentWindow				= std::make_shared<GUIWindow>(CapitalistWindows["PickedPresident"], mCapitalistMainWindow);
	mPickedPresidentWindow->setVisible(false);
	mFirstPresidentButton				= std::make_shared<GUIButton>(CapitalistButtons["FirstPresident"], mChoosePresidentWindow);
	mSecondPresidentButton				= std::make_shared<GUIButton>(CapitalistButtons["SecondPresident"], mChoosePresidentWindow);
	mPickedPresidentButton				= std::make_shared<GUIButton>(CapitalistButtons["PickedPresident"], mPickedPresidentWindow);
	mClosePresidentWindow				= std::make_shared<GUIButton>(CapitalistButtons["ClosePresident"], mChoosePresidentWindow);
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
	mLowerTaxesButton->setOnClickFunction([=]()					{ mTaxesUpdate = decreaseTaxCost(mTaxesUpdate); });
	mRaiseTaxesButton->setOnClickFunction([=]()					{ mTaxesUpdate = increaseTaxCost(mTaxesUpdate); });

	mCapitalistResourceButton->setOnClickFunction([=]()			{ mResourceWindow->setVisible(true); });
	mLowerFoodByTenButton->setOnClickFunction([=]()				{ setFood(-10);});			
	mLowerFoodByFiveButton->setOnClickFunction([=]()			{ setFood(-5);});				
	mLowerFoodByOneButton->setOnClickFunction([=]()				{ setFood(-1);});				
	mRaiseFoodByOneButton->setOnClickFunction([=]()				{ setFood(1);});				
	mRaiseFoodByFiveButton->setOnClickFunction([=]()			{ setFood(5);});				
	mRaiseFoodByTenButton->setOnClickFunction([=]()				{ setFood(10);});	

	mLowerGoodsByTenButton->setOnClickFunction([=]()			{ setGoods(-10);});	
	mLowerGoodsByFiveButton->setOnClickFunction([=]()			{ setGoods(-5);	});	
	mLowerGoodsByOneButton->setOnClickFunction([=]()			{ setGoods(-1);	});	
	mRaiseGoodsByOneButton->setOnClickFunction([=]()			{ setGoods(1);}	);	
	mRaiseGoodsByFiveButton->setOnClickFunction([=]()			{ setGoods(5);}	);		
	mRaiseGoodsByTenButton->setOnClickFunction([=]()			{ setGoods(10);	});

	mLowerTechByTenButton->setOnClickFunction([=]()				{ setTech(-10);	});	
	mLowerTechByFiveButton->setOnClickFunction([=]()			{ setTech(-5);	});	
	mLowerTechByOneButton->setOnClickFunction([=]()				{ setTech(-1);	});	
	mRaiseTechByOneButton->setOnClickFunction([=]()				{ setTech(1);	});		
	mRaiseTechByFiveButton->setOnClickFunction([=]()			{ setTech(5);	});		
	mRaiseTechByTenButton->setOnClickFunction([=]()				{ setTech(10);	});	

	mUpgradeNuclearWeaponButton->setOnClickFunction([=]()		{ upgradeNuclearWeapon(); });		
	mUpgradeSpaceProgramButton->setOnClickFunction([=]()		{ upgradeSpaceProgram();  });		
	mUpgradeSpyNetworkButton->setOnClickFunction([=]()			{ upgradeSpyNetwork();    });		

	/**/
	mCapitalistUpgradeButton->setOnClickFunction([=]()			{ mUpgradeWindow->setVisible(true); });

	mCapitalistExportButton->setOnClickFunction([=]()			{ mExportWindow->setVisible(true); });

	mCapitalistEndTurnButton->setOnClickFunction([=]()			{ std::cout << "NEJ ELLER JO";  GameManager::getInstance()->nextRound();  });


	mTaxesCloseButton->setOnClickFunction([=]()					{ mTaxesWindow->setVisible(false); 
																  mTaxes = mTaxesUpdate; std::cout << mTaxes << "\nGAY" << std::endl; });
	/*Stänger ner resources fönstret "Okay-knappen"*/
	mResourceCloseButton->setOnClickFunction([=]()				
	{ 
		mResourceWindow->setVisible(false); 														 
		mFood = mFoodUpdate;													
		mGoods = mGoodsUpdate;														 
		mTech = mTechUpdate; 														
		std::cout << "food: " <<mFood << '\n' << "goods: "<< mGoods<<'\n' << "tech: " << mTech<<'\n'; 
	});
	/*Stänger ner upgrade fönstret "Okay-knappen"*/
	mUpgradeCloseButton->setOnClickFunction([=]()				
	{ 
		mUpgradeWindow->setVisible(false); 														 
		mNuclearWeapon = mNuclearWeaponUpdate;														  
		mSpaceProgram	 = mSpaceProgramUpdate;														  
		mSpyNetwork	 = mSpyNetworkUpdate;															  
		std::cout << "NW " << mNuclearWeapon << '\n' << "SP " << mSpaceProgram <<'\n' << "SN " << mSpyNetwork <<'\n'; 
	});
	/*Stänger ner Export fönster "Okay-knappen"*/
	mExportCloseButton->setOnClickFunction([=]()				{ mExportWindow->setVisible(false); });

	mCapitalistPresident->setOnClickFunction([=]()				{});
	
	/*Val av president bild 1*/
	mFirstPresidentButton->setOnClickFunction([=]()				
	{ 
		mChoosePresidentWindow->setVisible(false); mPickedPresidentWindow->setVisible(true); 
		mPresident = mFirstPresident; 
		mPickedPresidentButton->setTexture(std::pair<sf::FloatRect, sf::Texture*>(mPickedPresidentButton->getRectangle(), mPresident->getTexture())); 
	});

	/*Val av president bild 2*/
	mSecondPresidentButton->setOnClickFunction([=]()			
	{ 
		mChoosePresidentWindow->setVisible(false); mPickedPresidentWindow->setVisible(true); 
		mPresident = mSecondPresident; 
		mPickedPresidentButton->setTexture(std::pair<sf::FloatRect, sf::Texture*>(mPickedPresidentButton->getRectangle(), mPresident->getTexture())); 
	});			
	

	mClosePresidentWindow->setOnClickFunction([=]()				
	{ 
		mChoosePresidentWindow->setVisible(false); 
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