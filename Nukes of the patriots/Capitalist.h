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
#include <SFML\Graphics\RenderWindow.hpp>
#include <SFML\Window\Event.hpp>
#include <SFML\Audio\Music.hpp>
#include <SFML/System.hpp>
#include "Sound.h"
#include "tinyxml2.h"
#include "RemoteClient.h"

class Capitalist : public SuperPower 
{
public:
	Capitalist();
	~Capitalist();

	void saveGame(tinyxml2::XMLDocument &doc);
	void loadGame(tinyxml2::XMLDocument &doc);

	std::shared_ptr<President>	getPresident();
	void						setPresident(std::shared_ptr<President>);

	void reset();


	bool upgradeNuclearWeapon(int value);
	bool upgradeSpaceProgram(int value);
	bool upgradeSpyNetwork(int value);
	
	void chooseLeader();
	void LANChooseLeader(std::shared_ptr<President> firstPresident, std::shared_ptr<President> secondPresident);
	void sendPresidentDataToOtherPlayer();

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
	void pauseMusic();
	void updateGUI();

	void resetResourcesValues();

	bool enoughFood();
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
	int	mPickedPresident;

	std::map<std::string, std::pair<sf::FloatRect, sf::Texture*> > CapitalistButtons;
	std::map<std::string, std::pair<sf::FloatRect, sf::Texture*> > CapitalistWindows;
	std::map<std::string, std::shared_ptr<sf::Music> >			   CapitalistMusic;
	
	std::shared_ptr<Sound> mCapitalistMainTheme; // capitalist main theme music
	std::shared_ptr<Sound> mIncreasedResoucesSound;

	
	void loadButtonPosition();
	void loadWindowPosition();
	void loadCapitalistMusic();
	void initializeGuiFunctions();
	void initializeCapitalistWindow();
	void upgradeWindowText();
	
	void changeCityImage();
	void initializeCityImages();
	
	std::vector<sf::Texture*> CityImages; 
	std::shared_ptr<GUIImage> mChangeCityImage;

	//President	*mPresident;

	std::shared_ptr<GUIWindow> mIncreasedResourcesWindow;
	std::shared_ptr<GUIButton> mCloseIncreasedResourcesWindow;

	std::shared_ptr<GUIText> mPopulationText;
	std::shared_ptr<GUIText> mCurrencyText;
	std::shared_ptr<GUIText> mPatriotismText;

	std::shared_ptr<GUIText> mStatsText[2][9];

	std::shared_ptr<GUIText> mWindowHeadlines[6];
	
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
	std::shared_ptr<GUIWindow> mCapitalistButtonFrame;
	std::shared_ptr<GUIWindow> mCapitalistBorderTop;
	std::shared_ptr<GUIImage>  mPresidentFrame;
	std::shared_ptr<GUIButton> mCapitalistPresident;
	std::shared_ptr<GUIButton> mCapitalistTaxesButton;
	std::shared_ptr<GUIButton> mCapitalistResourceButton;	
	std::shared_ptr<GUIButton> mCapitalistUpgradeButton;		
	std::shared_ptr<GUIButton> mCapitalistTradeButton;	
	/*GUI-pekare för end turn*/
	std::shared_ptr<GUIImage>  mEndTurnFrame;
	std::shared_ptr<GUIButton> mCapitalistEndTurnButton;

	/*GUI-pekare för taxes */
	std::shared_ptr<GUIWindow> mTaxesWindow;
	std::shared_ptr<GUIButton> mLowerTaxesButton;
	std::shared_ptr<GUIText>   mTaxValueText;
	std::shared_ptr<GUIText>   mTaxText;
	std::shared_ptr<GUIText>   mTaxesPatriotismChange;
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
	std::shared_ptr<GUIText>   mResourceFoodPriceText;

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
	std::shared_ptr<GUIText>   mExportedWithoutPriceText;

	std::shared_ptr<GUIWindow> mExportedResourcesWindow;
	std::shared_ptr<GUIText>   mExportIncomeHeadliner;
	std::shared_ptr<GUIText>   mExportHeadliner;
	std::shared_ptr<GUIButton> mCloseExportedResourceWindow;
	std::shared_ptr<GUIText>   mResourcesExportedText[3];
	std::shared_ptr<GUIText>   mExportedIncomeText;

	std::shared_ptr<GUIWindow> mChoosePresidentWindow;
	std::shared_ptr<GUIWindow> mPickedPresidentWindow;
	std::shared_ptr<GUIImage>  mFirstPresidentButton;
	std::shared_ptr<GUIButton> mFirstPresidentPlaque;
	std::shared_ptr<GUIImage>  mSecondPresidentButton;
	std::shared_ptr<GUIButton> mSecondPresidentPlaque;
	std::shared_ptr<GUIImage>  mPickedPresidentButton;
	std::shared_ptr<GUIImage> mPickedPresidentPlaque;
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

	std::shared_ptr<GUIImage> mLeftPanel;
	std::shared_ptr<GUIImage> mRightPanel;

	std::shared_ptr<GUIText>   mCapitalistHeadLine;

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

	std::shared_ptr<GUIWindow> mTaxesIncomeWindow;
	std::shared_ptr<GUIText>   mTaxesIncomeHeadLiner;
	std::shared_ptr<GUIText>   mCurrentPopulationText[2];
	std::shared_ptr<GUIText>   mCurrentTaxesText[2];
	std::shared_ptr<GUIText>   mTaxesIncomeText[2];      
	std::shared_ptr<GUIButton> mCloseTaxesIncomeWindow;

	std::shared_ptr<GUIWindow> mIncreasedResourcesPriceWindow;
	std::shared_ptr<GUIText>   mIncreasedResourcesPriceHeadliner;
	std::shared_ptr<GUIText>   mIncreasedResourcesText;
	std::shared_ptr<GUIButton> mCloseIncreasedResourcesPriceWindow;

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

	std::shared_ptr<GUIImage>  mToolTipInterface[6];
	Timer *mPresidentToolTipTimer;
	Timer *mTaxesToolTipTimer;
	Timer *mResourceToolTipTimer;
	Timer *mUpgradeToolTipTimer;
	Timer *mTradeToolTipTimer;
	Timer *mEndTurnToolTipTimer;
};


#endif