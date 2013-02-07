#ifndef _GAME_MANAGER_H
#define _GAME_MANAGER_H

#include <vector>

class SuperPower;
class President;

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
private:
	static GameManager* mInstance;
	GameManager();
	GameManager(const GameManager&){}
	GameManager operator=(const GameManager&){}

	void loadPresidents();

	int mYear;
	int mRound;
	// Theoretically you should be able to play with x amount of players - instead of only 2
	std::vector<std::shared_ptr<SuperPower> > mVecSuperPowers;
	std::vector<std::shared_ptr<SuperPower> > mVecPlayersLeft;
	std::vector<std::shared_ptr<President> > mPresidentVector;
	std::vector<std::shared_ptr<President> > mGeneralVector;
	std::shared_ptr<SuperPower> mCurrentPlayer;
};

#endif