#ifndef BLOCKMAP_H
#define BLOCKMAP_H

#include "Position.h"

#include <cassert>
#include <cmath>
#include <vector>
#include <iostream>

template <class CellType>
class Block
{
public:
	Block(int rows, int columns)
		: mPlantCells(new CellType[rows*columns])
		, mRows(rows)
		, mColumns(columns)
	{
		//std::cout << mRows << "x" << mColumns << std::endl;
	}

	Block(Block& other) = delete;

	Block(Block&& other)
		: mPlantCells(other.mPlantCells)
		, mRows(other.mRows)
		, mColumns(other.mColumns)
	{
		other.mPlantCells = nullptr;
	}

	Block& operator=(Block& other) = delete;

	Block& operator=(Block&& other)
	{
		mPlantCells = other.mPlantCells;
		mRows = other.mRows;
		mColumns = other.mColumns;
		other.mPlantCells = nullptr;
	}

	~Block()
	{
		delete[] mPlantCells;
	}

	CellType& cell(int row, int col)
	{
		assert(row < mRows && col < mColumns);
		return mPlantCells[row*mColumns + col];
	}

	const CellType& cell(int row, int col) const
	{
		assert(row < mRows && col < mColumns);
		return mPlantCells[row*mColumns + col];
	}

	CellType& cell(Position p)
	{
		return cell(p.row, p.col);
	}

	const CellType& cell(Position p) const
	{
		return cell(p.row, p.col);
	}

	int rows() const
	{
		return mRows;
	}

	int columns() const
	{
		return mColumns;
	}

private:
	CellType* mPlantCells;
	int mRows;
	int mColumns;
};

template <class CellType, int BLOCK_ROWS, int BLOCK_COLUMNS>
class BlockMap
{
public:
	BlockMap(int columns, int rows)
		: mRows(std::ceil((double)rows / BLOCK_ROWS))
		, mColumns(std::ceil((double)columns / BLOCK_COLUMNS))
	{
		blocks.resize(mRows);
		for (int i = 0; i < mRows; ++i) {
			for (int j = 0; j < mColumns; ++j) {
				int blockRows;
				int blockColumns;
				if ((i + 1) * BLOCK_ROWS <= rows) {
					blockRows = BLOCK_ROWS;
				} else {
					blockRows = rows - i*BLOCK_ROWS;
				}
				if ((j + 1) * BLOCK_COLUMNS <= columns) {
					blockColumns = BLOCK_COLUMNS;
				} else {
					blockColumns = columns - j*BLOCK_COLUMNS;
				}
				blocks[i].push_back(Block<CellType>(blockRows, blockColumns));
			}
		}
	}

	CellType& cell(int row, int col)
	{
		int blockRow = row / BLOCK_ROWS;
		int blockCol = col / BLOCK_COLUMNS;
		int cellRow = row - blockRow * BLOCK_ROWS;
		int cellCol = col - blockCol * BLOCK_COLUMNS;
		return blocks[blockRow][blockCol].cell(cellRow, cellCol);
	}

	const CellType& cell(int row, int col) const
	{
		int blockRow = row / BLOCK_ROWS;
		int blockCol = col / BLOCK_COLUMNS;
		int cellRow = row - blockRow * BLOCK_ROWS;
		int cellCol = col - blockCol * BLOCK_COLUMNS;
		return blocks[blockRow][blockCol].cell(cellRow, cellCol);
	}

	CellType& cell(Position p)
	{
		return cell(p.row, p.col);
	}

	const CellType& cell(Position p) const
	{
		return cell(p.row, p.col);
	}

	int rows() const
	{
		return mRows;
	}

	int columns() const
	{
		return mColumns;
	}

	Block<CellType>& block(int row, int col)
	{
		assert(row < mRows && col < mColumns);
		return blocks[row][col];
	}

	const Block<CellType>& block(int row, int col) const
	{
		assert(row < mRows && col < mColumns);
		return blocks[row][col];
	}

	Block<CellType>& block(Position p)
	{
		return block(p.row, p.col);
	}

	const Block<CellType>& block(Position p) const
	{
		return block(p.row, p.col);
	}

private:
	std::vector<std::vector<Block<CellType>>> blocks;
	int width;
	int height;
	int mRows;
	int mColumns;
};

#endif // BLOCKMAP_H
