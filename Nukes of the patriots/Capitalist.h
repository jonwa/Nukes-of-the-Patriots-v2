/*
 * Klass f�r kapitalisterna i spelet Nukes of the Patriots
 * 
 *    Arvid Backman 2013-01-21
 *	  Jon Wahlstr�m
 */

#ifndef CAPITALIST_H
#define CAPITALIST_H

//class President;
class GUIManager;
class GUIElement;
class GUIButton;
class GUIText;
class GUIWindow;
class President;
class Randomizer;

#include "SuperPower.h"
#include <vector>
#include <memory>
#include <map>
#include <sstream>
#include <SFML\Graphics\RenderWindow.hpp>
#include <SFML\Audio\Music.hpp>

class Capitalist : public SuperPower 
{
public:
	Capitalist();
	~Capitalist();

	std::shared_ptr<President>	getPresident();
	void						setPresident(std::shared_ptr<President>);


	void upgradeNuclearWeapon();
	void upgradeSpaceProgram();
	void upgradeSpyNetwork();
	
	void chooseLeader();

	void showGUI();
	void hideGUI();

	int	increaseTaxCost(int currentTax);
	int	decreaseTaxCost(int currentTax);

	void setFood(int value);
	void setGoods(int value);
	void setTech(int value);

	void playMusic();

private:
	std::string intToString(int i)
	{
		std::stringstream converter;
		converter << i;
		return converter.str();
	}

	std::shared_ptr<President> mPresident;
	std::shared_ptr<President> mFirstPresident;
	std::shared_ptr<President> mSecondPresident;

	/*Skapar tv� std::map. En dit alla knappar f�r kapitalisternas l�ggs in i samt 
	  en dit alla kapitalisternas f�nster l�ggs in i. Detta sker via h�mtade v�rden
	  fr�n XML dokument samt genom ResourceHandler  
	  Av: Jon Wahlstr�m 2013-01-31
																				*/
	std::map<std::string, std::pair<sf::FloatRect, sf::Texture*> > CapitalistButtons;
	std::map<std::string, std::pair<sf::FloatRect, sf::Texture*> > CapitalistWindows;
	std::map<std::string, std::shared_ptr<sf::Music> > CapitalistMusic;
	
	void loadButtonPosition();
	void loadWindowPosition();
	void loadCapitalistMusic();
	void initializeGuiFunctions();
	void initializeCapitalistWindow();

	
	//President	*mPresident;
	
	std::shared_ptr<GUIText> mNuclearText;
	std::shared_ptr<GUIText> mSpaceText;
	std::shared_ptr<GUIText> mSpyText;
	std::shared_ptr<GUIText> mFoodText;	
	std::shared_ptr<GUIText >mGoodsText;  
	std::shared_ptr<GUIText> mTechText;	  


	/*GUI-pekare f�r kapitalisternas interface*/
	std::shared_ptr<GUIWindow> mCapitalistMainWindow;
	std::shared_ptr<GUIButton> mCapitalistPresident;
	std::shared_ptr<GUIButton> mCapitalistTaxesButton;		
	std::shared_ptr<GUIButton> mCapitalistResourceButton;	
	std::shared_ptr<GUIButton> mCapitalistUpgradeButton;		
	std::shared_ptr<GUIButton> mCapitalistExportButton;	
	/*GUI-pekare f�r end turn*/
	std::shared_ptr<GUIButton> mCapitalistEndTurnButton;

	/*GUI-pekare f�r taxes */
	std::shared_ptr<GUIWindow> mTaxesWindow;
	std::shared_ptr<GUIButton> mLowerTaxesButton;
	std::shared_ptr<GUIButton> mRaiseTaxesButton;
	std::shared_ptr<GUIButton> mTaxesCloseButton;
	/*GUI-pekare f�r resources*/
	std::shared_ptr<GUIWindow> mResourceWindow;
	std::shared_ptr<GUIButton> mLowerFoodByTenButton;
	std::shared_ptr<GUIButton> mLowerFoodByFiveButton;
	std::shared_ptr<GUIButton> mLowerFoodByOneButton;
	std::shared_ptr<GUIButton> mRaiseFoodByOneButton;
	std::shared_ptr<GUIButton> mRaiseFoodByFiveButton;
	std::shared_ptr<GUIButton> mRaiseFoodByTenButton;
	std::shared_ptr<GUIButton> mLowerGoodsByTenButton;
	std::shared_ptr<GUIButton> mLowerGoodsByFiveButton;
	std::shared_ptr<GUIButton> mLowerGoodsByOneButton;
	std::shared_ptr<GUIButton> mRaiseGoodsByOneButton;
	std::shared_ptr<GUIButton> mRaiseGoodsByFiveButton;
	std::shared_ptr<GUIButton> mRaiseGoodsByTenButton;
	std::shared_ptr<GUIButton> mLowerTechByTenButton;
	std::shared_ptr<GUIButton> mLowerTechByFiveButton;
	std::shared_ptr<GUIButton> mLowerTechByOneButton;
	std::shared_ptr<GUIButton> mRaiseTechByOneButton;
	std::shared_ptr<GUIButton> mRaiseTechByFiveButton;
	std::shared_ptr<GUIButton> mRaiseTechByTenButton;
	std::shared_ptr<GUIButton> mResourceCloseButton;
	/*GIU-pekare f�r upgrade*/
	std::shared_ptr<GUIWindow> mUpgradeWindow;
	std::shared_ptr<GUIButton> mUpgradeNuclearWeaponButton;
	std::shared_ptr<GUIButton> mUpgradeSpaceProgramButton;
	std::shared_ptr<GUIButton> mUpgradeSpyNetworkButton;
	std::shared_ptr<GUIButton> mUpgradeCloseButton;
	/*GUI-pekare f�r export*/
	std::shared_ptr<GUIWindow> mExportWindow;
	std::shared_ptr<GUIButton> mExportLowerFoodButton;
	std::shared_ptr<GUIButton> mExportRaiseFoodButton;
	std::shared_ptr<GUIButton> mExportLowerGoodsButton;
	std::shared_ptr<GUIButton> mExportRaiseGoodsButton;
	std::shared_ptr<GUIButton> mExportLowerTechButton;
	std::shared_ptr<GUIButton> mExportRaiseTechButton;
	std::shared_ptr<GUIButton> mExportCloseButton;

	std::shared_ptr<GUIWindow> mChoosePresidentWindow;
	std::shared_ptr<GUIWindow> mPickedPresidentWindow;
	std::shared_ptr<GUIButton> mFirstPresidentButton;
	std::shared_ptr<GUIButton> mSecondPresidentButton;
	std::shared_ptr<GUIButton> mPickedPresidentButton;
	std::shared_ptr<GUIButton> mClosePresidentWindow;
};


#endif