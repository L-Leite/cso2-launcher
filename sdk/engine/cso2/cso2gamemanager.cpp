#include "tier0/dbg.h"	   
#include "cso2gamemanager.h"

#include "cso2gameroom.h"

CSO2GameManager* g_pCSO2GameManager = nullptr;

std::shared_ptr<CSO2GameRoom> CSO2GameManager::CreateAndJoinRoom( uint16_t iRoomId )
{
	std::shared_ptr<CSO2GameRoom> pNewRoom = std::make_shared<CSO2GameRoom>( iRoomId );

	if (!pNewRoom)
	{
		return nullptr;
	}

	m_Rooms[iRoomId] = pNewRoom;
	m_iCurrentRoomId = iRoomId;

	return pNewRoom;
}
