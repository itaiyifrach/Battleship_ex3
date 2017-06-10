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
#include <memory>

#define PLAYER_A_NUM 0
#define PLAYER_B_NUM 1
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

using namespace std;

class GameUtils
{
public:
	//Parses input args.
	static void GameUtils::parseArgs(int argc, char** argv, string& basePath);	

	// Dynamically allocate a 3D array
	static unique_ptr<unique_ptr<unique_ptr<char[]>[]>[]> GameUtils::allocateBoard(int rows, int cols, int depth);

	//Dynamically deallocate a 3D array
	//static void destroyBoard(char*** board, int rows, int cols, int depth);

	static vector<string> split(string str, char delimiter);

	//finds all *.sboard files of given path and returns a sorted vector of them
	static int GameUtils::parsePath(const string& basePath, vector<string>& boardNames);

	//Loads all legal boards to vector
	static int GameUtils::getBoards(const string& path, vector<string>& boardNames, vector<unique_ptr<unique_ptr<unique_ptr<char[]>[]>[]>>& boards);

	//Parse board from .sboard file
	static unique_ptr<unique_ptr<unique_ptr<char[]>[]>[]> GameUtils::parseBoard(const string& path, const string& boardName, int& x, int& y, int& z);	

	//finds all *.dll files of given path and returns a sorted vector of them
	static vector<string> getDLLNames(string& path);	

	//loads algorithm from dll.
	static pair<unique_ptr<IBattleshipGameAlgo>, HINSTANCE> GameUtils::loadAlgo(const string& path, const string& fileName);	

	//Loads all players to vector
	static int GameUtils::getPlayers(const string& path, vector<string>& dllNames, vector<pair<unique_ptr<IBattleshipGameAlgo>, HINSTANCE>>& playersVec);

	//Check if the board is valid
	static bool GameUtils::checkBoard(unique_ptr<unique_ptr<unique_ptr<char[]>[]>[]>& board, int rows, int cols, int depth, int* mistakes);

	static bool GameUtils::checkShape(unique_ptr<unique_ptr<char[]>[]>& board, unique_ptr<unique_ptr<char[]>[]>& markedBoard, int rows, int cols, int posI, int posJ, char shipType, int shipSize, int* mistakes, int player);

	static bool GameUtils::checkBound(unique_ptr<unique_ptr<char[]>[]>& board, char shipType, int i, int j, int* mistakes, int player, int* possibleAdj);

	static void GameUtils::checkBoardCut(unique_ptr<unique_ptr<char[]>[]>& board, int rows, int cols, int* mistakes, unique_ptr<int[]>& shipsTypeA, unique_ptr<int[]>& shipsTypeB);

	static unique_ptr<unique_ptr<char[]>[]> GameUtils::getBoardCut(unique_ptr<unique_ptr<unique_ptr<char[]>[]>[]>& board, int rows, int cols, int depth, bool cutByDepth);

	//Prints the board to console (for debug purposes)
	static void print3DBoard(char*** board, int rows, int cols, int depth);
	static void print2DBoard(char** board, int rows, int cols);
	
	//static char** initPlayerBoard(char** mainBoard, int playerNum);

	//check if ship is horizontal or vertical
	static bool isVertical(char** board, int rows, int cols, int i, int j);

	//returns the score-for-a-sink of a ship which is located in (i,j)th coordinate on mainBoard
	static int getScoreForSector(char boardSymbol);

	static bool selfHit(char boardSymbol, int attackPlayer);

	//free all dll libs
	static void GameUtils::freeLibs(vector<pair<unique_ptr<IBattleshipGameAlgo>, HINSTANCE>>& playersVec);
};
