#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>

struct ItemEffect
{											 
	std::string szEffectName;
	std::string szEffectMaterial;
};

// size: 0x1C
class CItemOptionInfo
{
public:
	std::map<int32_t, int32_t> m_Unknown00; //0x0014 unknown type
	std::vector<ItemEffect*> m_vItemEffects; //0x0008
	std::map<int32_t, int32_t> m_Unknown01; //0x0014 unknown type
};
