#include "OurBoardData.h"


char OurBoardData::charAt(Coordinate c) const
{
	if ((c.row < 1) || (_rows < c.row) ||
		(c.col < 1) || (_cols < c.col) ||
		(c.depth < 1) || (_depth < c.depth)) {
		return ' ';
	}
	if ((_playerNum == 10) ||
		(_board[c.row - 1][c.col - 1][c.depth - 1] == 'X') ||
		(_board[c.row - 1][c.col - 1][c.depth - 1] == '%')) {
		return _board[c.row - 1][c.col - 1][c.depth - 1];
	}
	if (_playerNum == 0) {
		if ((65 < _board[c.row - 1][c.col - 1][c.depth - 1]) && (_board[c.row - 1][c.col - 1][c.depth - 1] < 90)){
			return _board[c.row - 1][c.col - 1][c.depth - 1];
		}
	} else {
		if ((97 < _board[c.row - 1][c.col - 1][c.depth - 1]) && (_board[c.row - 1][c.col - 1][c.depth - 1] < 122)) {
			return _board[c.row - 1][c.col - 1][c.depth - 1];
		}
	}
	return ' ';
}


void OurBoardData::copyFromOtherDataBoard(const BoardData& playerBoard) {
	_rows = playerBoard.rows();
	_cols = playerBoard.cols();
	_depth = playerBoard.depth();

	// copy board cell by cell, relying on BoardData's interface.
	_board.resize(_rows);
	for (int i = 0; i < _rows; i++)
	{
		_board[i].resize(_cols);
		for (int j = 0; j < _cols; j++)
		{
			_board[i][j].resize(_depth);
			for (int k = 0; k < _depth; k++)
			{
				_board[i][j][k] = playerBoard.charAt(Coordinate(i + 1, j + 1, k + 1));
			}
		}
	}
}

void OurBoardData::setHit(const Coordinate& c)
{
	_board[c.row - 1][c.col - 1][c.depth - 1] = 'X';
}

void OurBoardData::setMiss(const Coordinate& c)
{
	_board[c.row - 1][c.col - 1][c.depth - 1] = '%';
}

void OurBoardData::setSpace(const Coordinate& c)
{
	_board[c.row - 1][c.col - 1][c.depth - 1] = ' ';
}