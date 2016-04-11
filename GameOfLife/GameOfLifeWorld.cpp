#include "GameOfLifeWorld.h"

namespace GameOfLife {

GameOfLifeWorld::GameOfLifeWorld()
	: rowCount(128)
	, columnCount(128)
	, initialLivingCellCount(rowCount * columnCount / 4)
	, randomToggleCellCount(1)
	, rowDistribution(0, columnCount - 1)
	, columnDistribution(0, rowCount - 1)
	, currentGrid(std::make_unique<Grid<Cell>>(rowCount, columnCount))
	, updateGrid(std::make_unique<Grid<Cell>>(rowCount, columnCount))
{
	std::random_device rd;
	int seed = rd();
	random.seed(seed);
}

bool GameOfLifeWorld::initialize()
{
	for (int i = 0; i < initialLivingCellCount; ++i) {
		currentGrid->at(randomAvailablePosition()) = 1;
	}

	renderer.initialize();

	return true;
}

void GameOfLifeWorld::update()
{
	for (int r = 0; r < rowCount; ++r) {
		for (int c = 0; c < columnCount; ++c) {
			Position p(r, c);
			Cell cell = currentGrid->at(p);
			MooreNeighborhood<Cell, 1> cells = currentGrid->mooreNeighborhoodAt<1>(p);
			int livingCellCount =
				cells[0][0] + cells[0][1] + cells[0][2] +
				cells[1][0]        +        cells[1][2] +
				cells[2][0] + cells[2][1] + cells[2][2];
			if (cell) {
				if (!(livingCellCount == 2 || livingCellCount == 3)) {
					updateGrid->at(p) = false;
				} else {
					updateGrid->at(p) = cell;
				}
			} else {
				if (livingCellCount == 3) {
					updateGrid->at(p) = true;
				} else {
					updateGrid->at(p) = cell;
				}
			}
		}
	}

	for (int i = 0; i < randomToggleCellCount; ++i) {
		Position p = randomPosition();
		Cell cell = updateGrid->at(p);
		updateGrid->at(p) = !cell;
	}

	std::swap(updateGrid, currentGrid);
}

void GameOfLifeWorld::render() const
{
	renderer.render(*this);
}

Position GameOfLifeWorld::randomPosition() const
{
	return {rowDistribution(random), columnDistribution(random)};
}

Position GameOfLifeWorld::randomAvailablePosition() const
{
	Position p;
	do {
		p = randomPosition();
	} while (currentGrid->at(p));
	return p;
}

} // namespace GameOfLife
