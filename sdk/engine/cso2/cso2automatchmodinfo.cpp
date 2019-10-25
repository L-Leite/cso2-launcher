#include "cso2automatchmodinfo.h" 

#define UNREF_VAR(var) var

//
// our virtuals won't be used anyway
//
bool CSO2AutoMatchModInfo::Unknown00()
{
	return true;
}

bool CSO2AutoMatchModInfo::AssignOptionValues(BaseOptionList* pOptionList, int32_t a3, KeyValues* a4)
{
	UNREF_VAR(pOptionList);
	UNREF_VAR(a3);
	UNREF_VAR(a4);
	return true;
}

BaseOptionList* CSO2AutoMatchModInfo::CreateOptionList()
{
	return nullptr;
}

bool CSO2AutoMatchModInfo::Unknown04()
{
	return true;
}
