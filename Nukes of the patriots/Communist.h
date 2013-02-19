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


class President;
class GUIElement;
class GUIButton;
class GUIWindow;
class GUIImage;
class GUIText;
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

	void showGUI();
	void hideGUI();
	void update();
private:
	std::string intToString(int i)
	{
		std::stringstream converter;
		converter << i;
		return converter.str();
	}

	int stringToInt(std::string str)
	{
		return atoi(str.c_str());
	}
	

	std::shared_ptr<President> mGeneral;
	std::shared_ptr<President> mFirstGeneral;
	std::shared_ptr<President> mSecondGeneral;
	std::shared_ptr<President> mThirdGeneral;
	std::shared_ptr<President> mFourthGeneral;
	std::shared_ptr<President> mFifthGeneral;

	std::vector<std::map<std::string, int>> mYearVector;

	void fiveYearInitialize();
	void setYearlyResources(int round, std::string, int value);
	void fiveYearGuiFunctions();
	void openFiveYearPlan();

	/*Skapar två std::map. En dit alla knappar för kommunisterna läggs in i samt 
	  en dit alla kommunisternas fönster läggs in i. Detta sker via hämtade värden
	  från XML dokument samt genom ResourceHandler  
	  Av: Jon Wahlström 2013-01-31
																				*/
	std::map<std::string, std::pair<sf::FloatRect, sf::Texture*> > CommunistButtons;
	std::map<std::string, std::pair<sf::FloatRect, sf::Texture*> > CommunistWindows;
	
	void loadButtonPosition();
	void loadWindowPosition();
	void initializeGuiFunctions();
	void initializeCommunistWindow();
 
	//President	*mPresident;

	std::shared_ptr<GUIText> mNuclearText;
	std::shared_ptr<GUIText> mSpaceText;
	std::shared_ptr<GUIText> mSpyText;
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
	std::shared_ptr<GUIWindow>	mTaxesWindow;
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
	/*fem år för mat*/
	std::shared_ptr<GUIWindow> mResourcesWindow;
	std::shared_ptr<GUIButton> mYearOneLowerFoodByTenButton;
	std::shared_ptr<GUIButton> mYearOneLowerFoodByFiveButton;
	std::shared_ptr<GUIButton> mYearOneLowerFoodByOneButton;
	std::shared_ptr<GUIButton> mYearOneRaiseFoodByOneButton;
	std::shared_ptr<GUIButton> mYearOneRaiseFoodByFiveButton;
	std::shared_ptr<GUIButton> mYearOneRaiseFoodByTenButton;
	std::shared_ptr<GUIText>   mYearOneFoodText;

	std::shared_ptr<GUIButton> mYearTwoLowerFoodByTenButton;
	std::shared_ptr<GUIButton> mYearTwoLowerFoodByFiveButton;
	std::shared_ptr<GUIButton> mYearTwoLowerFoodByOneButton;
	std::shared_ptr<GUIButton> mYearTwoRaiseFoodByOneButton;
	std::shared_ptr<GUIButton> mYearTwoRaiseFoodByFiveButton;
	std::shared_ptr<GUIButton> mYearTwoRaiseFoodByTenButton;
	std::shared_ptr<GUIText>   mYearTwoFoodText;

	std::shared_ptr<GUIButton> mYearThreeLowerFoodByTenButton;
	std::shared_ptr<GUIButton> mYearThreeLowerFoodByFiveButton;
	std::shared_ptr<GUIButton> mYearThreeLowerFoodByOneButton;
	std::shared_ptr<GUIButton> mYearThreeRaiseFoodByOneButton;
	std::shared_ptr<GUIButton> mYearThreeRaiseFoodByFiveButton;
	std::shared_ptr<GUIButton> mYearThreeRaiseFoodByTenButton;
	std::shared_ptr<GUIText>   mYearThreeFoodText;

	std::shared_ptr<GUIButton> mYearFourLowerFoodByTenButton;
	std::shared_ptr<GUIButton> mYearFourLowerFoodByFiveButton;
	std::shared_ptr<GUIButton> mYearFourLowerFoodByOneButton;
	std::shared_ptr<GUIButton> mYearFourRaiseFoodByOneButton;
	std::shared_ptr<GUIButton> mYearFourRaiseFoodByFiveButton;
	std::shared_ptr<GUIButton> mYearFourRaiseFoodByTenButton;
	std::shared_ptr<GUIText>   mYearFourFoodText;

	std::shared_ptr<GUIButton> mYearFiveLowerFoodByTenButton;
	std::shared_ptr<GUIButton> mYearFiveLowerFoodByFiveButton;
	std::shared_ptr<GUIButton> mYearFiveLowerFoodByOneButton;
	std::shared_ptr<GUIButton> mYearFiveRaiseFoodByOneButton;
	std::shared_ptr<GUIButton> mYearFiveRaiseFoodByFiveButton;
	std::shared_ptr<GUIButton> mYearFiveRaiseFoodByTenButton;
	std::shared_ptr<GUIText>   mYearFiveFoodText;
	/*Fem år för varor*/
	std::shared_ptr<GUIButton> mYearOneLowerGoodsByTenButton;
	std::shared_ptr<GUIButton> mYearOneLowerGoodsByFiveButton;
	std::shared_ptr<GUIButton> mYearOneLowerGoodsByOneButton;
	std::shared_ptr<GUIButton> mYearOneRaiseGoodsByOneButton;
	std::shared_ptr<GUIButton> mYearOneRaiseGoodsByFiveButton;
	std::shared_ptr<GUIButton> mYearOneRaiseGoodsByTenButton;
	std::shared_ptr<GUIText>   mYearOneGoodsText;

	std::shared_ptr<GUIButton> mYearTwoLowerGoodsByTenButton;
	std::shared_ptr<GUIButton> mYearTwoLowerGoodsByFiveButton;
	std::shared_ptr<GUIButton> mYearTwoLowerGoodsByOneButton;
	std::shared_ptr<GUIButton> mYearTwoRaiseGoodsByOneButton;
	std::shared_ptr<GUIButton> mYearTwoRaiseGoodsByFiveButton;
	std::shared_ptr<GUIButton> mYearTwoRaiseGoodsByTenButton;
	std::shared_ptr<GUIText>   mYearTwoGoodsText;

	std::shared_ptr<GUIButton> mYearThreeLowerGoodsByTenButton;
	std::shared_ptr<GUIButton> mYearThreeLowerGoodsByFiveButton;
	std::shared_ptr<GUIButton> mYearThreeLowerGoodsByOneButton;
	std::shared_ptr<GUIButton> mYearThreeRaiseGoodsByOneButton;
	std::shared_ptr<GUIButton> mYearThreeRaiseGoodsByFiveButton;
	std::shared_ptr<GUIButton> mYearThreeRaiseGoodsByTenButton;
	std::shared_ptr<GUIText>   mYearThreeGoodsText;

	std::shared_ptr<GUIButton> mYearFourLowerGoodsByTenButton;
	std::shared_ptr<GUIButton> mYearFourLowerGoodsByFiveButton;
	std::shared_ptr<GUIButton> mYearFourLowerGoodsByOneButton;
	std::shared_ptr<GUIButton> mYearFourRaiseGoodsByOneButton;
	std::shared_ptr<GUIButton> mYearFourRaiseGoodsByFiveButton;
	std::shared_ptr<GUIButton> mYearFourRaiseGoodsByTenButton;
	std::shared_ptr<GUIText>   mYearFourGoodsText;

	std::shared_ptr<GUIButton> mYearFiveLowerGoodsByTenButton;
	std::shared_ptr<GUIButton> mYearFiveLowerGoodsByFiveButton;
	std::shared_ptr<GUIButton> mYearFiveLowerGoodsByOneButton;
	std::shared_ptr<GUIButton> mYearFiveRaiseGoodsByOneButton;
	std::shared_ptr<GUIButton> mYearFiveRaiseGoodsByFiveButton;
	std::shared_ptr<GUIButton> mYearFiveRaiseGoodsByTenButton;
	std::shared_ptr<GUIText>   mYearFiveGoodsText;
	/*Fem år för teknologi*/
	std::shared_ptr<GUIButton> mYearOneLowerTechByTenButton;
	std::shared_ptr<GUIButton> mYearOneLowerTechByFiveButton;
	std::shared_ptr<GUIButton> mYearOneLowerTechByOneButton;
	std::shared_ptr<GUIButton> mYearOneRaiseTechByOneButton;
	std::shared_ptr<GUIButton> mYearOneRaiseTechByFiveButton;
	std::shared_ptr<GUIButton> mYearOneRaiseTechByTenButton;
	std::shared_ptr<GUIText>   mYearOneTechText;

	std::shared_ptr<GUIButton> mYearTwoLowerTechByTenButton;
	std::shared_ptr<GUIButton> mYearTwoLowerTechByFiveButton;
	std::shared_ptr<GUIButton> mYearTwoLowerTechByOneButton;
	std::shared_ptr<GUIButton> mYearTwoRaiseTechByOneButton;
	std::shared_ptr<GUIButton> mYearTwoRaiseTechByFiveButton;
	std::shared_ptr<GUIButton> mYearTwoRaiseTechByTenButton;
	std::shared_ptr<GUIText>   mYearTwoTechText;

	std::shared_ptr<GUIButton> mYearThreeLowerTechByTenButton;
	std::shared_ptr<GUIButton> mYearThreeLowerTechByFiveButton;
	std::shared_ptr<GUIButton> mYearThreeLowerTechByOneButton;
	std::shared_ptr<GUIButton> mYearThreeRaiseTechByOneButton;
	std::shared_ptr<GUIButton> mYearThreeRaiseTechByFiveButton;
	std::shared_ptr<GUIButton> mYearThreeRaiseTechByTenButton;
	std::shared_ptr<GUIText>   mYearThreeTechText;

	std::shared_ptr<GUIButton> mYearFourLowerTechByTenButton;
	std::shared_ptr<GUIButton> mYearFourLowerTechByFiveButton;
	std::shared_ptr<GUIButton> mYearFourLowerTechByOneButton;
	std::shared_ptr<GUIButton> mYearFourRaiseTechByOneButton;
	std::shared_ptr<GUIButton> mYearFourRaiseTechByFiveButton;
	std::shared_ptr<GUIButton> mYearFourRaiseTechByTenButton;
	std::shared_ptr<GUIText>   mYearFourTechText;

	std::shared_ptr<GUIButton> mYearFiveLowerTechByTenButton;
	std::shared_ptr<GUIButton> mYearFiveLowerTechByFiveButton;
	std::shared_ptr<GUIButton> mYearFiveLowerTechByOneButton;
	std::shared_ptr<GUIButton> mYearFiveRaiseTechByOneButton;
	std::shared_ptr<GUIButton> mYearFiveRaiseTechByFiveButton;
	std::shared_ptr<GUIButton> mYearFiveRaiseTechByTenButton;   
	std::shared_ptr<GUIText>   mYearFiveTechText;

	std::shared_ptr<GUIButton> mGoToNextSlideButton;
	std::shared_ptr<GUIButton> mGoToPreviousSlideButton;
	std::shared_ptr<GUIButton> mTaxesCloseButton;
	std::shared_ptr<GUIButton> mResourcesCloseButton;

	/*GUI-pekare för propaganda*/
	std::shared_ptr<GUIWindow> mPropagandaWindowFirst;
	std::shared_ptr<GUIWindow> mPropagandaWindowSecond;
	
	std::shared_ptr<GUIButton> mPropagandaBuyFoodButton;
	std::shared_ptr<GUIButton> mPropagandaBuyGoodsButton;
	std::shared_ptr<GUIButton> mPropagandaBuyTechButton;
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
	std::shared_ptr<GUIButton> mUpgradeSpaceProgramButton;
	std::shared_ptr<GUIButton> mUpgradeSpyNetworkButton;
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
	std::shared_ptr<GUIButton> mSecondGeneralButton;

	std::shared_ptr<GUIButton> mGoToNextPortraitButton;
	std::shared_ptr<GUIButton> mGoToPreviousPortraitButton;
	std::shared_ptr<GUIButton> mCloseGeneralWindow;



};

#endif