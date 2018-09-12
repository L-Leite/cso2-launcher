#include "hooks.h"
#include "strtools.h"
#include "tier0/icommandline.h"

#include <fstream>
#include <filesystem>				

extern bool g_bPrintMoreDebugInfo;

HOOK_EXPORT_DECLARE( hkCOM_TimestampedLog );

NOINLINE void hkCOM_TimestampedLog( char const *fmt, ... )
{
	static float s_LastStamp = 0.0;
	static bool	s_bFirstWrite = false;

	if (!g_bPrintMoreDebugInfo)
	{
		return;
	}

	char string[1024];
	va_list argptr;
	va_start( argptr, fmt );
	_vsnprintf( string, sizeof( string ), fmt, argptr );
	va_end( argptr );

	float curStamp = Plat_FloatTime();

	if (!s_bFirstWrite)
	{
		std::filesystem::remove( std::filesystem::current_path() / "timestamped.log" );
		s_bFirstWrite = true;
	}

	std::ofstream logStream( "timestamped.log" );

	if (logStream.good())
	{
		logStream << std::setw( 8 ) << std::setprecision( 4 )
			<< curStamp << curStamp - s_LastStamp << string << '\n';
	}

	s_LastStamp = curStamp;
}

HOOK_EXPORT_DECLARE( hkMsg );

NOINLINE void hkMsg( const tchar* pMsg, ... )
{
	if (!g_bPrintMoreDebugInfo)
	{
		return;
	}

	va_list va;
	va_start( va, pMsg );
	vprintf( pMsg, va );
	va_end( va );
}

HOOK_EXPORT_DECLARE( hkWarning );

NOINLINE void hkWarning( const tchar* pMsg, ... )
{
	if (!g_bPrintMoreDebugInfo)
	{
		return;
	}

	va_list va;
	va_start( va, pMsg );
	vprintf( pMsg, va );
	va_end( va );
}

void HookTier0()
{
	HOOK_EXPORT( L"tier0.dll", "COM_TimestampedLog", hkMsg );
	HOOK_EXPORT( L"tier0.dll", "Msg", hkCOM_TimestampedLog );
	HOOK_EXPORT( L"tier0.dll", "Warning", hkWarning );
}
