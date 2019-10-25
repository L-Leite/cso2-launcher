#include "cso2modinfo.h" 

#define UNREF_VAR(var) var

CSO2ModInfo::CSO2ModInfo()
{
	m_Unknown07 = nullptr;
	m_Unknown10 = 0;
	m_Unknown12 = 0;
	m_Unknown13 = 0;
	m_Unknown17 = 0;
}

//
// our virtuals won't be used anyway
//
bool CSO2ModInfo::Unknown00()
{
	return true;
}

bool CSO2ModInfo::AssignOptionValues( BaseOptionList* pOptionList, int32_t a3, KeyValues* a4 )
{
	UNREF_VAR( pOptionList );
	UNREF_VAR( a3 );
	UNREF_VAR( a4 );
	return true;
}

BaseOptionList* CSO2ModInfo::CreateOptionList()
{
	return nullptr;
}

bool CSO2ModInfo::Unknown04()
{
	return true;
}
