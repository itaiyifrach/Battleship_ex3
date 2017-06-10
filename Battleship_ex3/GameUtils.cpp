#include "GameUtils.h"
#include <memory>

void GameUtils::parseArgs(int argc, char** argv, string& basePath) {
		
	//TODO: need to add support for numOfThreads parameter
	// setting cwd as default path
	auto temp = _fullpath(nullptr, "", MAX_PATH);
	basePath = temp;
	free(temp);

}


unique_ptr<unique_ptr<unique_ptr<char[]>[]>[]> GameUtils::allocateBoard(int rows, int cols, int depth)
{
	unique_ptr<unique_ptr<unique_ptr<char[]>[]>[]> board = make_unique<unique_ptr<unique_ptr<char[]>[]>[]>(rows);
	for (int i = 0; i < rows; i++)
	{
		board[i] = make_unique<unique_ptr<char[]>[]>(cols);
		for (int j = 0; j < cols; j++)
		{
			board[i][j] = make_unique<char[]>(depth);
			for(int k=0;k<depth;k++)
			{
				board[i][j][k] = ' ';
			}
		}
	}
	return board;
	/*char*** board = new char**[rows];
	for (int i = 0; i < rows; ++i) 
	{
		board[i] = new char*[cols];
		for (int j = 0; j < cols; j++) 
		{
			board[i][j] = new char[depth];
			memset(board[i][j], ' ', depth);
		}
	}
	return board;*/
}


/*void GameUtils::destroyBoard(char*** board, int rows, int cols, int depth)
{
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
			delete[] board[i][j];
		delete[] board[i];
	}
	delete[] board;
}*/

int GameUtils::parsePath(const string& basePath,  vector<string>& boardNames)
{

	HANDLE dir;
	WIN32_FIND_DATAA fileData; //data struct for file	

	// iterate over *.sboard files in path	
	string file_suffix = BOARD_SUFFIX; //only *.sboard files
	
	dir = FindFirstFileA((basePath + file_suffix).c_str(), &fileData); // Notice: Unicode compatible version of FindFirstFile
	if (dir != INVALID_HANDLE_VALUE) //check if the dir opened successfully
	{
		// test each file suffix and set variables as needed
		do
		{
			string fileName = fileData.cFileName;
			boardNames.push_back(fileName);

		} while (FindNextFileA(dir, &fileData)); // Notice: Unicode compatible version of FindNextFile
	}
	else
	{
		//check if no .sboard file was found or there was a problem with the dir,and print error accordingly
		if (GetLastError() != ERROR_FILE_NOT_FOUND)
		{
			cout << WRONG_PATH << basePath << endl;
			return -2;
		}
		else
		{
			cout << MISSING_BOARD << basePath << endl;
			return -1;
		}
	}
	sort(boardNames.begin(), boardNames.end());
	return 0;	
}

vector<string> GameUtils::split(string str, char delimiter) {
	vector<string> internal;
	stringstream strSt(str); // Turn the string into a stream.
	string item;
	while (getline(strSt, item, delimiter)) {
		internal.push_back(item);
	}
	return internal;
}

unique_ptr<unique_ptr<unique_ptr<char[]>[]>[]> GameUtils::parseBoard(const string& path, const string& boardName, int& x, int& y, int& z)
{
	string boardPath = path + "\\" + boardName;
	ifstream ifs(boardPath);
	string line, token;
	string delimiter = "x";
	

	// getting first line, which includes the dimentions of the board
	getline(ifs, line);
	vector<string> tokens = split(line, 'x');
	if (tokens.size() != 3) {
		// possible bonus here
		return nullptr;
	}
	istringstream firstStream(tokens[0]);
	istringstream secondStream(tokens[1]);
	istringstream thirdStream(tokens[2]);
	firstStream >> x;
	secondStream >> y;
	thirdStream >> z;
	if (firstStream.fail() || secondStream.fail() || thirdStream.fail() || x<1  || y<1 || z<1)
	{
		// possible bonus here
		return nullptr;
	}	
	
	// memory allocation of the board
	auto board =allocateBoard(x, y, z);
	// skipping next line
	// possible bonus here
	getline(ifs, line);
	

	for (int k = 0; k < z; k++)
	{
		for (int i = 0; i < x; i++)
		{
			getline(ifs, line);
			cout << "(" << i << ", " << 'j' << ", " << k << ") ";
			cout << line << endl;
			for (int j = 0; j < y; j++)
			{
				if ((j < int(line.size())) && (line[j] == 'b' || line[j] == 'B' || line[j] == 'p' || line[j] == 'P' ||
					line[j] == 'm' || line[j] == 'M' || line[j] == 'd' || line[j] == 'D'))
				{
					board[i][j][k] = line[j];
				}
			}
		}
		// skipping lines until we find an empty line
		while (getline(ifs, line) && line != "");
	}
	ifs.close();

	return board;
}

unique_ptr<unique_ptr<char[]>[]> GameUtils::getBoardCut(unique_ptr<unique_ptr<unique_ptr<char[]>[]>[]>& board, int rows, int cols, int depth, bool cutByDepth)
{
	unique_ptr<unique_ptr<char[]>[]> boardCut =make_unique<unique_ptr<char[]>[]>(rows);
	for (int i = 0; i < rows; i++) {
		boardCut[i] = make_unique<char[]>(cols);
	}

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++)
		{
			if (!cutByDepth) {
				boardCut[i][j] = board[i][j][depth];
			}
			else {
				boardCut[i][j] = board[i][depth][j];
			}
		}
	}
	
	return boardCut;
}

bool GameUtils::checkBoard(unique_ptr<unique_ptr<unique_ptr<char[]>[]>[]>& board, int rows, int cols, int depth, int* mistakes)
{
	unique_ptr<unique_ptr<char[]>[]> boardCut;
	bool isValidBoard = true;
	unique_ptr<int[]> shipsTypeA = make_unique<int[]>(5);
	unique_ptr<int[]> shipsTypeB = make_unique<int[]>(5);

	// checking regular cuts
	for (int i = 0; i < depth; i++)
	{
		boardCut = getBoardCut(board, rows, cols, i, false);
		checkBoardCut(boardCut, rows, cols, mistakes, shipsTypeA, shipsTypeB);
	}

	// checking opposit cuts
	for (int i = 0; i < cols; i++)
	{
		boardCut = getBoardCut(board, rows, depth, i, true);
		checkBoardCut(boardCut, rows, cols, mistakes, shipsTypeA, shipsTypeB);
	}

	// checking number of valid ships of players A and B:
	if (shipsTypeA[0] != shipsTypeB[0]) {
		mistakes[3] = 1;
	}
	// checking if players A and B have the same ships types:
	if (shipsTypeA[0] != shipsTypeB[0] || shipsTypeA[1] != shipsTypeB[1] && shipsTypeA[2] != shipsTypeB[2]
		&& shipsTypeA[3] != shipsTypeB[3]) {
		mistakes[4] = 1;
	}

	// checking if there were any mistakes
	for (int i = 0; i < 5; i++) {
		if (mistakes[i] != 0) {
			isValidBoard = false;
		}
	}
	
	return isValidBoard;
}

/*	
 *	mistakes:
 *	[0] = Wrong size or shape for ship <char> for player A
 *	[1] = Wrong size or shape for ship <char> for player B
 *	[2] = Adjacent Ships on Board
 *	[3] = The players don't have the same number of ships
 *	[4] = The players don't have the same types of ships
 */
void GameUtils::checkBoardCut(unique_ptr<unique_ptr<char[]>[]>& board, int rows, int cols, int* mistakes, unique_ptr<int[]>& shipsTypeA, unique_ptr<int[]>& shipsTypeB)
{
	unique_ptr<unique_ptr<char[]>[]> markedBoard = make_unique<unique_ptr<char[]>[]>(rows);
	for (int i = 0; i < rows; i++) {
		markedBoard[i] = make_unique<char[]>(cols);
	}
	// copy board to markedBoard
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++)
			markedBoard[i][j] = board[i][j];

	bool isValid = true, validShape;
	int currPlayer, shipSize;

	// scanning the board
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			char currCharChecked = markedBoard[i][j];
			switch (currCharChecked)
			{
			// player A options:
			case 'B':
				validShape = checkShape(board, markedBoard, rows, cols, i, j, currCharChecked, SIZE_SHIP_B, mistakes, 0);
				currPlayer = 0;
				shipSize = SIZE_SHIP_B;
				break;
			case 'P':
				validShape = checkShape(board, markedBoard, rows, cols, i, j, currCharChecked, SIZE_SHIP_P, mistakes, 0);
				currPlayer = 0;
				shipSize = SIZE_SHIP_P;
				break;
			case 'M':
				validShape = checkShape(board, markedBoard, rows, cols, i, j, currCharChecked, SIZE_SHIP_M, mistakes, 0);
				currPlayer = 0;
				shipSize = SIZE_SHIP_M;
				break;
			case 'D':
				validShape = checkShape(board, markedBoard, rows, cols, i, j, currCharChecked, SIZE_SHIP_D, mistakes, 0);
				currPlayer = 0;
				shipSize = SIZE_SHIP_D;
				break;
			// player B options:
			case 'b':
				validShape = checkShape(board, markedBoard, rows, cols, i, j, currCharChecked, SIZE_SHIP_B, mistakes, 1);
				currPlayer = 1;
				shipSize = SIZE_SHIP_B;
				break;
			case 'p':
				validShape = checkShape(board, markedBoard, rows, cols, i, j, currCharChecked, SIZE_SHIP_P, mistakes, 1);
				currPlayer = 1;
				shipSize = SIZE_SHIP_P;
				break;
			case 'm':
				validShape = checkShape(board, markedBoard, rows, cols, i, j, currCharChecked, SIZE_SHIP_M, mistakes, 1);
				currPlayer = 1;
				shipSize = SIZE_SHIP_M;
				break;
			case 'd':
				validShape = checkShape(board, markedBoard, rows, cols, i, j, currCharChecked, SIZE_SHIP_D, mistakes, 1);
				currPlayer = 1;
				shipSize = SIZE_SHIP_D;
				break;
			default:
				continue;
			}

			if (validShape == true)
			{
				if (currPlayer == 0)
				{
					shipsTypeA[0]++;
					shipsTypeA[shipSize]++;
				}
				else
				{
					shipsTypeB[0]++;
					shipsTypeB[shipSize]++;
				}
			}
		}
	}	// finish scanning the board
	
	/*for (int i = 0; i < rows; i++) {
		delete board[i];
		delete markedBoard[i];
	}
	delete[](board);
	delete[](markedBoard);*/
}

bool GameUtils::checkShape(unique_ptr<unique_ptr<char[]>[]>& board, unique_ptr<unique_ptr<char[]>[]>& markedBoard, int rows, int cols, int posI, int posJ, char shipType, int shipSize, int* mistakes, int player)
{
	bool checkValid = true;
	// check if it is a single tile
	if (board[posI][posJ] != 'B' && board[posI][posJ] != 'b') {
		if (posI != 0)	//upper bound
		{
			if (board[posI][posJ - 1] != ' ')
				checkValid = false;
		}
		if (posI != rows - 1)	//lower bound
		{
			if (board[posI + 1][posJ] != ' ')
				checkValid = false;
		}
		if (posJ != 0)	//left bound
		{
			if (board[posI][posJ - 1] != ' ')
				checkValid = false;
		}
		if (posJ != cols - 1)	//right bound
		{
			if (board[posI + 1][posJ + 1] != ' ')
				checkValid = false;
		}
		if (checkValid) {
			markedBoard[posI][posJ] = 'X';
			return true;
		}
	}
	
	int countTiles = 0, possibleAdj = 0, special = false;
	int limit = min(posJ + shipSize, cols);

	// check horizontal shape
	for (int j = posJ; j < limit; j++)
	{
		// checking upper bound
		if (posI != 0)	// not an edge
		{
			if (checkBound(board, shipType, posI - 1, j, mistakes, player, &possibleAdj))
				checkValid = false;
		}
		// checking lower bound
		if (posI != rows - 1)	// not an edge
		{
			if (j == posJ)	// special case !!!
			{
				if (player == 0 && mistakes[0] == 0)
				{
					if (checkBound(board, shipType, posI + 1, j, mistakes, player, &possibleAdj)) {
						checkValid = false;
						special = true;
					}
				}
				if (player == 1 && mistakes[1] == 0)
				{
					if (checkBound(board, shipType, posI + 1, j, mistakes, player, &possibleAdj)) {
						checkValid = false;
						special = true;
					}
				}
			}
			else
			{
				if (checkBound(board, shipType, posI + 1, j, mistakes, player, &possibleAdj))
					checkValid = false;
			}
		}
		if (j == posJ)	// need to check the back also
		{
			if (j != 0)	// not an edge, checking the back
			{
				if (checkBound(board, shipType, posI, j - 1, mistakes, player, &possibleAdj))
					checkValid = false;
			}
		}
		if (j == posJ + shipSize - 1)	// need to check the next also
		{
			if (j != cols - 1)	// not an edge, checking the next
			{
				if (checkBound(board, shipType, posI, j + 1, mistakes, player, &possibleAdj))
					checkValid = false;
			}
		}

		if (board[posI][j] == shipType)
		{
			markedBoard[posI][j] = 'X'; // marking the curr tile
			countTiles++;				// counting fitted tiles
		}
		else break;		// if any tile isn't as the checked one
	} // end of for
	if (countTiles == shipSize)		// if marked the full ship size
	{
		if (checkValid)
		{
			if (possibleAdj == 1)
				mistakes[2] = 1;
			return true;
		}
	}
	// end of horizontal check

	// check vertical shape
	countTiles = 0, possibleAdj = 0;
	checkValid = true;
	limit = min(posI + shipSize, rows);

	for (int i = posI; i < limit; i++)
	{
		// checking left bound
		if (posJ != 0)	// not an edge
		{
			if (checkBound(board, shipType, i, posJ - 1, mistakes, player, &possibleAdj))
				checkValid = false;
		}
		// checking right bound
		if (posJ != cols - 1)	// not an edge
		{
			if (checkBound(board, shipType, i, posJ + 1, mistakes, player, &possibleAdj))
				checkValid = false;
		}
		if (i == posI)	// need to check the up also
		{
			if (i != 0)	// not an edge, checking the up
			{
				if (checkBound(board, shipType, i - 1, posJ, mistakes, player, &possibleAdj))
					checkValid = false;
			}
		}
		if (i == posI + shipSize - 1)	// need to check the down also
		{
			if (i != rows - 1)	// not an edge, checking the down
			{
				if (checkBound(board, shipType, i + 1, posJ, mistakes, player, &possibleAdj))
					checkValid = false;
			}
		}

		if (board[i][posJ] == shipType)
		{
			if (special)
			{
				if (player == 0)	// player A
					mistakes[0] = 0;
				else				// player B
					mistakes[1] = 0;
			}

			markedBoard[i][posJ] = 'X'; // marking the curr tile
			countTiles++;				// counting fitted tiles
		}
		else break;
	} // end of for
	if (countTiles == shipSize)
	{
		if (checkValid)
		{
			if (possibleAdj == 1)
				mistakes[2] = 1;
			return true;
		}
	}
	// end of vertical check

	// shape isn't horizental or vertical
	if (possibleAdj == 1)
		mistakes[2] = 1;
	if (player == 0)	// player A
	{
		if (mistakes[0] == 0)
			mistakes[0] = shipType;
	}
	else			// player B
		if (mistakes[1] == 0)
			mistakes[1] = shipType;
	return false;
}

bool GameUtils::checkBound(unique_ptr<unique_ptr<char[]>[]>& board, char shipType, int i, int j, int* mistakes, int player, int* possibleAdj)
{
	char checkChar = board[i][j];
	if (checkChar != ' ')	// if the tile isn't sea
	{
		if (checkChar == shipType)	// case of adjacent with same ship of the same player
		{
			// mark appropriate mistake - invalid shape. no need to count as valid ship of the player.
			if (player == 0)	// player A
			{
				if (mistakes[0] == 0)
					mistakes[0] = checkChar;
			}
			else			// player B
				if (mistakes[1] == 0)
					mistakes[1] = checkChar;
			return true;
		}
		else								// case of adjacent with another ship of the same/another player
		{
			// mark appropriate mistake - adjacent ships.
			*possibleAdj = 1;
			return false;
		}
	}
	return false;
}


void GameUtils::printBoard(char*** board, int rows, int cols, int depth)
{
	cout << "===========SHOWING BOARD===========";
	cout << endl;
	for (int k = 0; k < depth; k++) {
		cout << "DEPTH #" << k << endl;
		for (int i = 0; i < rows; i++)
		{
			cout << " " << endl;
			for (int j = 0; j < cols; j++)
			{
				cout << board[i][j][k] << " ";
			}
		}
	cout << endl;
	}
	cout << "===================================";
	cout << endl;
}

int GameUtils::getBoards(const string& path,vector<string>& boardNames, vector<unique_ptr<unique_ptr<unique_ptr<char[]>[]>[]>>& boards)
{
	int numOfBoards = 0;
	for (int j = 0; j<boardNames.size(); j++)
	{
		// parsing the board to 3D char array	
		int rows, cols, depth;
		unique_ptr<unique_ptr<unique_ptr<char[]>[]>[] > board = parseBoard(path,boardNames[j],rows,cols,depth);
		if (board != nullptr)	// parsing failed
		{
			// check if the board is valid
			int mistakes[5] = { 0 };			
			if (checkBoard(board, rows, cols, depth, mistakes) != false)
			{				
				boards.push_back(board);
				numOfBoards++;				
			}
		}
		//printBoard(board, 10, 10, 6);
	}
	return numOfBoards;
}

vector<string> GameUtils::getDLLNames(string& path)
{
	HANDLE dir;
	WIN32_FIND_DATAA fileData; //data struct for file
	vector<string> fileNames;

	// iterate over *.dll files in path	

	string file_suffix = DLL_SUFFIX; // only .dll endings
	dir = FindFirstFileA((path + file_suffix).c_str(), &fileData); // Notice: Unicode compatible version of FindFirstFile
	if (dir != INVALID_HANDLE_VALUE) //check if the dir opened successfully
	{
		// test each file suffix and set variables as needed
		do
		{
			string fileName = fileData.cFileName;			
			fileNames.push_back(fileName);

		} while (FindNextFileA(dir, &fileData)); // Notice: Unicode compatible version of FindNextFile
	}
	if (fileNames.size()<2)
	{
		cout << MISSING_ALGO << path << endl;
		return fileNames;
	}
	sort(fileNames.begin(), fileNames.end());
	return fileNames;

}

pair<unique_ptr<IBattleshipGameAlgo>, HINSTANCE> GameUtils::loadAlgo (const string& path, const string& fileName)
{
	
	// define function of the type we expect
	typedef IBattleshipGameAlgo *(*GetAlgoFuncType)();
	GetAlgoFuncType getAlgoFunc;
	string algoPath = path + "\\" + fileName;
	// Load dynamic library
	HINSTANCE hDll = LoadLibraryA(algoPath.c_str()); // Notice: Unicode compatible version of LoadLibrary
	if (!hDll)
	{		
		return make_pair(nullptr, nullptr);
	}
	
	// Get function pointer
	getAlgoFunc = reinterpret_cast<GetAlgoFuncType>(GetProcAddress(hDll,"GetAlgorithm"));
	if (!getAlgoFunc)
	{
		FreeLibrary(hDll);		
		return make_pair(nullptr, nullptr);
	}	
	std::pair< unique_ptr<IBattleshipGameAlgo>, HINSTANCE> res(getAlgoFunc(), hDll);	
	return res;

}

int GameUtils::getPlayers(const string& path, vector<string>& dllNames, vector<pair<unique_ptr<IBattleshipGameAlgo>, HINSTANCE>>& playersVec)
{
	int numOfPlayers = 0;

	for(int i=0;i<dllNames.size();i++)
	{
		auto player = loadAlgo(path, dllNames[i]);
		if(player.first!=nullptr)
		{
			playersVec.push_back(player);
			numOfPlayers++;
		}
	}

	return numOfPlayers;
}
/*
char** GameUtils::initPlayerBoard(char** mainBoard, int playerNum) {
	char** playerBoard = new char*[rows];
	for (int i = 0; i < rows; ++i)
	{
		playerBoard[i] = new char[cols];
		for (int j = 0; j < cols; j++) {
			if (playerNum == 0) {
				if ((65 < mainBoard[i][j]) && (mainBoard[i][j] < 90))
					playerBoard[i][j] = mainBoard[i][j];
				else
					playerBoard[i][j] = 32;
			}
			else {
				if ((97 < mainBoard[i][j]) && (mainBoard[i][j] < 122))
					playerBoard[i][j] = mainBoard[i][j];
				else
					playerBoard[i][j] = 32;
			}
		}
	}
	return playerBoard;
}
*/

bool GameUtils::isVertical(char** board, int rows, int cols, int i, int j) {
	if (((j == 0) && ((board[i][1] == 32) || (board[i][1] == '%'))) || ((j == (cols - 1)) && ((board[i][cols - 2] == 32) || (board[i][cols - 2] == '%'))))
		return true;
	if ((j == 0) || (j == (cols - 1)))
		return false;
	if (((board[i][j - 1] == 32) || (board[i][j - 1] == '%')) && ((board[i][j + 1] == 32) || (board[i][j + 1] == '%')))
		return true;
	return false;
}

int GameUtils::getScoreForSector(char boardSymbol) {
	switch (boardSymbol) {
	case 'b':
	case 'B':
		return 2;
	case 'p':
	case 'P':
		return 3;
	case 'm':
	case 'M':
		return 7;
	case 'd':
	case 'D':
		return 8;
	default:
		return 0;
	}
}

bool GameUtils::selfHit(char boardSymbol, int attackPlayer)
{
	if (attackPlayer == 0)
	{
		switch (boardSymbol) {
		case 'B':
		case 'P':
		case 'M':
		case 'D':
			return true;
		default:
			return false;
		}
	}
	switch (boardSymbol) {
	case 'b':
	case 'p':
	case 'm':
	case 'd':
		return true;
	default:
		return false;
	}
}

void GameUtils::freeLibs(vector<pair<unique_ptr<IBattleshipGameAlgo>, HINSTANCE>>& playersVec)
{
	for (int i = 0; i < playersVec.size(); i++)
		FreeLibrary(playersVec[i].second);
}