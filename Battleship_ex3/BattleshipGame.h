#pragma once

#include "GameUtils.h"
#include "OurBoardData.h"

#define PLAYER_A_WON_STR "Player A won"
#define PLAYER_B_WON_STR "Player B won"
#define POINTS_STR "Points:"
#define PLAYER_A_POINTS_STR "Player A: "
#define PLAYER_B_POINTS_STR "Player B: "

class BattleshipGame
{
	IBattleshipGameAlgo* PlayerA;
	IBattleshipGameAlgo* PlayerB;
	int numOfShipsA;
	int numOfShipsB;
	int rows;
	int cols;
	int depth;
	OurBoardData mainBoard;
	int scoreA;
	int scoreB;

public:
	BattleshipGame(pair<char3DArray, int>& board_pair, IBattleshipGameAlgo* player_A, IBattleshipGameAlgo* player_B) :
		PlayerA(player_A), PlayerB(player_B),
		numOfShipsA(board_pair.second), numOfShipsB(board_pair.second),
		rows(int(board_pair.first.size())), cols(int(board_pair.first[0].size())), depth(int(board_pair.first[0][0].size())),
		mainBoard(board_pair.first, rows, cols, depth, GAME_MANAGER_NUM),
		scoreA(0), scoreB(0)
	{
		PlayerA->setPlayer(PLAYER_A_NUM);
		PlayerB->setPlayer(PLAYER_B_NUM);		
		PlayerA->setBoard(OurBoardData(board_pair.first, rows, cols, depth, PLAYER_A_NUM));
		PlayerB->setBoard(OurBoardData(board_pair.first, rows, cols, depth, PLAYER_B_NUM));
	}	

	~BattleshipGame() = default;

	//Main loop of the gameplay-returns a tuple of <winnerNumber,points for player A,points for player B>
	tuple<int, int, int> BattleshipGame::playGame();

private:
	// returns attack result of the attack in coor(i,j,k) and updates mainBoard using "updateBoardAndCheckSink"
	std::pair<AttackResult, bool> BattleshipGame::getAttackResult(const Coordinate& att);

	//called by getAttackResult. Checks if the attack resulted by a sink and updates the mainBoard according to attack result:
	//if it's a sink, erases the ship and returns true
	//if it isn't a sink, updates the (i,j,k)th coordinate as 'X' (for hit) and returns false
	bool updateBoardAndCheckSink(const Coordinate& att);

	void BattleshipGame::getNextAttack(int& turnOf, bool& endGame, Coordinate& currAttack) const;
};
