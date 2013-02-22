/*
 * Klass för kapitalisterna i spelet Nukes of the Patriots
 * 
 *    Arvid Backman 2013-01-21
 *	  Jon Wahlström
 */

#ifndef CAPITALIST_H
#define CAPITALIST_H

class GUIManager;
class GUIElement;
class GUIButton;
class GUIText;
class GUIEditField;
class GUIWindow;
class GUIImage;
class President;
class Randomizer;
class Communist;

#include "SuperPower.h"
#include <vector>
#include <memory>
#include <map>
#include <sstream>
#include <SFML\Graphics\RenderWindow.hpp>
#include <SFML\Audio\Music.hpp>
#include <SFML/System.hpp>

class Capitalist : public SuperPower 
{
public:
	Capitalist();
	~Capitalist();

	std::shared_ptr<President>	getPresident();
	void						setPresident(std::shared_ptr<President>);


	bool upgradeNuclearWeapon(int value);
	bool upgradeSpaceProgram(int value);
	bool upgradeSpyNetwork(int value);
	
	void chooseLeader();

	void showGUI();
	void hideGUI();
	void update();
	void newYearStart();

	void setTaxesCost(int tax);

	bool setFood(int value);
	bool setGoods(int value);
	bool setTech(int value);

	void playMusic();
	void stopMusic();
	void updateGUI();

	void resetResourcesValues();

private:
	int mCount;
	sf::Thread *mUpdateGUIThread;

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


	std::shared_ptr<President> mPresident;
	std::shared_ptr<President> mFirstPresident;
	std::shared_ptr<President> mSecondPresident;

	/*Skapar två std::map. En dit alla knappar för kapitalisternas läggs in i samt 
	  en dit alla kapitalisternas fönster läggs in i. Detta sker via hämtade värden
	  från XML dokument samt genom ResourceHandler  
	  Av: Jon Wahlström 2013-01-31
																				*/
	std::map<std::string, std::pair<sf::FloatRect, sf::Texture*> > CapitalistButtons;
	std::map<std::string, std::pair<sf::FloatRect, sf::Texture*> > CapitalistWindows;
	std::map<std::string, std::shared_ptr<sf::Music> > CapitalistMusic;
	
	void loadButtonPosition();
	void loadWindowPosition();
	void loadCapitalistMusic();
	void initializeGuiFunctions();
	void initializeCapitalistWindow();
	void upgradeWindowText();
	
	void changeCityImage();
	void initializeCityImages();
	
	std::vector<sf::Texture*> CityImages; 
	std::shared_ptr<GUIButton> mChangeCityImage;

	//President	*mPresident;

	std::shared_ptr<GUIWindow> mIncreasedResourcesWindow;
	std::shared_ptr<GUIButton> mCloseIncreasedResourcesWindow;

	std::shared_ptr<GUIText> mPopulationText;
	std::shared_ptr<GUIText> mCurrencyText;
	std::shared_ptr<GUIText> mPatriotismText;

	std::shared_ptr<GUIText> mStatsText[2][9];
	
	std::shared_ptr<GUIText> mNuclearText;
	std::shared_ptr<GUIText> mSpaceText;
	std::shared_ptr<GUIText> mSpyText;
	std::shared_ptr<GUIText> mFoodText;	
	std::shared_ptr<GUIText> mGoodsText;  
	std::shared_ptr<GUIText> mTechText;

	std::shared_ptr<GUIText> mBuyFoodText;
	std::shared_ptr<GUIText> mBuyGoodsText;
	std::shared_ptr<GUIText> mBuyTechText;
	std::shared_ptr<GUIText> mFoodCost;
	std::shared_ptr<GUIText> mGoodsCost;
	std::shared_ptr<GUIText> mTechCost;
	std::shared_ptr<GUIText> mTotalResourcesCost;

	std::shared_ptr<GUIText> mBuyNuclearText;
	std::shared_ptr<GUIText> mBuySpaceProgramText;
	std::shared_ptr<GUIText> mBuySpyNetworkText;

	/*GUI-pekare för kapitalisternas interface*/
	std::shared_ptr<GUIWindow> mCapitalistMainWindow;
	std::shared_ptr<GUIWindow> mCapitalistBorder;
	std::shared_ptr<GUIWindow> mCapitalistBorderTop;
	std::shared_ptr<GUIButton> mCapitalistPresident;
	std::shared_ptr<GUIButton> mCapitalistTaxesButton;
	//std::shared_ptr<GUIButton> mCapitalistTaxesIsPresssedButton;
	std::shared_ptr<GUIButton> mCapitalistResourceButton;	
	std::shared_ptr<GUIButton> mCapitalistUpgradeButton;		
	std::shared_ptr<GUIButton> mCapitalistTradeButton;	
	/*GUI-pekare för end turn*/
	std::shared_ptr<GUIButton> mCapitalistEndTurnButton;

	/*GUI-pekare för taxes */
	std::shared_ptr<GUIWindow> mTaxesWindow;
	std::shared_ptr<GUIButton> mLowerTaxesButton;
	std::shared_ptr<GUIText>   mTaxValueText;
	std::shared_ptr<GUIText>   mTaxText;
	std::shared_ptr<GUIButton> mRaiseTaxesButton;
	std::shared_ptr<GUIButton> mTaxesCloseButton;
	/*GUI-pekare för resources*/
	std::shared_ptr<GUIWindow> mResourceWindow;
	std::shared_ptr<GUIButton> mLowerFoodByTenButton;
	std::shared_ptr<GUIButton> mLowerFoodByFiveButton;
	std::shared_ptr<GUIButton> mLowerFoodByOneButton;
	std::shared_ptr<GUIButton> mRaiseFoodByOneButton;
	std::shared_ptr<GUIButton> mRaiseFoodByFiveButton;
	std::shared_ptr<GUIButton> mRaiseFoodByTenButton;
	std::shared_ptr<GUIText> mResourceFoodPriceText;

	std::shared_ptr<GUIButton> mLowerGoodsByTenButton;
	std::shared_ptr<GUIButton> mLowerGoodsByFiveButton;
	std::shared_ptr<GUIButton> mLowerGoodsByOneButton;
	std::shared_ptr<GUIButton> mRaiseGoodsByOneButton;
	std::shared_ptr<GUIButton> mRaiseGoodsByFiveButton;
	std::shared_ptr<GUIButton> mRaiseGoodsByTenButton;
	std::shared_ptr<GUIText> mResourceGoodsPriceText;

	std::shared_ptr<GUIButton> mLowerTechByTenButton;
	std::shared_ptr<GUIButton> mLowerTechByFiveButton;
	std::shared_ptr<GUIButton> mLowerTechByOneButton;
	std::shared_ptr<GUIButton> mRaiseTechByOneButton;
	std::shared_ptr<GUIButton> mRaiseTechByFiveButton;
	std::shared_ptr<GUIButton> mRaiseTechByTenButton;
	std::shared_ptr<GUIText> mResourceTechPriceText;

	std::shared_ptr<GUIButton> mResourceCloseButton;
	/*GIU-pekare för upgrade*/
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

	std::shared_ptr<GUIWindow> mChoosePresidentWindow;
	std::shared_ptr<GUIWindow> mPickedPresidentWindow;
	std::shared_ptr<GUIButton> mFirstPresidentButton;
	std::shared_ptr<GUIImage>  mFirstPresidentPlaque;
	std::shared_ptr<GUIButton> mSecondPresidentButton;
	std::shared_ptr<GUIImage>  mSecondPresidentPlaque;
	std::shared_ptr<GUIButton> mPickedPresidentButton;
	std::shared_ptr<GUIImage>  mPickedPresidentPlaque;
	std::shared_ptr<GUIButton> mClosePresidentWindow;
	std::shared_ptr<GUIButton> mClosePickedPresidentWindow;

	std::shared_ptr<GUIText>   mPresidentBiography;
	std::shared_ptr<GUIText>   mPresidentPositiveText[2];
	std::shared_ptr<GUIText>   mPresidentNegativeText;
	std::shared_ptr<GUIText>   mPresidentStats;
	std::shared_ptr<GUIText>   mFirstPositiveStat[2];
	std::shared_ptr<GUIText>   mFirstNegativeStat;
	std::shared_ptr<GUIText>   mSecondPositiveStat[2];
	std::shared_ptr<GUIText>   mSecondNegativeStat;

	std::shared_ptr<GUIButton> mLeftPanel;
	std::shared_ptr<GUIButton> mRightPanel;

	std::shared_ptr<GUIText>   mCapitalistHeadLine;

	std::shared_ptr<GUIText>   mPatriotismChange;
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



	std::shared_ptr<GUIWindow> mTaxesIncomeWindow;
	std::shared_ptr<GUIText>   mCurrentPopulationText[2];
	std::shared_ptr<GUIText>   mCurrentTaxesText[2];
	std::shared_ptr<GUIText>   mTaxesIncomeText[2];      
	std::shared_ptr<GUIButton> mCloseTaxesIncomeWindow;

	std::shared_ptr<GUIWindow> mIncreasedResourcesPriceWindow;
	std::shared_ptr<GUIText>   mIncreasedResourcesText;
	std::shared_ptr<GUIButton> mCloseIncreasedResourcesPriceWindow;

	std::shared_ptr<GUIWindow> mPopulationEatsFoodWindow;
	std::shared_ptr<GUIText>   mPopulationEatsFoodText;
	std::shared_ptr<GUIButton> mClosePopulationEatsFoodWindow;

	std::shared_ptr<GUIImage>  mFoodImage[3];
	std::shared_ptr<GUIImage>  mGoodsImage[3];
	std::shared_ptr<GUIImage>  mTechImage[3];

	std::shared_ptr<GUIImage>  mNuclearPanel;
	std::shared_ptr<GUIImage>  mSpacePanel;
	std::shared_ptr<GUIImage>  mSpyPanel;

};


#endif