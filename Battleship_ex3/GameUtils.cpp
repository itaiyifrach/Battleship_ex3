#include "GameUtils.h"
#include <memory>

char*** GameUtils::initialize(int argc, char** argv, string& basePath) {
	string boardPath;	
	
	// setting cwd as default path
	auto temp = _fullpath(nullptr, "", MAX_PATH);
	basePath = temp;
	free(temp);

	//check if path was invalid
	auto check = parsePath(basePath, boardPath);
	if (check < 0)
	{
		return NULL;
	}

	// parsing the board to 3D char array
	char*** board = parseBoard(boardPath);
	if (board == NULL)	// parsing failed
	{
		return NULL;
	}

	printBoard(board, 10, 10, 6);

	// check if the board is valid
	int mistakes[5] = { 0 };
	int rows = 10, cols = 10, depth = 6, i;
	char shipMistakeTypeA, shipMistakeTypeB;
	if (checkBoard((const char***)board, rows, cols, depth, mistakes) == false)
	{
		for (i = 0; i < 5; i++)
		{
			if (mistakes[i] != 0)
			{
				switch (i)
				{
				case 0:
					shipMistakeTypeA = mistakes[0];
					cout << BOARD_MISTAKE_0 << shipMistakeTypeA << FOR_PLAYER << "A" << endl;
					break;
				case 1:
					shipMistakeTypeB = mistakes[1];
					cout << BOARD_MISTAKE_0 << shipMistakeTypeB << FOR_PLAYER << "B" << endl;
					break;
				case 2:
					cout << BOARD_MISTAKE_2 << endl;
					break;
				case 3:
					cout << BOARD_MISTAKE_3 << endl;
					break;
				case 4:
					cout << BOARD_MISTAKE_4 << endl;
					break;
				default:
					break;
				}
			}
		}
		return NULL;
	}

	//if all is well return 0
	return board;
}


// Dynamically allocate a 3D array
char*** GameUtils::allocateBoard(int rows, int cols, int depth)
{
	char*** board = new char**[rows];
	for (int i = 0; i < rows; ++i) 
	{
		board[i] = new char*[cols];
		for (int j = 0; j < cols; j++) 
		{
			board[i][j] = new char[depth];
			memset(board[i][j], ' ', depth);
		}
	}
	return board;
}

//Dynamically deallocate a 3D array
void GameUtils::destroyBoard(char*** board, int rows, int cols, int depth)
{
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
			delete[] board[i][j];
		delete[] board[i];
	}
	delete[] board;
}

int GameUtils::parsePath(const string& basePath, string& file_path)
{
	HANDLE dir;
	WIN32_FIND_DATAA fileData; //data struct for file
	string file_suffix = BOARD_SUFFIX; //only *.sboard files
	
	dir = FindFirstFileA((basePath + file_suffix).c_str(), &fileData); // Notice: Unicode compatible version of FindFirstFile
	if (dir != INVALID_HANDLE_VALUE) //check if the dir opened successfully
	{
		string fileName = fileData.cFileName;
		file_path = basePath + "\\" + fileName;
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

char*** GameUtils::parseBoard(const string& boardPath)
{
	ifstream ifs(boardPath);
	string line, token;
	string delimiter = "x";
	int x, y, z;

	// getting first line, which includes the dimentions of the board
	getline(ifs, line);
	vector<string> tokens = split(line, 'x');
	if (tokens.size() != 3) {
		// possible bonus here
		return NULL;
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
		return NULL;
	}

	// memory allocation of the board
	char*** board = allocateBoard(x, y, z);
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
				if ((j < (int)line.size()) && (line[j] == 'b' || line[j] == 'B' || line[j] == 'p' || line[j] == 'P' ||
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

char** GameUtils::getBoardCut(const char*** board, int rows, int cols, int depth, bool cutByDepth)
{
	char** boardCut = new char*[rows];
	for (int i = 0; i < rows; i++) {
		boardCut[i] = new char[cols];
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

bool GameUtils::checkBoard(const char*** board, int rows, int cols, int depth, int* mistakes)
{
	char** boardCut;
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
void GameUtils::checkBoardCut(char** board, int rows, int cols, int* mistakes, unique_ptr<int[]>& shipsTypeA, unique_ptr<int[]>& shipsTypeB)
{
	char** markedBoard = new char*[rows];
	for (int i = 0; i < cols; ++i)
		markedBoard[i] = new char[cols];
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
	
	for (int i = 0; i < rows; i++) {
		delete board[i];
		delete markedBoard[i];
	}
	delete[](board);
	delete[](markedBoard);
}

bool GameUtils::checkShape(char** board, char** markedBoard, int rows, int cols, int posI, int posJ, char shipType, int shipSize, int* mistakes, int player)
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

bool GameUtils::checkBound(char** board, char shipType, int i, int j, int* mistakes, int player, int* possibleAdj)
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
	else
	{
		cout << MISSING_ALGO << path << endl;
		return fileNames;
	}
	if (fileNames.size()<2)
	{
		cout << MISSING_ALGO << path << endl;
		return fileNames;
	}
	sort(fileNames.begin(), fileNames.end());
	return fileNames;

}

std::pair<IBattleshipGameAlgo*, HINSTANCE> GameUtils::loadAlgo (const string& path, const string& fileName)
{
	std::pair<IBattleshipGameAlgo*, HINSTANCE> res(nullptr, nullptr);
	// define function of the type we expect
	typedef IBattleshipGameAlgo *(*GetAlgoFuncType)();
	GetAlgoFuncType getAlgoFunc;
	string algoPath = path + "\\" + fileName;
	// Load dynamic library
	HINSTANCE hDll = LoadLibraryA(algoPath.c_str()); // Notice: Unicode compatible version of LoadLibrary
	if (!hDll)
	{
		cout << LOAD_DLL_ERR << algoPath <<endl;
		return res;
	}
	
	// Get function pointer
	getAlgoFunc = (GetAlgoFuncType)GetProcAddress(hDll, "GetAlgorithm");
	if (!getAlgoFunc)
	{
		FreeLibrary(hDll);
		cout << LOAD_DLL_ERR << algoPath << endl;
		return res;
	}
	res.first = getAlgoFunc();
	res.second = hDll;
	return res;

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