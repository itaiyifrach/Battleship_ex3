#pragma once
#include <stdlib.h>
#include <direct.h>
#include <string>
#include <cstring>
#include <algorithm>
#include <iostream> 
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <tuple>
#include <windows.h>
#include "IBattleshipGameAlgo.h"
#include "OurBoardData.h"
#include <memory>
#include "BSLogger.h"

#define PLAYER_A_NUM 0
#define PLAYER_B_NUM 1
#define GAME_MANAGER_NUM 10
#define SIZE_SHIP_B 1
#define SIZE_SHIP_P 2
#define SIZE_SHIP_M 3
#define SIZE_SHIP_D 4
#define BOARD_SUFFIX "\\*.sboard"
#define DLL_SUFFIX "\\*.dll"
#define WRONG_PATH "Wrong path: "
#define MISSING_BOARD "Missing board file (*.sboard) looking in path: "
#define MISSING_ALGO "Missing an algorithm (dll) file looking in path: "
#define LOAD_DLL_ERR "Cannot load dll: "
#define FOR_PLAYER " for player "
#define NOT_ENOUGH_LEGAL "Error: insufficient legal files to start the competition"
#define LEGAL_PLAYERS "Number of legal players: "
#define LEGAL_BOARDS "Number of legal boards: "
#define BOARD_MISTAKE_0 "Wrong size or shape for ship "
#define BOARD_MISTAKE_2 "Adjacent Ships on Board"
#define BOARD_MISTAKE_3 "The players don't have the same number of ships"
#define BOARD_MISTAKE_4 "The players don't have the same types of ships"
#define THREADS_DEFAULT 4

using namespace std;


class GameUtils
{
public:
	//Parses input args.
	static void parseArgs(int argc, char** argv, string& basePath, int& numOfThreads);

	// Board Utils
	static char3DArray allocateBoard(int rows, int cols, int depth);

	/*static char3DArray copyBoard(char3DArray& from,int rows, int cols, int depth);*/

	static vector<string> split(string str, char delimiter);

	//finds all *.sboard files of given path and returns a sorted vector of them
	static int parsePath(const string& basePath, vector<string>& boardNames);

	//Loads all legal boards to vector
	static int getBoards(const string& path, vector<string>& boardNames, vector<pair<char3DArray, int>>& boards);

	//Parse board from .sboard file
	static char3DArray parseBoard(const string& path, const string& boardName, int& x, int& y, int& z);

	//finds all *.dll files of given path and returns a sorted vector of them
	static list<string> getDLLNames(string& path);

	//loads/checks algorithm from dll.
	static bool checkAlgo(const string& path, const string& fileName);
	static pair<IBattleshipGameAlgo*, HINSTANCE> GameUtils::loadAlgo(const string& path, const string& fileName);

	//Loads all players to vector
	static int checkPlayers(const string& path, list<string>& dllNames);

	//Check if the board is valid
	static int checkBoard(char3DArray& board, int rows, int cols, int depth, int* mistakes);
	static bool checkShape(char2DArray& board, char2DArray& markedBoard, int rows, int cols, int posI, int posJ, char shipType, int shipSize, int* mistakes, int player);
	static bool checkBound(char2DArray& board, char shipType, int i, int j, int* mistakes, int player, int* possibleAdj);
	static void checkBoardCut(char2DArray& board, int rows, int cols, int* mistakes, unique_ptr<int[]>& shipsTypeA, unique_ptr<int[]>& shipsTypeB);
	static bool checkSingleTile(char2DArray& board, int rows, int cols, int posI, int posJ);
	static char2DArray getBoardCut(char3DArray& board, int rows, int cols, int depth, int cutBy);

	//Prints the board to console (for debug purposes)
	static void print3DBoard(char3DArray& board, int rows, int cols, int depth);
	static void print2DBoard(char2DArray& board, int rows, int cols);
	static void print1DBoard(vector<char>& board, int n);

	//returns the score-for-a-sink of a ship which is located in (i,j)th coordinate on mainBoard
	static int getScoreForSector(char boardSymbol);

	static bool selfHit(char boardSymbol, int attackPlayer);

	// Check the direction of the ship in "att" coordinate on "Board". return 0 if horizontal, 1 if vertical, 2 if into deep
	static int shipDirection(const OurBoardData& Board, const Coordinate& att);
	static bool isHorizontal(const OurBoardData& Board, const Coordinate& att);
	static bool isVertical(const OurBoardData& Board, const Coordinate& att);
	
	static bool coordinatesComparator(const Coordinate& first, const Coordinate& second);
};