#pragma once
#include "GameUtils.h"
#include "BattleshipGame.h"
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
	CompetitionManager(vector<string>& dllNames, vector<pair<char3DArray, int>>& boardVec,list<string>& playerNames, int numOfThreads,string path, int numOfGames) :
		 boardVec(boardVec), numOfThreads(numOfThreads), playerNames(playerNames.begin(), playerNames.end()), path(path),numOfGames(numOfGames) {};
	void threadWorker();
	void dataRegistration();
	void launcher();

private:
	vector<pair<char3DArray, int>> boardVec;
	vector<string> playerNames;
	int numOfThreads;
	string path;
	const int numOfGames;
	void printResults(int fixture);
	static bool percentCompare(pair<int, playerData> p1, pair<int, playerData> p2);
	int findMinGames() const;
	bool finished = false;
};