#include "pch.h"
#include "UUID.h"

#include <random>

namespace Venus {

	std::random_device s_RandomDevice;
	std::mt19937_64 s_RandomUUIDEngine(s_RandomDevice());
	std::uniform_int_distribution<uint64_t> s_UniformDistribution;

	UUID::UUID()
		:m_UUID(s_UniformDistribution(s_RandomDevice))
	{
	}

	UUID::UUID(uint64_t uuid)
		:m_UUID(uuid)
	{
	}
}