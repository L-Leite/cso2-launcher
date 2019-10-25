#pragma once

#include "engine/cso2/icso2gamemanager.h"

#include "cso2automatchmodinfo.h"
#include "cso2mapinfo.h"
#include "cso2modinfo.h"
#include "clanbattle.h"
#include "claninfo.h"
#include "itemoptioninfo.h"

class CSO2GameRoom;

// size: 0x4AC
class CSO2GameManager : public ICSO2GameManager
{
public:
	CSO2GameManager() = default;			   
	~CSO2GameManager() override = default;

	std::shared_ptr<CSO2GameRoom> CreateAndJoinRoom(uint16_t iRoomId);

	inline CSO2ModInfo* GetModInfo() { return &m_ModInfo; }
	inline CSO2MapInfo* GetMapInfo() { return &m_MapInfo; }

private:
	uint8_t Pad00[36]; //0x0004
	std::string m_Unknown00; //0x0028
	uint8_t Pad01[28]; //0x0040
	std::map<int32_t, int32_t> m_Unknown01; //0x005C unknown type
	uint8_t Pad02[12]; //0x0064
	CSO2ModInfo m_ModInfo; //0x0070
	CSO2AutoMatchModInfo m_AutoMatchModInfo; //0x0184
	CSO2MapInfo m_MapInfo; //0x0204
	uint8_t Pad03[20]; //0x02A4
	CItemOptionInfo m_ItemOptionInfo; //0x02B8
	CClanBattle m_ClanBattle; //0x02D4
	CClanInfo m_ClanInfo; //0x02DC
	uint32_t m_iCurrentRoomId; //0x02E8	// ok
	std::map<uint32_t, std::shared_ptr<CSO2GameRoom>> m_Rooms; //0x02EC
	uint8_t Pad04[4]; //0x02F4
	std::string m_Unknown03; //0x02F8
	uint8_t Pad05[412]; //0x0310
};

extern CSO2GameManager* g_pCSO2GameManager;
