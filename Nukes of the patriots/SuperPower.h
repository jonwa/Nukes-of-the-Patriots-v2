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
	
	virtual bool		upgradeNuclearWeapon() = 0;
	virtual bool		upgradeSpaceProgram() = 0;
	virtual bool		upgradeSpyNetwork() = 0;
	virtual void		setTaxesCost(int tax) = 0;
	virtual bool		setFood(int value) = 0;
	virtual bool		setGoods(int value) = 0;
	virtual bool		setTech(int value) = 0;
	virtual void		chooseLeader(){};
	virtual void		showGUI() = 0;
	virtual void		hideGUI() = 0;
	virtual void		update() = 0;
	virtual void		playMusic() {};
protected:
	PoliticalType mType;

	int mRound;

	int	mPatriotism;
	int	mCurrency;
	int	mPopulation;
	int	mFood;
	int	mGoods;
	int	mTech;
	int	mTaxes;
	int	mSpyNetwork;
	int	mNuclearWeapon;
	int	mSpaceProgram;
	bool mIncreasePopulation;
	bool mTaxDecreased;
};

#endif