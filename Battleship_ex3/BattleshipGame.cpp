#include "BattleshipGame.h"


void BattleshipGame::playGame(bool useAnimation, int delay) {
	//0 iff its A's turn
	int turnOf = 0;
	bool endGame = false;

	//initialize first attack (check if A/B have moves)
	pair<int, int> currAttack = PlayerA->attack();
	if (currAttack.first == -1)	
	{
		currAttack = PlayerB->attack();
		if (currAttack.first == -1)	
		{
			endGame = true;
		} 
		else 
		{
			turnOf = 1;
		}
	}

	// printing the starting colored board:
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if (useAnimation) {
		system("cls");						// clearing the console
		printColorBoard(&hConsole, turnOf);	// printing stating board
		hideCursor(&hConsole);				// hiding the console cursor
		Sleep(delay);
	}

	while ((!endGame) && (numOfShipsA) && (numOfShipsB)) {
		//attacks are saved between 1..ROWS in the files. We shift the coordinates to 0..(ROWS-1)
		int i = currAttack.first - 1;
		int j = currAttack.second - 1;
		
		bool selfHit = false;
		if (GameUtils::selfHit(mainBoard[i][j], turnOf)) {
			selfHit = true;
		}

		//check for the sink-score of the ship in (i,j)th sector
		int sectorScore = GameUtils::getScoreForSector(mainBoard[i][j]);

		//check for the attack result and update the mainBoard according to the result
		pair<AttackResult, bool> AttackResTupple = getAttackResult(i, j);
		AttackResult currAttackRes = AttackResTupple.first;
		bool wasAlreadyHit = AttackResTupple.second;
		
		//check for sinks to update the score
		if (currAttackRes == AttackResult::Sink)
		{
			if (((!selfHit) && (turnOf == 0)) || ((selfHit) && (turnOf == 1)))
			{
				scoreA += sectorScore;
				numOfShipsB--;
			}
			else
			{
				scoreB += sectorScore;
				numOfShipsA--;
			}
		}			
		
		//Notify players on results
		PlayerA->notifyOnAttackResult(turnOf, i+1, j+1, currAttackRes);
		PlayerB->notifyOnAttackResult(turnOf, i+1, j+1, currAttackRes);

		// updating the colored board with the attack results:
		if (useAnimation) {
			updateColorBoard(&hConsole, i, j, turnOf, currAttackRes, selfHit, delay);
			Sleep(delay);
		}

		//Switch if current player missed / made a self-hit
		if ((currAttackRes == AttackResult::Miss) 
			|| ((currAttackRes == AttackResult::Hit) && (wasAlreadyHit))
			||  (((currAttackRes == AttackResult::Hit) || (currAttackRes == AttackResult::Sink)) && selfHit && (!wasAlreadyHit)))
		{
			if (turnOf == 0)
			{
				turnOf = 1;
			}
			else
			{
				turnOf = 0;
			}
		}
		getNextAttack(turnOf, endGame, currAttack);
	}	// end of while

	if (useAnimation) {
		// skipping the color board in order to print the results
		gotoxy(GameUtils::rows + 9, 0);
		SetConsoleTextAttribute(hConsole, WHITE_COLOR);
	}

	if ((numOfShipsA) && (!numOfShipsB))
		cout << PLAYER_A_WON_STR << endl;
	else if ((!numOfShipsA) && (numOfShipsB))
		cout << PLAYER_B_WON_STR << endl;
	cout << POINTS_STR << endl;
	cout << PLAYER_A_POINTS_STR << scoreA << endl;
	cout << PLAYER_B_POINTS_STR << scoreB << endl;
}

void BattleshipGame::getNextAttack(int& turnOf, bool& endGame, pair<int, int>& currAttack) const
{
	if (turnOf == 0)
	{
		currAttack = PlayerA->attack();
		if (currAttack.first == -1)
		{
			currAttack = PlayerB->attack();
			if (currAttack.first == -1)
			{
				endGame = true;
			} else {
				turnOf = 1;
			}
		}
	}
	else
	{
		currAttack = PlayerB->attack();
		if (currAttack.first == -1)
		{
			currAttack = PlayerA->attack();
			if (currAttack.first == -1)
			{
				endGame = true;
			}
			else
			{
				turnOf = 0;
			}
		}
	}
}

std::pair<AttackResult, bool> BattleshipGame::getAttackResult(int i, int j) const{
	std::pair<AttackResult, bool> result;
	if (mainBoard[i][j] == 32) {
		result.first = AttackResult::Miss;
		result.second = false;
	}
	else if (mainBoard[i][j] == 'X') {
		result.first = AttackResult::Hit;
		result.second = true;
	}
	else if (updateBaordAndCheckSink(i, j)) {
		result.first = AttackResult::Sink;
		result.second = false;
	}
	else {
		result.first = AttackResult::Hit;
		result.second = false;
	}
	return result;
}

bool BattleshipGame::updateBaordAndCheckSink(int i, int j) const {
	if (GameUtils::isVertical(mainBoard, GameUtils::rows, GameUtils::cols, i, j)) {
		int rowIndex = i;
		//find upper corner and save it in "upper"
		while ((rowIndex > 0) && (mainBoard[rowIndex][j] != 32))
			rowIndex--;	
		if (mainBoard[rowIndex][j] == 32)
			rowIndex++;
		int upper = rowIndex;

		//count how many parts of the ship remain un-hit
		int count = 0;
		while ((rowIndex < GameUtils::rows) && (mainBoard[rowIndex][j] != 32)) {
			if (mainBoard[rowIndex][j] != 'X')
				count++;
			rowIndex++;
		}

		//if we have only one un-hit part and it was the initial part in (i,j), we erase the ship and return that it's a sink
		if ((count == 1) && (mainBoard[i][j]!='X') && (mainBoard[i][j] != 32)) {
			rowIndex = upper;
			while ((rowIndex < GameUtils::rows) && (mainBoard[rowIndex][j] != 32))
				mainBoard[rowIndex++][j] = 32;
			return true;
		}
		//if it's not a sink we just update the (i,j) coordinate on the mainBoard as a hit and return it isn't a sink
		mainBoard[i][j] = 'X';
		return false;
	}
	else {
		int colIndex = j;
		//find left corner and save it in "left"
		while ((colIndex > 0) && (mainBoard[i][colIndex] != 32))
			colIndex--;
		if (mainBoard[i][colIndex] == 32)
			colIndex++;
		int left = colIndex;

		//count how many parts of the ship remain un-hit
		int count = 0;
		while ((colIndex < GameUtils::cols) && (mainBoard[i][colIndex] != 32))
			if (mainBoard[i][colIndex++] != 'X')
				count++;

		//if we have only one un-hit part, it was the initial part in (i,j), we erase the ship and return that it's a sink
		if (count == 1) {
			colIndex = left;
			while ((colIndex < GameUtils::cols) && (mainBoard[i][colIndex] != 32))
				mainBoard[i][colIndex++] = 32;
			return true;
		}
		//if it's not a sink we just update the (i,j) coordinate on the mainBoard as a hit and return it isn't a sink
		mainBoard[i][j] = 'X';
		return false;
	}
}

/*void BattleshipGame::printColorBoard(HANDLE* hConsole, int turnOf) const
{
	// printing the board frame:
	SetConsoleTextAttribute(*hConsole, WHITE_COLOR);
	int rowLength = 5 + 2 * GameUtils::rows;
	int spaceLength = (rowLength - 2 - 10) / 2;
	cout << "|";
	for (int i = 0; i < spaceLength; i++) 
	{
		cout << "-";
	}
	cout << "BATTLESHIP";
	for (int i = 0; i < rowLength - 2 - 10 - spaceLength; i++)
	{
		cout << "-";
	}
	cout << "|" << endl << "|";
	for (int i = 0; i < rowLength - 2; i++)
	{
		cout << " ";
	}
	cout << "|" << endl;

	// printing the tile color according to the player:
	for (int i = 0; i < GameUtils::rows; ++i)
	{
		SetConsoleTextAttribute(*hConsole, WHITE_COLOR);
		cout << "|  ";
		for (int j = 0; j < GameUtils::cols; j++) {
			if ((65 < mainBoard[i][j]) && (mainBoard[i][j] < 90)) {				// player A
				SetConsoleTextAttribute(*hConsole, PLAYER_A_COLOR);
				cout << mainBoard[i][j] << " ";
			}
			if ((97 < mainBoard[i][j]) && (mainBoard[i][j] < 122))  {			// player B
				SetConsoleTextAttribute(*hConsole, PLAYER_B_COLOR);
				cout << mainBoard[i][j] << " ";
			}
			if (mainBoard[i][j] == 32)											// sea		
			{
				SetConsoleTextAttribute(*hConsole, SEA_COLOR);
				cout << '~' << " ";
			}
		}
		SetConsoleTextAttribute(*hConsole, WHITE_COLOR);
		cout << " |" << endl;
	}

	// printing players colors:
	SetConsoleTextAttribute(*hConsole, WHITE_COLOR);
	cout << "|";
	for (int i = 0; i < rowLength - 2; i++)
	{
		cout << "_";
	}
	cout << "|" << endl;
	cout << "Player A:";
	SetConsoleTextAttribute(*hConsole, 144);
	cout << " ";
	SetConsoleTextAttribute(*hConsole, WHITE_COLOR);
	cout << ", Ships Left: " << numOfShipsA << endl;
	cout << "Player B:";
	SetConsoleTextAttribute(*hConsole, 207);
	cout << " ";
	SetConsoleTextAttribute(*hConsole, WHITE_COLOR);
	cout << ", Ships Left: " << numOfShipsB << endl;

	// printing player's turn and attack result
	SetConsoleTextAttribute(*hConsole, WHITE_COLOR);
	if (turnOf == 0)
	{
		cout << endl << "Player's Turn: A" << endl;
	}
	else
	{
		cout << endl << "Player's Turn: B" << endl;
	}
	cout << "Attack Result: " << endl;
}

void BattleshipGame::gotoxy(int i, int j)
{
	COORD coord;
	coord.X = j;
	coord.Y = i;
	SetConsoleCursorPosition(
		GetStdHandle(STD_OUTPUT_HANDLE),
		coord
	);
}

void BattleshipGame::updateColorBoard(HANDLE* hConsole, int i, int j, int turnOf, AttackResult attackResult, bool selfHit, int delay) const
{
	// updating player's turn
	SetConsoleTextAttribute(*hConsole, WHITE_COLOR);
	gotoxy(GameUtils::rows + 6, 15);
	if (turnOf == 0)
	{
		cout << "A";
	}
	else
	{
		cout << "B";
	}
	// clearing last attack result
	gotoxy(GameUtils::rows + 7, 15);
	cout << CLEAN_MSG;

	// line/col offsets due to the board frame
	int lineOffset = 2, colOffset = 3, color = WHITE_COLOR;
	int x = j * 2 + colOffset, y = i + lineOffset;
	
	gotoxy(y, x);

	// setting X color due to type of the tile
	if (cleanInitialBoard[i][j] == 'B' || cleanInitialBoard[i][j] == 'P' || cleanInitialBoard[i][j] == 'M' || cleanInitialBoard[i][j] == 'D')
	{
		color = PLAYER_A_COLOR;
	}
	if (cleanInitialBoard[i][j] == 'b' || cleanInitialBoard[i][j] == 'p' || cleanInitialBoard[i][j] == 'm' || cleanInitialBoard[i][j] == 'd')
	{
		color =  PLAYER_B_COLOR;
	}
	if (cleanInitialBoard[i][j] == ' ')
	{
		color = SEA_COLOR;
	}
	// bombing animation using '@'
	SetConsoleTextAttribute(*hConsole, BOMB_COLOR);
	Sleep(delay);
	cout << "@";
	Sleep(delay);
	gotoxy(y, x);
	cout << " ";
	Sleep(delay);
	gotoxy(y, x);
	cout << "@";
	Sleep(delay);
	gotoxy(y, x);
	cout << " ";
	Sleep(delay);

	// updating attack result message:
	gotoxy(GameUtils::rows + 7, 15);
	SetConsoleTextAttribute(*hConsole, BOMB_COLOR);
	if (turnOf == 0) {
		cout << "Player A ";
	}
	else {
		cout << "Player B ";
	}

	if (selfHit)
	{
		cout << SELF_HIT_MSG;
	}
	else {
		if (attackResult == AttackResult::Miss)
		{
			cout << MISS_MSG;
		}
		else if (attackResult == AttackResult::Hit)
		{
			cout << HIT_MSG;
		}
		else if (attackResult == AttackResult::Sink)
		{
			if (turnOf == 0) {
				cout << SINK_A_MSG;
			}
			else {
				cout << SINK_B_MSG;
			}
		}
	}
	if (attackResult == AttackResult::Sink) 
	{
		// updating ship's left message:
		SetConsoleTextAttribute(*hConsole, WHITE_COLOR);
		gotoxy(3 + GameUtils::rows, 24);
		cout << numOfShipsA;
		gotoxy(4 + GameUtils::rows, 24);
		cout << numOfShipsB;
	}

	// marking the tile
	SetConsoleTextAttribute(*hConsole, color);
	gotoxy(y, x);
	Sleep(delay);
	cout << "X";
}

void BattleshipGame::hideCursor(HANDLE* hConsole)
{
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = FALSE;
	SetConsoleCursorInfo(*hConsole, &info);
}*/