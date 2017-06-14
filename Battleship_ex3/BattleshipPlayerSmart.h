#pragma once

#include <algorithm>
#include <ctime>
#include <iostream>
#include <random>
#include <list>
#include "IBattleshipGameAlgo.h"
#include <algorithm>
#include "GameUtils.h"
#include "OurBoardData.h"

using namespace std;

class BattleshipPlayerSmart : public IBattleshipGameAlgo
{	
	enum class AttackState {
		searchForFirstHit,
		isHorizontalAttLeft,
		isHorizontalAttRight,
		foundHorizontalAttLeft,
		foundHorizontalAttRight,
		isVerticalAttUp,
		isVerticalAttDown,
		foundVerticalAttUp,
		foundVerticalAttDown,
		foundDepthAttInside,
		foundDepthAttOutside,
	};
	AttackState state;

	Coordinate nextAttack;
	Coordinate rightmostHit;
	Coordinate leftmostHit;
	Coordinate upmostHit;
	Coordinate downmostHit;
	Coordinate inmostHit;
	Coordinate outmostHit;

	OurBoardData myBoard;
	list<Coordinate> attackMoves;
	int playerNum;
public:
	BattleshipPlayerSmart() : state(AttackState::searchForFirstHit),
		nextAttack(-1, -1, -1), rightmostHit(-1, -1, -1),
		leftmostHit(-1, -1, -1), upmostHit(-1, -1, -1),
		downmostHit(-1, -1, -1), inmostHit(-1, -1, -1),
		outmostHit(-1, -1, -1), playerNum(-1) {}

	virtual void setPlayer(int player) {
		playerNum = player;
	}

	// called once to notify player on his myBoard
	void setBoard(const BoardData& board) override;

	// ask player for his move
	Coordinate attack() override;

	// notify on last move result
	void notifyOnAttackResult(int player, Coordinate move, AttackResult result) override;

private:
	//generates a list with all possible attack moves after initializing players' board and stores
	//it in "attackMoves" member with the following logic: first we look for ships large ships and we dscend to the smallest
	list<Coordinate> BattleshipPlayerSmart::generateAllAttackMoves() const;

	//mark sinked ship and its perimiter on board and erase the relevant moves from the attackMoves list
	void markSinkedShipAndUpdateAttacks(Coordinate att);

	void notifyOnMyAttackResult(Coordinate att, AttackResult result);

	void notifyOnOpponentsAttackResult(Coordinate att, AttackResult result);

	void searchForNextFirstHit(Coordinate att, AttackResult result);

	//helper functions for dealing with the transition between attack states of the algorithm. Called from notifyOnMyAttackResult.
	void searchForFirstHitTransition(Coordinate att, AttackResult result);

	void checkHorizontalAttLeftTransition(Coordinate att, AttackResult result);

	void checkHorizontalAttRightTransition(Coordinate att, AttackResult result);

	void foundHorizontalAttLeftTransition(Coordinate att, AttackResult result);

	void foundHorizontalAttRightTransition(Coordinate att, AttackResult result);

	void checkVerticalAttUpTransition(Coordinate att, AttackResult result);

	void checkVerticalAttDownTransition(Coordinate att, AttackResult result);

	void foundVerticalAttUpTransition(Coordinate att, AttackResult result);
	
	void foundVerticalAttDownTransition(Coordinate att, AttackResult result);

	void foundDepthAttInsideTransition(Coordinate att, AttackResult result);
	
	void foundDepthAttOutsideTransition(Coordinate att, AttackResult result);
};