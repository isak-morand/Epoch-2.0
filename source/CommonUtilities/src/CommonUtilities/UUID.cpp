#include "UUID.h"
#include <random>

namespace CU
{
	const UUID UUID::Null(0);

	thread_local static std::mt19937_64 staticEngine(std::random_device{}());
	thread_local static std::uniform_int_distribution<uint64_t> staticUniformDistribution;

	UUID UUID::Random()
	{
		return UUID{ staticUniformDistribution(staticEngine) };
	}
}
