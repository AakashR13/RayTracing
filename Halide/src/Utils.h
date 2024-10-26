#pragma once

#include <glm/glm.hpp>

namespace Utils
{
	static uint32_t ConvertToRGBA(const glm::vec4& color)
	{
		// clamping is usually done in GPU by APIs like vulkan
		auto r = (uint8_t)(color.r * 255.0f);
		auto g = (uint8_t)(color.g * 255.0f);
		auto b = (uint8_t)(color.b * 255.0f);
		auto a = (uint8_t)(color.a * 255.0f);

		uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;
		return result;
	}

	static uint32_t PCG_Hash(uint32_t input) // Random Functions
	{
		uint32_t state = input * 747796405u + 2891336453u;
		uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
		return (word >> 22u) ^ word;
	}

	static float RandomFloat(uint32_t& seed)
	{
		seed = PCG_Hash(seed);
		return (float)seed / (float)std::numeric_limits<uint32_t>::max();
	}
	static float  RandomFloat(uint32_t& seed, float min, float max)
	{
		return RandomFloat(seed) * (max - min) + min;
	}
	static glm::vec3 RandomVec3(uint32_t& seed)
	{
		return glm::vec3{ RandomFloat(seed) };
	}

	static glm::vec3 RandomVec3(uint32_t& seed, float min, float max)
	{
		return glm::vec3{ RandomFloat(seed,min,max) };
	}

	static glm::vec3 InUnitSphere(uint32_t& seed) // For ray reflection
	{
		return glm::normalize(RandomVec3(seed, -1, 1));
	}
}