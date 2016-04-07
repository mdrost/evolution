#include "GridWorld.h"

#include "Restorer.h"

#include <cmath>
#include <random>

#include <iostream>

GridWorld::GridWorld()
	: rows(128)
	, columns(128)
	, cellBlocks(rows, columns)
	, xPositionDistribution(0, columns - 1)
	, yPositionDistribution(0, rows - 1)
	, positionOffsetDistribution(0, 7)
	, geneOffsetDistribution(-1, 1)
{
	std::random_device rd;
	int seed = rd();
	random.seed(seed);
}

bool GridWorld::initialize()
{
	std::cout << "sizeof(Cell): " << sizeof(Cell) << std::endl;

	std::uniform_int_distribution<> plantDistribution(1, 10);
	for (int i = 0; i < 2500; ++i) {
		auto position = randomPosition();
		Cell &cell = cellBlocks.cell(position);
		cell.setPlant({
			plantDistribution(random),
			plantDistribution(random),
			plantDistribution(random),
			1, 1, 1
		});
	}

	std::uniform_int_distribution<> herbivoreDistribution(10, 100);
	for (int i = 0; i < 1000; ++i) {
		Position position;
		do {
			position = randomPosition();
		} while (cellBlocks.cell(position).hasHerbivore() || cellBlocks.cell(position).hasCarnivore());
		Cell &cell = cellBlocks.cell(position);
		cell.setHerbivore({
			herbivoreDistribution(random),
			herbivoreDistribution(random),
			herbivoreDistribution(random),
			1, 1, 1,
			1
		});
	}

	std::uniform_int_distribution<> carnivoreDistribution(10, 100);
	for (int i = 0; i < 500; ++i) {
		Position position;
		do {
			position = randomPosition();
		} while (cellBlocks.cell(position).hasHerbivore() || cellBlocks.cell(position).hasCarnivore());
		Cell &cell = cellBlocks.cell(position);
		cell.setCarnivore({
			carnivoreDistribution(random),
			carnivoreDistribution(random),
			carnivoreDistribution(random),
			1, 1, 1
		});
	}

	renderer.initialize();

	return true;
}

void GridWorld::update()
{
	Position p;
	for (int blockRow = 0; blockRow < cellBlocks.rows(); ++blockRow) {
		Restorer<int> colRestorer(p.col);
		for (int blockCol = 0; blockCol < cellBlocks.columns(); ++blockCol) {
			Restorer<int> rowRestorer(p.row);
			Block<Cell>& block = cellBlocks.block(blockRow, blockCol);

			int cellRow = 0;
			if (cellRow < block.rows()) {
				Restorer<int> colRestorer(p.col);
				for (int cellCol = 0; cellCol < block.columns(); ++cellCol) {
					peripheralBlockUpdate(block, {cellRow, cellCol}, p);
					p.col += 1;
				}
				p.row += 1;
			}
			for (cellRow = 1; cellRow < (block.rows()-1); ++cellRow) {
				Restorer<int> colRestorer(p.col);
				int cellCol = 0;
				if (cellCol < block.columns()) {
					peripheralBlockUpdate(block, {cellRow, cellCol}, p);
					p.col += 1;
				}
				for (cellCol = 1; cellCol < (block.columns()-1); ++cellCol) {
					innerBlockUpdate(block, {cellRow, cellCol}, p);
					p.col += 1;
				}
				if (cellCol < block.columns()) {
					peripheralBlockUpdate(block, {cellRow, cellCol}, p);
					p.col += 1;
				}
				p.row += 1;
			}
			if (cellRow < block.rows()) {
				Restorer<int> colRestorer(p.col);
				for (int cellCol = 0; cellCol < block.columns(); ++cellCol) {
					peripheralBlockUpdate(block, {cellRow, cellCol}, p);
					p.col += 1;
				}
				p.row += 1;
			}

			p.col += block.columns();
		}
		p.row += BLOCK_ROWS;
	}

	applyAccidents();
}

void GridWorld::peripheralBlockUpdate(Block<Cell> &block, Position localPosition, Position globalPosition)
{
	Cell &cell = block.cell(localPosition);
	if (cell.hasPlant()) {
		peripheralBlockPlantUpdate(block, localPosition, globalPosition);
	}
	if (cell.hasHerbivore()) {
		peripheralBlockHerbivoreUpdate(block, localPosition, globalPosition);
	}
	if (cell.hasCarnivore()) {
		peripheralBlockCarnivoreUpdate(block, localPosition, globalPosition);
	}
}

void GridWorld::topLeftWorldPeripheralBlockUpdate(Block<Cell> &block, Position localPosition, Position globalPosition)
{

}

void GridWorld::topWorldPeripheralBlockUpdate(Block<Cell> &block, Position localPosition, Position globalPosition)
{

}

void GridWorld::topRightWorldPeripheralBlockUpdate(Block<Cell> &block, Position localPosition, Position globalPosition)
{

}

void GridWorld::leftWorldPeripheralBlockUpdate(Block<Cell> &block, Position localPosition, Position globalPosition)
{

}

void GridWorld::rightWorldPeripheralBlockUpdate(Block<Cell> &block, Position localPosition, Position globalPosition)
{

}

void GridWorld::bottomLeftWorldPeripheralBlockUpdate(Block<Cell> &block, Position localPosition, Position globalPosition)
{

}

void GridWorld::bottomWorldPeripheralBlockUpdate(Block<Cell> &block, Position localPosition, Position globalPosition)
{

}

void GridWorld::bottomRightWorldPeripheralBlockUpdate(Block<Cell> &block, Position localPosition, Position globalPosition)
{

}

void GridWorld::innerBlockUpdate(Block<Cell> &block, Position localPosition, Position globalPosition)
{
	Cell &cell = block.cell(localPosition);
	if (cell.hasPlant()) {
		innerBlockPlantUpdate(block, localPosition, globalPosition);
	}
	if (cell.hasHerbivore()) {
		innerBlockHerbivoreUpdate(block, localPosition, globalPosition);
	}
	if (cell.hasCarnivore()) {
		innerBlockCarnivoreUpdate(block, localPosition, globalPosition);
	}
}

void GridWorld::peripheralBlockPlantUpdate(Block<Cell> &block, Position localPosition, Position globalPosition)
{
	Cell& cell = block.cell(localPosition);
	Plant* plant = cell.plant();
	Plant tmpPlant = *plant;
	if (tmpPlant.energy >= tmpPlant.reproductionEnergy) {
		Position nearbyGlobalPosition = randomNearbyWraparoundedPosition(globalPosition);
		Cell &nearbyCell = cellBlocks.cell(nearbyGlobalPosition);
		if (!nearbyCell.hasPlant()) {
			nearbyCell.setPlant(reproduce(tmpPlant));
		}
	}
	if (cell.hasHerbivore() || cell.hasCarnivore()) {
		tmpPlant.energy -= 1;
	} else {
		tmpPlant.energy += 1;
	}
	if (tmpPlant.energy <= 0) {
		cell.removePlant();
	} else {
		*plant = tmpPlant;
	}
}

void GridWorld::innerBlockPlantUpdate(Block<Cell> &block, Position localPosition, Position globalPosition)
{
	Cell& cell = block.cell(localPosition);
	Plant* plant = cell.plant();
	Plant tmpPlant = *plant;
	if (tmpPlant.energy >= tmpPlant.reproductionEnergy) {
		Position nearbyLocalPosition = randomNearbyPosition(localPosition);
		Cell &nearbyCell = block.cell(nearbyLocalPosition);
		if (!nearbyCell.hasPlant()) {
			nearbyCell.setPlant(reproduce(tmpPlant));
		}
	}
	if (cell.hasHerbivore() || cell.hasCarnivore()) {
		tmpPlant.energy -= 1;
	} else {
		tmpPlant.energy += 1;
	}
	if (tmpPlant.energy <= 0) {
		cell.removePlant();
	} else {
		*plant = tmpPlant;
	}
}

void GridWorld::peripheralBlockHerbivoreUpdate(Block<Cell> &block, Position localPosition, Position globalPosition)
{
	Cell* cell = &block.cell(localPosition);
	Herbivore* herbivore = cell->herbivore();
	Position nearbyGlobalPosition = randomNearbyWraparoundedPosition(globalPosition);
	Cell* nearbyCell = &cellBlocks.cell(nearbyGlobalPosition);
	if (herbivore->energy >= herbivore->reproductionEnergy) {
		if (!nearbyCell->hasHerbivore() && !nearbyCell->hasCarnivore()) {
			nearbyCell->setHerbivore(reproduce(*herbivore));
		}
	} else {
		if (!nearbyCell->hasHerbivore() && !nearbyCell->hasCarnivore()) {
			nearbyCell->setHerbivore(*herbivore);
			cell->removeHerbivore();
			cell = nearbyCell;
			herbivore = cell->herbivore();
		}
		if (cell->hasPlant()) {
			Plant* plant = cell->plant();
			int feast = std::min(herbivore->feastSize, plant->energy);
			herbivore->energy += (feast * 2/3);
			plant->energy -= feast;
			if (plant->energy == 0) {
				cell->removePlant();
			}
		}
	}
	herbivore->energy -= 1;
	if (herbivore->energy <= 0) {
		cell->removeHerbivore();
	}
}

void GridWorld::innerBlockHerbivoreUpdate(Block<Cell> &block, Position localPosition, Position globalPosition)
{
	Cell* cell = &block.cell(localPosition);
	Herbivore* herbivore = cell->herbivore();
	Position nearbyLocalPosition = randomNearbyPosition(localPosition);
	Cell* nearbyCell = &block.cell(nearbyLocalPosition);
	if (herbivore->energy >= herbivore->reproductionEnergy) {
		if (!nearbyCell->hasHerbivore() && !nearbyCell->hasCarnivore()) {
			nearbyCell->setHerbivore(reproduce(*herbivore));
		}
	} else {
		if (!nearbyCell->hasHerbivore() && !nearbyCell->hasCarnivore()) {
			nearbyCell->setHerbivore(*herbivore);
			cell->removeHerbivore();
			cell = nearbyCell;
			herbivore = cell->herbivore();
		}
		if (cell->hasPlant()) {
			Plant* plant = cell->plant();
			int feast = std::min(herbivore->feastSize, plant->energy);
			herbivore->energy += (feast * 2/3);
			plant->energy -= feast;
			if (plant->energy == 0) {
				cell->removePlant();
			}
		}
	}
	herbivore->energy -= 1;
	if (herbivore->energy <= 0) {
		cell->removeHerbivore();
	}
}

void GridWorld::peripheralBlockCarnivoreUpdate(Block<Cell> &block, Position localPosition, Position globalPosition)
{
	Carnivore* carnivore = block.cell(localPosition).carnivore();
	Position nearbyGlobalPosition = randomNearbyWraparoundedPosition(globalPosition);
	Cell* nearbyCell = &cellBlocks.cell(nearbyGlobalPosition);
	if (carnivore->energy >= carnivore->reproductionEnergy) {
		if (!nearbyCell->hasHerbivore() && !nearbyCell->hasCarnivore()) {
			nearbyCell->setCarnivore(reproduce(*carnivore));
		}
	} else {
		if (!nearbyCell->hasHerbivore()) {
			nearbyGlobalPosition = randomNearbyWraparoundedPosition(globalPosition);
			nearbyCell = &cellBlocks.cell(nearbyGlobalPosition);
		}
		if (!nearbyCell->hasCarnivore()) {
			if (nearbyCell->hasHerbivore()) {
				carnivore->energy += (nearbyCell->herbivore()->energy * 2/3);
				nearbyCell->removeHerbivore();
			}
			nearbyCell->setCarnivore(*carnivore);
			block.cell(localPosition).removeCarnivore();
			carnivore = nearbyCell->carnivore();
			globalPosition = nearbyGlobalPosition;
		}
	}
	carnivore->energy -= 1;
	if (carnivore->energy <= 0) {
		cellBlocks.cell(globalPosition).removeCarnivore();
	}
}

void GridWorld::innerBlockCarnivoreUpdate(Block<Cell> &block, Position localPosition, Position globalPosition)
{
	Carnivore* carnivore = block.cell(localPosition).carnivore();
	Position nearbyLocalPosition = randomNearbyPosition(localPosition);
	Cell* nearbyCell = &block.cell(nearbyLocalPosition);
	if (carnivore->energy >= carnivore->reproductionEnergy) {
		if (!nearbyCell->hasHerbivore() && !nearbyCell->hasCarnivore()) {
			nearbyCell->setCarnivore(reproduce(*carnivore));
		}
	} else {
		if (!nearbyCell->hasHerbivore()) {
			nearbyLocalPosition = randomNearbyWraparoundedPosition(globalPosition);
			nearbyCell = &block.cell(nearbyLocalPosition);
		}
		if (!nearbyCell->hasCarnivore()) {
			if (nearbyCell->hasHerbivore()) {
				carnivore->energy += (nearbyCell->herbivore()->energy * 2/3);
				nearbyCell->removeHerbivore();
			}
			nearbyCell->setCarnivore(*carnivore);
			block.cell(localPosition).removeCarnivore();
			carnivore = nearbyCell->carnivore();
			localPosition = nearbyLocalPosition;
		}
	}
	carnivore->energy -= 1;
	if (carnivore->energy <= 0) {
		block.cell(localPosition).removeCarnivore();
	}
}

void GridWorld::clearAccidents()
{
	/*for (auto position : lastAccidents) {
		Cell& cell = cellBlocks.cell(position);
		cell.accident() = false;
	}*/
	lastAccidents.clear();
}

void GridWorld::applyAccidents()
{
	//clearAccidents();
	for (int i = 0; i < 5; ++i) {
		Position position = randomPosition();
		Cell& cell = cellBlocks.cell(position);
		//cell.accident() = true;
		cell.removePlant();
		cell.removeHerbivore();
		//lastAccidents.push_back(position);
	}
}

Plant GridWorld::reproduce(Plant &parent)
{
	Plant child;
	child.energy = parent.offspringEnergy;
	child.reproductionEnergy = std::max(parent.reproductionEnergy + randomOffset(parent), 1);
	child.offspringEnergy = std::max(parent.offspringEnergy + randomOffset(parent), 1);
	child.geneDecrementFactor = std::max(parent.geneDecrementFactor + randomOffset(parent), 1);
	child.geneStabilizeFactor = std::max(parent.geneStabilizeFactor + randomOffset(parent), 1);
	child.geneIncrementFactor = std::max(parent.geneIncrementFactor + randomOffset(parent), 1);

	parent.energy -= (parent.offspringEnergy * 1.5);

	return child;
}

Herbivore GridWorld::reproduce(Herbivore &parent)
{
	Herbivore child;
	child.energy = parent.offspringEnergy;
	child.reproductionEnergy = std::max(parent.reproductionEnergy + randomOffset(parent), 1);
	child.offspringEnergy = std::max(parent.offspringEnergy + randomOffset(parent), 1);
	child.geneDecrementFactor = std::max(parent.geneDecrementFactor + randomOffset(parent), 1);
	child.geneStabilizeFactor = std::max(parent.geneStabilizeFactor + randomOffset(parent), 1);
	child.geneIncrementFactor = std::max(parent.geneIncrementFactor + randomOffset(parent), 1);
	child.feastSize = std::max(parent.feastSize + randomOffset(parent), 0);

	parent.energy -= (parent.offspringEnergy * 1.5);

	return child;
}

Carnivore GridWorld::reproduce(Carnivore &parent)
{
	Carnivore child;
	child.energy = parent.offspringEnergy;
	child.reproductionEnergy = std::max(parent.reproductionEnergy + randomOffset(parent), 1);
	child.offspringEnergy = std::max(parent.offspringEnergy + randomOffset(parent), 1);
	child.geneDecrementFactor = std::max(parent.geneDecrementFactor + randomOffset(parent), 1);
	child.geneStabilizeFactor = std::max(parent.geneStabilizeFactor + randomOffset(parent), 1);
	child.geneIncrementFactor = std::max(parent.geneIncrementFactor + randomOffset(parent), 1);

	parent.energy -= (parent.offspringEnergy * 1.5);

	return child;
}

void GridWorld::render() const
{
	renderer.render(*this);
}

Position GridWorld::randomPosition()
{
	return {
		xPositionDistribution(random),
		yPositionDistribution(random)
	};
}

Position GridWorld::randomNearbyPosition(Position position)
{
	const Position offsets[] = {
		{-1, -1}, {-1, 0}, {-1, 1},
		{0, -1}, {0, 1},
		{1, -1}, {1, 0}, {1, 1},
	};
	Position offset = offsets[positionOffsetDistribution(random)];
	position.row += offset.row;
	position.col += offset.col;
	return position;
}

Position GridWorld::randomNearbyWraparoundedPosition(Position position)
{
	return wraparound(randomNearbyPosition(position));
}

Position GridWorld::wraparound(Position position)
{
	while (position.row < 0) {
		position.row += rows;
	}
	while (position.col < 0) {
		position.col += columns;
	}
	while (position.row >= rows) {
		position.row -= rows;
	}
	while (position.col >= columns) {
		position.col -= columns;
	}
	return position;
}

int GridWorld::randomOffset(int decrementFactor, int stabilizeFactor, int incrementFactor) const
{
	ModuloIntDistribution<int> dist(0, decrementFactor + stabilizeFactor + incrementFactor - 1);
	int n = dist(random);
	if (n < decrementFactor) {
		return -1;
	} else if (n < (decrementFactor + stabilizeFactor)) {
		return 0;
	} else {
		return 1;
	}
}
