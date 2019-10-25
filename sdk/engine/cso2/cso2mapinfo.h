#pragma once

#include "cso2modmapinfo.h"		

#define MAPINFO_FLAG_NEW 1
#define MAPINFO_FLAG_EVENT 2
#define MAPINFO_FLAG_CONTINUOUS 4

// size: 0xA0
class CSO2MapInfo : public CSO2ModMapInfo
{
public:
	CSO2MapInfo() = default;
	~CSO2MapInfo() override = default;
	bool Unknown00() override;
	bool AssignOptionValues(BaseOptionList* pOptionList, int32_t a3, KeyValues* a4) override;
	//virtual void Unknown02();
	BaseOptionList* CreateOptionList() override;
	bool Unknown04() override;

private:
	// leftovers from an early version?
	std::vector<std::string> m_vLoadingBackground; //0x0074

	// if (flag & MAPINFO_FLAG_NEW == true) then it's a new map
	// if (flag & MAPINFO_FLAG_EVENT == true) then it's an event map
	// if (flag & MAPINFO_FLAG_CONTINUOUS == true) then it can be used in a playlist or something
	std::vector<int32_t> m_vMapFlags; //0x0080

	std::vector<int32_t> m_vCategory; //0x008C
	std::map<int32_t, int32_t> m_Unknown03; //0x0098 unknown type
};