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
#include <algorithm>

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

struct OrganismStatistics
{
	void clear()
	{
		energies.clear();
		reproductionEnergies.clear();
		offspringEnergies.clear();
		geneDecrementFactors.clear();
		geneStabilizeFactors.clear();
		geneIncrementFactors.clear();
	}

	void record(const Organism& organism)
	{
		energies.push_back(organism.energy);
		reproductionEnergies.push_back(organism.reproductionEnergy);
		offspringEnergies.push_back(organism.offspringEnergy);
		geneDecrementFactors.push_back(organism.geneDecrementFactor);
		geneStabilizeFactors.push_back(organism.geneStabilizeFactor);
		geneIncrementFactors.push_back(organism.geneIncrementFactor);
	}

	std::vector<int> energies;
	std::vector<int> reproductionEnergies;
	std::vector<int> offspringEnergies;
	std::vector<int> geneDecrementFactors;
	std::vector<int> geneStabilizeFactors;
	std::vector<int> geneIncrementFactors;
};

struct PlantStatistics : public OrganismStatistics
{
};

struct HerbivoreStatistics : public OrganismStatistics
{
	void clear()
	{
		OrganismStatistics::clear();
		feastSizes.clear();
	}

	void record(const Herbivore& herbivore)
	{
		OrganismStatistics::record(herbivore);
		feastSizes.push_back(herbivore.feastSize);
	}

	std::vector<int> feastSizes;
};

struct CarnivoreStatistics : public OrganismStatistics
{
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

	// draw

	mutable std::vector<QuadAttributes> quadAttributes;

	mutable PlantStatistics plantStatistics;
	mutable HerbivoreStatistics herbivoreStatistics;
	mutable CarnivoreStatistics carnivoreStatistics;

	static std::string medianElement(std::vector<int>& v, std::string empty = "-");

	std::string plantEnergy() const { return medianElement(plantStatistics.energies); }
	std::string plantReproduction() const { return medianElement(plantStatistics.reproductionEnergies); }
	std::string plantOffspring() const { return medianElement(plantStatistics.offspringEnergies); }
	std::string plantGeneDecrementFactor() const { return medianElement(plantStatistics.geneDecrementFactors); }
	std::string plantGeneStabilizeFactor() const { return medianElement(plantStatistics.geneStabilizeFactors); }
	std::string plantGeneIncrementFactor() const { return medianElement(plantStatistics.geneIncrementFactors); }

	std::string herbivoreEnergy() const { return medianElement(herbivoreStatistics.energies); }
	std::string herbivoreReproduction() const { return medianElement(herbivoreStatistics.reproductionEnergies); }
	std::string herbivoreOffspring() const { return medianElement(herbivoreStatistics.offspringEnergies); }
	std::string herbivoreGeneDecrementFactor() const { return medianElement(herbivoreStatistics.geneDecrementFactors); }
	std::string herbivoreGeneStabilizeFactor() const { return medianElement(herbivoreStatistics.geneStabilizeFactors); }
	std::string herbivoreGeneIncrementFactor() const { return medianElement(herbivoreStatistics.geneIncrementFactors); }
	std::string herbivoreFeastSize() const { return medianElement(herbivoreStatistics.feastSizes); }

	std::string carnivoreEnergy() const { return medianElement(carnivoreStatistics.energies); }
	std::string carnivoreReproduction() const { return medianElement(carnivoreStatistics.reproductionEnergies); }
	std::string carnivoreOffspring() const { return medianElement(carnivoreStatistics.offspringEnergies); }
	std::string carnivoreGeneDecrementFactor() const { return medianElement(carnivoreStatistics.geneDecrementFactors); }
	std::string carnivoreGeneStabilizeFactor() const { return medianElement(carnivoreStatistics.geneStabilizeFactors); }
	std::string carnivoreGeneIncrementFactor() const { return medianElement(carnivoreStatistics.geneIncrementFactors); }
};

#endif // GRIDWORLD_H
