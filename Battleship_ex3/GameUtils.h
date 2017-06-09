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
#include <tuple>
#include <windows.h>
#include "IBattleshipGameAlgo.h"

#define PLAYER_A_NUM 0
#define PLAYER_B_NUM 1
#define SIZE_SHIP_B 1
#define SIZE_SHIP_P 2
#define SIZE_SHIP_M 3
#define SIZE_SHIP_D 4
#define BOARD_SUFFIX "\\*.sboard"
#define ATTACK_SUFFIX "\\*.attack"
#define DLL_SUFFIX "\\*.dll"
#define WRONG_PATH "Wrong path: "
#define MISSING_BOARD "Missing board file (*.sboard) looking in path: "
#define MISSING_ALGO "Missing an algorithm (dll) file looking in path: "
#define INIT_FAILED "Algorithm initialization failed for dll: "
#define LOAD_DLL_ERR "Cannot load dll: "
#define BOARD_MISTAKE_0 "Wrong size or shape for ship "
#define FOR_PLAYER " for player "
#define BOARD_MISTAKE_2 "Adjacent Ships on Board"
#define BOARD_MISTAKE_3 "The players don't have the same number of ships"
#define BOARD_MISTAKE_4 "The players don't have the same types of ships"

using namespace std;

class GameUtils
{
public:
	//Check if input args are valid. If so parse the board and check its validity.
	static int initialize(int argc, char** argv, char** board, int numRows, int numCols, string& basePath);
	
	// Dynamically allocate a 3D array
	static char*** allocateBoard(int rows, int cols, int depth);
	//Dynamically deallocate a 3D array
	static void destroyBoard(char*** board, int rows, int cols, int depth);
	static vector<string> split(string str, char delimiter);


	/*
	function that extracts a .file_suffix file from given path.
	return 0 upon success, -1 o/w.
	*/
	static int parsePath(const string& basePath, string& file_path);
	
	//Parse rows*cols matrix from .sboard file to board
	static char*** parseBoard(const string& boardPath);
	
	//finds all *.dll files of given path and returns a sorted vector of them
	static vector<string> getDLLNames(string& path);
	
	//loads algorithm from dll. if an error occured prints it and frees dll lib if neccessery.
	static std::pair<IBattleshipGameAlgo*, HINSTANCE> loadAlgo(const string& path, const string& fileName);
	
	//Check if the board is valid. if yes returns true, Otherwise returns false and prints to console the relevant mistakes
	static bool checkBoard(const char*** board, int rows, int cols, int depth, int* mistakes);
	static bool checkShape(char** board, char** markedBoard, int rows, int cols, int posI, int posJ, char shipType, int shipSize, int* mistakes, int player);
	static bool checkBound(char** board, char shipType, int i, int j, int* mistakes, int player, int* possibleAdj);
	static void checkBoardCut(char** board, int rows, int cols, int* mistakes, unique_ptr<int[]>& shipsTypeA, unique_ptr<int[]>& shipsTypeB);
	static char** getBoardCut(const char*** board, int rows, int cols, int depth, bool cutByDepth);

	//Prints the board to console (for debug purposes)
	static void printBoard(char** board);
	
	static char** initPlayerBoard(char** mainBoard, int playerNum);

	//check if ship is horizontal or vertical
	static bool isVertical(char** board, int rows, int cols, int i, int j);

	//returns the score-for-a-sink of a ship which is located in (i,j)th coordinate on mainBoard
	static int getScoreForSector(char boardSymbol);

	static bool selfHit(char boardSymbol, int attackPlayer);
};
