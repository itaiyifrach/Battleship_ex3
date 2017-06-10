#include "OurBoardData.h"


char OurBoardData::charAt(Coordinate c) const
{

	if (_playerNum == 0) {
		if ((65 < board[c.row][c.col][c.depth]) && (board[c.row][c.col][c.depth] < 90))
			return board[c.row][c.col][c.depth];
		else
			return ' ';
	}
	else {
		if ((97 < board[c.row][c.col][c.depth]) && (board[c.row][c.col][c.depth] < 122))
			return board[c.row][c.col][c.depth];
		else
			return ' ';
	}

}
