#pragma once
#include "GameUtils.h"
#include "BattleshipGame.h"
#include "PlayerComb.h"
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <iomanip>
#include <thread>

#define PRINT_FREQ 10
#define LAUNCHING_COMP "The competition has started with "
#define EXITING_COMP "The competition has ended after "

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
extern vector<vector<playerData>> playersData;
//global mutexs
extern mutex printerMutex;
extern mutex queueMutex;
extern mutex dataMutex;
extern mutex debugMutex;
//condition variable for printer
extern condition_variable result_printer;
//atomic vars for the printing
extern int ourLastPrintNumOfGames;
extern atomic<int> currentNumOfGames;
//global var from PlayerComb.h
extern bool finished;


class CompetitionManager
{


public:
	CompetitionManager( vector<pair<char3DArray, int>>& boardVec,	vector<tuple<GetAlgoFuncType, HINSTANCE,string>>& playersVec,
		string path, int numOfBoards,int numOfPlayers,int numOfGames, int numOfThreads) :
		 boardVec(boardVec), playersVec(playersVec), path(path),
		 numOfBoards(numOfBoards),numOfPlayers(numOfPlayers) , numOfGames(numOfGames),numOfThreads(min(numOfThreads,numOfGames))
	{
		playersData = vector<vector<playerData>>(numOfPlayers);
	};
	static void threadWorker(CompetitionManager& competition, PlayerComb& gamesQueue);
	static void launcher(CompetitionManager& competition);

private:
	const vector<pair<char3DArray, int>> boardVec;
	const vector<tuple<GetAlgoFuncType, HINSTANCE,string>> playersVec;
	const string path;
	const int numOfBoards;
	const int numOfPlayers;
	const int numOfGames;
	const int numOfThreads;	

	//helper functions
	static void printResults(CompetitionManager& competition, int maxLengthName);
	static int getMaxLengthName(CompetitionManager& competition);
	static bool percentCompare(pair<int, playerData> p1, pair<int, playerData> p2);
	static int findMinGames();
	static void updatePlayersData(int playerIndexA, int playerIndexB, int winnerNumber, int pointsForPlayerA, int pointsForPlayerB);
	
};