#include "SuperPower.h"


SuperPower::SuperPower() : 
	mPopulation(50),			//Befolkning i miljoner
	mPatriotism(20),
	mTaxes(30),
	mFood(0),
	mTech(0),
	mGoods(0),
	mSpyNetwork(0),
	mSpaceProgram(0),
	mNuclearWeapon(10)
{
	mCurrencyUpdate			= mCurrency;
	mFoodUpdate				= mFood;
	mGoodsUpdate			= mGoods;
	mTechUpdate				= mTech;
	mTaxesUpdate			= mTaxes;
	mNuclearWeaponUpdate	= mNuclearWeapon;
	mSpaceProgramUpdate		= mSpaceProgram;
	mSpyNetworkUpdate		= mSpyNetwork;
}


SuperPower::~SuperPower()
{
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
	/*	Om m�ngden mat �r tillr�cklig blir m�ngden mat subtraherad med antalet befolkningen i miljoner
		Populationen �kar och om landet har mer eller lika mycket pengar som befolkning blir mIncreasePopulation true.
		Den anv�nds sedan f�r att ge m�jligheten att betala en viss summa f�r att �ka befolkningen*/
	if(enoughFood())
	{
		mFood -= mPopulation;
		++mPopulation;
		if(mCurrency >= mPopulation)
		{
			mIncreasePopulation = true;
		}
	}
	/*	Om m�ngden mat �r mindre �n h�flten av befolkningen tilldelas mFood noll
		mPatriotism subtraheras �ven med tv�*/
	else if(mFood <= mPopulation / 2)
	{
		mFood = 0;
		mPatriotism -= 2;
	}
	/*	Om det inte finns tillr�ckligt med mat och mFood inte �r lika med noll
		�kar inte patriotismen men mFood tilldelas noll*/
	else if(!enoughFood() && mFood != 0)
	{
		mFood = 0;
	}
	/*	Om inga av det �verst�ende st�mmer, inneb�r det att maten �r lika med noll och befolkningen inte f�tt n�gon mat
		Detta ger minus fyra i patriotism*/
	else
	{
		mFood = 0;
		mPatriotism -= 4;
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