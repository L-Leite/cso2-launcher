#pragma once

#include "cso2modmapinfo.h"		

// size: 0x80
class CSO2AutoMatchModInfo : public CSO2ModMapInfo
{
public:
	CSO2AutoMatchModInfo() = default;
	~CSO2AutoMatchModInfo() override = default;
	bool Unknown00() override;
	bool AssignOptionValues(BaseOptionList* pOptionList, int32_t a3, KeyValues* a4) override;
	//virtual void Unknown02();
	BaseOptionList* CreateOptionList() override;
	bool Unknown04() override;
private:
	std::vector<int32_t> m_vPenaltyLeaveTime; //0x0074
};