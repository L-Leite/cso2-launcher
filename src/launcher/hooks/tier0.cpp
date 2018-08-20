#include "strtools.h"
#include "hooks.h"
#include "tier0/icommandline.h"
#include "tier0/memdbgon.h"

extern bool g_bPrintDebugInfo;

HOOK_EXPORT_DECLARE(hkCOM_TimestampedLog);

NOINLINE void hkCOM_TimestampedLog(char const *fmt, ...)
{
	static float s_LastStamp = 0.0;
	static bool s_bShouldLog = false;
	static bool s_bChecked = false;
	static bool	s_bFirstWrite = false;

	if (!g_bPrintDebugInfo)
		return;

	if (!s_bChecked)
	{
		s_bShouldLog = (IsX360() || CommandLine()->CheckParm("-profile")) ? true : false;
		s_bChecked = true;
	}
	/*if ( !s_bShouldLog )
	{
	return;
	}*/

	char string[1024];
	va_list argptr;
	va_start(argptr, fmt);
	_vsnprintf(string, sizeof(string), fmt, argptr);
	va_end(argptr);

	float curStamp = Plat_FloatTime();

#if defined( _X360 )
	XBX_rTimeStampLog(curStamp, string);
#endif

	if (IsPC())
	{
		if (!s_bFirstWrite)
		{
			unlink("timestamped.log");
			s_bFirstWrite = true;
		}

		FILE* fp = fopen("timestamped.log", "at+");
		fprintf(fp, "%8.4f / %8.4f:  %s\n", curStamp, curStamp - s_LastStamp, string);
		fclose(fp);
	}

	s_LastStamp = curStamp;
}

HOOK_EXPORT_DECLARE(hkMsg);

static char s_szMsgBuffer[512] = { 0 };

NOINLINE void hkMsg(const tchar* pMsg, ...)
{
	if (!g_bPrintDebugInfo)
		return;

	size_t iMsgLength = strlen(pMsg);
	assert((iMsgLength + 1) <= sizeof(s_szMsgBuffer));

	if (pMsg[iMsgLength - 1] != '\n')
	{	
		V_strcpy(s_szMsgBuffer, pMsg);
		s_szMsgBuffer[iMsgLength] = '\n';
		s_szMsgBuffer[iMsgLength + 1] = '\0';
		pMsg = s_szMsgBuffer;
	}
	
	va_list va;
	va_start(va, pMsg);
	vprintf(pMsg, va);
	va_end(va);
}

HOOK_EXPORT_DECLARE(hkWarning);

NOINLINE void hkWarning(const tchar* pMsg, ...)
{
	if (!g_bPrintDebugInfo)
		return;

	size_t iMsgLength = strlen(pMsg);
	assert((iMsgLength + 1) <= sizeof(s_szMsgBuffer));

	if (pMsg[iMsgLength - 1] != '\n')
	{
		V_strcpy(s_szMsgBuffer, pMsg);
		s_szMsgBuffer[iMsgLength] = '\n';
		s_szMsgBuffer[iMsgLength + 1] = '\0';
		pMsg = s_szMsgBuffer;
	}

	va_list va;
	va_start(va, pMsg);
	vprintf(pMsg, va);
	va_end(va);
}

void HookTier0()
{						
	HOOK_EXPORT(L"tier0.dll", "COM_TimestampedLog", hkMsg);
	HOOK_EXPORT(L"tier0.dll", "Msg", hkCOM_TimestampedLog);
	HOOK_EXPORT(L"tier0.dll", "Warning", hkWarning);
}