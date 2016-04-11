#include "PlantWorld.h"

#include "../ModuloIntDistribution.h"
#include "../PositionOffset.h"

#include <cassert>

#include <iostream>

namespace PlantWorld {

PlantWorld::PlantWorld()
	: rowCount(128)
	, columnCount(128)
	, initialPlantCount(rowCount * columnCount / 32)
	, rowDistribution(0, columnCount - 1)
	, columnDistribution(0, rowCount - 1)
	, currentGrid(std::make_unique<Grid<Cell>>(rowCount, columnCount))
	, updateGrid(std::make_unique<Grid<Cell>>(rowCount, columnCount))
{
	std::random_device rd;
	int seed = rd();
	random.seed(seed);
}

bool PlantWorld::initialize()
{
	for (int i = 0; i < initialPlantCount; ++i) {
		Cell& cell = currentGrid->at(randomAvailablePosition());
		cell.hasPlant = true;
		cell.plant = randomPlant();
	}

	renderer.initialize();

	return true;
}

void PlantWorld::update()
{
	updateCopy();

	//reproduce();

	updateEnergy();

	reproduce();

	applyAccidents();

	std::swap(updateGrid, currentGrid);
}

void PlantWorld::updateCopy()
{
	for (int r = 0; r < rowCount; ++r) {
		for (int c = 0; c < columnCount; ++c) {
			Position p(r, c);
			updateGrid->at(p) = currentGrid->at(p);
		}
	}
}

void PlantWorld::reproduce()
{
	setWantedReproductionPositions();

	for (int rowIndex = 0; rowIndex < rowCount; ++rowIndex) {
		for (int colIndex = 0; colIndex < columnCount; ++colIndex) {
			const Position globalPosition(rowIndex, colIndex);
			Cell& updateCell = updateGrid->at(globalPosition);
			if (updateCell.hasPlant) {
				continue;
			}
			MooreNeighborhood<Cell, 1> mn = currentGrid->mooreNeighborhoodAt<1>(globalPosition);
			int willingReproductorCount = 0;
			for (int r = 0; r < 3; ++r) {
				for (int c = 0; c < 3; ++c) {
					const Cell& cell = mn[r][c];
					if (cell.hasPlant) {
						const Plant& plant = cell.plant;
						if (plant.wantReproduceAt(globalPosition)) {
							willingReproductorCount += 1;
						}
					}
				}
			}
			if (willingReproductorCount == 0) {
				continue;
			}
			ModuloIntDistribution<> dist(0, willingReproductorCount-1);
			int successfulReproductorIndex = dist(random);
			int willingReproductorIndex = 0;
			for (int r = 0; r < 3; ++r) {
				for (int c = 0; c < 3; ++c) {
					const Cell& cell = mn[r][c];
					if (cell.hasPlant) {
						const Plant& plant = cell.plant;
						if (plant.wantReproduceAt(globalPosition)) {
							if (successfulReproductorIndex == willingReproductorIndex) {
								updateCell.hasPlant = true;
								updateCell.plant = reproduce(plant);
								//initialPlantCount += 1;
								//std::cout << "reproduce: " << initialPlantCount << std::endl;
								goto nextCell;
							}
							willingReproductorIndex += 1;
						}
					}
				}
			}
			nextCell:;
		}
	}
}

void PlantWorld::setWantedReproductionPositions()
{
	for (int r = 0; r < rowCount; ++r) {
		for (int c = 0; c < columnCount; ++c) {
			Position p(r, c);
			Cell& cell = currentGrid->at(p);
			if (cell.hasPlant) {
				Plant& plant  = cell.plant;
				if (plant.wantReproduce()) {
					plant.reproductionPosition = randomNearbyWraparoundedPosition(p);
				}
			}
		}
	}
}

void PlantWorld::updateEnergy()
{
	addRandomEnergyBySize();

	for (int r = 0; r < rowCount; ++r) {
		for (int c = 0; c < columnCount; ++c) {
			Position p(r, c);
			Cell& cell = updateGrid->at(p);
			if (cell.hasPlant) {
				Plant& plant  = cell.plant;
				plant.energy -= (1 + plant.wantReproduce());
				if (plant.energy <= 0) {
					cell.hasPlant = false;
					//initialPlantCount -= 1;
					//std::cout << "death: " << initialPlantCount << std::endl;
				} else {
					if (plant.size < plant.maxSize) {
						plant.size += 1;
					}
					plant.age += 1;
				}
			}
		}
	}
	//std::cout << "count: " << initialPlantCount << std::endl;
}

void PlantWorld::addRandomEnergyBySize()
{
	for (int r = 0; r < rowCount; ++r) {
		for (int c = 0; c < columnCount; ++c) {
			Position p(r, c);
			MooreNeighborhood<Cell, 1> mn = currentGrid->mooreNeighborhoodAt<1>(p);
			Position energyPosition = p + (randomPositionBySize(mn) - Position(1, 1));
			energyPosition = wraparound(energyPosition);
			Cell& updateCell = updateGrid->at(energyPosition);
			if (updateCell.hasPlant) {
				updateCell.plant.energy += 1;
			}
		}
	}
}

void PlantWorld::applyAccidents()
{
	for (int i = 0; i < 10; ++i) {
		Position position = randomPosition();
		Cell& cell = updateGrid->at(position);
		cell.hasPlant = false;
	}
}

void PlantWorld::render() const
{
	renderer.render(*this);
}

Plant PlantWorld::randomPlant() const
{
	ModuloIntDistribution<> dist(1, 10);
	Plant plant;
	plant.age = 0;
	plant.energy = dist(random);
	plant.size = 1;
	plant.maxSize = dist(random);
	//plant.reproductionAge = 1;
	plant.reproductionEnergy = dist(random);

	return plant;
}

Plant PlantWorld::reproduce(const Plant &parent) const
{
	Plant child;
	child.energy = 1;
	child.size = 1;
	child.maxSize = std::max(parent.maxSize + parent.mutagenicity.generate(random), 1);
	child.reproductionEnergy = std::max(parent.reproductionEnergy + parent.mutagenicity.generate(random), 1);
	child.mutagenicity = parent.mutagenicity.selfMutated(random);

	return child;
}

Position PlantWorld::randomPosition() const
{
	return {rowDistribution(random), columnDistribution(random)};
}

Position PlantWorld::randomAvailablePosition() const
{
	Position p;
	do {
		p = randomPosition();
	} while (currentGrid->at(p).hasPlant);
	return p;
}

Position PlantWorld::randomPositionBySize(const MooreNeighborhood<Cell, 1>& mn) const
{
	int sum = 0;
	for (int r = 0; r < 3; ++r) {
		for (int c = 0; c < 3; ++c) {
			const Cell& cell = mn[r][c];
			if (cell.hasPlant) {
				sum += cell.plant.size;
			}
		}
	}
	if (sum == 0) {
		return Position(1, 1);
	}
	ModuloIntDistribution<> dist(0, sum-1);
	int number = dist(random);
	sum = 0;
	for (int r = 0; r < 3; ++r) {
		for (int c = 0; c < 3; ++c) {
			const Cell& cell = mn[r][c];
			if (cell.hasPlant) {
				sum += cell.plant.size;
				if (number <= sum) {
					return Position(r, c);
				}
			}
		}
	}
	assert(false);
	return Position(1, 1);
}

Position PlantWorld::randomNearbyPosition(Position position) const
{
	const PositionOffset offsets[] = {
		{-1, -1}, {-1, 0}, {-1, 1},
		{ 0, -1},          { 0, 1},
		{ 1, -1}, { 1, 0}, { 1, 1},
	};
	ModuloIntDistribution<> positionOffsetDistribution(0, 7);
	PositionOffset offset = offsets[positionOffsetDistribution(random)];
	Position nearbyPosition = position + offset;
	assert(nearbyPosition != position);
	return nearbyPosition;
}

Position PlantWorld::randomNearbyWraparoundedPosition(Position position) const
{
	return wraparound(randomNearbyPosition(position));
}

Position PlantWorld::wraparound(Position position) const
{
	while (position.row < 0) {
		position.row += rowCount;
	}
	while (position.col < 0) {
		position.col += columnCount;
	}
	while (position.row >= rowCount) {
		position.row -= rowCount;
	}
	while (position.col >= columnCount) {
		position.col -= columnCount;
	}
	return position;
}

} // namespace PlantWorld
