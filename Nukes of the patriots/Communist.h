 /*
Klass för kommunisterna i spelet Nukes of the Patriots

	Av:Arvid Backman 2013-01-21
	   Jon Wahlström

*/

#ifndef COMMUNIST_H
#define COMMUNIST_H

#include "SuperPower.h"
#include <memory>
#include <sstream>
#include <map>
#include <iostream>
#include <SFML\Graphics\Texture.hpp>
#include <SFML\Audio\Music.hpp>


class President;
class GUIElement;
class GUIButton;
class GUIWindow;
class GUIText;
class GUIImage;
class GUIEditField;
class Randomizer;


class Communist : public SuperPower
{
public:
	Communist();
	~Communist();

	void		getTaxIncome();
	void		updateFood();
	bool		enoughFood();

	bool		setFood(int value);
	bool		setGoods(int value);
	bool		setTech(int value);

	bool		upgradeNuclearWeapon(int value);
	bool		upgradeSpaceProgram(int value);
	bool		upgradeSpyNetwork(int value);

	void		setTaxesCost(int tax);

	int			getYearlyFood(int round);
	int			getYearlyGoods(int round);
	int			getYearlyTech(int round);
	int			getYearlyTaxes(int round);

	void		buyPropagandaFood(int round);
	void		buyPropagandaGoods(int round);
	void		buyPropagandaTech(int round);

	void		chooseLeader();

	void playMusic();
	void stopMusic();

	void showGUI();
	void hideGUI();
	void update();
	void newYearStart();

private:
	int mCount;

	std::string intToString(int i)
	{
		std::stringstream converter;
		converter << i;
		return converter.str();
	}

	int stringToInt(std::string str)
	{
		if(str.size() == 0)
			return 0;
		return atoi(str.c_str());
	}
	
	std::vector<sf::Texture*> PropagandaFood;
	std::vector<sf::Texture*> PropagandaGoods;
	std::vector<sf::Texture*> PropagandaTech;

	// Skapar två std::map. En dit alla knappar för kommunisterna läggs in i samt 
	// en dit alla kommunisternas fönster läggs in i. Detta sker via hämtade värden
	// från XML dokument samt genom ResourceHandler  
	//  Av: Jon Wahlström 2013-01-31
																		
	std::map<std::string, std::pair<sf::FloatRect, sf::Texture*> > CommunistButtons;
	std::map<std::string, std::pair<sf::FloatRect, sf::Texture*> > CommunistWindows;
	std::map<std::string, std::shared_ptr<sf::Music> >			   CommunistMusic;

	std::vector<std::map<std::string, int> > mYearVector;
	std::vector<std::shared_ptr<GUIButton> > mResourcesFoodButtons;
	std::vector<std::shared_ptr<GUIButton> > mResourcesGoodsButtons;
	std::vector<std::shared_ptr<GUIButton> > mResourcesTechButtons;



	void fiveYearInitialize();
	void setYearlyResources(int year, std::string, int value);
	void updateAllResources();
	void fiveYearGuiFunctions();
	void openFiveYearPlan();

	void changeCityImage();

	void initializeCityImages();

	void upgradeWindowText();

	void propagandaInitialize();

	void loadButtonPosition();
	void loadWindowPosition();
	void initializeGuiFunctions();
	void initializeCommunistWindow();

	void loadCommunistMusic();

	std::vector<sf::Texture*> CityImages; 
	std::shared_ptr<GUIButton> mChangeCityImage;

	std::shared_ptr<President> mGeneral;
	std::shared_ptr<President> mFirstGeneral;

 
	std::shared_ptr<GUIText> mNuclearText;
	std::shared_ptr<GUIText> mSpaceText;
	std::shared_ptr<GUIText> mSpyText;
	std::shared_ptr<GUIText> mBuyNuclearText;
	std::shared_ptr<GUIText> mBuySpaceProgramText;
	std::shared_ptr<GUIText> mBuySpyNetworkText;

	std::shared_ptr<GUIText> mFoodText;	
	std::shared_ptr<GUIText >mGoodsText;  
	std::shared_ptr<GUIText> mTechText;	
	
	/*GUI-pekare för kommunisternas interface*/
	std::shared_ptr<GUIWindow> mCommunistMainWindow;
	std::shared_ptr<GUIButton> mCommunistGeneralButton;
	std::shared_ptr<GUIButton> mCommunistFiveYearPlanButton;		
	std::shared_ptr<GUIButton> mCommunistPropagandaButton;	
	std::shared_ptr<GUIButton> mCommunistUpgradeButton;		
	std::shared_ptr<GUIButton> mCommunistTradeButton;	
	/*GUI-pekare för end turn*/
	std::shared_ptr<GUIButton> mCommunistEndTurnButton;

	/*GUI-pekare för fem års planen*/
	std::shared_ptr<GUIWindow>	mFiveYearPlanWindow;
	std::shared_ptr<GUIButton>	mYearOneLowerTaxesButton;
	std::shared_ptr<GUIButton>	mYearOneRaiseTaxesButton;
	std::shared_ptr<GUIButton>	mYearTwoLowerTaxesButton;
	std::shared_ptr<GUIButton>	mYearTwoRaiseTaxesButton;
	std::shared_ptr<GUIButton>	mYearThreeLowerTaxesButton;
	std::shared_ptr<GUIButton>	mYearThreeRaiseTaxesButton;
	std::shared_ptr<GUIButton>	mYearFourLowerTaxesButton;
	std::shared_ptr<GUIButton>	mYearFourRaiseTaxesButton;
	std::shared_ptr<GUIButton>	mYearFiveLowerTaxesButton;
	std::shared_ptr<GUIButton>	mYearFiveRaiseTaxesButton;

	std::shared_ptr<GUIText>	mYearOneTaxesText;
	std::shared_ptr<GUIText>	mYearTwoTaxesText;
	std::shared_ptr<GUIText>	mYearThreeTaxesText;
	std::shared_ptr<GUIText>	mYearFourTaxesText;
	std::shared_ptr<GUIText>	mYearFiveTaxesText;

	std::shared_ptr<GUIText>	mIncomeYearOne;
	std::shared_ptr<GUIText>	mIncomeYearTwo;
	std::shared_ptr<GUIText>	mIncomeYearThree;
	std::shared_ptr<GUIText>	mIncomeYearFour;
	std::shared_ptr<GUIText>	mIncomeYearFive;

	/*fem år för mat*/
	std::shared_ptr<GUIEditField> mYearOneFood;
	std::shared_ptr<GUIEditField> mYearTwoFood;
	std::shared_ptr<GUIEditField> mYearThreeFood;
	std::shared_ptr<GUIEditField> mYearFourFood;
	std::shared_ptr<GUIEditField> mYearFiveFood;

	std::shared_ptr<GUIText>	  mYearOneFoodCost;
	std::shared_ptr<GUIText>	  mYearTwoFoodCost;
	std::shared_ptr<GUIText>	  mYearThreeFoodCost;
	std::shared_ptr<GUIText>	  mYearFourFoodCost;
	std::shared_ptr<GUIText>	  mYearFiveFoodCost;

	/*Fem år för varor*/
	std::shared_ptr<GUIEditField> mYearOneGoods;
	std::shared_ptr<GUIEditField> mYearTwoGoods;
	std::shared_ptr<GUIEditField> mYearThreeGoods;
	std::shared_ptr<GUIEditField> mYearFourGoods;
	std::shared_ptr<GUIEditField> mYearFiveGoods;

	std::shared_ptr<GUIText>	  mYearOneGoodsCost;
	std::shared_ptr<GUIText>	  mYearTwoGoodsCost;
	std::shared_ptr<GUIText>	  mYearThreeGoodsCost;
	std::shared_ptr<GUIText>	  mYearFourGoodsCost;
	std::shared_ptr<GUIText>	  mYearFiveGoodsCost;
	
	/*Fem år för teknologi*/
	std::shared_ptr<GUIEditField> mYearOneTech;
	std::shared_ptr<GUIEditField> mYearTwoTech;
	std::shared_ptr<GUIEditField> mYearThreeTech;
	std::shared_ptr<GUIEditField> mYearFourTech;
	std::shared_ptr<GUIEditField> mYearFiveTech;

	std::shared_ptr<GUIText>	  mYearOneTechCost;
	std::shared_ptr<GUIText>	  mYearTwoTechCost;
	std::shared_ptr<GUIText>	  mYearThreeTechCost;
	std::shared_ptr<GUIText>	  mYearFourTechCost;
	std::shared_ptr<GUIText>	  mYearFiveTechCost;

	std::shared_ptr<GUIButton>	  mFiveYearPlanCloseButton;

	std::shared_ptr<GUIText>	  mTotalCostYearOne;
	std::shared_ptr<GUIText>	  mTotalCostYearTwo;
	std::shared_ptr<GUIText>	  mTotalCostYearThree;
	std::shared_ptr<GUIText>	  mTotalCostYearFour;
	std::shared_ptr<GUIText>	  mTotalCostYearFive;

	/*GUI-pekare för propaganda*/
	std::shared_ptr<GUIWindow> mPropagandaWindowFirst;
	std::shared_ptr<GUIWindow> mPropagandaWindowSecond;
	
	std::shared_ptr<GUIButton> mPropagandaBuyFoodButton;
	std::shared_ptr<GUIButton> mPropagandaBuyGoodsButton;
	std::shared_ptr<GUIButton> mPropagandaBuyTechButton;
	std::shared_ptr<GUIButton> mShowBoughtPropaganda;
	//std::shared_ptr<GUIButton> mPropagandaBuyFoodFirstYearButton;
	//std::shared_ptr<GUIButton> mPropagandaBuyGoodsFirstYearButton;
	//std::shared_ptr<GUIButton> mPropagandaBuyTechFristYearButton;
	//
	//std::shared_ptr<GUIButton> mPropagandaBuyFoodSecondYearButton;
	//std::shared_ptr<GUIButton> mPropagandaBuyGoodsSecondYearButton;
	//std::shared_ptr<GUIButton> mPropagandaBuyTechSecondYearButton;
	//
	//std::shared_ptr<GUIButton> mPropagandaBuyFoodThirdYearButton;
	//std::shared_ptr<GUIButton> mPropagandaBuyGoodsThirdYearButton;
	//std::shared_ptr<GUIButton> mPropagandaBuyTechThirdYearButton;
	//
	//std::shared_ptr<GUIButton> mPropagandaBuyFoodFourthYearButton;
	//std::shared_ptr<GUIButton> mPropagandaBuyGoodsFourthYearButton;
	//std::shared_ptr<GUIButton> mPropagandaBuyTechFourthYearButton;
	//
	//std::shared_ptr<GUIButton> mPropagandaBuyFoodFifthYearButton;
	//std::shared_ptr<GUIButton> mPropagandaBuyGoodsFifthYearButton;
	//std::shared_ptr<GUIButton> mPropagandaBuyTechFifthYearButton;

	std::shared_ptr<GUIButton> mPropagandaWindowFirstCloseButton;
	std::shared_ptr<GUIButton> mPropagandaWindowSecondCloseButton;

	/*GUI-pekare för upgrade*/
	std::shared_ptr<GUIWindow> mUpgradeWindow;
	std::shared_ptr<GUIButton> mUpgradeNuclearWeaponButton;
	std::shared_ptr<GUIText>   mNuclearGoodsCost;
	std::shared_ptr<GUIText>   mNuclearTechCost;
	std::shared_ptr<GUIButton> mUpgradeSpaceProgramButton;
	std::shared_ptr<GUIText>   mSpaceProgramGoodsCost;
	std::shared_ptr<GUIText>   mSpaceProgramTechCost;
	std::shared_ptr<GUIButton> mUpgradeSpyNetworkButton;
	std::shared_ptr<GUIText>   mSpyNetworkGoodsCost;
	std::shared_ptr<GUIText>   mSpyNetworkTechCost;
	std::shared_ptr<GUIButton> mCancelUpgradeNuclearWeaponButton;
	std::shared_ptr<GUIButton> mCancelUpgradeSpaceProgramButton;
	std::shared_ptr<GUIButton> mCancelUpgradeSpyNetworkButton;   
	std::shared_ptr<GUIButton> mUpgradeCloseButton; 


	/*GUI-pekare för export*/
	std::shared_ptr<GUIWindow> mExportWindow;
	std::shared_ptr<GUIText> mExportTotalPriceText[3];
	std::shared_ptr<GUIImage> mExportQuantityBackground[3];
	std::shared_ptr<GUIText> mExportQuantityText[3];
	std::shared_ptr<GUIButton> mExportButtonMinus[3][3];
	std::shared_ptr<GUIButton> mExportButtonPlus[3][3];
	std::shared_ptr<GUIEditField> mExportFoodCost;
	std::shared_ptr<GUIEditField> mExportGoodsCost;
	std::shared_ptr<GUIEditField> mExportTechCost;
	std::shared_ptr<GUIButton> mExportConfirmButton;

	std::shared_ptr<GUIWindow> mImportWindow;
	std::shared_ptr<GUIText> mImportResourcesAvailableText[3];
	std::shared_ptr<GUIText> mImportPriceText[3];
	std::shared_ptr<GUIText> mImportBuyQuantityText[3];
	std::shared_ptr<GUIImage> mImportBuyQuantityBackground[3];
	std::shared_ptr<GUIText> mImportCostText[3];
	std::shared_ptr<GUIButton> mImportBuyButtonMinus[3][3];
	std::shared_ptr<GUIButton> mImportBuyButtonPlus[3][3];
	std::shared_ptr<GUIButton> mImportGotoExportButton;

	std::shared_ptr<GUIWindow> mChooseGeneralWindow;
	std::shared_ptr<GUIWindow> mPickedGeneralWindow;
	std::shared_ptr<GUIButton> mPickedGeneralButton;

	std::shared_ptr<GUIButton> mFirstGeneralButton;
	std::shared_ptr<GUIImage>  mFirstGeneralPlaque;
	std::shared_ptr<GUIImage>  mPickedGeneralPlaque;

	std::shared_ptr<GUIButton> mGoToNextPortraitButton;
	std::shared_ptr<GUIButton> mGoToPreviousPortraitButton;
	std::shared_ptr<GUIButton> mCloseGeneralWindow;
	std::shared_ptr<GUIButton> mClosePickedGeneralWindow;

	std::shared_ptr<GUIButton> mLeftPanel;
	std::shared_ptr<GUIButton> mRightPanel;

};

#endif