#ifndef MODULOINTDISTRIBUTION_H
#define MODULOINTDISTRIBUTION_H

#include <limits>
#include <type_traits>

template <class IntType = int>
class ModuloIntDistribution
{
	static_assert(std::is_integral<IntType>::value,
		  "template argument not an integral type");

public:
	typedef IntType result_type;
	typedef IntType param_type;

	explicit ModuloIntDistribution(IntType a = 0, IntType b = std::numeric_limits<IntType>::max())
		: a(a)
		, b(b)
	{
	}

	template<typename UniformRandomNumberGenerator>
	result_type operator()(UniformRandomNumberGenerator& urng)
	{
		return urng() % (b - a + 1) + a;
	}

private:
	IntType a;
	IntType b;
};

#endif // MODULOINTDISTRIBUTION_H
