#include "strtools.h"
#include "hooks.h"
#include "tier0/icommandline.h"
#include "tier0/memdbgon.h"

extern bool g_bPrintMoreDebugInfo;

HOOK_EXPORT_DECLARE(hkCOM_TimestampedLog);

NOINLINE void hkCOM_TimestampedLog(char const *fmt, ...)
{
	static float s_LastStamp = 0.0;
	static bool s_bShouldLog = false;
	static bool s_bChecked = false;
	static bool	s_bFirstWrite = false;

	if (!g_bPrintMoreDebugInfo)
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
	if (!g_bPrintMoreDebugInfo)
		return;
	
	va_list va;
	va_start(va, pMsg);
	vprintf(pMsg, va);
	va_end(va);
}

HOOK_EXPORT_DECLARE(hkWarning);

NOINLINE void hkWarning(const tchar* pMsg, ...)
{
	if (!g_bPrintMoreDebugInfo)
		return;	   

	va_list va;
	va_start(va, pMsg);
	vprintf(pMsg, va);
	va_end(va);
}

void BytePatchTier(const uintptr_t dwTierBase)
{
	//
	// disable hard coded command line argument check
	//
	// jmp near 0x1D8 bytes forward
	uint8_t addArgPatch[] = { 0xE9, 0xD8, 0x01, 0x00, 0x00 };
	WriteProtectedMemory(addArgPatch, (dwTierBase + 0x1D63));
}

void HookTier0()
{	
	const uintptr_t dwTierBase = g_ModuleList.Get("tier0.dll");
	BytePatchTier(dwTierBase);

	HOOK_EXPORT(L"tier0.dll", "COM_TimestampedLog", hkMsg);
	HOOK_EXPORT(L"tier0.dll", "Msg", hkCOM_TimestampedLog);
	HOOK_EXPORT(L"tier0.dll", "Warning", hkWarning);
}