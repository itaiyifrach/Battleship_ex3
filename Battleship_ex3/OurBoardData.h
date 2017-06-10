#pragma once
#include "IBattleshipGameAlgo.h"
#include "GameUtils.h"
class OurBoardData: public BoardData
{
public:
	OurBoardData(unique_ptr<unique_ptr<unique_ptr<char[]>[]>[]> board, int rows, int cols, int depth,int playerNum): _rows(rows),_cols(cols),_depth(depth),_playerNum(playerNum)
	{

		
	};
	virtual ~OurBoardData() = default;
	//returns only selected players' chars
	virtual char charAt(Coordinate c) const override;

protected:
	int _rows = 0;
	int _cols = 0; 
	int _depth = 0; 
	int _playerNum = 0;
private:
	unique_ptr<unique_ptr<unique_ptr<char[]>[]>[]> board;
};