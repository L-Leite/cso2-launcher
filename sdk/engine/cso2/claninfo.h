#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct ClanGrade
{											 
	int32_t iLevel;
	int32_t iMinExp;
	int32_t iMaxExp;
	int32_t iBonusExp;
	std::string iGrade;
};

class CClanInfo
{
private:
	std::vector<ClanGrade*> m_vClanGrades;
};
