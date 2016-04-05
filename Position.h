#ifndef POSITION_H
#define POSITION_H

class Position
{
public:
	Position()
		: Position(0, 0)
	{}

	Position(int row, int col)
		: row(row), col(col)
	{}

	int row;
	int col;
};

#endif // POSITION_H
