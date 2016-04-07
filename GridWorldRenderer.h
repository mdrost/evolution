#ifndef RENDERER_H
#define RENDERER_H

#include "Buffer.h"
#include "Cell.h"
#include "Shader.h"
#include "VertexArrayObject.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <vector>

class GridWorld;

class GridWorldRenderer
{
	struct OrganismStatistics
	{
		void clear();

		void record(const Organism& organism);

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
		void clear();

		void record(const Herbivore& herbivore);

		std::vector<int> feastSizes;
	};

	struct CarnivoreStatistics : public OrganismStatistics
	{
	};

	struct Quad
	{
		glm::vec2 position;
		glm::vec3 color;
	};

public:

	GridWorldRenderer();

	void initialize();

	void render(const GridWorld& gridWorld) const;

private:

	void initializeShaders();


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

	ShaderProgram program;
	VertexBufferObject instanceVertexBufferObject;
	VertexArrayObject quadVertexArrayObject;
	VertexBufferObject quadVertexBufferObject;

	mutable std::vector<Quad> quads;
	mutable PlantStatistics plantStatistics;
	mutable HerbivoreStatistics herbivoreStatistics;
	mutable CarnivoreStatistics carnivoreStatistics;
};

#endif // RENDERER_H
