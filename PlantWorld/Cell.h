#ifndef PLANTWORLD_CELL_H
#define PLANTWORLD_CELL_H

#include "../ModuloIntDistribution.h"
#include "../Position.h"

#include <algorithm>

namespace PlantWorld {

struct MutagenicityGenes
{
	int stabilityFactor = 1;
	int incrementFactor = 1;
	int decrementFactor = 1;

	template<typename UniformRandomNumberGenerator>
	int generate(UniformRandomNumberGenerator& urng) const
	{
		ModuloIntDistribution<int> dist(0, stabilityFactor + incrementFactor + decrementFactor - 1);
		int n = dist(urng);
		if (n < stabilityFactor) {
			return 0;
		} else if (n < (stabilityFactor + incrementFactor)) {
			return 1;
		} else {
			return -1;
		}
	}

	template<typename UniformRandomNumberGenerator>
	MutagenicityGenes selfMutated(UniformRandomNumberGenerator& urng) const
	{
		MutagenicityGenes result = *this;
		result.stabilityFactor = std::max(result.stabilityFactor + generate(urng), 1);
		result.incrementFactor = std::max(result.incrementFactor + generate(urng), 1);
		result.decrementFactor = std::max(result.decrementFactor + generate(urng), 1);
		return result;
	}
};

struct Plant
{
	// state
	int age = 0;
	int energy = 0;
	int size;

	// genes
	int maxSize;
	//int reproductionAge;
	int reproductionEnergy;
	MutagenicityGenes mutagenicity;

	// while update
	bool wantReproduce() const noexcept
	{
		//return age >= reproductionAge;
		return energy >= reproductionEnergy;
		//return true;
	}

	bool wantReproduceAt(Position p) const
	{
		return wantReproduce() && reproductionPosition == p;
	}

	Position reproductionPosition;
};

struct Cell
{
	bool hasPlant = false;
	Plant plant;
};

} // namespace PlantWorld

#endif // PLANTWORLD_CELL_H
