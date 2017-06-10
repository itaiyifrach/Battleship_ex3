#include "GameUtils.h"

int main(int argc, char** argv) 
{
	string basePath;
	vector<string> boardNames;
	vector<unique_ptr<unique_ptr<unique_ptr<char[]>[]>[]>> boardVec;
	vector<pair<unique_ptr<IBattleshipGameAlgo>, HINSTANCE>> playersVec;
	int numOfBoards = 0, numOfPlayers = 0, numOfGames = 0, numOfThreads = 4;
	

	GameUtils::parseArgs(argc, argv, basePath, numOfThreads);
	auto check = GameUtils::parsePath(basePath, boardNames);
	//check if path was invalid(error printed inside function)
	if(check == -2)
	{
		return -1;
	}
	
	//creates a vector of all *.dll files in the given path
	vector<string> dllNames = GameUtils::getDLLNames(basePath);
	//check if number of dll's/boards is invalid (errors printed inside functions)
	if(dllNames.size() < 2 || check == -1)
	{
		return -1;
	}
	//loads all boards
	numOfBoards = GameUtils::getBoards(basePath, boardNames, boardVec);
	
	//loads all dll's
	numOfPlayers = GameUtils::getPlayers(basePath,dllNames,playersVec);
	//print number of legal players and boards
	cout << LEGAL_PLAYERS << numOfPlayers << endl;
	cout << LEGAL_BOARDS << numOfBoards << endl;

	if(numOfBoards < 1 || numOfPlayers < 2)
	{
		cout << NOT_ENOUGH_LEGAL << endl;
		GameUtils::freeLibs(playersVec);
		return -1;
	}

	/*char** tempBoard = GameUtils::initPlayerBoard(mainBoard, PLAYER_A_NUM);
	playerA->setBoard(PLAYER_A_NUM, (const char**) tempBoard, GameUtils::rows, GameUtils::cols);
	for (int i = 0; i < GameUtils::rows; i++)
		delete tempBoard[i];
	delete[] tempBoard;*/


	/*
	BattleshipGame game(playerA, playerB, mainBoard);
	game.playGame(useAnimation, delay);
	FreeLibrary(playerAdll.second);
	FreeLibrary(playerBdll.second);
	return 0;
	*/
	return 0;
}