/*
#include "GameUtils.h"
#include <windows.h>

#define PLAYER_A_WON_STR "Player A won"
#define PLAYER_B_WON_STR "Player B won"
#define POINTS_STR "Points:"
#define PLAYER_A_POINTS_STR "Player A: "
#define PLAYER_B_POINTS_STR "Player B: "
#define PLAYER_A_COLOR 9	//blue
#define PLAYER_B_COLOR 12	//red
#define SEA_COLOR 11
#define WHITE_COLOR 15
#define BOMB_COLOR 14	//yellow
#define MISS_MSG "MISSED!       "
#define HIT_MSG "HIT!          "
#define SINK_A_MSG "SANK B's ship!"
#define SINK_B_MSG "SANK A's ship!"
#define SELF_HIT_MSG "SELF HIT!     "
#define CLEAN_MSG "                        "


class BattleshipGame
{
	IBattleshipGameAlgo* PlayerA;
	IBattleshipGameAlgo* PlayerB;
	int numOfShipsA = SHIPS_NUM;
	int numOfShipsB = SHIPS_NUM;
	int scoreA = 0;
	int scoreB = 0;
	char** mainBoard;
	char** cleanInitialBoard;

public:
	BattleshipGame(IBattleshipGameAlgo* playerA, IBattleshipGameAlgo* playerB, char** board)
		: PlayerA(playerA), PlayerB(playerB) {

		//copy mainBoard
		this->mainBoard = new char*[GameUtils::rows];
		this->cleanInitialBoard = new char*[GameUtils::rows];
		for (int i = 0; i < GameUtils::rows; ++i)
		{
			this->mainBoard[i] = new char[GameUtils::cols];
			this->cleanInitialBoard[i] = new char[GameUtils::cols];
			for (int j = 0; j < GameUtils::cols; ++j)
			{
				this->mainBoard[i][j] = board[i][j];
				this->cleanInitialBoard[i][j] = board[i][j];
			}	
		}
	}

	BattleshipGame(const BattleshipGame&) = delete;

	BattleshipGame& operator=(const BattleshipGame&) = delete;

	~BattleshipGame() {
		for (int i = 0; i < GameUtils::rows; i++)
			delete mainBoard[i];
		delete[](mainBoard);
		for (int i = 0; i < GameUtils::rows; i++)
			delete cleanInitialBoard[i];
		delete[](cleanInitialBoard);
	}

	//Main loop of the gameplay
	void playGame(bool useAnimation, int delay);

private:
	// returns attack result of the attack in coor(i,j) and updates mainBoard using "updateBaordAndCheckSink"
	std::pair<AttackResult, bool> getAttackResult(int i, int j) const;

	//called by getAttackResult. Checks if the attack resulted by a sink and updates the mainBoard according to attack result:
	//if it's a sink, erases the ship and returns true
	//if it isn't a sink, updates the (i,j)th coordinate as 'X' (for hit) and returns false
	bool updateBaordAndCheckSink(int i, int j) const;

	void BattleshipGame::getNextAttack(int& turnOf, bool& endGame, pair<int, int>& currAttack) const;

	// animation functions
	void printColorBoard(HANDLE* hConsole, int turnOf) const;
	static void gotoxy(int i, int j);
	void updateColorBoard(HANDLE* hConsole, int i, int j, int turnOf, AttackResult attackResult, bool selfHit, int delay) const;
	static void hideCursor(HANDLE* hConsole);
};
*/