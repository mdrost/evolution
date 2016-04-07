#ifndef CELL_H
#define CELL_H

#include <cassert>

#define CELL_USE_PIMPL 0

#define CELL_ORGANISM_USE_PTR 0

#define CELL_ORGANISM_USE_DOUBLE_PTR 0

struct Organism
{
	Organism()
	{
	}

	Organism(int energy, int reproductionEnergy, int offspringEnergy, int geneDecrementFactor, int geneStabilizeFactor, int geneIncrementFactor)
		: energy(energy)
		, reproductionEnergy(reproductionEnergy)
		, offspringEnergy(offspringEnergy)
		, geneDecrementFactor(geneDecrementFactor)
		, geneStabilizeFactor(geneStabilizeFactor)
		, geneIncrementFactor(geneIncrementFactor)
	{
	}

	int energy;
	int reproductionEnergy;
	int offspringEnergy;
	int geneDecrementFactor;
	int geneStabilizeFactor;
	int geneIncrementFactor;
};

struct Plant : public Organism
{
public:
	using Organism::Organism;
};

struct Herbivore : public Organism
{
public:
	Herbivore()
	{
	}

	Herbivore(int energy, int reproductionEnergy, int offspringEnergy, int geneDecrementFactor, int geneStabilizeFactor, int geneIncrementFactor, int feastSize)
		: Organism(energy, reproductionEnergy, offspringEnergy, geneDecrementFactor, geneStabilizeFactor, geneIncrementFactor)
		, feastSize(feastSize)
	{
	}

	int feastSize;
};

struct Carnivore : public Organism
{
public:
	using Organism::Organism;
};

#if CELL_ORGANISM_USE_PTR

#if CELL_ORGANISM_USE_DOUBLE_PTR

template <class OrganismType>
class OrganismHelper
{
public:
	OrganismHelper()
		: mRetainedOrganism(new OrganismType())
	{
	}

	~OrganismHelper()
	{
		delete mRetainedOrganism;
	}

	inline void setOrganism(const OrganismType& organism)
	{
		mOrganism = mRetainedOrganism;
		*mOrganism = organism;
	}

	inline void removeOrganism()
	{
		mOrganism = nullptr;
	}

	inline bool hasOrganism() const
	{
		return mOrganism != nullptr;
	}

	inline OrganismType* organism()
	{
		return mOrganism;
	}

	inline const OrganismType* organism() const
	{
		return mOrganism;
	}

private:
	OrganismType *mOrganism = nullptr;
	OrganismType *mRetainedOrganism;
};

#else // CELL_ORGANISM_USE_DOUBLE_PTR

template <class OrganismType>
class OrganismHelper
{
public:
	~OrganismHelper()
	{
		delete mOrganism;
	}

	inline void setOrganism(const OrganismType& organism)
	{
		if (hasOrganism()) {
			*mOrganism = organism;
		} else {
			mOrganism = new OrganismType(organism);
		}
	}

	inline void removeOrganism()
	{
		delete mOrganism;
		mOrganism = nullptr;
	}

	inline bool hasOrganism() const
	{
		return mOrganism != nullptr;
	}

	inline OrganismType* organism()
	{
		return mOrganism;
	}

	inline const OrganismType* organism() const
	{
		return mOrganism;
	}

private:
	OrganismType *mOrganism = nullptr;
};

#endif // CELL_ORGANISM_USE_DOUBLE_PTR

#else // CELL_ORGANISM_USE_PTR

template <class OrganismType>
class OrganismHelper
{
public:
	inline void setOrganism(const OrganismType& organism)
	{
		mHasOrganism = true;
		mOrganism = organism;
	}

	inline void removeOrganism()
	{
		mHasOrganism = false;
	}

	inline bool hasOrganism() const
	{
		return mHasOrganism;
	}

	inline OrganismType* organism()
	{
		return &mOrganism;
	}

	inline const OrganismType* organism() const
	{
		return &mOrganism;
	}

private:
	bool mHasOrganism = false;
	OrganismType mOrganism;
};

#endif // CELL_ORGANISM_USE_PTR

#if CELL_USE_PIMPL
class CellHelper
#else
class Cell
#endif
{
public:

	// accident

	/*inline bool& accident()
	{
		return mAccident;
	}

	inline bool accident() const
	{
		return mAccident;
	}*/

	// plant

	inline void setPlant(const Plant& plant)
	{
		mPlantHelper.setOrganism(plant);
		assert(hasPlant());
	}

	inline void removePlant()
	{
		mPlantHelper.removeOrganism();
		assert(!hasPlant());
	}

	inline bool hasPlant() const
	{
		return mPlantHelper.hasOrganism();
	}

	inline Plant* plant()
	{
		assert(hasPlant());
		return mPlantHelper.organism();
	}

	inline const Plant* plant() const
	{
		assert(hasPlant());
		return mPlantHelper.organism();
	}

	// herbivore

	inline void setHerbivore(const Herbivore& herbivore)
	{
		assert(!hasCarnivore());
		mHerbivoreHelper.setOrganism(herbivore);
		assert(hasHerbivore());
	}

	inline void removeHerbivore()
	{
		mHerbivoreHelper.removeOrganism();
		assert(!hasHerbivore());
	}

	inline bool hasHerbivore() const
	{
		return mHerbivoreHelper.hasOrganism();
	}

	inline Herbivore* herbivore()
	{
		assert(hasHerbivore());
		return mHerbivoreHelper.organism();
	}

	inline const Herbivore* herbivore() const
	{
		assert(hasHerbivore());
		return mHerbivoreHelper.organism();
	}

	// carnivore

	inline void setCarnivore(const Carnivore& carnivore)
	{
		assert(!hasHerbivore());
		mCarnivoreHelper.setOrganism(carnivore);
		assert(hasCarnivore());
	}

	inline void removeCarnivore()
	{
		mCarnivoreHelper.removeOrganism();
		assert(!hasCarnivore());
	}

	inline bool hasCarnivore() const
	{
		return mCarnivoreHelper.hasOrganism();
	}

	inline Carnivore* carnivore()
	{
		assert(hasCarnivore());
		return mCarnivoreHelper.organism();
	}

	inline const Carnivore* carnivore() const
	{
		assert(hasCarnivore());
		return mCarnivoreHelper.organism();
	}

private:
	bool mAccident = false;
	OrganismHelper<Plant> mPlantHelper;
	OrganismHelper<Herbivore> mHerbivoreHelper;
	OrganismHelper<Carnivore> mCarnivoreHelper;
};

#if CELL_USE_PIMPL

class Cell
{
public:
	Cell()
		: p(new CellHelper)
	{
	}

	Cell(const Cell& other)
		: p(new CellHelper)
	{
		*p = *other.p;
	}

	Cell(Cell&& other)
		: p(other.p)
	{
		other.p = nullptr;
	}

	Cell& operator=(const Cell& other)
	{
		*p = *other.p;
		return *this;
	}

	Cell& operator=(Cell&& other)
	{
		p = other.p;
		other.p = nullptr;
		return *this;
	}

	~Cell()
	{
		delete p;
	}

	// accident

	inline bool& accident()
	{
		return p->accident();
	}

	inline bool accident() const
	{
		return p->accident();
	}

	// plant

	inline void setPlant(const Plant& plant)
	{
		p->setPlant(plant);
		assert(hasPlant());
	}

	inline void removePlant()
	{
		p->removePlant();
		assert(!hasPlant());
	}

	inline bool hasPlant() const
	{
		return p->hasPlant();
	}

	inline Plant* plant()
	{
		assert(hasPlant());
		return p->plant();
	}

	inline const Plant* plant() const
	{
		assert(hasPlant());
		return p->plant();
	}

	// herbivore

	inline void setHerbivore(const Herbivore& herbivore)
	{
		p->setHerbivore(herbivore);
		assert(hasHerbivore());
	}

	inline void removeHerbivore()
	{
		p->removeHerbivore();
		assert(!hasHerbivore());
	}

	inline bool hasHerbivore() const
	{
		return p->hasHerbivore();
	}

	inline Herbivore* herbivore()
	{
		assert(hasHerbivore());
		return p->herbivore();
	}

	inline const Herbivore* herbivore() const
	{
		assert(hasHerbivore());
		return p->herbivore();
	}

	// carnivore

	inline void setCarnivore(const Carnivore& carnivore)
	{
		p->setCarnivore(carnivore);
		assert(hasCarnivore());
	}

	inline void removeCarnivore()
	{
		p->removeCarnivore();
		assert(!hasCarnivore());
	}

	inline bool hasCarnivore() const
	{
		return p->hasCarnivore();
	}

	inline Carnivore* carnivore()
	{
		assert(hasCarnivore());
		return p->carnivore();
	}

	inline const Carnivore* carnivore() const
	{
		assert(hasCarnivore());
		return p->carnivore();
	}

private:
	CellHelper *p;
};

#endif // CELL_USE_PIMPL

#endif // CELL_H
