#ifndef PROBABILITYGENERATOR_H
#define PROBABILITYGENERATOR_H

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

#endif // PROBABILITYGENERATOR_H
