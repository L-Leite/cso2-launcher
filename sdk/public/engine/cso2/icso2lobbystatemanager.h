#pragma once

abstract_class ICSO2LobbyStateManager
{
public:
	virtual ~ICSO2LobbyStateManager() = default;
	virtual void Unknown00() = 0;
	virtual void Unknown01() = 0;
	virtual bool SetCurrentState(int iNewState) = 0;
	virtual void Unknown02() = 0;
	virtual void Unknown03() = 0;
	virtual void Unknown04() = 0;
	virtual void Unknown05() = 0;
	virtual void Unknown06() = 0;
	virtual void Unknown07() = 0;
	virtual void Unknown08() = 0;
	virtual void Unknown09() = 0;
	virtual void Unknown10() = 0;
	virtual void Unknown11() = 0;
	virtual void Unknown12() = 0;
	virtual void Unknown13() = 0;
	virtual void Unknown14() = 0;
	virtual void Unknown15() = 0;
};

extern ICSO2LobbyStateManager* g_pLobbyStateManager;
