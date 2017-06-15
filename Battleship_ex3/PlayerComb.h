#pragma once
#include <vector>
#include <deque>
#include <random> //random generators
#include <algorithm>  //std::shuffle
#include <tuple>
#include "CompetitionManager.h"


using namespace std;

class PlayerComb
{
public:
	 PlayerComb(int numOfPlayers, int numOfBoards, int numOfGames): gamesQueue(numOfPlayers),numOfPlayers(numOfPlayers), numOfBoards(numOfBoards), numOfGames(numOfGames)
	 {
		 random_device rd;
		 mt19937 g(rd());
		
		 for (int i = 0; i < numOfPlayers; i++)
		 {
			 gamesQueue[i] = buildQueue(i, numOfPlayers);
			 shuffle(gamesQueue[i].begin(), gamesQueue[i].end(), g);
		 }
	 	
	 }	 

	 tuple<int, int, int> getGameParams();

private:
	//games queue is a vector which holds for each player(indices according to vector of player filenames) a deque of pairs-
	// <rival number , current board number for the game>
	vector<deque<pair<int, int>>> gamesQueue;
	const int numOfPlayers;
	const int numOfBoards;
	const int numOfGames;
	int currentIndex = 0;
	int numOfExtractedGames= 0;

	bool PlayerComb::isEmpty() const;
	static deque<pair<int, int>> buildQueue(int playerNum, int numOfPlayers);

};