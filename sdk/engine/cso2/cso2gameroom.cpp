#include "cso2gameroom.h"
#include "modulelist.h"

CSO2GameRoom::CSO2GameRoom( uint16_t iRoomId )
{
	using fn_t = void( __thiscall* )(CSO2GameRoom*, uint16_t);
	static fn_t pFn = nullptr;

	if (!pFn)
	{
		// points to the address of CSO2GameRoom constructor
		// we need this so we get the game's vftable
		pFn = reinterpret_cast<fn_t>(g_ModuleList.Get( "engine.dll" ) + 0x2D92A0);
	}

	pFn( this, iRoomId );
}

void CSO2GameRoom::SetupRoomRules( uint8_t iGameModeId, uint8_t iWinLimit, uint8_t iKillLimit )
{
	m_iGameModeId = iGameModeId;
	m_iWinLimit = iWinLimit;
	m_iKillLimit = iKillLimit;
	m_iForceCamera = 1;
	m_bEnablePlayerMarkers = true;
}

void CSO2GameRoom::SetBots( int8_t iCtBots, int8_t iTrBots )
{
	m_bBotsEnabled = true;
	m_iCtBots = iCtBots;
	m_iTrBots = iTrBots;
}

// our virtuals wont be used
uint16_t CSO2GameRoom::GetId()
{
	return 0;
}

uint64_t CSO2GameRoom::GetFlags()
{
	return 0;
}

const char* CSO2GameRoom::GetName()
{
	return nullptr;
}

uint8_t CSO2GameRoom::GetUnknown00()
{
	return 0;
}

uint8_t CSO2GameRoom::GetUnknown01()
{
	return 0;
}

const char* CSO2GameRoom::GetSomething()
{
	return nullptr;
}

uint16_t CSO2GameRoom::GetUnknown02()
{
	return 0;
}

uint8_t CSO2GameRoom::GetGameModeId()
{
	return 0;
}

void CSO2GameRoom::SetMapId( uint8_t iMapId )
{
}

uint8_t CSO2GameRoom::GetMapId()
{
	return 0;
}

uint8_t CSO2GameRoom::GetUnknown03()
{
	return 0;
}

int8_t CSO2GameRoom::GetPlayersNum()
{
	return 0;
}

int8_t CSO2GameRoom::GetWinLimit()
{
	return 0;
}

int16_t CSO2GameRoom::GetKillLimit()
{
	return 0;
}

int8_t CSO2GameRoom::GetForceCamera()
{
	return 0;
}

bool CSO2GameRoom::ArePlayerMarkersEnabled()
{
	return false;
}

uint8_t CSO2GameRoom::GetUnknown04()
{
	return 0;
}

uint32_t CSO2GameRoom::GetUnknown05()
{
	return 0;
}

uint8_t CSO2GameRoom::GetUnknown06()
{
	return 0;
}

uint8_t CSO2GameRoom::GetUnknown07()
{
	return 0;
}

uint32_t CSO2GameRoom::GetUnknown08()
{
	return 0;
}

uint16_t CSO2GameRoom::GetUnknown09()
{
	return 0;
}

uint16_t CSO2GameRoom::GetUnknown10()
{
	return 0;
}

uint32_t CSO2GameRoom::GetUnknown11()
{
	return 0;
}

uint16_t CSO2GameRoom::GetUnknown12()
{
	return 0;
}

uint16_t CSO2GameRoom::GetUnknown13()
{
	return 0;
}

uint8_t CSO2GameRoom::GetUnknown14()
{
	return 0;
}

uint8_t CSO2GameRoom::GetUnknown15()
{
	return 0;
}

uint32_t CSO2GameRoom::GetUnknown16()
{
	return 0;
}

uint32_t CSO2GameRoom::GetUnknown17()
{
	return 0;
}

uint32_t CSO2GameRoom::GetUnknown18()
{
	return 0;
}

uint32_t CSO2GameRoom::GetUnknown19()
{
	return 0;
}

uint8_t CSO2GameRoom::GetUnknown20()
{
	return 0;
}

uint8_t CSO2GameRoom::GetUnknown21()
{
	return 0;
}

uint8_t CSO2GameRoom::GetUnknown22()
{
	return 0;
}

uint16_t CSO2GameRoom::GetUnknown23()
{
	return 0;
}

uint16_t CSO2GameRoom::GetUnknown24()
{
	return 0;
}

uint8_t CSO2GameRoom::GetUnknown25()
{
	return 0;
}

uint32_t CSO2GameRoom::GetUnknown29()
{
	return 0;
}

uint8_t CSO2GameRoom::GetUnknown30()
{
	return 0;
}

bool CSO2GameRoom::AreBotsEnabled()
{
	return false;
}

uint8_t CSO2GameRoom::GetUnknown31()
{
	return 0;
}

int8_t CSO2GameRoom::GetNumCtBots()
{
	return 0;
}

int8_t CSO2GameRoom::GetNumTrBots()
{
	return 0;
}

uint64_t CSO2GameRoom::GetUnknown34()
{
	return 0;
}

const char* CSO2GameRoom::GetSomething2()
{
	return nullptr;
}

uint8_t CSO2GameRoom::GetUnknown32()
{
	return 0;
}

uint8_t CSO2GameRoom::GetUnknown35()
{
	return 0;
}

int8_t CSO2GameRoom::GetTeamChange()
{
	return 0;
}

int32_t CSO2GameRoom::GetNextMapEnabled()
{
	return 0;
}

uint8_t CSO2GameRoom::GetUnknown37()
{
	return 0;
}

uint8_t CSO2GameRoom::GetUnknown38()
{
	return 0;
}

uint8_t CSO2GameRoom::GetUnknown39()
{
	return 0;
}

uint32_t CSO2GameRoom::SomethingToDoWith40and41()
{
	return 0;
}

uint8_t CSO2GameRoom::GetUnknown43()
{
	return 0;
}

uint8_t CSO2GameRoom::GetUnknown44()
{
	return 0;
}

uint8_t& CSO2GameRoom::AddToUnknown45( uint8_t _Keyval, uint8_t& _Mapval )
{
	static uint8_t res = 0;
	return res;
}

uint8_t& CSO2GameRoom::GetUnknown45( uint8_t _Keyval )
{
	static uint8_t res = 0;
	return res;
}

void CSO2GameRoom::SetUnknown30( uint8_t iUnknown30 )
{
}

uint32_t CSO2GameRoom::SomethingToDoWith45()
{
	return 0;
}

uint8_t CSO2GameRoom::GetUnknown36()
{
	return 0;
}

uint8_t CSO2GameRoom::GetUnknown47()
{
	return 0;
}

uint8_t CSO2GameRoom::GetUnknown48()
{
	return 0;
}

uint8_t CSO2GameRoom::GetUnknown46()
{
	return 0;
}

bool CSO2GameRoom::IsBroadcastEnabled()
{
	return false;
}

int8_t CSO2GameRoom::GetDifficulty()
{
	return 0;
}

void CSO2GameRoom::SetEasyDifficulty()
{
}

uint8_t CSO2GameRoom::GetUnknown49()
{
	return 0;
}

void CSO2GameRoom::NullifyUnknown49()
{
}

bool CSO2GameRoom::IsGamemodeSomething()
{
	return false;
}

bool CSO2GameRoom::IsGamemodeSomething2()
{
	return false;
}

bool CSO2GameRoom::IsGamemodeSomething3()
{
	return false;
}

bool CSO2GameRoom::IsPractice()
{
	return false;
}

bool CSO2GameRoom::IsGamemodeSomething4()
{
	return false;
}

uint8_t CSO2GameRoom::GetUnknown26()
{
	return 0;
}

uint8_t CSO2GameRoom::GetUnknown27()
{
	return 0;
}

