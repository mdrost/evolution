#ifndef POSITIONOFFSET_H
#define POSITIONOFFSET_H

#include "Position.h"

struct PositionOffset
{
	PositionOffset()
		: PositionOffset(0, 0)
	{}

	PositionOffset(int rowOffset, int colOffset)
		: rowOffset(rowOffset), colOffset(colOffset)
	{}

	int rowOffset;
	int colOffset;
};

inline Position operator+(Position p, PositionOffset po)
{
	return Position(p.row + po.rowOffset, p.col + po.colOffset);
}

inline Position operator-(Position p, PositionOffset po)
{
	return Position(p.row - po.rowOffset, p.col - po.colOffset);
}

inline PositionOffset operator-(Position p1, Position p2)
{
	return PositionOffset(p1.row - p2.row, p1.col - p2.col);
}

#endif // POSITIONOFFSET_H
