#pragma once

#include "tier0/dbg.h"
#include <cstdint>

abstract_class ICSO2GameRoom
{
public:
	virtual ~ICSO2GameRoom() = default;
	virtual uint16_t GetId() = 0;
	virtual uint64_t GetFlags() = 0;
	virtual const char* GetName() = 0;
	virtual uint8_t GetUnknown00() = 0;
	virtual uint8_t GetUnknown01() = 0;
	virtual const char* GetSomething() = 0;
	virtual uint16_t GetUnknown02() = 0;
	virtual uint8_t GetGameModeId() = 0;
	virtual void SetMapId(uint8_t iMapId) = 0;
	virtual uint8_t GetMapId() = 0;
	virtual uint8_t GetUnknown03() = 0;
	virtual int8_t GetPlayersNum() = 0;
	virtual int8_t GetWinLimit() = 0;
	virtual int16_t GetKillLimit() = 0;
	virtual int8_t GetForceCamera() = 0;
	virtual bool ArePlayerMarkersEnabled() = 0;
	virtual uint8_t GetUnknown04() = 0;
	virtual uint32_t GetUnknown05() = 0;
	virtual uint8_t GetUnknown06() = 0;
	virtual uint8_t GetUnknown07() = 0;
	virtual uint32_t GetUnknown08() = 0;
	virtual uint16_t GetUnknown09() = 0;
	virtual uint16_t GetUnknown10() = 0;
	virtual uint32_t GetUnknown11() = 0;
	virtual uint16_t GetUnknown12() = 0;
	virtual uint16_t GetUnknown13() = 0;
	virtual uint8_t GetUnknown14() = 0;
	virtual uint8_t GetUnknown15() = 0;
	virtual uint32_t GetUnknown16() = 0;
	virtual uint32_t GetUnknown17() = 0;
	virtual uint32_t GetUnknown18() = 0;
	virtual uint32_t GetUnknown19() = 0;
	virtual uint8_t GetUnknown20() = 0;
	virtual uint8_t GetUnknown21() = 0;
	virtual uint8_t GetUnknown22() = 0;
	virtual uint16_t GetUnknown23() = 0;
	virtual uint16_t GetUnknown24() = 0;
	virtual uint8_t GetUnknown25() = 0;
	virtual uint32_t GetUnknown29() = 0;
	virtual uint8_t GetUnknown30() = 0;
	virtual bool AreBotsEnabled() = 0;
	virtual uint8_t GetUnknown31() = 0;
	virtual int8_t GetNumCtBots() = 0;
	virtual int8_t GetNumTrBots() = 0;
	virtual uint64_t GetUnknown34() = 0;
	virtual const char* GetSomething2() = 0;
	virtual uint8_t GetUnknown32() = 0;
	virtual uint8_t GetUnknown35() = 0;
	virtual int8_t GetTeamChange() = 0;
	virtual int32_t GetNextMapEnabled() = 0;
	virtual uint8_t GetUnknown37() = 0;
	virtual uint8_t GetUnknown38() = 0;
	virtual uint8_t GetUnknown39() = 0;
	virtual uint32_t SomethingToDoWith40and41() = 0;
	virtual uint8_t GetUnknown43() = 0;
	virtual uint8_t GetUnknown44() = 0;
	virtual uint8_t& AddToUnknown45(uint8_t _Keyval, uint8_t& _Mapval) = 0;	// these two might be wrong
	virtual uint8_t& GetUnknown45(uint8_t _Keyval) = 0;
	virtual void SetUnknown30(uint8_t iUnknown30) = 0;
	virtual uint32_t SomethingToDoWith45() = 0;
	virtual uint8_t GetUnknown36() = 0;
	virtual uint8_t GetUnknown47() = 0;
	virtual uint8_t GetUnknown48() = 0;
	virtual uint8_t GetUnknown46() = 0;
	virtual bool IsBroadcastEnabled() = 0;
	virtual int8_t GetDifficulty() = 0;
	virtual void SetEasyDifficulty() = 0;
	virtual uint8_t GetUnknown49() = 0;
	virtual void NullifyUnknown49() = 0;
	virtual bool IsGamemodeSomething() = 0;
	virtual bool IsGamemodeSomething2() = 0;
	virtual bool IsGamemodeSomething3() = 0;
	virtual bool IsPractice() = 0;
	virtual bool IsGamemodeSomething4() = 0;
};