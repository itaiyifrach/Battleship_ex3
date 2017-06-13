
#include "BattleshipGame.h"


void BattleshipGame::playGame() {
	//0 iff its A's turn
	int turnOf = 0;
	bool endGame = false;

	//initialize first attack (check if A/B have moves)
	Coordinate currAttack = PlayerA->attack();
	if (currAttack.row == -1)	
	{
		currAttack = PlayerB->attack();
		if (currAttack.row == -1)	
		{
			endGame = true;
		} 
		else 
		{
			turnOf = 1;
		}
	}

	while ((!endGame) && (numOfShipsA) && (numOfShipsB)) {
		//attacks are saved between 1..ROWS in the files. We shift the coordinates to 0..(ROWS-1)
		int i = currAttack.row - 1;
		int j = currAttack.col - 1;
		int k = currAttack.depth - 1;
		bool selfHit = false;
		if (GameUtils::selfHit(mainBoard[i][j][k], turnOf)) {
			selfHit = true;
		}

		//check for the sink-score of the ship in (i,j,k)th sector
		int sectorScore = GameUtils::getScoreForSector(mainBoard[i][j][k]);

		//check for the attack result and update the mainBoard according to the result
		pair<AttackResult, bool> AttackResTupple = getAttackResult(i, j, k);
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
		//TODO-is the change from i+1,j+1 to currAttack is fine?
		PlayerA->notifyOnAttackResult(turnOf,currAttack, currAttackRes);
		PlayerB->notifyOnAttackResult(turnOf, currAttack, currAttackRes);


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

	//TODO- return results instead of printing them
	if ((numOfShipsA) && (!numOfShipsB))
		cout << PLAYER_A_WON_STR << endl;
	else if ((!numOfShipsA) && (numOfShipsB))
		cout << PLAYER_B_WON_STR << endl;
	cout << POINTS_STR << endl;
	cout << PLAYER_A_POINTS_STR << scoreA << endl;
	cout << PLAYER_B_POINTS_STR << scoreB << endl;
}

void BattleshipGame::getNextAttack(int& turnOf, bool& endGame, Coordinate& currAttack) const
{
	if (turnOf == 0)
	{
		currAttack = PlayerA->attack();
		if (currAttack.row == -1)
		{
			currAttack = PlayerB->attack();
			if (currAttack.row == -1)
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
		if (currAttack.row == -1)
		{
			currAttack = PlayerA->attack();
			if (currAttack.row == -1)
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

std::pair<AttackResult, bool> BattleshipGame::getAttackResult(int i,int j,int k) const  {
	std::pair<AttackResult, bool> result;
	if (mainBoard[i][j][k] == 32) {
		result.first = AttackResult::Miss;
		result.second = false;
	}
	else if (mainBoard[i][j][k]== 'X') {
		result.first = AttackResult::Hit;
		result.second = true;
	}
	else if (updateBoardAndCheckSink(i, j, k)) {
		result.first = AttackResult::Sink;
		result.second = false;
	}
	else {
		result.first = AttackResult::Hit;
		result.second = false;
	}
	return result;
}

bool BattleshipGame::updateBoardAndCheckSink(int i, int j, int k) const
{
	auto direction = shipDirection(i, j, k);
	//horizontal
	if (direction == 0) 
	{
		int colIndex = j;
		//find left corner and save it in "left"
		while ((colIndex > 0) && (mainBoard[i][colIndex][k] != 32))
			colIndex--;
		if (mainBoard[i][colIndex][k] == 32)
			colIndex++;
		int left = colIndex;

		//count how many parts of the ship remain un-hit
		int count = 0;
		while ((colIndex < cols) && (mainBoard[i][colIndex][k] != 32))
			if (mainBoard[i][colIndex++][k] != 'X')
				count++;

		//if we have only one un-hit part, it was the initial part in (i,j,k), we erase the ship and return that it's a sink
		if (count == 1) {
			colIndex = left;
			while ((colIndex < cols) && (mainBoard[i][colIndex][k] != 32))
				mainBoard[i][colIndex++][k] = 32;
			return true;
		}
		//if it's not a sink we just update the (i,j,k) coordinate on the mainBoard as a hit and return it isn't a sink
		mainBoard[i][j][k] = 'X';
		return false;

	}
	//vertical
	if (direction==1)
	{
		int rowIndex = i;
		//find lower corner and save it in "lower"
		while ((rowIndex > 0) && (mainBoard[rowIndex][j][k] != 32))
			rowIndex--;	
		if (mainBoard[rowIndex][j][k] == 32)
			rowIndex++;
		int lower = rowIndex;

		//count how many parts of the ship remain un-hit
		int count = 0;
		while ((rowIndex < rows) && (mainBoard[rowIndex][j][k] != 32)) {
			if (mainBoard[rowIndex][j][k] != 'X')
				count++;
			rowIndex++;
		}

		//if we have only one un-hit part and it was the initial part in (i,j,k), we erase the ship and return that it's a sink
		if ((count == 1) && (mainBoard[i][j][k]!='X') && (mainBoard[i][j][k] != 32)) {
			rowIndex = lower;
			while ((rowIndex < rows) && (mainBoard[rowIndex][j][k] != 32))
				mainBoard[rowIndex++][j][k] = 32;
			return true;
		}
		//if it's not a sink we just update the (i,j,k) coordinate on the mainBoard as a hit and return it isn't a sink
		mainBoard[i][j][k] = 'X';
		return false;
	}

	//depth
		int depthIndex = k;
		//find deepest corner and save it in "deepest"
		while ((depthIndex > 0) && (mainBoard[i][j][depthIndex] != 32))
			depthIndex--;
		if (mainBoard[i][j][depthIndex] == 32)
			depthIndex++;
		int deepest = depthIndex;

		//count how many parts of the ship remain un-hit
		int count = 0;
		while ((depthIndex < depth) && (mainBoard[i][j][depth] != 32))
			if (mainBoard[i][j][depthIndex++] != 'X')
				count++;

		//if we have only one un-hit part, it was the initial part in (i,j,k), we erase the ship and return that it's a sink
		if (count == 1) {
			depthIndex = deepest;
			while ((depthIndex < depth) && (mainBoard[i][j][depthIndex] != 32))
				mainBoard[i][j][depthIndex++] = 32;
			return true;
		}
		//if it's not a sink we just update the (i,j,k) coordinate on the mainBoard as a hit and return it isn't a sink
		mainBoard[i][j][k] = 'X';
		return false;
	}


int BattleshipGame::shipDirection(int i, int j, int k) const {
	//TODO-% is redundant?
	//check horizontal
	//if we are in a corner spot
	if (j == 0)
	{
		if (mainBoard[i][1][k] != 32)
			return 0;
	}
	else if (j == cols - 1)
	{
		if (mainBoard[i][cols - 2][k] != 32)
			return 0;
	}
	//if we are not in a corner
	else
		if ((mainBoard[i][j - 1][k] != 32) || (mainBoard[i][j + 1][k] != 32))
			return 0;
	//not horizontal-check vertical
	if (i == 0)
	{
		if (mainBoard[1][j][k] != 32)
			return 1;
	}
	else if (i == rows - 1)
	{
		if (mainBoard[rows-2][j][k] != 32)
			return 1;
	}
	//if we are not in a corner
	else
		if ((mainBoard[i-1][j][k] != 32) || (mainBoard[i+1][j][k] != 32))
			return 1;
	//not horizontal and not vertical
	return 2;


	/*
	if (((j == 0) && ((mainBoard[i][1][k] == 32) || (mainBoard[i][1][k] == '%'))) || ((j == (cols - 1)) && ((mainBoard[i][cols - 2][k] == 32) || (mainBoard[i][cols - 2][k] == '%'))))
		return 1;
	if ((j == 0) || (j == (cols - 1)))
		return 0;
	if (((mainBoard[i][j - 1][k] == 32) || (mainBoard[i][j - 1][k] == '%')) && ((mainBoard[i][j + 1][k] == 32) || (mainBoard[i][j + 1][k] == '%')))
		return 1;
	return 0;*/
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
}
*/
