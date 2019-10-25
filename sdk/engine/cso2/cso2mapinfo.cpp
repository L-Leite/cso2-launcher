#include "cso2mapinfo.h" 

#define UNREF_VAR(var) var

//
// our virtuals won't be used anyway
//
bool CSO2MapInfo::Unknown00()
{
	return true;
}

bool CSO2MapInfo::AssignOptionValues( BaseOptionList* pOptionList, int32_t a3, KeyValues* a4 )
{
	UNREF_VAR( pOptionList );
	UNREF_VAR( a3 );
	UNREF_VAR( a4 );
	return true;
}

BaseOptionList* CSO2MapInfo::CreateOptionList()
{
	return nullptr;
}

bool CSO2MapInfo::Unknown04()
{
	return true;
}
