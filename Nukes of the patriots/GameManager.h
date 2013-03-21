#ifndef _GAME_MANAGER_H
#define _GAME_MANAGER_H

#include <vector>
#include <map>
#include <string>
#include <sstream>
#include "GUIWindow.h"
#include "GUIButton.h"
#include "GUIImage.h"
#include "RemoteClient.h"
#include <SFML\Graphics\Texture.hpp>
#include <SFML\Network.hpp>

class SuperPower;
class Communist;
class Capitalist;
class President;
class GUIText;
class Event;
class Timer;
class GUIElement;
class GUIEditField;
class Sound;
class SoundHandler;

namespace sf
{
class TcpServer;
class TcpClient;
class UdpServer;
class UdpClient;
}

enum ServerState {WAITING, CLOSED, FULL};
enum GameType {VERSUS, LAN};
enum CommunicationRole { SERVER, CLIENT };

class GameManager
{
public:
	static GameManager* getInstance();
	
	~GameManager();

	typedef std::vector<std::string> SaveFilesVec;
	void saveGame();
	void loadGame();
	void saveFileName();
	void loadFileName();
	void setDocumentName(std::string fileName);
	void overwriteCurrentDocument(std::string fileName);
	std::string getDocumentName() const;
	SaveFilesVec& getSaveFilesVec();

	int											getYear()const;
	std::shared_ptr<SuperPower> 				getCurrentPlayer()const;
	std::vector<std::shared_ptr<SuperPower> >	getPlayers()const;
	void										selectStartingPlayer(std::shared_ptr<SuperPower> startingPlayer = nullptr);
	void										setCurrentPlayer(std::shared_ptr<SuperPower> nextPlayer);
	void										setYear(int year);
	void										startRound();
	void										updateStatsWindow();
	void										nextRound();
	void										addSuperPower(std::shared_ptr<SuperPower> power);
	//void										setVector(std::vector<std::shared_ptr<SuperPower> > SuperPowerVec);
	
	void										searchForServers();
	void										createServer();
	void										connectToServer(std::string ipAdress, unsigned short port);

	void										tick(sf::RenderWindow &window);
	void										update(sf::Event &event);

	std::shared_ptr<President>					getRandomPresident();
	std::shared_ptr<President>					getGeneral(int number);

	sf::Texture&								getPresidentPlaque(std::shared_ptr<President> president);
	sf::Texture&								getGeneralPlaque(std::shared_ptr<President> general);

	std::shared_ptr<SuperPower>					getCapitalist();
	std::shared_ptr<SuperPower>					getCommunist();
	std::shared_ptr<Capitalist>					getCap();
	std::shared_ptr<Communist>					getCom();
	std::shared_ptr<RemoteClient>				getRemoteClient();
	void										reset();
	
	std::shared_ptr<GUIWindow>					getStatsWindow();

	void										init(int year);

	GameType									getGameType();
	void										syncGUIClick(std::shared_ptr<GUIElement> guiElement);
	void										syncGUIMouseEnter(std::shared_ptr<GUIElement> guiElement);
	void										syncGUIMouseLeave(std::shared_ptr<GUIElement> guiElement);
	void										syncGUIChange(std::shared_ptr<GUIElement> guiElement);
	void										syncGUIEditField(std::shared_ptr<GUIElement> guiElement);

	bool										isMyTurnToPlay();
	void										triggerOtherPlayersEvent(std::string eventName, sf::Packet &packet, std::string type = "TCP");

	std::shared_ptr<President>					getPresidentByName(std::string name);
	void										removePresidentFromList(std::shared_ptr<President> president);
	void										nextPlayersTurn();
	void										setEnemyTurn();
	void										setMyTurn();
	void										setRandomPlayer(std::shared_ptr<SuperPower> player);
	void										showWaitingScreen(bool show);
	void										stopSearchingForServer();
	void										setInitialized(bool initialized);
	bool										isInitialized();
private:
	std::string mFileName;
	SaveFilesVec mSaveFiles;
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
	void loadWinScreenMusic();
	bool initWinningScreen();

	int mYear;
	int mRound;
	bool mLoaded;

	std::shared_ptr<GUIText> mYearText;
	// Theoretically you should be able to play with x amount of players - instead of only 2
	std::vector<std::shared_ptr<SuperPower> > mVecSuperPowers;
	std::vector<std::shared_ptr<SuperPower> > mVecPlayersLeft;
	std::vector<std::shared_ptr<President> > mPresidentVector;
	std::vector<std::shared_ptr<President> > mGeneralVector;

	std::map<std::string, std::pair<sf::FloatRect, sf::Texture*> > BetweenTurnsWindow;
	std::map<std::string, std::pair<sf::FloatRect, sf::Texture*> > BetweenTurnsButton;

	std::map<std::string, std::shared_ptr<sf::Music> >	mWinScreenMusic;

	std::map<std::shared_ptr<President>, sf::Texture*> mPresidentPlaqueMap;
	std::map<std::shared_ptr<President>, sf::Texture*> mGeneralPlaqueMap;

	std::shared_ptr<Sound> mCapitalistWins;
	std::shared_ptr<Sound> mCommunistWins;

	std::shared_ptr<SuperPower> mCurrentPlayer;

	std::shared_ptr<GUIWindow> mParentWindow;

	std::shared_ptr<GUIWindow> mStatsWindow[2];
	std::shared_ptr<GUIButton> mCloseStatsWindow;

	std::shared_ptr<GUIWindow> mFirstDecideWhoStartWindow;   //Om bägge har samma spy
	std::shared_ptr<GUIWindow> mSecondDecideWhoStartWindow; //Om den ena har högre spy än den andra
	std::shared_ptr<GUIButton> mCapitalistButton;
	std::shared_ptr<GUIButton> mCommunistButton;
	std::shared_ptr<GUIButton> mCloseFirstWindow;

	// Texter för de två fönster som finns för att visa lagens spionnätverk för rundan som varit
	std::shared_ptr<GUIText>   mFirstCapitalistSpyNetworkText;
	std::shared_ptr<GUIText>   mFirstCommunistSpyNetworkText;
	std::shared_ptr<GUIText>   mSecondCapitalistSpyNetworkText;
	std::shared_ptr<GUIText>   mSecondCommunistSpyNetworkText;

	std::shared_ptr<GUIText>   mCommunistHeadline[2];
	std::shared_ptr<GUIText>   mCapitalistHeadline[2];

	std::shared_ptr<GUIWindow> mUnableToSaveWindow;
	std::shared_ptr<GUIButton> mCancelSaveButton;
	std::shared_ptr<GUIButton> mOverWriteButton;
	std::shared_ptr<GUIText>   mUnableToSaveText;

	std::shared_ptr<GUIWindow>    mWinScreenWindow[2];
	std::shared_ptr<GUIText>	  mWinningTeamName[2];
	std::shared_ptr<GUIButton>    mWinScreenOkayButton[2];
	std::shared_ptr<GUIImage>     mWinningTeamBanners[2];
	std::shared_ptr<GUIText>      mWinScreenText[2];

	sf::TcpServer* mTcpServer;
	sf::TcpClient* mTcpClient;

	sf::UdpServer* mUdpServer;
	sf::UdpClient* mUdpClient;

	Event *mCreateServerEvent;
	Event *mConnectToServerEvent;
	Timer *mCreateServerTimer;
	Timer *mSearchForServerTimer;

	ServerState			mServerState;
	GameType			mGameType;
	CommunicationRole	mRole;

	std::shared_ptr<RemoteClient> mRemoteClient;
	std::string mRemoteIpAddress;
	unsigned short mRemotePort;

	int mPlayersTurn; // 0 = servers turn
	bool mReady;
	bool mShowWaitingScreen;
	bool mInitialized;
};


#endif