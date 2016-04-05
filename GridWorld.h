#ifndef GRIDWORLD_H
#define GRIDWORLD_H

#include "BlockMap.h"
#include "Buffer.h"
#include "ModuloIntDistribution.h"
#include "Position.h"
#include "Shader.h"
#include "Simulation.h"
#include "VertexArrayObject.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <vector>
#include <set>
#include <random>

class ProbabilityGenerator
{
public:
	ProbabilityGenerator(double probability)
		: probability(probability)
	{
	}

	template<class Generator>
	bool operator() (Generator& g)
	{
		return distribution(g) < probability;
	}

private:
	double probability;
	std::uniform_real_distribution<> distribution;
};

struct QuadAttributes
{
	glm::vec2 offset;
	glm::vec3 color;
};

class GridWorld final : public Simulation
{
public:
	GridWorld();

	virtual bool initialize() override;

	virtual void update() override;

	virtual void draw() const override;

private:
	void initializeShaders();

	void clearAccidents();

	void applyAccidents();

	void peripheralBlockUpdate(Block<Cell> &block, Position localPosition, Position globalPosition);

	void innerBlockUpdate(Block<Cell> &block, Position localPosition, Position globalPosition);

	void peripheralBlockPlantUpdate(Block<Cell> &block, Position localPosition, Position globalPosition);

	void innerBlockPlantUpdate(Block<Cell> &block, Position localPosition, Position globalPosition);

	void peripheralBlockHerbivoreUpdate(Block<Cell> &block, Position localPosition, Position globalPosition);

	void innerBlockHerbivoreUpdate(Block<Cell> &block, Position localPosition, Position globalPosition);

	void peripheralBlockCarnivoreUpdate(Block<Cell> &block, Position localPosition, Position globalPosition);

	void innerBlockCarnivoreUpdate(Block<Cell> &block, Position localPosition, Position globalPosition);

	Plant reproduce(Plant &parent);

	Herbivore reproduce(Herbivore &parent);

	Carnivore reproduce(Carnivore &parent);

	Position randomPosition();

	Position randomNearbyPosition(Position position);

	Position randomNearbyWraparoundedPosition(Position position);

	Position wraparound(Position position);

	std::string medianElement(std::vector<int> &vector) const;

	template <class T>
	int randomOffset(const T& o) const
	{
		return randomOffset(o.geneDecrementFactor, o.geneStabilizeFactor, o.geneIncrementFactor);
	}

	int randomOffset(int decrementFactor, int stabilizeFactor, int incrementFactor) const;

private:
	mutable std::minstd_rand0 random; // fastest

	int rows;
	int columns;

	static constexpr int BLOCK_ROWS = 128;
	static constexpr int BLOCK_COLUMNS = 128;

	BlockMap<Cell, BLOCK_ROWS, BLOCK_COLUMNS> cellBlocks;

	//ProbabilityGenerator accidentGenerator;
	std::vector<Position> lastAccidents;

	ModuloIntDistribution<> xPositionDistribution;
	ModuloIntDistribution<> yPositionDistribution;
	ModuloIntDistribution<> positionOffsetDistribution;
	ModuloIntDistribution<> geneOffsetDistribution;

	ShaderProgram program;
	VertexBufferObject instanceVertexBufferObject;
	VertexArrayObject quadVertexArrayObject;
	VertexBufferObject quadVertexBufferObject;

	mutable std::vector<QuadAttributes> quadAttributes;

	mutable std::vector<int> plantEnergies;
	mutable std::vector<int> plantReproductionEnergies;
	mutable std::vector<int> plantOffspringEnergies;
	mutable std::vector<int> plantGeneDecrementFactors;
	mutable std::vector<int> plantGeneStabilizeFactors;
	mutable std::vector<int> plantGeneIncrementFactors;

	mutable std::vector<int> herbivoreEnergies;
	mutable std::vector<int> herbivoreReproductionEnergies;
	mutable std::vector<int> herbivoreOffspringEnergies;
	mutable std::vector<int> herbivoreGeneDecrementFactors;
	mutable std::vector<int> herbivoreGeneStabilizeFactors;
	mutable std::vector<int> herbivoreGeneIncrementFactors;
	mutable std::vector<int> herbivoreFeastSizes;

	mutable std::vector<int> carnivoreEnergies;
	mutable std::vector<int> carnivoreReproductionEnergies;
	mutable std::vector<int> carnivoreOffspringEnergies;
	mutable std::vector<int> carnivoreGeneDecrementFactors;
	mutable std::vector<int> carnivoreGeneStabilizeFactors;
	mutable std::vector<int> carnivoreGeneIncrementFactors;
};

#endif // GRIDWORLD_H
