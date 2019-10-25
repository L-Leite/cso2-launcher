#include "engine/cso2/icso2gameroom.h"
#include <map>

// size: 0xE8
class CSO2GameRoom : public ICSO2GameRoom
{
public:
	CSO2GameRoom(uint16_t iRoomId);
	~CSO2GameRoom() override = default;

	uint16_t GetId() override;
	uint64_t GetFlags() override;
	const char* GetName() override;
	uint8_t GetUnknown00() override;
	uint8_t GetUnknown01() override;
	const char* GetSomething() override;
	uint16_t GetUnknown02() override;
	uint8_t GetGameModeId() override;
	void SetMapId(uint8_t iMapId) override;
	uint8_t GetMapId() override;
	uint8_t GetUnknown03() override;
	int8_t GetPlayersNum() override;
	int8_t GetWinLimit() override;
	int16_t GetKillLimit() override;
	int8_t GetForceCamera() override;
	bool ArePlayerMarkersEnabled() override;
	uint8_t GetUnknown04() override;
	uint32_t GetUnknown05() override;
	uint8_t GetUnknown06() override;
	uint8_t GetUnknown07() override;
	uint32_t GetUnknown08() override;
	uint16_t GetUnknown09() override;
	uint16_t GetUnknown10() override;
	uint32_t GetUnknown11() override;
	uint16_t GetUnknown12() override;
	uint16_t GetUnknown13() override;
	uint8_t GetUnknown14() override;
	uint8_t GetUnknown15() override;
	uint32_t GetUnknown16() override;
	uint32_t GetUnknown17() override;
	uint32_t GetUnknown18() override;
	uint32_t GetUnknown19() override;
	uint8_t GetUnknown20() override;
	uint8_t GetUnknown21() override;
	uint8_t GetUnknown22() override;
	uint16_t GetUnknown23() override;
	uint16_t GetUnknown24() override;
	uint8_t GetUnknown25() override;
	uint32_t GetUnknown29() override;
	uint8_t GetUnknown30() override;
	bool AreBotsEnabled() override;
	uint8_t GetUnknown31() override;
	int8_t GetNumCtBots() override;
	int8_t GetNumTrBots() override;
	uint64_t GetUnknown34() override;
	const char* GetSomething2() override;
	uint8_t GetUnknown32() override;
	uint8_t GetUnknown35() override;
	int8_t GetTeamChange() override;
	int32_t GetNextMapEnabled() override;
	uint8_t GetUnknown37() override;
	uint8_t GetUnknown38() override;
	uint8_t GetUnknown39() override;
	uint32_t SomethingToDoWith40and41() override;
	uint8_t GetUnknown43() override;
	uint8_t GetUnknown44() override;
	uint8_t& AddToUnknown45(uint8_t _Keyval, uint8_t& _Mapval) override;	// these two might be wrong
	uint8_t& GetUnknown45(uint8_t _Keyval) override;
	void SetUnknown30(uint8_t iUnknown30) override;
	uint32_t SomethingToDoWith45() override;
	uint8_t GetUnknown36() override;
	uint8_t GetUnknown47() override;
	uint8_t GetUnknown48() override;
	uint8_t GetUnknown46() override;
	bool IsBroadcastEnabled() override;
	int8_t GetDifficulty() override;
	void SetEasyDifficulty() override;
	uint8_t GetUnknown49() override;
	void NullifyUnknown49() override;
	bool IsGamemodeSomething() override;
	bool IsGamemodeSomething2() override;
	bool IsGamemodeSomething3() override;
	bool IsPractice() override;
	bool IsGamemodeSomething4() override;
	virtual uint8_t GetUnknown26();
	virtual uint8_t GetUnknown27();

	void SetupRoomRules(uint8_t iGameModeId, uint8_t iWinLimit, uint8_t iKillLimit);
	void SetBots(int8_t iCtBots, int8_t iTrBots);
	inline void SetRoomName(const char* szRoomName) { m_szRoomName = szRoomName; }
	inline void SetPlayersNum(int8_t iPlayersNum) { m_iPlayersNum = iPlayersNum; }

private:
	uint16_t m_iRoomId;	// 0x04
	uint64_t m_dwFlags;	// 0x08
	uint8_t m_Unknown00; // 0x10
	uint8_t m_Unknown01; // 0x11
	uint16_t m_Unknown02; // 0x12
	uint8_t m_iGameModeId; // 0x14
	uint8_t m_iMapId; // 0x15
	uint8_t m_Unknown03; // 0x16
	int8_t m_iPlayersNum; // 0x17
	int8_t m_iWinLimit; // 0x18
						// 1 byte pad here
	int16_t m_iKillLimit; // 0x1A
	int8_t m_iForceCamera; // 0x1C
	bool m_bEnablePlayerMarkers; // 0x1D
	uint8_t m_Unknown04; // 0x1E
						 // 1 byte pad
	uint32_t m_Unknown05; // 0x20
	uint8_t m_Unknown06; // 0x24
	uint8_t m_Unknown07; // 0x25
						 // 2 byte pad
	uint32_t m_Unknown08; // 0x28
	uint16_t m_Unknown09; // 0x2C
	uint16_t m_Unknown10; // 0x2E
	uint32_t m_Unknown11; // 0x30
	uint16_t m_Unknown12; // 0x34
	uint16_t m_Unknown13; // 0x36
	uint8_t m_Unknown14; // 0x38
	uint8_t m_Unknown15; // 0x39
	uint32_t m_Unknown16; // 0x3C
	uint32_t m_Unknown17; // 0x40
	uint32_t m_Unknown18; // 0x44
	uint32_t m_Unknown19; // 0x48
	uint8_t m_Unknown20; // 0x4C
	uint8_t m_Unknown21; // 0x4D
	uint8_t m_Unknown22; // 0x4E
	uint16_t m_Unknown23; // 0x50
	uint8_t m_Unknown24; // 0x52
	uint8_t m_Unknown25; // 0x53
	uint8_t m_Unknown26; // 0x54
	uint8_t m_Unknown27; // 0x55
	uint8_t m_Unknown28; // 0x56
	uint32_t m_Unknown29; // 0x58
	uint8_t m_Unknown30; // 0x5C
	bool m_bBotsEnabled; // 0x005D
	uint8_t m_Unknown31; //0x005E
	int8_t m_iCtBots; //0x005F
	int8_t m_iTrBots; //0x0060
	uint8_t m_Unknown32; // 0x61
	uint32_t m_Unknown33; // 0x64
	uint64_t m_Unknown34; // 0x68
	uint8_t m_Unknown35; // 0x70
	int8_t m_iTeamChange; //0x0071
	uint8_t m_Unknown36; //0x0072
	int32_t m_iNextMapEnabled; //0x0074
	uint8_t m_Unknown37; // 0x78
	uint8_t m_Unknown38; // 0x79
	uint8_t m_Unknown39; // 0x7A
	uint32_t m_Unknown40; // 0x7C
	uint32_t m_Unknown41; // 0x80
	uint32_t m_Unknown42; // 0x84
	uint8_t m_Unknown43; // 0x88
	uint8_t m_Unknown44; // 0x89
	std::map<uint8_t, uint8_t> m_Unknown45; // 0x8C
	uint8_t m_Unknown46; // 0x94
	bool m_bBroadcastEnabled; //0x0095
	uint8_t m_Unknown47; // 0x96
	uint8_t m_Unknown48; // 0x97
	int8_t m_iDifficulty; //0x0098
	uint8_t m_Unknown49; //0x0099
						 // 2 byte pad
	std::string m_szRoomName; //0x009C
	std::string m_szSomething; //0x00B4
	std::string m_szSomething2; //0x00CC
	uint32_t m_Unknown50; //0x00E4
};