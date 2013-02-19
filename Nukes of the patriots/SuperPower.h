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

	int			getExportedFood();
	int			getExportedFoodPrice();
	int			getExportedGoods();
	int			getExportedGoodsPrice();
	int			getExportedTech();
	int			getExportedTechPrice();
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

	int			getPatriotismPreviousRound()		{ return mPatriotismPreviousRound; }; 
	int			getCurrencyPreviousRound()			{ return mCurrencyPreviousRound; };
	int			getPopulationPreviousRound()		{ return mPopulationPreviousRound; };
	int			getFoodPreviousRound()				{ return mFoodPreviousRound; };
	int			getGoodsPreviousRound()				{ return mGoodsPreviousRound; };
	int			getTechPreviousRound()				{ return mTechPreviousRound; };
	int			getExportedFoodPreviousRound()		{ return mExportedFoodPreviousRound; };
	int			getExportedGoodsPreviousRound()		{ return mExportedGoodsPreviousRound; };
	int			getExportedTechPreviousRound()		{ return mExportedTechPreviousRound; };
	int			getTaxPreviousRound()				{ return mTaxesPreviousRound; };
	int			getSpyNetworkPreviousRound()		{ return mSpyNetworkPreviousRound; };
	int			getNuclearWeaponPreviousRound()		{ return mNuclearWeaponPreviousRound; };
	int			getSpaceProgramPreviousRound()		{ return mSpaceProgramPreviousRound; };

	PoliticalType	getType();

	void		setRound(int round);

	void		getTaxIncome();
	void		updateFood();
	bool		enoughFood();

	bool		enableToIncreasePopulation();
	void		increasePopulation();
	
	virtual bool		upgradeNuclearWeapon(int value) = 0;
	virtual bool		upgradeSpaceProgram(int value) = 0;
	virtual bool		upgradeSpyNetwork(int value) = 0;
	virtual void		setTaxesCost(int tax) = 0;
	virtual bool		setFood(int value) = 0;
	virtual bool		setGoods(int value) = 0;
	virtual bool		setTech(int value) = 0;
	virtual void		chooseLeader(){};
	virtual void		showGUI() = 0;
	virtual void		hideGUI() = 0;
	virtual void		update() = 0;
	virtual void		playMusic() {};
	virtual void			newYearStart() = 0;
protected:
	PoliticalType mType;

	int mRound;

	int	mPatriotism, mPatriotismPreviousRound;
	int	mCurrency, mCurrencyPreviousRound;
	int	mPopulation, mPopulationPreviousRound;
	int	mFood, mFoodPreviousRound, mExportedFood, mExportedFoodPreviousRound, mExportedFoodPrice;
	int	mGoods, mGoodsPreviousRound, mExportedGoods,mExportedGoodsPreviousRound, mExportedGoodsPrice;
	int	mTech, mTechPreviousRound, mExportedTech, mExportedTechPreviousRound, mExportedTechPrice;
	int	mTaxes, mTaxesPreviousRound;
	int	mSpyNetwork, mSpyNetworkPreviousRound;
	int	mNuclearWeapon, mNuclearWeaponPreviousRound;
	int	mSpaceProgram, mSpaceProgramPreviousRound;
	bool mIncreasePopulation;
	bool mTaxDecreased;
};

#endif