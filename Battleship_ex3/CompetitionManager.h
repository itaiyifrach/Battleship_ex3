#pragma once
#include "GameUtils.h"

using namespace std;

struct playerData
{
	int wins;
	int losses;
	int pointsFor;
	int pointsAgainst;
	playerData() :wins(0), losses(0), pointsFor(0), pointsAgainst(0) {};

};

vector<vector<playerData>> playersData;
class CompetitionManager
{

	vector<string> dllNames;
	vector<pair<char3DArray, int>> boardVec;
	unique_ptr<IBattleshipGameAlgo[]> players = nullptr;
	int numOfThreads;

	CompetitionManager(vector<pair<char3DArray, int>> boardVec,
		vector<string> dllNames, int numOfThreads) : dllNames(dllNames),boardVec(boardVec),numOfThreads(numOfThreads)
	{
		

	}


};