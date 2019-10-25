#pragma once

#include <cstdint>
#include <string>

struct BaseOptionList
{
	BaseOptionList();
	virtual ~BaseOptionList() = default;

	uint32_t m_Unknown00; //0x0004
	uint32_t m_Unknown01; //0x0008
};

struct ModOptionList : public BaseOptionList
{
	ModOptionList();
	~ModOptionList() override = default;		

	uint32_t m_Unknown02; //0x000C
	uint32_t m_Unknown03; //0x0010
	uint16_t m_Unknown04; //0x0014
	std::string m_szTeamNameCt; //0x0018
	std::string m_szTeamNameTr; //0x0030
	std::string m_szGamemodeDescription; //0x0048
	uint32_t m_Unknown05; //0x0060
	uint8_t m_Unknown06; //0x0064
};

struct AutoMatchOptionList : public BaseOptionList
{
	AutoMatchOptionList();
	~AutoMatchOptionList() override = default;

	std::string m_Unknown02;
	uint32_t m_Unknown03;
};
