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
	numOfPlayers = GameUtils::checkPlayers(basePath,dllNames);
	//print number of legal players and boards
	cout << LEGAL_PLAYERS << numOfPlayers << endl;
	BSLogger::loggerPrintInfo(LEGAL_PLAYERS + to_string(numOfPlayers));
	cout << LEGAL_BOARDS << numOfBoards << endl;
	BSLogger::loggerPrintInfo(LEGAL_BOARDS + to_string(numOfBoards));

	if (numOfBoards < 1 || numOfPlayers < 2)
	{
		cout << NOT_ENOUGH_LEGAL << endl;
		BSLogger::loggerPrintError(NOT_ENOUGH_LEGAL);
		BSLogger::closeLogger();
		return -1;
	}
    //auto pair1 = GameUtils::loadAlgo(basePath, dllNames.front());
	//auto pair2 = GameUtils::loadAlgo(basePath, dllNames.back());
	/*BattleshipGame game(boardVec[0], pair1.first, pair2.first);
	auto gameResults=game.playGame();
	string third = "game results:\nthe winner is player number " + std::to_string(get<0>(gameResults)) + "point for playerA: " + std::to_string(get<1>(gameResults)) + "points for playerB: " + std::to_string(get<2>(gameResults));
	cout << third;*/
	int numOfShipsA = boardVec[0].second;
	int numOfShipsB = boardVec[0].second;
	int rows = int(boardVec[0].first.size());
	int cols = int(boardVec[0].first[0].size());
	int depth = int(boardVec[0].first[0][0].size());
	auto mainBoard = OurBoardData(boardVec[0].first, rows, cols, depth, GAME_MANAGER_NUM);
	auto playerA = new BattleshipPlayerSmart();
	auto playerB = new BattleshipPlayerSmart();
	playerA->setPlayer(PLAYER_A_NUM);
	playerB->setPlayer(PLAYER_B_NUM);
	playerA->setBoard(OurBoardData(boardVec[0].first, rows, cols, depth, PLAYER_A_NUM));
	playerB->setBoard(OurBoardData(boardVec[0].first, rows, cols, depth, PLAYER_B_NUM));
	auto c = playerA->attack();
	numOfGames = numOfPlayers*(numOfPlayers - 1)*numOfBoards;
	CompetitionManager competition(boardVec, dllNames, basePath, numOfBoards, numOfPlayers, numOfGames, numOfThreads);
	CompetitionManager::launcher(competition);
	BSLogger::closeLogger();
	return 0;
}
