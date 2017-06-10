#include "GameUtils.h"

int main(int argc, char** argv) 
{
	string basePath;
	char*** mainBoard;

	mainBoard = GameUtils::initialize(argc, argv, basePath);
	//check if path was invalid(error printed inside function)
	if (mainBoard == NULL) {
		GameUtils::destroyBoard(mainBoard, 10, 10, 6);
		return -1;
	}
	GameUtils::destroyBoard(mainBoard, 10, 10, 6);

	/*
	//creates a vector of all *.dll files in the given path
	vector<string> fileNames= GameUtils::getDLLNames(basePath);
	//check if number of dll's is invalid or there was a problem with the board (errors printed inside functions)
	if(fileNames.size()<2||check==-1)
	{
		for (int i = 0; i < GameUtils::rows; i++)
			delete mainBoard[i];
		delete[](mainBoard);
		return -1;
	}
	
	//loads first DLL and creates playerA
	auto playerAdll = GameUtils::loadAlgo(basePath, fileNames[0]);
	IBattleshipGameAlgo* playerA = playerAdll.first;
	//checks if dll was loaded successfully(error printed inside function)
	if(!playerA)
	{
		for (int i = 0; i < GameUtils::rows; i++)
			delete mainBoard[i];
		delete[](mainBoard);
		return -1;
	}
	
	char** tempBoard = GameUtils::initPlayerBoard(mainBoard, PLAYER_A_NUM);
	playerA->setBoard(PLAYER_A_NUM, (const char**) tempBoard, GameUtils::rows, GameUtils::cols);
	for (int i = 0; i < GameUtils::rows; i++)
		delete tempBoard[i];
	delete[] tempBoard;

	//init for playerA-relevant only if playerA type is BattelshipPlayerFromFile
	if (!playerA->init(basePath))
	{
		delete playerA;
		FreeLibrary(playerAdll.second);
		cout << INIT_FAILED << basePath << "\\" << fileNames[0] << endl;
		for (int i = 0; i < GameUtils::rows; i++)
			delete mainBoard[i];
		delete[](mainBoard);
		return -1;
	}
	
	//loads second DLL and creates playerB
	auto playerBdll = GameUtils::loadAlgo(basePath, fileNames[1]);
	IBattleshipGameAlgo* playerB = playerBdll.first;
	//checks if dll was loaded successfully(error printed inside function)
	if (!playerB)
	{
		delete playerA;
		FreeLibrary(playerAdll.second);
		for (int i = 0; i < GameUtils::rows; i++)
			delete mainBoard[i];
		delete[] (mainBoard);
		return -1;
	}

	tempBoard = GameUtils::initPlayerBoard(mainBoard, PLAYER_B_NUM);
	playerB->setBoard(PLAYER_B_NUM, (const char**) tempBoard, GameUtils::rows, GameUtils::cols);
	for (int i = 0; i < GameUtils::rows; i++)
		delete tempBoard[i];
	delete[] tempBoard;

	//init for playerB -relevant only if playerB type is BattelshipPlayerFromFile
	if (!playerB->init(basePath))
	{
		delete playerA;
		delete playerB;
		FreeLibrary(playerAdll.second);
		FreeLibrary(playerBdll.second);
		cout << INIT_FAILED << basePath << "\\" << fileNames[1] << endl;
		for (int i = 0; i < GameUtils::rows; i++)
			delete mainBoard[i];
		delete[](mainBoard);
		return -1;
	}

	BattleshipGame game(playerA, playerB, mainBoard);
	game.playGame(useAnimation, delay);

	delete playerA;
	delete playerB;
	FreeLibrary(playerAdll.second);
	FreeLibrary(playerBdll.second);
	for (int i = 0; i < GameUtils::rows; i++)
		delete mainBoard[i];
	delete[]mainBoard;
	return 0;
	*/
}