#include "GameUtils.h"

int main(int argc, char** argv) 
{
	string basePath;
	vector<string> boardNames;
	vector<tuple<char3DArray,int,int,int,int>> boardVec;
	vector<pair<unique_ptr<IBattleshipGameAlgo>, HINSTANCE>> playersVec;
	int numOfBoards = 0, numOfPlayers = 0, numOfGames = 0, numOfThreads = 4;
	
	GameUtils::parseArgs(argc, argv, basePath, numOfThreads);
	auto check = GameUtils::parsePath(basePath, boardNames);
	//check if path was invalid(error printed inside function)
	if(check == -2)
	{
		return -1;
	}
	/*
	//creates a vector of all *.dll files in the given path
	vector<string> dllNames = GameUtils::getDLLNames(basePath);
	//check if number of dll's/boards is invalid (errors printed inside functions)
	if(dllNames.size() < 2 || check == -1)
	{
		return -1;
	}*/
	//loads all boards
	numOfBoards = GameUtils::getBoards(basePath, boardNames, boardVec);
	cout << "# of Valid BOARDS = " << numOfBoards << endl;
	/*
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
	*/

	//prepare all the games

	/*
	BattleshipGame game(playerA, playerB, mainBoard);
	game.playGame(useAnimation, delay);
	FreeLibrary(playerAdll.second);
	FreeLibrary(playerBdll.second);
	return 0;
	*/
	return 0;
}