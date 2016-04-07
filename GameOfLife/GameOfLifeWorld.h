#ifndef GAMEOFLIFEWORLD_H
#define GAMEOFLIFEWORLD_H

#include "Cell.h"

#include "../Grid.h"
#include "../ModuloIntDistribution.h"
#include "../Position.h"
#include "../Simulation.h"

#include "GameOfLifeWorldRenderer.h"

#include <random>

namespace GameOfLife {

class GameOfLifeWorld final : public Simulation
{
public:

	GameOfLifeWorld();

	virtual bool initialize() override;

	virtual void update() override;

	virtual void render() const override;

private:
	Position randomPosition() const;

	Position randomAvailablePosition() const;

private:

	int rowCount;
	int columnCount;

	int initialLivingCellCount;
	int randomToggleCellCount;

	mutable std::minstd_rand0 random; // NOTE: fastest from std
	mutable ModuloIntDistribution<> rowDistribution;
	mutable ModuloIntDistribution<> columnDistribution;

	Grid<Cell> *currentGrid;
	Grid<Cell> *updateGrid;

	friend class GameOfLifeWorldRenderer;
	GameOfLifeWorldRenderer renderer;
};

} // namespace GameOfLife

#endif // GAMEOFLIFEWORLD_H
