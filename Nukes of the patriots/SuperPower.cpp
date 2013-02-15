#include "SuperPower.h"


SuperPower::SuperPower() : 
	mPopulation(50),			//Befolkning i miljoner
	mPatriotism(20),
	mCurrency(500), 
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
	mTaxDecreased(false)
{
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

/*Funktioner som returnerar medlemsvariablernas värden*/
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
	if(mFood >= mPopulation)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void SuperPower::updateFood()
{
	/*	Om mängden mat är tillräcklig blir mängden mat subtraherad med antalet befolkningen i miljoner
		Populationen ökar och om landet har mer eller lika mycket pengar som befolkning blir mIncreasePopulation true.
		Den används sedan för att ge möjligheten att betala en viss summa för att öka befolkningen*/
	if(enoughFood())
	{
		mFood -= mPopulation;
		++mPopulation;
		if(mCurrency >= mPopulation)
		{
			mIncreasePopulation = true;
		}
	}
	/*	Om mängden mat är mindre än häflten av befolkningen tilldelas mFood noll
		mPatriotism subtraheras även med två*/
	else if(mFood <= mPopulation / 2)
	{
		mFood = 0;
		mPatriotism -= 2;
	}
	/*	Om det inte finns tillräckligt med mat och mFood inte är lika med noll
		ökar inte patriotismen men mFood tilldelas noll*/
	else if(!enoughFood() && mFood != 0)
	{
		mFood = 0;
	}
	/*	Om inga av det överstående stämmer, innebär det att maten är lika med noll och befolkningen inte fått någon mat
		Detta ger minus fyra i patriotism*/
	else
	{
		mFood = 0;
		mPatriotism -= 4;
	}
}
/*Kontrollerar ifall det är möjligt att öka sin population*/
bool SuperPower::enableToIncreasePopulation()
{
	return mIncreasePopulation;
}
/*	mCurrency subtraheras med mPopulation, befolkningen i miljoner.
	Ökar befolkningen med ett, en miljon.
	Och tilldelar boolean:en mIncreasePopulation till false.*/
void SuperPower::increasePopulation()
{
	mCurrency -= mPopulation;

	++mPopulation;

	mIncreasePopulation = false;
}