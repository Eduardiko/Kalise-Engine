#include "UUID_.h"

#include <random>

#include <unordered_map>


static std::unordered_map<UUID_, std::string> m_Map;

static void AddToMap()
{
	m_Map[UUID_()] = "UUIDMap";
}

static std::random_device s_RandomDevice;
static std::mt19937_64 s_Engine(s_RandomDevice());
static std::uniform_int_distribution<uint64_t> s_UniformDistribution;

UUID_::UUID_() : m_UUID(s_UniformDistribution(s_Engine))
{

}

UUID_::UUID_(uint64_t uuid) : m_UUID(uuid)
{

}
