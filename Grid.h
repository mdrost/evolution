#ifndef GRID_H
#define GRID_H

#include "Position.h"

#include <array>
#include <cassert>
#include <vector>

template <class T, int Radius>
using MooreNeighborhood = std::array<std::array<T, 2*Radius+1>, 2*Radius+1>;

template <class T>
class Grid
{
public:

	typedef T value_type;
	typedef T& reference;
	typedef const T& const_reference;
	typedef int size_type;

	Grid(int rowCount, int columnCount)
		: mRowCount(rowCount)
		, mColumnCount(columnCount)
	{
		cells.resize(rowCount);
		for (int i = 0; i < mRowCount; ++i) {
			cells[i].resize(mColumnCount);
		}
	}

	Grid(const Grid &other) = delete;

	Grid(Grid&& other)
		: mRowCount(other.mRowCount)
		, mColumnCount(other.mColumnCount)
		, cells(std::move(other.cells))
	{
	}

	Grid& operator()(const Grid& other) = delete;

	Grid& operator()(Grid&& other)
	{
		mRowCount = other.mRowCount;
		mColumnCount = other.mColumnCount;
		cells = std::move(other.cells);
	}

	size_type rowCount() const noexcept
	{
		return mRowCount;
	}

	size_type columnCount() const noexcept
	{
		return mColumnCount;
	}

	reference at(int rowIndex, int colIndex)
	{
		assert(rowIndex < mRowCount && colIndex < mColumnCount);
		return cells[rowIndex][colIndex];
	}

	const_reference at(int rowIndex, int colIndex) const
	{
		assert(rowIndex < mRowCount && colIndex < mColumnCount);
		return cells[rowIndex][colIndex];
	}

	reference at(Position p)
	{
		return at(p.row, p.col);
	}

	const_reference at(Position p) const
	{
		return at(p.row, p.col);
	}

	template <size_type Radius>
	MooreNeighborhood<value_type, Radius> mooreNeighborhoodAt(int rowIndex, int colIndex)
	{
		assert(rowIndex < mRowCount && colIndex < mColumnCount);
		if (__builtin_expect(needWraparound<Radius>(rowIndex, colIndex), 0)) {
			return mooreNeighborhoodWraparoundedAt<Radius>(rowIndex, colIndex);
		}
		MooreNeighborhood<value_type, Radius> result;
		for (int r = 0 ; r < (2*Radius+1); ++r) {
			int row = rowIndex - (Radius - r);
			for (int c = 0 ; c < (2*Radius+1); ++c) {
				int col = colIndex - (Radius - c);
				result[r][c] = at(row, col);
			}
		}
		return result;
	}

	template <size_type Radius>
	MooreNeighborhood<value_type, Radius> mooreNeighborhoodAt(Position p)
	{
		return mooreNeighborhoodAt<Radius>(p.row, p.col);
	}

	size_type size() const noexcept
	{
		return mRowCount * mColumnCount;
	}

private:

	template <size_type Radius>
	MooreNeighborhood<value_type, Radius> mooreNeighborhoodWraparoundedAt(int rowIndex, int colIndex)
	{
		assert(rowIndex < mRowCount && colIndex < mColumnCount);
		MooreNeighborhood<value_type, Radius> result;
		for (int r = 0 ; r < (2*Radius+1); ++r) {
			int row = rowIndex - (Radius - r);
			row = wraparoundRow(row);
			for (int c = 0 ; c < (2*Radius+1); ++c) {
				int col = colIndex - (Radius - c);
				col = wraparoundColumn(col);
				result[r][c] = at(row, col);
			}
		}
		return result;
	}

	constexpr bool needWraparound(int rowIndex, int colIndex, int radius) const noexcept
	{
		return (rowIndex < radius)
		|| (colIndex < radius)
		|| (rowIndex >= (mRowCount - radius))
		|| (colIndex >= (mColumnCount - radius));
	}

	template <int Radius>
	constexpr bool needWraparound(int rowIndex, int colIndex) const noexcept
	{
		return needWraparound(rowIndex, colIndex, Radius);
	}

	int wraparound(int index, int dimensionSize) const noexcept
	{
		if (index < 0) {
			index += dimensionSize;
		} else if (index >= dimensionSize) {
			index -= dimensionSize;
		}
		return index;
	}

	int wraparoundRow(int rowIndex) const noexcept
	{
		return wraparound(rowIndex, mRowCount);
	}

	int wraparoundColumn(int colIndex) const noexcept
	{
		return wraparound(colIndex, mColumnCount);
	}

private:

	size_type mRowCount;
	size_type mColumnCount;
	std::vector<std::vector<value_type>> cells;
};

#endif // GRID_H
