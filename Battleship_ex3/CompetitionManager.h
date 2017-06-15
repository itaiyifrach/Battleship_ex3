#pragma once
#include "GameUtils.h"
#include "BattleshipGame.h"
#include "PlayerComb.h"
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <iomanip>
#include <thread>

#define PRINT_FREQ 5
using namespace std;

struct playerData
{
	int wins;
	int losses;
	int pointsFor;
	int pointsAgainst;
	int gamesPlayed;
	playerData() :wins(0), losses(0), pointsFor(0), pointsAgainst(0),gamesPlayed(0) {};

};
//global structure to hold players data results
vector<vector<playerData>> playersData;
//global queue of games

//global mutex
mutex printerMutex;
mutex queueMutex;
mutex dataMutex;
condition_variable result_printer;
atomic<int> ourLastPrintNumOfGames = 0;
atomic<int> currentNumOfGames = 0;

class CompetitionManager
{


public:
	CompetitionManager( vector<pair<char3DArray, int>>& boardVec,list<string>& playerNames,string path, int numOfBoards,int numOfPlayers,int numOfGames, int numOfThreads) :
		 boardVec(boardVec), playerNames(playerNames.begin(), playerNames.end()), path(path),
		 numOfBoards(numOfBoards),numOfPlayers(numOfPlayers) , numOfGames(numOfGames),numOfThreads(numOfThreads) {};
	void threadWorker(PlayerComb& playerComb);	
	void launcher();

private:
	vector<pair<char3DArray, int>> boardVec;
	vector<string> playerNames;
	const string path;
	const int numOfBoards;
	const int numOfPlayers;
	const int numOfGames;
	const int numOfThreads;		
	bool finished = false;

	//helper functions
	void CompetitionManager::printResults();
	static bool percentCompare(pair<int, playerData> p1, pair<int, playerData> p2);
	int findMinGames() const;
	static void CompetitionManager::updatePlayersData(int playerIndexA, int playerIndexB, int winnerNumber, int pointsForPlayerA, int pointsForPlayerB);
	
};