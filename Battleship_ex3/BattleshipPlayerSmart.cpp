/*
#include "BattleshipPlayerSmart.h"



void BattleshipPlayerSmart::setBoard(int player, const char** playerBoard, int numRows, int numCols) {
	playerNum = player; 
	cols = numCols;
	rows = numRows;
	myBoard = new char*[numRows];
	for (int i = 0; i < numRows; ++i)
	{
		myBoard[i] = new char[numCols];
		for (int j = 0; j < numCols; ++j)
		{
			myBoard[i][j] = 32;
		}
	}

	vector<pair<int, int>> deceptionMovesVec;
	for (int i = 0; i < numRows; ++i)
	{
		for (int j = 0; j < numCols; ++j)
		{
			if (playerBoard[i][j] != 32)
			{
				myBoard[i][j] = '%';
				if (i > 0)
				{
					myBoard[i - 1][j] = '%';
					if (playerBoard[i - 1][j] == 32)
					{
						deceptionMovesVec.emplace_back(i, j + 1);
					}
				}
				if (i < (numRows-1))
				{
					myBoard[i + 1][j] = '%';
					if (playerBoard[i + 1][j] == 32)
					{
						deceptionMovesVec.emplace_back(i + 2, j + 1);
					}
				}
				if (j > 0)
				{
					myBoard[i][j - 1] = '%';
					if (playerBoard[i][j - 1] == 32)
					{
						deceptionMovesVec.emplace_back(i + 1, j);
					}
				}
				if (j < (numCols-1))
				{
					myBoard[i][j + 1] = '%';
					if (playerBoard[i][j + 1] == 32)
					{
						deceptionMovesVec.emplace_back(i + 1, j + 2);
					}
				}
			}
		}
	}
	random_device rd;
	mt19937 g(rd());
	std::shuffle(deceptionMovesVec.begin(), deceptionMovesVec.end(), g);
	deceptionMoves = { deceptionMovesVec.begin() , deceptionMovesVec.end() };

	attackMoves = generateAllAttackMoves();
	if (attackMoves.empty())
	{
		nextAttack = { -1,-1 };
	}
	else
	{
		nextAttack = attackMoves.back();
	}
}


list<pair<int, int>> BattleshipPlayerSmart::generateAllAttackMoves() const
{
	srand((int) time(nullptr));
	int randOffset = rand() % 4;
	//generate 4-gaps attack moves
	vector<pair<int, int>> fourGapsMovesVec;
	for (int i = 0; i < rows; i++) {
		for (int j = ((3 + randOffset) - i) % 4; j <= cols; j = j + 4) {
			if (myBoard[i][j] == 32)
			{
				fourGapsMovesVec.emplace_back(i + 1, j + 1);
			}
		}
	}
	random_device rd;
	mt19937 g(rd());
	std::shuffle(fourGapsMovesVec.begin(), fourGapsMovesVec.end(),g);
	list<pair<int, int>> fourGapsMoves{ fourGapsMovesVec.begin(), fourGapsMovesVec.end() };

	//cover the 3/2-gaps attack moves
	vector<pair<int, int>> threeTwoGapsMovesVec;
	for (int i = 0; i<rows; i++) {
		for (int j = ((5 + randOffset) - i) % 4; j <= cols; j = j + 4) {
			if (myBoard[i][j] == 32)
			{
				threeTwoGapsMovesVec.emplace_back(i + 1, j + 1);
			}
		}
	}
	std::shuffle(threeTwoGapsMovesVec.begin(), threeTwoGapsMovesVec.end(),g);
	list<pair<int, int>> threeTwoGapsMoves{ threeTwoGapsMovesVec.begin(), threeTwoGapsMovesVec.end() };

	//cover the rest of the board
	vector<pair<int, int>> restOfMovesVec;
	for (int i = 0; i<rows; i++) {
		for (int j = (i + randOffset) % 2; j <= cols; j = j + 2) {
			if (myBoard[i][j] == 32)
			{
				restOfMovesVec.emplace_back(i + 1, j + 1);
			}
		}
	}
	std::shuffle(restOfMovesVec.begin(), restOfMovesVec.end(),g);
	list<pair<int, int>> restOfMoves{ restOfMovesVec.begin(), restOfMovesVec.end() };

	//append all moves (first will be the "restOf" and the last are "fourGaps"
	restOfMoves.insert(restOfMoves.end(), threeTwoGapsMoves.begin(), threeTwoGapsMoves.end());
	restOfMoves.insert(restOfMoves.end(), fourGapsMoves.begin(), fourGapsMoves.end());
	return restOfMoves;
}


void BattleshipPlayerSmart::searchForNextFirstHit(int row, int col, AttackResult result)
{
	if (result == AttackResult::Sink)
	{
		myBoard[row - 1][col - 1] = 'X';
		markSinkedShipAndUpdateAttacks(row - 1, col - 1);
	}
	else
	{
		myBoard[row - 1][col - 1] = '%';
	}
	
	//reset smartAttackAlgo members:
	rightmostHit.first = rightmostHit.second = -1;
	leftmostHit.first = leftmostHit.second = -1;
	upmostHit.first = upmostHit.second = -1;
	downmostHit.first = downmostHit.second = -1;
	 
	//set next attack start point
	state = AttackState::searchForFirstHit;
	if (attackMoves.empty())
	{
		nextAttack = { -1,-1 };
	}
	else
	{
		nextAttack = attackMoves.back();
	}
}

void BattleshipPlayerSmart::markSinkedShipAndUpdateAttacks(int i, int j) {
	if (GameUtils::isVertical(myBoard, rows, cols, i, j)) { // erasure of all perimiter cells and moves - vertical ship
		//find upper corner
		while ((i > 0) && (myBoard[i][j] != 32) && (myBoard[i][j] != '%')) { i--; }
		if ((myBoard[i][j] == 32) || (myBoard[i][j] == '%'))
		{
			i++;
		}
		if (i > 0)
		{
			myBoard[i-1][j] = '%';
			attackMoves.remove(pair<int, int> { i, j + 1 });
				
		}
		while ((i < rows) && (myBoard[i][j] != 32) && (myBoard[i][j] != '%'))
		{
			if (j > 0)
			{
				myBoard[i][j - 1] = '%';
				attackMoves.remove(pair<int, int> { i + 1, j });
			}
			if (j < (cols-1))
			{
				myBoard[i][j + 1] = '%';
				attackMoves.remove(pair<int, int> { i + 1, j + 2 });
			}
			++i;
		}
		i--;
		if (i < (rows-1))
		{
			myBoard[i+1][j] = '%';
			attackMoves.remove(pair<int, int> { i + 2, j + 1});
		}
	} // symatric erasure of all perimiter cells and moves - now for horizontal ship
	else {
		//find left corner
		while ((j > 0) && (myBoard[i][j] != 32) && (myBoard[i][j] != '%'))
		{
			j--;
		}
		if ((myBoard[i][j] == 32) || (myBoard[i][j] == '%'))
		{
			j++;
		}
		if (j > 0)
		{
			myBoard[i][j-1] = '%';
			attackMoves.remove(pair<int, int> { i + 1, j });
		}
		while ((j < cols) && (myBoard[i][j] != 32) && (myBoard[i][j] != '%'))
		{
			if (i > 0)
			{
				myBoard[i-1][j] = '%';
				attackMoves.remove(pair<int, int> { i, j + 1 });
			}
			if (i < (rows-1))
			{
				myBoard[i+1][j] = '%';
				attackMoves.remove(pair<int, int> { i + 2, j + 1 });
			}
			++j;
		}
		j--;
		if (j < (cols - 1))
		{
			myBoard[i][j+1] = '%';
			attackMoves.remove(pair<int, int> { i + 1, j + 2});
		}
	}
}

std::pair<int, int> BattleshipPlayerSmart::attack() {
	if (numOfDeceptionMoves > 0)
	{
		isDecepting = true;
		nextAttack = deceptionMoves.back();
		deceptionMoves.pop_back();
		numOfDeceptionMoves--;
	}
	else if (attackMoves.empty())
	{
		nextAttack = { -1,-1 };
	}
	else if (nextAttack == attackMoves.back())
	{
		attackMoves.pop_back();
	}
	else
	{
		if (find(attackMoves.begin(), attackMoves.end(), nextAttack) != attackMoves.end())
		{
			attackMoves.remove(nextAttack);
		}
		else
		{
			nextAttack = attackMoves.back();
			attackMoves.pop_back();
		}
	}
	return nextAttack;
}


void BattleshipPlayerSmart::notifyOnAttackResult(int player, int row, int col, AttackResult result)
{
	if (playerNum == player)
	{
		if (!isDecepting)
		{
			notifyOnMyAttackResult(row, col, result);
		}
		else
		{
			isDecepting = false;
		}
	}
	else
	{
		notifyOnOpponentsAttackResult(row, col, result);
	}
}


void BattleshipPlayerSmart::notifyOnMyAttackResult(int row, int col, AttackResult result)
{
	switch (state)
	{
	case AttackState::searchForFirstHit:
		searchForFirstHitTransition(row, col, result);
		return;
	case AttackState::afterFirstHitCheckHorizontalAttLeft:
		afterFirstHitCheckHorizontalAttLeftTransition(row, col, result);
		return;
	case AttackState::afterFirstHitCheckHorizontalAttRight:
		afterFirstHitCheckHorizontalAttRightTransition(row, col, result);
		return;
	case AttackState::foundHorizontalAttLeft:
		foundHorizontalAttLeftTransition(row, col, result);
		return;
	case AttackState::foundHorizontalAttRight:
		foundHorizontalAttRightTransition(row, col, result);
		return;
	case AttackState::foundVerticalAttUp:
		foundVerticalAttUpTransition(row, col, result);
		return;
	case AttackState::foundVerticalAttDown:
		foundVerticalAttDownTransition(row, col, result);
	}
}


// First AttackState: searching the board for a first hit
void BattleshipPlayerSmart::searchForFirstHitTransition(int row, int col, AttackResult result)
{
	if (result == AttackResult::Hit)
	{
		myBoard[row - 1][col - 1] = 'X';
		//init first attack attributes
		rightmostHit.first = leftmostHit.first = upmostHit.first = downmostHit.first = row;
		rightmostHit.second = leftmostHit.second = upmostHit.second = downmostHit.second = col;

		//set next attack to the left of the first hit
		if ((leftmostHit.second > 1) && (myBoard[row - 1][col - 2] == 32))
		{
			state = AttackState::afterFirstHitCheckHorizontalAttLeft;
			nextAttack.first = leftmostHit.first;
			nextAttack.second = leftmostHit.second - 1;
		}
		else if ((rightmostHit.second < cols) && (myBoard[row - 1][col] == 32)) //set next attack to the right of the first hit
		{
			state = AttackState::afterFirstHitCheckHorizontalAttRight;
			nextAttack.first = rightmostHit.first;
			nextAttack.second = rightmostHit.second + 1;
		}
		else if ((upmostHit.first > 1) && (myBoard[row - 2][col - 1] == 32)) //set next attack up to the first hit
		{
			state = AttackState::foundVerticalAttUp;
			nextAttack.first = upmostHit.first - 1;
			nextAttack.second = upmostHit.second;
		}
		else //set next attack down to the first hit
		{
			state = AttackState::foundVerticalAttDown;
			nextAttack.first = downmostHit.first + 1;
			nextAttack.second = downmostHit.second;
		}
	}
	else 
	{
		searchForNextFirstHit(row, col, result);
	}
}

// Second state: after first hit we've attacked on the left to check horizontalness
void BattleshipPlayerSmart::afterFirstHitCheckHorizontalAttLeftTransition(int row, int col, AttackResult result)
{
	if (result == AttackResult::Hit)
	{
		myBoard[row - 1][col - 1] = 'X';
		leftmostHit.first = row;
		leftmostHit.second = col;

		//set next attack to the left of the last hit
		if ((leftmostHit.second > 1) && (myBoard[leftmostHit.first - 1][leftmostHit.second - 2] == 32))
		{
			state = AttackState::foundHorizontalAttLeft;
			nextAttack.first = leftmostHit.first;
			nextAttack.second = leftmostHit.second - 1;
		}
		else  //set next attack to the right of the first hit
		{
			state = AttackState::foundHorizontalAttRight;
			nextAttack.first = rightmostHit.first;
			nextAttack.second = rightmostHit.second + 1;
		} 
	}
	else if (result == AttackResult::Miss)
	{
		myBoard[row - 1][col - 1] = '%';
		if ((rightmostHit.second < cols) && (myBoard[rightmostHit.first - 1][rightmostHit.second] == 32)) //set next attack to the right of the first hit
		{
			state = AttackState::afterFirstHitCheckHorizontalAttRight;
			nextAttack.first = rightmostHit.first;
			nextAttack.second = rightmostHit.second + 1;
		}
		else if ((upmostHit.first > 1) && (myBoard[upmostHit.first - 2][upmostHit.second - 1] == 32)) //set next attack up to the first hit
		{
			state = AttackState::foundVerticalAttUp;
			nextAttack.first = upmostHit.first - 1;
			nextAttack.second = upmostHit.second;
		}
		else //set next attack down to the first hit
		{
			state = AttackState::foundVerticalAttDown;
			nextAttack.first = downmostHit.first + 1;
			nextAttack.second = downmostHit.second;
		}
	}
	else
	{
		searchForNextFirstHit(row, col, result);
	}
}

// Third state: after first hit, we've attacked on the left and missed or met board's barrier. Now we check horizontalness by attacking on the right
void BattleshipPlayerSmart::afterFirstHitCheckHorizontalAttRightTransition(int row, int col, AttackResult result)
{
	if (result == AttackResult::Hit)
	{
		myBoard[row - 1][col - 1] = 'X';
		state = AttackState::foundHorizontalAttRight;
		//set next attack to the right of the last hit
		nextAttack.first = rightmostHit.first = row;
		rightmostHit.second = col;
		nextAttack.second = rightmostHit.second + 1;
	}
	else if (result == AttackResult::Miss)
	{
		myBoard[row - 1][col - 1] = '%';
		if ((upmostHit.first > 1) && (myBoard[upmostHit.first - 2][upmostHit.second - 1] == 32)) //set next attack up to the first hit
		{
			state = AttackState::foundVerticalAttUp;
			nextAttack.first = upmostHit.first - 1;
			nextAttack.second = upmostHit.second;
		}
		else //set next attack down to the first hit
		{
			state = AttackState::foundVerticalAttDown;
			nextAttack.first = downmostHit.first + 1;
			nextAttack.second = downmostHit.second;
		}
	}
	else
	{
		searchForNextFirstHit(row, col, result);
	}
}

// Fourth state: after we found the ship is horizontal we continue attacking on the left
void BattleshipPlayerSmart::foundHorizontalAttLeftTransition(int row, int col, AttackResult result)
{
	if (result == AttackResult::Hit)
	{
		myBoard[row - 1][col - 1] = 'X';
		nextAttack.first = leftmostHit.first = row;
		leftmostHit.second = col;

		//set next attack to the left of the last hit, remain in the same state
		if ((leftmostHit.second > 1) && (myBoard[leftmostHit.first - 1][leftmostHit.second - 2] == 32))
		{
			nextAttack.first = leftmostHit.first;
			nextAttack.second = leftmostHit.second - 1;
		}
		else //set next attack to the right of the first hit
		{
			state = AttackState::foundHorizontalAttRight;
			nextAttack.first = rightmostHit.first;
			nextAttack.second = rightmostHit.second + 1;
		}
	}
	else if (result == AttackResult::Miss)
	{
		myBoard[row - 1][col - 1] = '%';
		state = AttackState::foundHorizontalAttRight;
		//set next attack to the right of the first hit
		nextAttack.first = rightmostHit.first;
		nextAttack.second = rightmostHit.second + 1;
	}
	else
	{
		searchForNextFirstHit(row, col, result);
	}
}


// Fifth state: after we found the ship is horizontal, "finished" with the left side, we continue attacking on the right till it sinks
void BattleshipPlayerSmart::foundHorizontalAttRightTransition(int row, int col, AttackResult result)
{
	if (result == AttackResult::Hit)
	{
		myBoard[row - 1][col - 1] = 'X';
		//set next attack to the right of the last hit, remain the state
		nextAttack.first = rightmostHit.first = row;
		rightmostHit.second = col;
		nextAttack.second = rightmostHit.second + 1;
	}
	else
	{
		searchForNextFirstHit(row, col, result);
	}
}


// Sixth state: after we found the ship is vertcal, we attack "up" to the first hit
void BattleshipPlayerSmart::foundVerticalAttUpTransition(int row, int col, AttackResult result)
{
	if (result == AttackResult::Hit)
	{
		myBoard[row - 1][col - 1] = 'X';
		//set next attack up to the last hit, remain the state
		upmostHit.first = row;
		nextAttack.second = upmostHit.second = col;

		//check if we're not on the first row
		if ((upmostHit.first > 1) && (myBoard[upmostHit.first - 2][upmostHit.second - 1] == 32)) //set next attack up to the first hit
		{
			//set next attack up to the last hit
			nextAttack.first = upmostHit.first - 1;
		}
		else //set next attack down to the first hit
		{
			state = AttackState::foundVerticalAttDown;
			nextAttack.first = downmostHit.first + 1;
			nextAttack.second = downmostHit.second;
		}		
	}
	else if (result == AttackResult::Miss)
	{
		myBoard[row - 1][col - 1] = '%';
		state = AttackState::foundVerticalAttDown;
		//set next attack down to the first hit
		nextAttack.first = downmostHit.first + 1;
		nextAttack.second = downmostHit.second;
	}
	else
	{
		searchForNextFirstHit(row, col, result);
	}
}


// Seventh state: after we found the ship is vertcal, "finished" with the upper side, we continue attacking down till it sinks
void BattleshipPlayerSmart::foundVerticalAttDownTransition(int row, int col, AttackResult result)
{
	if (result == AttackResult::Hit)
	{
		myBoard[row - 1][col - 1] = 'X';
		//set next attack down to the last hit, remain the state
		downmostHit.first = row;
		nextAttack.first = downmostHit.first + 1;
		nextAttack.second = downmostHit.second = col;
	}
	else
	{
		searchForNextFirstHit(row, col, result);
	}
}

void BattleshipPlayerSmart::notifyOnOpponentsAttackResult(int row, int col, AttackResult result)
{
	if (result == AttackResult::Miss)
	{
		myBoard[row - 1][col - 1] = '%';
		//delete (row - 1,col - 1) from the attack options! and only this cell. leave the perimiter (in case we are being decepted)
		attackMoves.remove(pair<int, int> { row, col});
	}
	else if ((result == AttackResult::Hit) && (myBoard[row-1][col-1] == 32))
	{
		myBoard[row - 1][col - 1] = 'X';
		//delete (row - 1,col - 1) from the attack options!
		attackMoves.remove(pair<int, int> { row, col});

		//set next attack from attackMoves (not nextAttack variable)
		if ((col > 1) && (myBoard[row - 1][col - 2] == 32)) //set next attack to the left of the first hit
		{
			auto itr = find(attackMoves.begin(), attackMoves.end(), pair<int, int> { row, col-1});
			//emplace (row - 1,col - 2) as the next attack in our list (if it was not deleted and legal)
			if (itr != attackMoves.end())
			{
				attackMoves.erase(itr);
				attackMoves.emplace_back(row, col - 1);
			}
			
		}
		if ((col < cols) && (myBoard[row - 1][col] == 32)) //set next attack to the right of the first hit
		{
			auto itr = find(attackMoves.begin(), attackMoves.end(), pair<int, int> { row , col+1});
			//emplace (row - 1,col) as the next attack in our list (if it was not deleted and legal)
			if (itr != attackMoves.end())
			{
				attackMoves.erase(itr);
				attackMoves.emplace_back(row, col+1);
			}
		}
		if ((row > 1) && (myBoard[row - 2][col - 1] == 32)) //set next attack up to the first hit
		{
			auto itr = find(attackMoves.begin(), attackMoves.end(), pair<int, int> { row-1, col});
			//emplace (row - 1,col) as the next attack in our list (if it was not deleted and legal)
			if (itr != attackMoves.end())
			{
				attackMoves.erase(itr);
				attackMoves.emplace_back(row-1, col);
			}
		}
		if ((row < rows) && (myBoard[row][col - 1] == 32))//set next attack down to the first hit
		{
			auto itr = find(attackMoves.begin(), attackMoves.end(), pair<int, int> { row + 1, col});
			//emplace (row - 1,col) as the next attack in our list (if it was not deleted and legal)
			if (itr != attackMoves.end())
			{
				attackMoves.erase(itr);
				attackMoves.emplace_back(row + 1, col);
			}
		}
	}
	else if ((result == AttackResult::Sink) && (myBoard[row - 1][col - 1] == 32))
	{
		myBoard[row - 1][col - 1] = 'X';
		if ((nextAttack.first == row) && (nextAttack.second == col))
		{
			searchForNextFirstHit(row, col, result);
		}
		markSinkedShipAndUpdateAttacks(row - 1, col - 1);
	}
}

// This method must be implemented in each player(algorithm) .cpp file
IBattleshipGameAlgo* GetAlgorithm()
{
	return new BattleshipPlayerSmart();
}
*/