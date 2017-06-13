#include "OurBoardData.h"


char OurBoardData::charAt(Coordinate c) const
{
	
	if (_playerNum == 0) {
		if ((65 < _board[c.row][c.col][c.depth]) && (_board[c.row][c.col][c.depth] < 90))
			return _board[c.row][c.col][c.depth];
		else
			return ' ';
	}
	else {
		if ((97 < _board[c.row][c.col][c.depth]) && (_board[c.row][c.col][c.depth] < 122))
			return _board[c.row][c.col][c.depth];
		else
			return ' ';
	}

}

