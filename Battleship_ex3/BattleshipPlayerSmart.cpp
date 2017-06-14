#include "BattleshipPlayerSmart.h"

void BattleshipPlayerSmart::setBoard(const BoardData& playerBoard) {

	myBoard.copyFromOtherDataBoard(playerBoard);

	for (int i = 1; i <= myBoard.rows(); ++i)
	{
		for (int j = 1; j <= myBoard.cols(); ++j)
		{
			for (int k = 1; k <= myBoard.cols(); ++k)
			{
				if (myBoard.charAt(Coordinate(i, j, k)) != 32)
				{
					myBoard.setMiss(Coordinate(i, j, k));
					if (i > 1)
					{
						myBoard.setMiss(Coordinate(i - 1, j, k));
					}
					if (i < playerBoard.rows())
					{
						myBoard.setMiss(Coordinate(i + 1, j, k));
					}
					if (j > 1)
					{
						myBoard.setMiss(Coordinate(i, j - 1, k));
					}
					if (j < playerBoard.cols())
					{
						myBoard.setMiss(Coordinate(i, j + 1, k));
					}
					if (k > 1)
					{
						myBoard.setMiss(Coordinate(i, j, k - 1));
					}
					if (k < playerBoard.depth())
					{
						myBoard.setMiss(Coordinate(i, j, k + 1));
					}
				}
			}
		}
	}

	attackMoves = generateAllAttackMoves();
	if (attackMoves.empty()) {
		nextAttack = Coordinate(-1,-1,-1);
	} else {
		nextAttack = attackMoves.back();
	}
}

/* Smart Attack moves order generator (diagonals generator) from previous exsr, was not adapted to 3D of excsr 3 due to time constraints
list<Coordinate> BattleshipPlayerSmart::generateAllAttackMoves() const
{
	srand((int) time(nullptr));
	int randOffset = rand() % 4;
	//generate 4-gaps attack moves
	vector<Coordinate> fourGapsMovesVec;
	for (int i = 0; i < myBoard.rows(); i++) {
		for (int j = ((3 + randOffset) - i) % 4; j <= myBoard.cols(); j = j + 4) {
			if (myBoard[i][j] == 32)
			{
				fourGapsMovesVec.emplace_back(i + 1, j + 1);
			}
		}
	}
	random_device rd;
	mt19937 g(rd());
	std::shuffle(fourGapsMovesVec.begin(), fourGapsMovesVec.end(),g);
	list<Coordinate> fourGapsMoves{ fourGapsMovesVec.begin(), fourGapsMovesVec.end() };

	//cover the 3/2-gaps attack moves
	vector<Coordinate> threeTwoGapsMovesVec;
	for (int i = 0; i<myBoard.rows(); i++) {
		for (int j = ((5 + randOffset) - i) % 4; j <= myBoard.cols(); j = j + 4) {
			if (myBoard[i][j] == 32)
			{
				threeTwoGapsMovesVec.emplace_back(i + 1, j + 1);
			}
		}
	}
	std::shuffle(threeTwoGapsMovesVec.begin(), threeTwoGapsMovesVec.end(),g);
	list<Coordinate> threeTwoGapsMoves{ threeTwoGapsMovesVec.begin(), threeTwoGapsMovesVec.end() };

	//cover the rest of the board
	vector<Coordinate> restOfMovesVec;
	for (int i = 0; i<myBoard.rows(); i++) {
		for (int j = (i + randOffset) % 2; j <= myBoard.cols(); j = j + 2) {
			if (myBoard[i][j] == 32)
			{
				restOfMovesVec.emplace_back(i + 1, j + 1);
			}
		}
	}
	std::shuffle(restOfMovesVec.begin(), restOfMovesVec.end(),g);
	list<Coordinate> restOfMoves{ restOfMovesVec.begin(), restOfMovesVec.end() };

	//append all moves (first will be the "restOf" and the last are "fourGaps"
	restOfMoves.insert(restOfMoves.end(), threeTwoGapsMoves.begin(), threeTwoGapsMoves.end());
	restOfMoves.insert(restOfMoves.end(), fourGapsMoves.begin(), fourGapsMoves.end());
	return restOfMoves;
}
*/

list<Coordinate> BattleshipPlayerSmart::generateAllAttackMoves() const
{
	srand((int)time(nullptr));
	vector<Coordinate> MovesVec;
	
	for (int i = 1; i <= myBoard.rows(); ++i)
	{
		for (int j = 1; j <= myBoard.cols(); ++j)
		{
			for (int k = 1; k <= myBoard.depth(); ++k)
			{
				Coordinate curr(i, j, k);
				if (myBoard.charAt(curr) != 32)
				{
					MovesVec.emplace_back(curr);
				}
			}
		}
	}

	random_device rd;
	mt19937 g(rd());
	std::shuffle(MovesVec.begin(), MovesVec.end(), g);
	list<Coordinate> shuffledMoves{ MovesVec.begin(), MovesVec.end() };
	return shuffledMoves;
}

void BattleshipPlayerSmart::searchForNextFirstHit(Coordinate att, AttackResult result)
{
	if (result == AttackResult::Sink)
	{
		myBoard.setHit(att);
		markSinkedShipAndUpdateAttacks(att);
	}
	else
	{
		myBoard.setMiss(att);
	}
	 
	//set next attack start point and reset state machine
	state = AttackState::searchForFirstHit;
	if (attackMoves.empty()) {
		nextAttack = Coordinate(-1, -1, -1);
	} else {
		nextAttack = attackMoves.back();
	}
}

//refactor all remove from attacklist (use find to dind approptiate iterator and remove it)***************************************************
//finish refactoring - from line175

void BattleshipPlayerSmart::markSinkedShipAndUpdateAttacks(Coordinate att) {
	int direction = GameUtils::shipDirection(myBoard, att);
	if (direction == 0) { //erasure of all perimiter cells and moves - horizontal ship
		//find left corner
		int colIndex = att.col;
		//find left corner and save it in "left"
		while ((colIndex > 0) && 
			(myBoard.charAt(Coordinate(att.row, colIndex, att.depth)) != 32) &&
			(myBoard.charAt(Coordinate(att.row, colIndex, att.depth)) != '%')) {
			colIndex--;
		}
		if ((myBoard.charAt(Coordinate(att.row, colIndex, att.depth)) == 32) ||
			(myBoard.charAt(Coordinate(att.row, colIndex, att.depth)) == '%'))
		{
			colIndex++;
		}
		if (colIndex > 0)
		{
			myBoard.setMiss(Coordinate(att.row, colIndex - 1, att.depth));
			attackMoves.remove(Coordinate(att.row, colIndex - 1, att.depth));
		}
		while ((j < myBoard.cols()) && (myBoard[i][j] != 32) && (myBoard[i][j] != '%'))
		{
			if (i > 0)
			{
				myBoard[i - 1][j] = '%';
				attackMoves.remove(Coordinate{ i, j + 1 });
			}
			if (i < (myBoard.rows() - 1))
			{
				myBoard[i + 1][j] = '%';
				attackMoves.remove(Coordinate{ i + 2, j + 1 });
			}
			++j;
		}
		j--;
		if (j < (myBoard.cols() - 1))
		{
			myBoard[i][j + 1] = '%';
			attackMoves.remove(Coordinate{ i + 1, j + 2 });
		}
	} 
	else if (direction == 1) { // erasure of all perimiter cells and moves - vertical ship
		//find upper corner
		while ((i > 0) && (myBoard[i][j] != 32) && (myBoard[i][j] != '%')) { i--; }
		if ((myBoard[i][j] == 32) || (myBoard[i][j] == '%'))
		{
			i++;
		} if (i > 0) {
			myBoard[i-1][j] = '%';
			attackMoves.remove(Coordinate { i, j + 1 });
				
		}
		while ((i < myBoard.rows()) && (myBoard[i][j] != 32) && (myBoard[i][j] != '%'))
		{
			if (j > 0)
			{
				myBoard[i][j - 1] = '%';
				attackMoves.remove(Coordinate { i + 1, j });
			}
			if (j < (myBoard.cols()-1))
			{
				myBoard[i][j + 1] = '%';
				attackMoves.remove(Coordinate { i + 1, j + 2 });
			}
			++i;
		}
		i--;
		if (i < (myBoard.rows()-1))
		{
			myBoard[i+1][j] = '%';
			attackMoves.remove(Coordinate { i + 2, j + 1});
		}
	} else { // erasure of all perimiter cells and moves - depth sheep
		
	}
}

Coordinate BattleshipPlayerSmart::attack() {
	if (attackMoves.empty())
	{
		nextAttack = Coordinate(-1,-1,-1);
	}
	else if (GameUtils::coordinatesComperator(nextAttack,attackMoves.back()))
	{
		attackMoves.pop_back();
	}
	else
	{
		Coordinate searchFor(nextAttack.row, nextAttack.col, nextAttack.depth);
		if (find_if(attackMoves.begin(), attackMoves.end(), [&searchFor](const Coordinate& coor) {
																return GameUtils::coordinatesComperator(searchFor, coor); }) != attackMoves.end())
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


void BattleshipPlayerSmart::notifyOnAttackResult(int player, Coordinate att, AttackResult result)
{
	if (playerNum == player) {
		notifyOnMyAttackResult(att, result);
	} else {
		notifyOnOpponentsAttackResult(att, result);
	}
}


void BattleshipPlayerSmart::notifyOnMyAttackResult(Coordinate att, AttackResult result)
{
	switch (state)
	{
	case AttackState::searchForFirstHit:
		searchForFirstHitTransition(att, result);
		return;
	case AttackState::isHorizontalAttLeft:
		checkHorizontalAttLeftTransition(att, result);
		return;
	case AttackState::isHorizontalAttRight:
		checkHorizontalAttRightTransition(att, result);
		return;
	case AttackState::foundHorizontalAttLeft:
		foundHorizontalAttLeftTransition(att, result);
		return;
	case AttackState::foundHorizontalAttRight:
		foundHorizontalAttRightTransition(att, result);
		return;
	case AttackState::isVerticalAttUp:
		checkVerticalAttUpTransition(att, result);
		return;
	case AttackState::isVerticalAttDown:
		checkVerticalAttDownTransition(att, result);
		return;
	case AttackState::foundVerticalAttUp:
		foundVerticalAttUpTransition(att, result);
		return;
	case AttackState::foundVerticalAttDown:
		foundVerticalAttDownTransition(att, result);
		return;
	case AttackState::foundDepthAttInside:
		foundDepthAttInsideTransition(att, result);
		return;
	case AttackState::foundDepthAttOutside:
		foundDepthAttOutsideTransition(att, result);
	}
}


// First AttackState: searching the board for a first hit
void BattleshipPlayerSmart::searchForFirstHitTransition(Coordinate att, AttackResult result)
{
	if (result == AttackResult::Hit)
	{
		myBoard.setHit(att);
		//init first attack attributes
		leftmostHit = rightmostHit = upmostHit = downmostHit = inmostHit = outmostHit = att;

		//set next attack to the left of the first hit
		if ((leftmostHit.col > 1) && (myBoard.charAt(Coordinate(leftmostHit.row, leftmostHit.col - 1, leftmostHit.depth)) == 32))
		{
			state = AttackState::isHorizontalAttLeft;
			nextAttack = leftmostHit;
			nextAttack.col--;
		}
		else if ((rightmostHit.col < myBoard.cols()) && (myBoard.charAt(Coordinate(rightmostHit.row, rightmostHit.col + 1, rightmostHit.depth)) == 32)) //set next attack to the right of the first hit
		{
			state = AttackState::isHorizontalAttRight;
			nextAttack = rightmostHit;
			nextAttack.col++;
		}
		else if ((upmostHit.row > 1) && (myBoard.charAt(Coordinate(upmostHit.row - 1, upmostHit.col, upmostHit.depth)) == 32)) //set next attack up to the first hit
		{
			state = AttackState::isVerticalAttUp;
			nextAttack = upmostHit;
			nextAttack.row--;
		}
		else if ((downmostHit.row < myBoard.rows()) && (myBoard.charAt(Coordinate(downmostHit.row + 1, downmostHit.col, downmostHit.depth)) == 32)) //set next attack down to the first hit
		{
			state = AttackState::isVerticalAttDown;
			nextAttack = downmostHit;
			nextAttack.row++;
		}
		else if ((inmostHit.depth > 1) && (myBoard.charAt(Coordinate(inmostHit.row, inmostHit.col, inmostHit.depth - 1)) == 32)) //set next attack inner by one to the first hit
		{
			state = AttackState::foundDepthAttInside;
			nextAttack = inmostHit;
			nextAttack.depth--;
		}
		else if ((outmostHit.depth < myBoard.depth()) && (myBoard.charAt(Coordinate(outmostHit.row, outmostHit.col, outmostHit.depth + 1)) == 32)) //set next attack outer by one to the first hit
		{
			state = AttackState::foundDepthAttOutside;
			nextAttack = outmostHit;
			nextAttack.depth++;
		}
		else {
			searchForNextFirstHit(att, result);
		}
	}
	else 
	{
		searchForNextFirstHit(att, result);
	}
}

// Second state: after first hit we've attacked on the left to check horizontalness
void BattleshipPlayerSmart::checkHorizontalAttLeftTransition(Coordinate att, AttackResult result)
{
	if (result == AttackResult::Hit)
	{
		myBoard.setHit(att);
		leftmostHit = att;

		//set next attack to the left of the last hit
		if ((leftmostHit.col > 1) && (myBoard.charAt(Coordinate(att.row, att.col - 1, att.depth)) == 32))
		{
			state = AttackState::foundHorizontalAttLeft;
			nextAttack = leftmostHit;
			nextAttack.col--;
		}
		else  //set next attack to the right of the first hit
		{
			state = AttackState::foundHorizontalAttRight;
			nextAttack = rightmostHit;
			nextAttack.col++;
		}
	}
	else if (result == AttackResult::Miss)
	{
		myBoard.setMiss(att);
		if ((rightmostHit.col < myBoard.cols()) && (myBoard.charAt(Coordinate(rightmostHit.row, rightmostHit.col + 1, rightmostHit.depth)) == 32)) //set next attack to the right of the first hit
		{
			state = AttackState::isHorizontalAttRight;
			nextAttack = rightmostHit;
			nextAttack.col++;
		}
		else if ((upmostHit.row > 1) && (myBoard.charAt(Coordinate(upmostHit.row - 1, upmostHit.col, upmostHit.depth)) == 32)) //set next attack up to the first hit
		{
			state = AttackState::isVerticalAttUp;
			nextAttack = upmostHit;
			nextAttack.row--;
		}
		else if ((downmostHit.row < myBoard.rows()) && (myBoard.charAt(Coordinate(downmostHit.row + 1, downmostHit.col, downmostHit.depth)) == 32)) //set next attack down to the first hit
		{
			state = AttackState::isVerticalAttDown;
			nextAttack = downmostHit;
			nextAttack.row++;
		}
		else if ((inmostHit.depth > 1) && (myBoard.charAt(Coordinate(inmostHit.row, inmostHit.col, inmostHit.depth - 1)) == 32)) //set next attack inner by one to the first hit
		{
			state = AttackState::foundDepthAttInside;
			nextAttack = inmostHit;
			nextAttack.depth--;
		}
		else if ((outmostHit.depth < myBoard.depth()) && (myBoard.charAt(Coordinate(outmostHit.row, outmostHit.col, outmostHit.depth + 1)) == 32)) //set next attack outer by one to the first hit
		{
			state = AttackState::foundDepthAttOutside;
			nextAttack = outmostHit;
			nextAttack.depth++;
		}
		else {
			searchForNextFirstHit(att, result);
		}
	}
	else
	{
		searchForNextFirstHit(att, result);
	}
}

// Third state: after first hit, we've attacked on the left and missed or met board's barrier. Now we check horizontalness by attacking on the right
void BattleshipPlayerSmart::checkHorizontalAttRightTransition(Coordinate att, AttackResult result)
{
	if (result == AttackResult::Hit)
	{
		myBoard.setHit(att);
		state = AttackState::foundHorizontalAttRight;
		//set next attack to the right of the last hit
		nextAttack = rightmostHit = att;
		nextAttack.col++;
	}
	else if (result == AttackResult::Miss)
	{
		myBoard.setMiss(att);
		if ((upmostHit.row > 1) && (myBoard.charAt(Coordinate(upmostHit.row - 1, upmostHit.col, upmostHit.depth)) == 32)) //set next attack up to the first hit
		{
			state = AttackState::isVerticalAttUp;
			nextAttack = upmostHit;
			nextAttack.row--;
		}
		else if ((downmostHit.row < myBoard.rows()) && (myBoard.charAt(Coordinate(downmostHit.row + 1, downmostHit.col, downmostHit.depth)) == 32)) //set next attack down to the first hit
		{
			state = AttackState::isVerticalAttDown;
			nextAttack = downmostHit;
			nextAttack.row++;
		}
		else if ((inmostHit.depth > 1) && (myBoard.charAt(Coordinate(inmostHit.row, inmostHit.col, inmostHit.depth - 1)) == 32)) //set next attack inner by one to the first hit
		{
			state = AttackState::foundDepthAttInside;
			nextAttack = inmostHit;
			nextAttack.depth--;
		}
		else if ((outmostHit.depth < myBoard.depth()) && (myBoard.charAt(Coordinate(outmostHit.row, outmostHit.col, outmostHit.depth + 1)) == 32)) //set next attack outer by one to the first hit
		{
			state = AttackState::foundDepthAttOutside;
			nextAttack = outmostHit;
			nextAttack.depth++;
		}
		else {
			searchForNextFirstHit(att, result);
		}
	}
	else
	{
		searchForNextFirstHit(att, result);
	}
}

// Fourth state: after we found the ship is horizontal we continue attacking on the left
void BattleshipPlayerSmart::foundHorizontalAttLeftTransition(Coordinate att, AttackResult result)
{
	if (result == AttackResult::Hit)
	{
		myBoard.setHit(att);
		leftmostHit = att;
		
		//set next attack to the left of the last hit
		if ((leftmostHit.col > 1) && (myBoard.charAt(Coordinate(att.row, att.col - 1, att.depth)) == 32))
		{
			nextAttack = leftmostHit;
			nextAttack.col--;
		}
		else  //set next attack to the right of the first hit
		{
			state = AttackState::foundHorizontalAttRight;
			nextAttack = rightmostHit;
			nextAttack.col++;
		}
	}
	else if (result == AttackResult::Miss)
	{
		myBoard.setMiss(att);
		state = AttackState::foundHorizontalAttRight;
		//set next attack to the right of the first hit
		nextAttack = rightmostHit;
		nextAttack.col++;
	}
	else
	{
		searchForNextFirstHit(att, result);
	}
}


// Fifth state: after we found the ship is horizontal, "finished" with the left side, we continue attacking on the right till it sinks
void BattleshipPlayerSmart::foundHorizontalAttRightTransition(Coordinate att, AttackResult result)
{
	if (result == AttackResult::Hit)
	{
		myBoard.setHit(att);
		//set next attack to the right of the last hit, remain in the same state
		nextAttack = rightmostHit = att;
		nextAttack.col++;
	}
	else
	{
		searchForNextFirstHit(att, result);
	}
}

// Sixth state: after ensuring the ship isn't horizontal, we check whether it is vertical by attacking upwards
void BattleshipPlayerSmart::checkVerticalAttUpTransition(Coordinate att, AttackResult result)
{
	if (result == AttackResult::Hit)
	{
		myBoard.setHit(att);
		//set next attack up to the last hit, remain the state
		upmostHit = att;

		//check if we're not on the first row
		if ((upmostHit.row > 1) && (myBoard.charAt(Coordinate(upmostHit.row - 1, upmostHit.col, upmostHit.depth)) == 32)) 
		{
			//set next attack up to the last hit
			state = AttackState::foundVerticalAttUp;
			nextAttack = upmostHit;
			nextAttack.row--;
		}
		else //set next attack down to the first hit
		{
			state = AttackState::foundVerticalAttDown;
			nextAttack = downmostHit;
			nextAttack.row++;
		}		
	}
	else if (result == AttackResult::Miss)
	{
		myBoard.setMiss(att);
		if ((downmostHit.row < myBoard.rows()) && (myBoard.charAt(Coordinate(downmostHit.row + 1, downmostHit.col, downmostHit.depth)) == 32)) //set next attack down to the first hit
		{
			state = AttackState::isVerticalAttDown;
			nextAttack = downmostHit;
			nextAttack.row++;
		}
		else if ((inmostHit.depth > 1) && (myBoard.charAt(Coordinate(inmostHit.row, inmostHit.col, inmostHit.depth - 1)) == 32)) //set next attack inner by one to the first hit
		{
			state = AttackState::foundDepthAttInside;
			nextAttack = inmostHit;
			nextAttack.depth--;
		}
		else if ((outmostHit.depth < myBoard.depth()) && (myBoard.charAt(Coordinate(outmostHit.row, outmostHit.col, outmostHit.depth + 1)) == 32)) //set next attack outer by one to the first hit
		{
			state = AttackState::foundDepthAttOutside;
			nextAttack = outmostHit;
			nextAttack.depth++;
		}
		else {
			searchForNextFirstHit(att, result);
		}
	}
	else
	{
		searchForNextFirstHit(att, result);
	}
}


void BattleshipPlayerSmart::checkVerticalAttDownTransition(Coordinate att, AttackResult result)
{
	if (result == AttackResult::Hit)
	{
		myBoard.setHit(att);
		//set next attack down to the last hit
		state = AttackState::foundVerticalAttDown;
		nextAttack = downmostHit = att;
		nextAttack.row++;
	}
	else if (result == AttackResult::Miss)
	{
		myBoard.setMiss(att);
		if ((inmostHit.depth > 1) && (myBoard.charAt(Coordinate(inmostHit.row, inmostHit.col, inmostHit.depth - 1)) == 32)) //set next attack inner by one to the first hit
		{
			state = AttackState::foundDepthAttInside;
			nextAttack = inmostHit;
			nextAttack.depth--;
		}
		else if ((outmostHit.depth < myBoard.depth()) && (myBoard.charAt(Coordinate(outmostHit.row, outmostHit.col, outmostHit.depth + 1)) == 32)) //set next attack outer by one to the first hit
		{
			state = AttackState::foundDepthAttOutside;
			nextAttack = outmostHit;
			nextAttack.depth++;
		}
		else {
			searchForNextFirstHit(att, result);
		}
	}
	else
	{
		searchForNextFirstHit(att, result);
	}
}


void BattleshipPlayerSmart::foundVerticalAttUpTransition(Coordinate att, AttackResult result) {
	if (result == AttackResult::Hit)
	{
		myBoard.setHit(att);
		upmostHit = att;

		if ((upmostHit.row > 1) && (myBoard.charAt(Coordinate(upmostHit.row - 1, upmostHit.col, upmostHit.depth)) == 32))
		{
			nextAttack = upmostHit;
			nextAttack.row--;
		} else {
			state = AttackState::foundVerticalAttDown;
			nextAttack = downmostHit;
			nextAttack.row++;
		}
	}
	else if (result == AttackResult::Miss)
	{
		myBoard.setMiss(att);
		state = AttackState::foundVerticalAttDown;
		nextAttack = downmostHit;
		nextAttack.row++;
	}
	else
	{
		searchForNextFirstHit(att, result);
	}
}


void BattleshipPlayerSmart::foundVerticalAttDownTransition(Coordinate att, AttackResult result) {
	if (result == AttackResult::Hit)
	{
		myBoard.setHit(att);
		//set next attack to the right of the last hit, remain in the same state
		nextAttack = downmostHit = att;
		nextAttack.row++;
	}
	else
	{
		searchForNextFirstHit(att, result);
	}
}


void BattleshipPlayerSmart::foundDepthAttInsideTransition(Coordinate att, AttackResult result) {
	if (result == AttackResult::Hit)
	{
		myBoard.setHit(att);
		inmostHit = att;

		if ((inmostHit.depth > 1) && (myBoard.charAt(Coordinate(inmostHit.depth - 1, upmostHit.col, upmostHit.depth)) == 32))
		{
			nextAttack = inmostHit;
			inmostHit.depth--;
		}
		else {
			state = AttackState::foundDepthAttOutside;
			nextAttack = outmostHit;
			nextAttack.depth++;
		}
	}
	else if (result == AttackResult::Miss)
	{
		myBoard.setMiss(att);
		state = AttackState::foundDepthAttOutside;
		nextAttack = outmostHit;
		nextAttack.depth++;
	}
	else
	{
		searchForNextFirstHit(att, result);
	}
}


void BattleshipPlayerSmart::foundDepthAttOutsideTransition(Coordinate att, AttackResult result) {
	if (result == AttackResult::Hit)
	{
		myBoard.setHit(att);
		//set next attack to the right of the last hit, remain in the same state
		nextAttack = outmostHit = att;
		nextAttack.depth++;
	}
	else
	{
		searchForNextFirstHit(att, result);
	}
}


void BattleshipPlayerSmart::notifyOnOpponentsAttackResult(Coordinate att, AttackResult result)
{
	//delete the relevant coordinate from the attack options! and only this cell. leave the perimiter (in case we are being decepted)
	attackMoves.remove(att);

	if (result == AttackResult::Miss) {
		myBoard.setMiss(att);
	}
	else if ((result == AttackResult::Hit) && (myBoard.charAt(att) == 32)) 	{
		myBoard.setHit(att);
		
		//set next attack from attackMoves (not nextAttack variable)
		if ((att.col > 1) && (myBoard.charAt(Coordinate(att.row, att.col - 1, att.depth)) == 32)) //set next attack to the left of the first hit
		{		
			Coordinate searchFor(att.row, att.col - 1, att.depth);
			auto itr = find_if(attackMoves.begin(), attackMoves.end(), [&searchFor](const Coordinate& coor) {
				return GameUtils::coordinatesComperator(searchFor, coor); });
			if (itr != attackMoves.end())
			{
				attackMoves.erase(itr);
				attackMoves.emplace_back(att.row, att.col - 1, att.depth);
			}
			
		}
		if ((att.col < myBoard.cols()) && (myBoard.charAt(Coordinate(att.row, att.col + 1, att.depth)) == 32)) //set next attack to the right of the first hit
		{
			Coordinate searchFor(att.row, att.col + 1, att.depth);
			auto itr = find_if(attackMoves.begin(), attackMoves.end(), [&searchFor](const Coordinate& coor) {
				return GameUtils::coordinatesComperator(searchFor, coor); });
			if (itr != attackMoves.end())
			{
				attackMoves.erase(itr);
				attackMoves.emplace_back(att.row, att.col + 1, att.depth);
			}
		}
		if ((att.row > 1) && (myBoard.charAt(Coordinate(att.row - 1, att.col, att.depth)) == 32)) //set next attack up to the first hit
		{
			Coordinate searchFor(att.row - 1, att.col, att.depth);
			auto itr = find_if(attackMoves.begin(), attackMoves.end(), [&searchFor](const Coordinate& coor) {
				return GameUtils::coordinatesComperator(searchFor, coor); });
			//emplace (row - 1,col) as the next attack in our list (if it was not deleted and legal)
			if (itr != attackMoves.end())
			{
				attackMoves.erase(itr);
				attackMoves.emplace_back(att.row - 1, att.col, att.depth);
			}
		}
		if ((att.row < myBoard.rows()) && (myBoard.charAt(Coordinate(att.row + 1,att.col, att.depth)) == 32)) //set next attack down to the first hit
		{
			Coordinate searchFor(att.row + 1, att.col, att.depth);
			auto itr = find_if(attackMoves.begin(), attackMoves.end(), [&searchFor](const Coordinate& coor) {
				return GameUtils::coordinatesComperator(searchFor, coor); });
			//emplace (row - 1,col) as the next attack in our list (if it was not deleted and legal)
			if (itr != attackMoves.end())
			{
				attackMoves.erase(itr);
				attackMoves.emplace_back(att.row + 1, att.col, att.depth);
			}
		}
		if ((att.depth > 1) && (myBoard.charAt(Coordinate(att.row, att.col, att.depth - 1)) == 32)) //set next attack down to the first hit
		{
			Coordinate searchFor(att.row, att.col, att.depth - 1);
			auto itr = find_if(attackMoves.begin(), attackMoves.end(), [&searchFor](const Coordinate& coor) {
				return GameUtils::coordinatesComperator(searchFor, coor); });
			//emplace (row - 1,col) as the next attack in our list (if it was not deleted and legal)
			if (itr != attackMoves.end())
			{
				attackMoves.erase(itr);
				attackMoves.emplace_back(att.row, att.col, att.depth - 1);
			}
		}
		if ((att.depth < myBoard.depth()) && (myBoard.charAt(Coordinate(att.row, att.col, att.depth + 1)) == 32)) //set next attack down to the first hit
		{
			Coordinate searchFor(att.row, att.col, att.depth + 1);
			auto itr = find_if(attackMoves.begin(), attackMoves.end(), [&searchFor](const Coordinate& coor) {
				return GameUtils::coordinatesComperator(searchFor, coor); });
			//emplace (row - 1,col) as the next attack in our list (if it was not deleted and legal)
			if (itr != attackMoves.end())
			{
				attackMoves.erase(itr);
				attackMoves.emplace_back(att.row, att.col, att.depth + 1);
			}
		}
	}
	else if ((result == AttackResult::Sink) && (myBoard.charAt(att) == 32))
	{
		myBoard.setHit(att);
		if (GameUtils::coordinatesComperator(nextAttack,att))
		{
			searchForNextFirstHit(att , result);
		}
		markSinkedShipAndUpdateAttacks(att);
	}
}

// This method must be implemented in each player(algorithm) .cpp file
IBattleshipGameAlgo* GetAlgorithm()
{
	return new BattleshipPlayerSmart();
}