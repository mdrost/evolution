#include "GridWorld.h"

#include "Restorer.h"

#include <random>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <cstddef>

GridWorld::GridWorld()
	: rows(128)
	, columns(128)
	, cellBlocks(rows, columns)
	//, accidentGenerator(0.005)
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

	initializeShaders();

	instanceVertexBufferObject = VertexBufferObject::generate();

	GLfloat quadVertices[] = {
		-0.005f,  0.005f,
		 0.005f, -0.005f,
		-0.005f, -0.005f,

		-0.005f,  0.005f,
		 0.005f, -0.005f,
		 0.005f,  0.005f,
	};

	quadVertexArrayObject = VertexArrayObject::generate();
	quadVertexBufferObject = VertexBufferObject::generate();

	quadVertexArrayObject.bind();
	quadVertexBufferObject.setData(quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);

	instanceVertexBufferObject.bind();
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(QuadAttributes), (GLvoid*)offsetof(QuadAttributes, offset));
	glVertexAttribDivisor(1, 1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(QuadAttributes), (GLvoid*)offsetof(QuadAttributes, color));
	glVertexAttribDivisor(2, 1);
	instanceVertexBufferObject.unbind();

	quadVertexArrayObject.unbind();

	return true;
}

void GridWorld::initializeShaders()
{
	VertexShader vertexShader(
		"#version 330 core\n"
		"layout (location = 0) in vec2 position;\n"
		"layout (location = 1) in vec2 offset;\n"
		"layout (location = 2) in vec3 color;\n"
		"out vec3 fColor;\n"
		"void main()\n"
		"{\n"
		"	gl_Position = vec4(position + offset, 0.0f, 1.0f);\n"
		"	fColor = color;\n"
		"}\n"
	);
	FragmentShader fragmentShader(
		"#version 330 core\n"
		"in vec3 fColor;\n"
		"out vec4 color;\n"
		"void main()\n"
		"{\n"
		"	color = vec4(fColor, 1.0f);\n"
		"}\n"
	);

	program = ShaderProgram(vertexShader, fragmentShader);
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

void GridWorld::draw() const
{
	quadAttributes.clear();

	plantStatistics.clear();
	herbivoreStatistics.clear();
	carnivoreStatistics.clear();


	Position p;
	for (int r = 0; r < cellBlocks.rows(); ++r) {
		Restorer<int> colRestorer(p.col);
		for (int c = 0; c < cellBlocks.columns(); ++c) {
			const Block<Cell>& block = cellBlocks.block(r, c);
			//std::uniform_real_distribution<float> cd(0.0f, 1.0f);
			//glm::vec3 color(cd(mt), cd(mt), cd(mt));
			Restorer<int> rowRestorer(p.row);
			for (int i = 0; i < block.rows(); ++i) {
				Restorer<int> colRestorer(p.col);
				for (int j = 0; j < block.columns(); ++j) {
					const Cell &cell = block.cell(i, j);
					GLfloat qx = 0.95f - GLfloat(1.90 * p.col) / columns;
					GLfloat qy = 0.95f - GLfloat(1.90 * p.row) / rows;
					if (cell.hasPlant()) {
						quadAttributes.push_back({{qx, qy}, {0.0f, 1.0f, 0.0f}});
						//quadAttributes.push_back({{qx, qy}, color});
						plantStatistics.record(*cell.plant());

						if (cell.hasHerbivore()) {
							quadAttributes.push_back({{qx, qy}, {0.0f, 0.1f, 1.0f}});
							herbivoreStatistics.record(*cell.herbivore());
						}
						if (cell.hasCarnivore()) {
							quadAttributes.push_back({{qx, qy}, {1.0f, 0.1f, 0.0f}});
							carnivoreStatistics.record(*cell.carnivore());
						}
					} else {
						if (cell.hasHerbivore()) {
							quadAttributes.push_back({{qx, qy}, {0.0f, 0.0f, 1.0f}});
							herbivoreStatistics.record(*cell.herbivore());
						}
						if (cell.hasCarnivore()) {
							quadAttributes.push_back({{qx, qy}, {1.0f, 0.0f, 0.0f}});
							carnivoreStatistics.record(*cell.carnivore());
						}
					}
					/*if (cell.accident()) {
						quadAttributes.push_back({{qx, qy}, {1.0f, 1.0f, 0.0f}});
					}*/
					p.col += 1;
				}
				p.row += 1;
			}
			p.col += block.columns();
		}
		p.row += BLOCK_ROWS;
	}

	std::cout << std::endl;

	{
		using namespace std;
		cout << "      plant herbi carni" << endl;
		cout << "count" << setw(6)<<right << plantStatistics.energies.size() << setw(6)<<right << herbivoreStatistics.energies.size() << setw(6)<<right << carnivoreStatistics.energies.size() << endl;
		cout << "energ" << setw(6)<<right << plantEnergy() << setw(6)<<right << herbivoreEnergy() << setw(6)<<right << carnivoreEnergy() << endl;
		cout << "repro" << setw(6)<<right << plantReproduction() << setw(6)<<right << herbivoreReproduction() << setw(6)<<right << carnivoreReproduction() <<  std::endl;
		cout << "offsp" << setw(6)<<right << plantOffspring() << setw(6)<<right << herbivoreOffspring() << setw(6)<<right << carnivoreOffspring() << std::endl;
		cout << "decre" << setw(6)<<right << plantGeneDecrementFactor() << setw(6)<<right << herbivoreGeneDecrementFactor() << setw(6)<<right << carnivoreGeneDecrementFactor() << std::endl;
		cout << "stabi" << setw(6)<<right << plantGeneStabilizeFactor() << setw(6)<<right << herbivoreGeneStabilizeFactor() << setw(6)<<right << carnivoreGeneStabilizeFactor() << std::endl;
		cout << "incre" << setw(6)<<right << plantGeneIncrementFactor() << setw(6)<<right << herbivoreGeneIncrementFactor() << setw(6)<<right << carnivoreGeneIncrementFactor() << std::endl;
		cout << "feast" << setw(6)<<right << "-" << setw(6)<<right << herbivoreFeastSize() << setw(6)<<right << "-" << endl;
		cout << endl;
	}

	program.use();
	quadVertexArrayObject.bind();
	instanceVertexBufferObject.setData(quadAttributes, GL_DYNAMIC_DRAW);
	//std::cout << "size: " << quadAttributes.size() << std::endl;
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, quadAttributes.size());
	quadVertexArrayObject.unbind();
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

std::string GridWorld::medianElement(std::vector<int>& v, std::string empty)
{
	if (v.empty()) {
		return empty;
	} else {
		// NOTE: fuck evens
		int i = v.size()/2;
		std::nth_element(v.begin(), v.begin() + i, v.end());
		return std::to_string(v[i]);
	}
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
