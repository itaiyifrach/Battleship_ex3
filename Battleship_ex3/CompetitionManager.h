#pragma once
#include "GameUtils.h"

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

class CompetitionManager
{

	vector<string> dllNames;
	vector<pair<char3DArray, int>> boardVec;
	vector<string> playerNames;
	int numOfThreads;

	CompetitionManager(vector<string>& dllNames, vector<pair<char3DArray, int>>& boardVec, vector<string>& playerNames, int numOfThreads) :
		dllNames(dllNames), boardVec(boardVec), playerNames(playerNames), numOfThreads(numOfThreads) {};





};