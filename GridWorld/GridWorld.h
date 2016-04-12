#ifndef GRIDWORLD_H
#define GRIDWORLD_H

#include "GridWorldRenderer.h"

#include "../BlockMap.h"
#include "../ModuloIntDistribution.h"
#include "../Position.h"
#include "../Simulation.h"

#include <vector>
#include <set>
#include <random>
#include <algorithm>

class GridWorld final : public Simulation
{
public:
	GridWorld();

	virtual bool initialize() override;

	virtual void update() override;

	virtual void render() const override;

private:

	void clearAccidents();

	void applyAccidents();


	void peripheralBlockUpdate(Block<Cell> &block, Position localPosition, Position globalPosition);

	void topLeftWorldPeripheralBlockUpdate(Block<Cell> &block, Position localPosition, Position globalPosition);

	void topWorldPeripheralBlockUpdate(Block<Cell> &block, Position localPosition, Position globalPosition);

	void topRightWorldPeripheralBlockUpdate(Block<Cell> &block, Position localPosition, Position globalPosition);

	void leftWorldPeripheralBlockUpdate(Block<Cell> &block, Position localPosition, Position globalPosition);

	void rightWorldPeripheralBlockUpdate(Block<Cell> &block, Position localPosition, Position globalPosition);

	void bottomLeftWorldPeripheralBlockUpdate(Block<Cell> &block, Position localPosition, Position globalPosition);

	void bottomWorldPeripheralBlockUpdate(Block<Cell> &block, Position localPosition, Position globalPosition);

	void bottomRightWorldPeripheralBlockUpdate(Block<Cell> &block, Position localPosition, Position globalPosition);

	void innerBlockUpdate(Block<Cell> &block, Position localPosition, Position globalPosition);


	void peripheralBlockPlantUpdate(Block<Cell> &block, Position localPosition, Position globalPosition);

	void peripheralBlockHerbivoreUpdate(Block<Cell> &block, Position localPosition, Position globalPosition);

	void peripheralBlockCarnivoreUpdate(Block<Cell> &block, Position localPosition, Position globalPosition);


	void innerBlockPlantUpdate(Block<Cell> &block, Position localPosition, Position globalPosition);

	void innerBlockHerbivoreUpdate(Block<Cell> &block, Position localPosition, Position globalPosition);

	void innerBlockCarnivoreUpdate(Block<Cell> &block, Position localPosition, Position globalPosition);


	Plant reproduce(Plant &parent);

	Herbivore reproduce(Herbivore &parent);

	Carnivore reproduce(Carnivore &parent);


	Position randomPosition();

	Position randomNearbyPosition(Position position);

	Position randomNearbyWraparoundedPosition(Position position);

	Position wraparound(Position position);

	template <class T>
	int randomOffset(const T& o) const
	{
		return randomOffset(o.geneDecrementFactor, o.geneStabilizeFactor, o.geneIncrementFactor);
	}

	int randomOffset(int decrementFactor, int stabilizeFactor, int incrementFactor) const;

	int randomOffset() const
	{
		return randomOffset(1, 1, 1);
	}

private:
	mutable std::minstd_rand0 random; // NOTE: fastest from std

	int rows;
	int columns;

	static constexpr int BLOCK_ROWS = 128;
	static constexpr int BLOCK_COLUMNS = 128;

	BlockMap<Cell, BLOCK_ROWS, BLOCK_COLUMNS> cellBlocks;

	std::vector<Position> lastAccidents;

	ModuloIntDistribution<> xPositionDistribution;
	ModuloIntDistribution<> yPositionDistribution;
	ModuloIntDistribution<> positionOffsetDistribution;
	ModuloIntDistribution<> geneOffsetDistribution;

	friend class GridWorldRenderer;

	GridWorldRenderer renderer;
};

#endif // GRIDWORLD_H
