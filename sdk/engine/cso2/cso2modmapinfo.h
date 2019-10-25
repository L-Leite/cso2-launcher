#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>

struct BaseOptionList;
struct ModOptionList;
class KeyValues;

// size: 0x74
class CSO2ModMapInfo
{
public:
	CSO2ModMapInfo() = default;
	virtual ~CSO2ModMapInfo() = default;
	virtual bool Unknown00() = 0;
	virtual bool AssignOptionValues(BaseOptionList* pOptionList, int32_t a3, KeyValues* a4) = 0;
	virtual void Unknown02();	  
	virtual BaseOptionList* CreateOptionList() = 0;
	virtual bool Unknown04();

	inline std::vector<int32_t>& GetId() { return m_vId; }
	inline std::vector<std::string>& GetName() { return m_vName; }
	inline std::map<std::string, int32_t>& GetOptions() { return m_Options; }
	inline int32_t GetCount() { return m_iCount; }

private:
	std::vector<int32_t> m_vId; //0x0004
	std::vector<int32_t> m_vOrder; //0x0010
	std::vector<int32_t> m_vUse; //0x001C unknown array type
	int32_t m_iCount; //0x0028
	std::vector<int32_t> m_vLeague; //0x002C unknown array type
	int32_t m_Unknown00; //0x0038 same as m_iCount
	std::vector<std::string> m_vName; //0x003C
	std::vector<int32_t> m_Unknown01; //0x0048 unknown array type, it's a struct 0x18 long
	std::vector<int32_t> m_Unknown02; //0x0054
	std::map<std::string, int32_t> m_Options; //0x0060
	std::vector<ModOptionList*> m_vOptionList; //0x0068
};