#include "SuperPower.h"
#include "GUIText.h"


SuperPower::SuperPower() : 
	mPopulation(50),			//Befolkning i miljoner
	mPatriotism(20),
	mCurrency(0),
	mTaxes(30),
	mFood(0),
	mExportedFood(0),
	mExportedFoodPrice(0),
	mTech(0),
	mExportedTech(0),
	mExportedTechPrice(0),
	mGoods(0),
	mExportedGoods(0),
	mExportedGoodsPrice(0),
	mSpyNetwork(0),
	mSpaceProgram(0),
	mNuclearWeapon(10),
	mTaxDecreased(false),
	mPatriotismPreviousRound(mPatriotism),
	mCurrencyPreviousRound(mCurrency),
	mPopulationPreviousRound(mPopulation),
	mFoodPreviousRound(mFood),
	mGoodsPreviousRound(mGoods),
	mTechPreviousRound(mTech),
	mExportedFoodPreviousRound(mExportedFood),
	mExportedGoodsPreviousRound(mExportedGoods),
	mExportedTechPreviousRound(mExportedTech),
	mTaxesPreviousRound(mTaxes),
	mSpyNetworkPreviousRound(mSpyNetwork),
	mNuclearWeaponPreviousRound(mNuclearWeapon),
	mSpaceProgramPreviousRound(mSpaceProgram),
	mExportedFoodSold(0), 
	mExportedGoodsSold(0), 
	mExportedTechSold(0)
{
}

void SuperPower::reset()
{
	mPopulation								= 50;			//Befolkning i miljoner
	mPatriotism								= 20;
	mCurrency								= 500;
	mTaxes									= 30;
	mFood									= 0;
	mExportedFood							= 0;
	mExportedFoodPrice						= 0;
	mTech									= 100;
	mExportedTech							= 0;
	mExportedTechPrice						= 0;
	mGoods									= 100;
	mExportedGoods							= 0;
	mExportedGoodsPrice						= 0;
	mSpyNetwork								= 0;
	mSpaceProgram							= 0;
	mNuclearWeapon							= 10;
	mTaxDecreased							= false;
	mPatriotismPreviousRound				= mPatriotism;
	mCurrencyPreviousRound                  = mCurrency;
	mPopulationPreviousRound				= mPopulation;
	mFoodPreviousRound						= mFood;
	mGoodsPreviousRound						= mGoods;
	mTechPreviousRound					    = mTech;
	mExportedFoodPreviousRound				= mExportedFood;
	mExportedGoodsPreviousRound				= mExportedGoods;
	mExportedTechPreviousRound				= mExportedTech;
	mTaxesPreviousRound						= mTaxes;
	mSpyNetworkPreviousRound				= mSpyNetwork;
	mNuclearWeaponPreviousRound				= mNuclearWeapon;
	mSpaceProgramPreviousRound				= mSpaceProgram;
	mExportedFoodSold						= 0;
	mExportedGoodsSold						= 0;
	mExportedTechSold						= 0;
}

SuperPower::~SuperPower()
{
}

int SuperPower::getExportedFood()
{
	return mExportedFood;
}

int SuperPower::getExportedFoodPrice()
{
	return mExportedFoodPrice;
}

int SuperPower::getExportedGoods()
{
	return mExportedGoods;
}

int SuperPower::getExportedGoodsPrice()
{
	return mExportedGoodsPrice;
}

int SuperPower::getExportedTech()
{
	return mExportedTech;
}

int SuperPower::getExportedTechPrice()
{
	return mExportedTechPrice;
}

/*Funktioner som returnerar medlemsvariablernas v�rden*/
int SuperPower::getCurrency()
{
	return mCurrency;
}

int SuperPower::getFood()
{
	return mFood;
}

PoliticalType SuperPower::getType()
{
	return mType;
}

int SuperPower::getGoods()
{
	return mGoods;
}

int SuperPower::getPopulation()
{
	return mPopulation;
}

int SuperPower::getTaxes()
{
	return mTaxes;
}

int SuperPower::getTech()
{
	return mTech;
}

int SuperPower::getNuclearWeapon()
{
	return mNuclearWeapon;
}

int SuperPower::getSpaceProgram()
{
	return mSpaceProgram;
}

int SuperPower::getSpyNetwork()
{
	return mSpyNetwork;
}

int SuperPower::getRound()
{
	return mRound;
}

void SuperPower::setRound(int round)
{
	mRound = round;
}
//-----------------------------------------------------------

/*Uppgraderar mCurrency med mTaxes multiplicerat med mPopulation*/
void SuperPower::getTaxIncome()
{
	mCurrency += mTaxes * mPopulation;
}

/*Kontrollerar ifall det finns nog med mat till hela befolkningen*/
bool SuperPower::enoughFood()
{
	return mFood >= mPopulation;
}

void SuperPower::updateFood(std::shared_ptr<GUIText> text)
{
	/*	Om m�ngden mat �r tillr�cklig blir m�ngden mat subtraherad med antalet befolkningen i miljoner
		Populationen �kar och om landet har mer eller lika mycket pengar som befolkning blir mIncreasePopulation true.
		Den anv�nds sedan f�r att ge m�jligheten att betala en viss summa f�r att �ka befolkningen*/
	if(enoughFood())
	{
		mPopulation++;
		if(mCurrency >= mPopulation)
		{
			mIncreasePopulation = true;
			text->setText("There is plenty of food for the whole population.\n\nThey now grow to " + intToString(mPopulation) +" million.\n\n"
				"Do you want to stimulate the population growth\nfor " + intToString(mPopulation) + " �?");
		}
		else
			text->setText("There is plenty of food for the whole population.\n\nThey now grow to " + intToString(mPopulation) +" million.");
	}
	/*	Om m�ngden mat �r mindre �n h�flten av befolkningen tilldelas mFood noll
		mPatriotism subtraheras �ven med tv�*/
	else if(mFood == 0)
	{
		text->setText("There is no food at all for the population.\n\nThis is upsetting.");
		mIncreasePopulation = false;
	}
	/*	Om det inte finns tillr�ckligt med mat och mFood inte �r lika med noll
		�kar inte patriotismen men mFood tilldelas noll*/
	else if(mFood > mPopulation / 2)
	{
		text->setText("There is not quite enough food for \nthe population.");
		mIncreasePopulation = false;
	}
	/*	Om inga av det �verst�ende st�mmer, inneb�r det att maten �r lika med noll och befolkningen inte f�tt n�gon mat
		Detta ger minus fyra i patriotism*/
	else
	{
		text->setText("There is very little food for the population.\n\nThey starve and grumble.");
		mIncreasePopulation = false;
	}
}
/*Kontrollerar ifall det �r m�jligt att �ka sin population*/
bool SuperPower::enableToIncreasePopulation()
{
	return mIncreasePopulation;
}
/*	mCurrency subtraheras med mPopulation, befolkningen i miljoner.
	�kar befolkningen med ett, en miljon.
	Och tilldelar boolean:en mIncreasePopulation till false.*/
void SuperPower::increasePopulation()
{
	mCurrency -= mPopulation;

	++mPopulation;

	mIncreasePopulation = false;
}