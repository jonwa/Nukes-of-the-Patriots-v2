#ifndef _GAME_MANAGER_H
#define _GAME_MANAGER_H

#include <vector>
#include <map>
#include <string>
#include <sstream>
#include "GUIWindow.h"
#include "GUIButton.h"
#include <SFML\Graphics\Texture.hpp>

class SuperPower;
class Communist;
class Capitalist;
class President;
class GUIText;

class GameManager
{
public:
	static GameManager* getInstance();
	
	~GameManager();

	int											getYear()const;
	std::shared_ptr<SuperPower> 				getCurrentPlayer()const;
	std::vector<std::shared_ptr<SuperPower> >	getPlayers()const;
	void										selectStartingPlayer(std::shared_ptr<SuperPower> startingPlayer);
	void										setCurrentPlayer(std::shared_ptr<SuperPower> nextPlayer);
	void										setYear(int year);
	void										startRound();
	void										nextRound();
	void										addSuperPower(std::shared_ptr<SuperPower> power);
	//void										setVector(std::vector<std::shared_ptr<SuperPower> > SuperPowerVec);
	
	std::shared_ptr<President>					getRandomPresident();
	std::shared_ptr<President>					getGeneral(int number);
	void										init(int year);

	std::shared_ptr<SuperPower> getCapitalist();
	std::shared_ptr<SuperPower> getCommunist();
private:
	static GameManager* mInstance;
	GameManager();
	GameManager(const GameManager&){}
	GameManager operator=(const GameManager&){}

	std::string intToString(int i)
	{
		std::stringstream converter;
		converter << i;
		return converter.str();
	}

	void loadPresidents();
	void loadWindowPosition();
	void loadButtonPosition();
	void initializeGuiElement();
	void initializeGuiFunctions();


	int mYear;
	int mRound;

	std::shared_ptr<GUIText> mYearText;
	// Theoretically you should be able to play with x amount of players - instead of only 2
	std::vector<std::shared_ptr<SuperPower> > mVecSuperPowers;
	std::vector<std::shared_ptr<SuperPower> > mVecPlayersLeft;
	std::vector<std::shared_ptr<President> > mPresidentVector;
	std::vector<std::shared_ptr<President> > mGeneralVector;
	std::map<std::string, std::pair<sf::FloatRect, sf::Texture*> > BetweenTurnsWindow;
	std::map<std::string, std::pair<sf::FloatRect, sf::Texture*> > BetweenTurnsButton;
	std::shared_ptr<SuperPower> mCurrentPlayer;

	std::shared_ptr<GUIWindow> mStatsWindow;
	std::shared_ptr<GUIButton> mCloseStatsWindow;

	std::shared_ptr<GUIWindow> mFirstDecideWhoStartWindow;   //Om bägge har samma spy
	std::shared_ptr<GUIWindow> mSecondDecideWhoStartWindow; //Om den ena har högre spy än den andra
	std::shared_ptr<GUIButton> mCapitalistButton;
	std::shared_ptr<GUIButton> mCommunistButton;
	std::shared_ptr<GUIButton> mCloseFirstWindow;

	std::shared_ptr<GUIText>   mFirstCapitalistSpyNetworkText;
	std::shared_ptr<GUIText>   mFirstCommunistSpyNetworkText;
	std::shared_ptr<GUIText>   mSecondCapitalistSpyNetworkText;
	std::shared_ptr<GUIText>   mSecondCommunistSpyNetworkText;
};


#endif