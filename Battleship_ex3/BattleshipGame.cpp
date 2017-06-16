
#include "BattleshipGame.h"


tuple<int,int,int> BattleshipGame::playGame() {
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
		bool selfHit = false;
		if (GameUtils::selfHit(mainBoard.charAt(currAttack), turnOf)) {
			selfHit = true;
		}

		//check for the sink-score of the ship in (i,j,k)th sector
		int sectorScore = GameUtils::getScoreForSector(mainBoard.charAt(currAttack));

		//check for the attack result and update the mainBoard according to the result
		pair<AttackResult, bool> AttackResTupple = getAttackResult(currAttack);
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
		PlayerA->notifyOnAttackResult(turnOf, currAttack, currAttackRes);
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
	int winnerNumber = -1;
	//TODO- return results instead of printing them
	if ((numOfShipsA) && (!numOfShipsB))
	{
		winnerNumber = PLAYER_A_NUM;
		//cout << PLAYER_A_WON_STR << endl;
	}
		
	else if ((!numOfShipsA) && (numOfShipsB))
	{		
		winnerNumber = PLAYER_B_NUM;
		//cout << PLAYER_B_WON_STR << endl;
	}
	return make_tuple(winnerNumber, scoreA, scoreB);
		
	/*cout << POINTS_STR << endl;
	cout << PLAYER_A_POINTS_STR << scoreA << endl;
	cout << PLAYER_B_POINTS_STR << scoreB << endl;*/
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

std::pair<AttackResult, bool> BattleshipGame::getAttackResult(const Coordinate& att)
{
	std::pair<AttackResult, bool> result;
	if (mainBoard.charAt(att) == 32) {
		result.first = AttackResult::Miss;
		result.second = false;
	}
	else if (mainBoard.charAt(att)== 'X') {
		result.first = AttackResult::Hit;
		result.second = true;
	}
	else if (updateBoardAndCheckSink(att)) {
		result.first = AttackResult::Sink;
		result.second = false;
	}
	else {
		result.first = AttackResult::Hit;
		result.second = false;
	}
	return result;
}

bool BattleshipGame::updateBoardAndCheckSink(const Coordinate& att)
{
	int direction = GameUtils::shipDirection(mainBoard, att);
	//horizontal
	if (direction == 0) 
	{
		int colIndex = att.col;
		//find left corner and save it in "left"
		while ((colIndex > 1) && (mainBoard.charAt(Coordinate(att.row, colIndex, att.depth)) != 32)) {
			colIndex--;
		}
		if (mainBoard.charAt(Coordinate(att.row, colIndex, att.depth)) == 32) {
			colIndex++;
		}
		int left = colIndex;

		//count how many parts of the ship remain un-hit
		int count = 0;
		while ((colIndex < cols) && (mainBoard.charAt(Coordinate(att.row, colIndex, att.depth)) != 32))
		{
			if (mainBoard.charAt(Coordinate(att.row, colIndex++, att.depth)) != 'X') {
				count++;
			}
		}
		//if we have only one un-hit part, it was the initial part in (i,j,k), we erase the ship and return that it's a sink
		if (count == 1) {
			colIndex = left;
			while ((colIndex < cols) && (mainBoard.charAt(Coordinate(att.row, colIndex, att.depth)) != 32)) {
				mainBoard.setSpace(Coordinate(att.row, colIndex++, att.depth));
			}
			return true;
		}
		//if it's not a sink we just update the (i,j,k) coordinate on the mainBoard as a hit and return it isn't a sink
		mainBoard.setHit(att);
		return false;
	}

	//vertical
	else if (direction == 1)
	{
		int rowIndex = att.row;
		//find lower corner and save it in "lower"
		while ((rowIndex > 1) && (mainBoard.charAt(Coordinate(rowIndex, att.col, att.depth)) != 32)) {
			rowIndex--;
		}
		if (mainBoard.charAt(Coordinate(rowIndex, att.col, att.depth)) == 32) {
			rowIndex++;
		}
		int lower = rowIndex;

		//count how many parts of the ship remain un-hit
		int count = 0;
		while ((rowIndex < rows) && (mainBoard.charAt(Coordinate(rowIndex, att.col, att.depth)) != 32)) {
			if (mainBoard.charAt(Coordinate(rowIndex++, att.col, att.depth)) != 'X') {
				count++;
			}
		}

		//if we have only one un-hit part and it was the initial part in (i,j,k), we erase the ship and return that it's a sink
		if ((count == 1) && (mainBoard.charAt(att)!='X') && (mainBoard.charAt(att) != 32)) {
			rowIndex = lower;
			while ((rowIndex < rows) && (mainBoard.charAt(Coordinate(rowIndex, att.col, att.depth)) != 32)) {
				mainBoard.setSpace(Coordinate(rowIndex++, att.col, att.depth));
			}
			return true;
		}
		//if it's not a sink we just update the (i,j,k) coordinate on the mainBoard as a hit and return it isn't a sink
		mainBoard.setHit(att);
		return false;
	}
	else {
		//depth
		int depthIndex = att.depth;
		//find deepest corner and save it in "deepest"
		while ((depthIndex > 1) && (mainBoard.charAt(Coordinate(att.row, att.col, depthIndex)) != 32)) {
			depthIndex--;
		}
		if (mainBoard.charAt(Coordinate(att.row, att.col, depthIndex)) == 32) {
			depthIndex++;
		}
		int deepest = depthIndex;

		//count how many parts of the ship remain un-hit
		int count = 0;
		while ((depthIndex < depth) && (mainBoard.charAt(Coordinate(att.row, att.col, depthIndex)) != 32)) {
			if (mainBoard.charAt(Coordinate(att.row, att.col, depthIndex++)) != 'X') {
				count++;
			}
		}

		//if we have only one un-hit part, it was the initial part in (i,j,k), we erase the ship and return that it's a sink
		if (count == 1) {
			depthIndex = deepest;
			while ((depthIndex < depth) && (mainBoard.charAt(Coordinate(att.row, att.col, depthIndex)) != 32)) {
				mainBoard.setSpace(Coordinate(att.row, att.col, depthIndex++));
			}
			return true;
		}
		//if it's not a sink we just update the (i,j,k) coordinate on the mainBoard as a hit and return it isn't a sink
		mainBoard.setHit(att);
	}
	return false;
}