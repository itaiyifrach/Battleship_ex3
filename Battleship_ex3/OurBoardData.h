#pragma once
#include "IBattleshipGameAlgo.h"
#include <vector>
using namespace std;

typedef vector<vector<char>> char2DArray;
typedef vector<vector<vector<char>>> char3DArray;

class OurBoardData: public BoardData
{
public:
	// Empty constructor for the game algorithm to start with something before SetBoard is called
	OurBoardData() : _playerNum(-1), _board(char3DArray())	{}

	OurBoardData(char3DArray board, int rows, int cols, int depth, int playerNum) : _playerNum(playerNum), _board(board)
	{
		// this sets BoardData protected members:
		this->_rows = rows;
		this->_cols = cols;
		this->_depth = depth;
	}

	virtual ~OurBoardData() = default;
	
	//returns only selected players' chars
	virtual char charAt(Coordinate c) const override;

	void copyFromOtherDataBoard(const BoardData& playerBoard);
	
	void setHit(const Coordinate& c);

	void setMiss(const Coordinate& c);

	void setSpace(const Coordinate& c);

protected:
	// 0 for player A, 1 for player b, 10 to GameManager
	int _playerNum;
private:
	char3DArray _board;
};