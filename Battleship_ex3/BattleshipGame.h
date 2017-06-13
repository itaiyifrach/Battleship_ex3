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
	int scoreA = 0;
	int scoreB = 0;
	char3DArray mainBoard;
	

public:
	BattleshipGame(tuple<char3DArray, int, int, int, int> board_tuple,unique_ptr<IBattleshipGameAlgo> player_A, unique_ptr<IBattleshipGameAlgo> player_B)
	{
		numOfShipsA = std::get<4>(board_tuple);
		numOfShipsB = std::get<4>(board_tuple);
		rows = std::get<1>(board_tuple);
		cols = std::get<2>(board_tuple);
		depth = std::get<3>(board_tuple);		
		mainBoard = GameUtils::copyBoard(std::get<0>(board_tuple));
		PlayerA = player_A.release();
		PlayerB = player_B.release();
		PlayerA->setPlayer(PLAYER_A_NUM);
		PlayerB->setPlayer(PLAYER_B_NUM);
		OurBoardData player_a_board_data;
		player_a_board_data.setData(mainBoard, rows, cols, depth, PLAYER_A_NUM);
		OurBoardData player_b_board_data;
		player_a_board_data.setData(mainBoard, rows, cols, depth, PLAYER_B_NUM);
		PlayerA->setBoard(player_a_board_data);
		PlayerB->setBoard(player_b_board_data);

	}

	BattleshipGame(const BattleshipGame&) = delete;

	BattleshipGame& operator=(const BattleshipGame&) = delete;

	~BattleshipGame() = default;

	//Main loop of the gameplay
	void playGame();

private:
	// returns attack result of the attack in coor(i,j,k) and updates mainBoard using "updateBoardAndCheckSink"
	std::pair<AttackResult, bool> BattleshipGame::getAttackResult(int i, int j, int k) const;

	//called by getAttackResult. Checks if the attack resulted by a sink and updates the mainBoard according to attack result:
	//if it's a sink, erases the ship and returns true
	//if it isn't a sink, updates the (i,j,k)th coordinate as 'X' (for hit) and returns false
	bool updateBoardAndCheckSink(int i, int j,int k) const;

	//check if ship is horizontal or vertical or
	int BattleshipGame::shipDirection(int i, int j, int k) const;

	void BattleshipGame::getNextAttack(int& turnOf, bool& endGame, Coordinate& currAttack) const;

};
