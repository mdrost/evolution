#ifndef PLANTWORLD_PLANTWORLD_H
#define PLANTWORLD_PLANTWORLD_H


#include "Cell.h"

#include "../Grid.h"
#include "../ModuloIntDistribution.h"
#include "../Position.h"
#include "../Simulation.h"

#include "PlantWorldRenderer.h"

#include <memory>
#include <random>

namespace PlantWorld {

class PlantWorld final : public Simulation
{
public:

	PlantWorld();

	virtual bool initialize() override;

	virtual void update() override;

	virtual void render() const override;

private:

	void updateCopy();

	void reproduce();

	void setWantedReproductionPositions();

	void updateEnergy();

	void addRandomEnergyBySize();

	void applyAccidents();

	Plant randomPlant() const;

	Plant reproduce(const Plant &parent) const;

	Position randomPosition() const;

	Position randomAvailablePosition() const;

	Position randomPositionBySize(const MooreNeighborhood<Cell, 1>& mn) const;

	Position randomNearbyPosition(Position position) const;

	Position randomNearbyWraparoundedPosition(Position position) const;

	Position wraparound(Position position) const;

private:

	int rowCount;
	int columnCount;

	int initialPlantCount;

	mutable std::minstd_rand0 random; // NOTE: fastest from std
	mutable ModuloIntDistribution<> rowDistribution;
	mutable ModuloIntDistribution<> columnDistribution;

	std::unique_ptr<Grid<Cell>> currentGrid;
	std::unique_ptr<Grid<Cell>> updateGrid;

	friend class PlantWorldRenderer;
	PlantWorldRenderer renderer;
};

} // namespace PlantWorld

#endif // PLANTWORLD_PLANTWORLD_H
