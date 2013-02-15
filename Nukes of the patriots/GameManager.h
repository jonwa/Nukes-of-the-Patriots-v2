#ifndef _GAME_MANAGER_H
#define _GAME_MANAGER_H

#include <vector>
#include <string>
#include <sstream>
#include "GUIWindow.h"
#include "GUIButton.h"

class SuperPower;
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

	sf::Texture&								getPresidentPlaque(std::shared_ptr<President> president);
	sf::Texture&								getGeneralPlaque(std::shared_ptr<President> general);

	std::shared_ptr<SuperPower>					getCapitalist();
	std::shared_ptr<SuperPower>					getCommunist();

	void										init(int year);
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
	void initializeGuiFunctions();

	int mYear;
	int mRound;

	std::shared_ptr<GUIText> mYearText;
	// Theoretically you should be able to play with x amount of players - instead of only 2
	std::vector<std::shared_ptr<SuperPower> > mVecSuperPowers;
	std::vector<std::shared_ptr<SuperPower> > mVecPlayersLeft;
	std::vector<std::shared_ptr<President> > mPresidentVector;
	std::vector<std::shared_ptr<President> > mGeneralVector;
	std::map<std::shared_ptr<President>, sf::Texture*> mPresidentPlaqueMap;
	std::map<std::shared_ptr<President>, sf::Texture*> mGeneralPlaqueMap;
	std::shared_ptr<SuperPower> mCurrentPlayer;

	std::shared_ptr<GUIWindow> mFirstDecideWhoStartWindow;   //Om bägge har samma spy
	std::shared_ptr<GUIWindow> mSecondDecideWhoStartWindow; //Om den ena har högre spy än den andra
};


#endif