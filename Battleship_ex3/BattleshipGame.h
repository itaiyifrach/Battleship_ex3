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
	unique_ptr<IBattleshipGameAlgo, ? ? ? ? ? ? ? > PlayerA;
	unique_ptr<IBattleshipGameAlgo, ? ? ? ? ? ? ? > PlayerB;
	int numOfShipsA;
	int numOfShipsB;
	int rows;
	int cols;
	int depth;
	int scoreA = 0;
	int scoreB = 0;
	OurBoardData mainBoard;
	

public:
	BattleshipGame(tuple<char3DArray, int, int, int, int> board_tuple,unique_ptr<IBattleshipGameAlgo, ???????>& player_A, unique_ptr<IBattleshipGameAlgo, ? ? ? ? ? ? ? >& player_B)
	{
		numOfShipsA = std::get<4>(board_tuple);
		numOfShipsB = std::get<4>(board_tuple);
		rows = std::get<1>(board_tuple);
		cols = std::get<2>(board_tuple);
		depth = std::get<3>(board_tuple);		
		mainBoard = OurBoardData(std::get<0>(board_tuple), rows, cols, depth, GAME_MANAGER_NUM);
		
		PlayerA = std::move(player_A);
		PlayerB = std::move(player_B);
		PlayerA->setPlayer(PLAYER_A_NUM);
		PlayerB->setPlayer(PLAYER_B_NUM);
		PlayerA->setBoard(OurBoardData(std::get<0>(board_tuple), rows, cols, depth, PLAYER_A_NUM));
		PlayerB->setBoard(OurBoardData(std::get<0>(board_tuple), rows, cols, depth, PLAYER_B_NUM));
	}

	~BattleshipGame() = default;

	//Main loop of the gameplay
	void playGame();

private:
	// returns attack result of the attack in coor(i,j,k) and updates mainBoard using "updateBoardAndCheckSink"
	std::pair<AttackResult, bool> BattleshipGame::getAttackResult(const Coordinate& att);

	//called by getAttackResult. Checks if the attack resulted by a sink and updates the mainBoard according to attack result:
	//if it's a sink, erases the ship and returns true
	//if it isn't a sink, updates the (i,j,k)th coordinate as 'X' (for hit) and returns false
	bool updateBoardAndCheckSink(const Coordinate& att);

	void BattleshipGame::getNextAttack(int& turnOf, bool& endGame, Coordinate& currAttack) const;
};
