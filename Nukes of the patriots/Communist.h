 /*
Klass för kommunisterna i spelet Nukes of the Patriots

	Av:Arvid Backman 2013-01-21
	   Jon Wahlström
	   Aleksi Lindeman
*/

#ifndef COMMUNIST_H
#define COMMUNIST_H

#include "SuperPower.h"
#include <memory>
#include <map>
#include <iostream>
#include <SFML\Graphics\Texture.hpp>
#include <SFML\Audio\Music.hpp>
#include "Sound.h"
#include "tinyxml2.h"

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

	void saveGame(tinyxml2::XMLDocument &doc);
	void loadGame(tinyxml2::XMLDocument &doc);

	void		reset();

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
	void pauseMusic();
	void showGUI();
	void hideGUI();
	void update();
	void newYearStart();
	void updateGUI();

	void LANResourcesIncome(int food, int goods, int tech);
	void LANBuyPropaganda(int amount, std::string type);
private:
	int mCount;
	sf::Thread *mUpdateGUIThread;
	
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
	
	std::shared_ptr<Sound> mCommunistMainTheme;

	void fiveYearInitialize();
	void setYearlyResources(int year, std::string, int value);
	void updateAllResources();
	void resourceIncome();
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
	std::shared_ptr<GUIImage> mChangeCityImage;

	std::shared_ptr<President> mGeneral;
	std::shared_ptr<President> mFirstGeneral;
	std::shared_ptr<GUIText>   mGeneralBiography;

	std::shared_ptr<GUIText>   mWindowHeadlines[6];

 	std::shared_ptr<GUIText> mPopulationText;
	std::shared_ptr<GUIText> mCurrencyText;
	std::shared_ptr<GUIText> mPatriotismText;

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
	std::shared_ptr<GUIWindow> mCommunistBorder;
	std::shared_ptr<GUIWindow> mCommunistButtonFrame;
	std::shared_ptr<GUIWindow> mCommunistBorderTop;
	std::shared_ptr<GUIImage>  mGeneralFrame;
	std::shared_ptr<GUIButton> mCommunistGeneralButton;
	std::shared_ptr<GUIButton> mCommunistFiveYearPlanButton;		
	std::shared_ptr<GUIButton> mCommunistPropagandaButton;	
	std::shared_ptr<GUIButton> mCommunistUpgradeButton;		
	std::shared_ptr<GUIButton> mCommunistTradeButton;	
	/*GUI-pekare för end turn*/
	std::shared_ptr<GUIImage>  mEndTurnFrame;
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

	std::shared_ptr<GUIText>      mFiveYearTotalCostText;
	std::shared_ptr<GUIText>	  mFiveYearIncomeText;
	std::shared_ptr<GUIText>	  mFiveYearTaxText;
	std::shared_ptr<GUIText>	  mTaxesPatriotismChange;

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
	std::shared_ptr<GUIText>   mPropagandaHeadliner[2];
	std::shared_ptr<GUIWindow> mPropagandaWindowSecond;
	std::shared_ptr<GUIText>   mPropagandaCostText;
	std::shared_ptr<GUIText>   mBoughtPropagandaText;
	
	std::shared_ptr<GUIButton> mPropagandaBuyFoodButton;
	std::shared_ptr<GUIButton> mPropagandaBuyGoodsButton;
	std::shared_ptr<GUIButton> mPropagandaBuyTechButton;
	std::shared_ptr<GUIButton> mShowBoughtPropaganda;
	
	std::shared_ptr<GUIButton> mPropagandaWindowFirstCloseButton;
	std::shared_ptr<GUIButton> mPropagandaWindowSecondCloseButton;

	/*GUI-pekare för upgrade*/
	std::shared_ptr<GUIWindow> mUpgradeWindow;
	std::shared_ptr<GUIText>   mUpgradeHeadliner;
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
	std::shared_ptr<GUIText>   mExportHeadliner;
	std::shared_ptr<GUIText> mExportQuantityLabel, mExportCostLabel, mExportPriceLabel, mExportTotalPriceLabel, mExportTotalPriceValue;
	std::shared_ptr<GUIText> mExportPriceText[3];
	std::shared_ptr<GUIImage> mExportQuantityBackground[3];
	std::shared_ptr<GUIText> mExportQuantityText[3];
	std::shared_ptr<GUIButton> mExportButtonMinus[3][3];
	std::shared_ptr<GUIButton> mExportButtonPlus[3][3];
	std::shared_ptr<GUIEditField> mExportFoodCost;
	std::shared_ptr<GUIEditField> mExportGoodsCost;
	std::shared_ptr<GUIEditField> mExportTechCost;
	std::shared_ptr<GUIButton> mExportConfirmButton;

	std::shared_ptr<GUIWindow> mImportWindow;
	std::shared_ptr<GUIText>   mImportHeadliner;
	std::shared_ptr<GUIText> mImportResourceLabel, mImportPriceLabel, mImportQuantityLabel, mImportCostLabel, mImportTotalCostLabel;
	std::shared_ptr<GUIText> mImportTotalCostText;
	std::shared_ptr<GUIText> mImportResourcesAvailableText[3];
	std::shared_ptr<GUIText> mImportBuyQuantityText[3];
	std::shared_ptr<GUIImage> mImportBuyQuantityBackground[3];
	std::shared_ptr<GUIText> mImportPriceText[3];
	std::shared_ptr<GUIText> mImportTotalPriceText[3];
	std::shared_ptr<GUIButton> mImportBuyButtonMinus[3][3];
	std::shared_ptr<GUIButton> mImportBuyButtonPlus[3][3];
	std::shared_ptr<GUIButton> mImportGotoExportButton;

	std::shared_ptr<GUIWindow> mExportedWithoutPriceWindow;
	std::shared_ptr<GUIText> mExportedWithoutPriceText;

	std::shared_ptr<GUIWindow> mExportedResourcesWindow;
	std::shared_ptr<GUIButton> mCloseExportedResourceWindow;
	std::shared_ptr<GUIText>   mResourcesExportedText[3];
	std::shared_ptr<GUIText>   mExportedIncomeText;
	std::shared_ptr<GUIText>   mExportIncomeHeadliner;

	std::shared_ptr<GUIWindow> mChooseGeneralWindow;
	std::shared_ptr<GUIWindow> mPickedGeneralWindow;
	std::shared_ptr<GUIImage>  mPickedGeneralButton;

	std::shared_ptr<GUIImage>  mFirstGeneralButton;
	std::shared_ptr<GUIButton> mFirstGeneralPlaque;
	std::shared_ptr<GUIImage>  mPickedGeneralPlaque;

	std::shared_ptr<GUIButton> mGoToNextPortraitButton;
	std::shared_ptr<GUIButton> mGoToPreviousPortraitButton;
	std::shared_ptr<GUIButton> mCloseGeneralWindow;
	std::shared_ptr<GUIButton> mClosePickedGeneralWindow;

	std::shared_ptr<GUIImage>  mLeftPanel;
	std::shared_ptr<GUIImage>  mRightPanel;

	std::shared_ptr<GUIText>   mCommunistHeadLine;

	std::shared_ptr<GUIText>   mPatriotismChange;
	std::shared_ptr<GUIText>   mNewPatriotism;
	std::shared_ptr<GUIText>   mPopulationChange;
	std::shared_ptr<GUIText>   mFoodChange;
	std::shared_ptr<GUIText>   mTechChange;
	std::shared_ptr<GUIText>   mExportedChange;
	std::shared_ptr<GUIText>   mTaxChange;
	std::shared_ptr<GUIText>   mNuclearWeaponChange;
	std::shared_ptr<GUIText>   mSpaceProgramMoreThanEnemyText;
	std::shared_ptr<GUIText>   mSpaceProgramIncreasedText;

	std::shared_ptr<GUIText>   mPatriotismChangeValue;
	std::shared_ptr<GUIText>   mPopulationChangeValue;
	std::shared_ptr<GUIText>   mFoodChangeValue;
	std::shared_ptr<GUIText>   mTechChangeValue;
	std::shared_ptr<GUIText>   mExportedChangeValue;
	std::shared_ptr<GUIText>   mTaxChangeValue;
	std::shared_ptr<GUIText>   mNuclearWeaponChangeValue;
	std::shared_ptr<GUIText>   mSpaceProgramMoreThanEnemyTextValue;
	std::shared_ptr<GUIText>   mSpaceProgramIncreasedTextValue;

	std::shared_ptr<GUIWindow> mResourceIncomeWindow;
	std::shared_ptr<GUIText>   mResourcesIncomeHeadLiner;
	std::shared_ptr<GUIButton> mCloseResourceIncomeWindow;
	std::shared_ptr<GUIText>   mFoodIncome;
	std::shared_ptr<GUIText>   mGoodsIncome;
	std::shared_ptr<GUIText>   mTechIncome;

	std::shared_ptr<GUIWindow> mTaxesIncomeWindow;
	std::shared_ptr<GUIText>   mTaxesIncomeHeadLiner;
	std::shared_ptr<GUIText>   mCurrentPopulationText[2];
	std::shared_ptr<GUIText>   mCurrentTaxesText[2];
	std::shared_ptr<GUIText>   mTaxesIncomeText[2];      
	std::shared_ptr<GUIButton> mCloseTaxesIncomeWindow;

	std::shared_ptr<GUIWindow> mPopulationEatsFoodWindow;
	std::shared_ptr<GUIText>   mPopulationEatsFoodHeadliner;
	std::shared_ptr<GUIText>   mPopulationEatsFoodText;
	std::shared_ptr<GUIButton> mClosePopulationEatsFoodWindow;
	std::shared_ptr<GUIButton> mDoIncreasePopulation;
	std::shared_ptr<GUIButton> mDoNotIncreasePopulation;

	std::shared_ptr<GUIImage>  mFoodImage[3];
	std::shared_ptr<GUIImage>  mGoodsImage[3];
	std::shared_ptr<GUIImage>  mTechImage[3];

	std::shared_ptr<GUIImage>  mNuclearPanel;
	std::shared_ptr<GUIImage>  mSpacePanel;
	std::shared_ptr<GUIImage>  mSpyPanel;


	std::shared_ptr<GUIImage>  mToolTipInterface[4];
	Timer *mGeneralToolTipTimer;
	Timer *mFiveYearPlanToolTipTimer;
	Timer *mPropagandaToolTipTimer;
	Timer *mUpgradeToolTipTimer;
	Timer *mTradeToolTipTimer;
	Timer *mEndTurnToolTipTimer;
};

#endif