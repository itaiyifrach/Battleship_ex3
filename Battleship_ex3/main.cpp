#include "GameUtils.h"
#include "BattleshipGame.h"
#include "CompetitionManager.h"
#include "BSLogger.h"
#include "BattleshipPlayerSmart.h"

int main(int argc, char** argv) 
{
	string basePath;
	vector<string> boardNames;
	vector<pair<char3DArray,int>> boardVec;	
	int numOfBoards = 0, numOfPlayers = 0, numOfGames = 0, numOfThreads = THREADS_DEFAULT;
	
	GameUtils::parseArgs(argc, argv, basePath, numOfThreads);

	// creating the logger:
	BSLogger::initLogger(basePath);

	auto check = GameUtils::parsePath(basePath, boardNames);
	//check if path was invalid(error printed inside function)
	if (check == -2)
	{
		BSLogger::closeLogger();
		return -1;
	}
	
	//creates a vector of all *.dll files in the given path
	list<string> dllNames = GameUtils::getDLLNames(basePath);
	//check if number of dll's/boards is invalid (errors printed inside functions)
	if (dllNames.size() < 2 || check == -1)
	{
		BSLogger::closeLogger();
		return -1;
	}
	//loads all boards
	numOfBoards = GameUtils::getBoards(basePath, boardNames, boardVec);
	BSLogger::loggerPrintInfo(LEGAL_BOARDS + to_string(numOfBoards));
	auto playersVec = GameUtils::loadPlayers(basePath,dllNames);
	numOfPlayers = int(playersVec.size());
	BSLogger::loggerPrintInfo(LEGAL_PLAYERS + to_string(numOfPlayers));
	//print number of legal players and boards
	cout << LEGAL_PLAYERS << numOfPlayers << endl;
	cout << LEGAL_BOARDS << numOfBoards << endl;

	if (numOfBoards < 1 || numOfPlayers < 2)
	{
		cout << NOT_ENOUGH_LEGAL << endl;
		BSLogger::loggerPrintError(NOT_ENOUGH_LEGAL);
		BSLogger::closeLogger();
		return -1;
	}

	numOfGames = numOfPlayers*(numOfPlayers - 1)*numOfBoards;
	CompetitionManager competition(boardVec,  playersVec, basePath, numOfBoards, numOfPlayers, numOfGames, numOfThreads);
	CompetitionManager::launcher(competition);
	BSLogger::closeLogger();
	GameUtils::freeLibs(playersVec);
	return 0;
}
