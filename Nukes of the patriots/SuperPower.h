#ifndef SUPERPOWER_H
#define SUPERPOWER_H

#include <memory>

enum PoliticalType
{
	CAPITALIST, COMMUNIST
}; 

class SuperPower
{
public:
	SuperPower();
	~SuperPower();

	int			getFood();
	int			getGoods();
	int			getTech();
	int			getTaxes();
	int			getPopulation();
	int			getNuclearWeapon();
	int			getSpaceProgram();
	int			getSpyNetwork();
	int			getCurrency();
	int			getRound();

	PoliticalType getType();

	void		setRound(int round);

	void		getTaxIncome();
	void		updateFood();
	bool		enoughFood();

	bool		enableToIncreasePopulation();
	void		increasePopulation();
	
	virtual void		upgradeNuclearWeapon() = 0;
	virtual void		upgradeSpaceProgram() = 0;
	virtual void		upgradeSpyNetwork() = 0;
	virtual int			increaseTaxCost(int currentTax) = 0;
	virtual int			decreaseTaxCost(int currentTax) = 0;
	virtual void		setFood(int value) = 0;
	virtual void		setGoods(int value) = 0;
	virtual void		setTech(int value) = 0;
	virtual void		chooseLeader(){};
	virtual void		showGUI() = 0;
	virtual void		hideGUI() = 0;
	virtual void		playMusic() {};
protected:
	PoliticalType mType;

	int mRound;

	int	mPatriotism;
	int	mCurrency, mCurrencyUpdate;
	int	mPopulation;
	int	mFood, mFoodUpdate;
	int	mGoods, mGoodsUpdate;
	int	mTech, mTechUpdate;
	int	mTaxes, mTaxesUpdate;
	int	mSpyNetwork, mSpyNetworkUpdate;
	int	mNuclearWeapon, mNuclearWeaponUpdate;
	int	mSpaceProgram, mSpaceProgramUpdate;
	bool mIncreasePopulation;
};

#endif