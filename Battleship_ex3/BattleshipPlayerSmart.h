#pragma once

#include <algorithm>
#include <ctime>
#include <iostream>
#include <random>
#include <list>
#include "IBattleshipGameAlgo.h"
#include <algorithm>
#include "GameUtils.h"

using namespace std;

class BattleshipPlayerSmart : public IBattleshipGameAlgo
{
	int playerNum;
	int cols;
	int rows;
	char** myBoard;
	
	list<pair<int, int>> attackMoves;
	list<pair<int, int>> deceptionMoves;
	int numOfDeceptionMoves;
	bool isDecepting;

	enum class AttackState {
		searchForFirstHit,
		afterFirstHitCheckHorizontalAttLeft,
		afterFirstHitCheckHorizontalAttRight,
		foundHorizontalAttLeft,
		foundHorizontalAttRight,
		foundVerticalAttUp,
		foundVerticalAttDown,
	};
	AttackState state;

	pair<int, int> nextAttack;
	pair<int, int> rightmostHit;
	pair<int, int> leftmostHit;
	pair<int, int> upmostHit;
	pair<int, int> downmostHit;

public:
	BattleshipPlayerSmart() : numOfDeceptionMoves(2), isDecepting(false), state(AttackState::searchForFirstHit), rightmostHit{ -1,-1 }, leftmostHit{ -1,-1 }, upmostHit{ -1,-1 }, downmostHit{ -1,-1 } {}

	BattleshipPlayerSmart(const BattleshipPlayerSmart&) = delete;

	BattleshipPlayerSmart& operator=(const BattleshipPlayerSmart&) = delete;

	~BattleshipPlayerSmart() {
		for (int i = 0; i < rows; i++)
			delete myBoard[i];
		delete[](myBoard);
	}

	// called once to notify player on his myBoard
	virtual void setBoard(int player, const char** mainBoard, int numRows, int numCols) override;

	// ask player for his move
	virtual std::pair<int, int> attack() override;

	// notify on last move result
	virtual void notifyOnAttackResult(int player, int row, int col, AttackResult result) override;

	// called once to allow init from files if needed returns whether the init succeeded or failed
	virtual bool init(const std::string& path) override
	{
		return true;
	}




private:
	//generates a list with all possible attack moves after initializing players' board and stores
	//it in "attackMoves" member with the following logic: first we look for ships large ships and we dscend to the smallest
	list<pair<int, int>> BattleshipPlayerSmart::generateAllAttackMoves() const;

	//mark sinked ship and its perimiter on board and erase the relevant moves from the attackMoves list
	void markSinkedShipAndUpdateAttacks(int i, int j);

	void notifyOnMyAttackResult(int row, int col, AttackResult result);

	void notifyOnOpponentsAttackResult(int row, int col, AttackResult result);

	void searchForNextFirstHit(int row, int col, AttackResult result);

	//helper functions for dealing with the transition between attack states of the algorithm. Called from notifyOnMyAttackResult.
	void searchForFirstHitTransition(int row, int col, AttackResult result);

	void afterFirstHitCheckHorizontalAttLeftTransition(int row, int col, AttackResult result);

	void afterFirstHitCheckHorizontalAttRightTransition(int row, int col, AttackResult result);

	void foundHorizontalAttLeftTransition(int row, int col, AttackResult result);

	void foundHorizontalAttRightTransition(int row, int col, AttackResult result);

	void foundVerticalAttUpTransition(int row, int col, AttackResult result);
	
	void foundVerticalAttDownTransition(int row, int col, AttackResult result);
};