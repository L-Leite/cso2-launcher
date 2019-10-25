#pragma once

#include <tier0/platform.h>

#define CSO2ERRORLOG_MAX_INDEX 4

class CSO2DocumentLog
{	
public:				 
	DLL_CLASS_IMPORT CSO2DocumentLog();
	DLL_CLASS_IMPORT ~CSO2DocumentLog();

	DLL_CLASS_IMPORT void Create();
	DLL_CLASS_IMPORT void AddMsg( int unused, const char* szFormat, ... );
	DLL_CLASS_IMPORT CSO2DocumentLog& operator=( const CSO2DocumentLog& other );

private:
	bool m_bInitialized;
	char m_szBuffer[512];
};

PLATFORM_INTERFACE CSO2DocumentLog g_CSO2DocLog;

class CSO2ErrorLog
{
public:
	DLL_CLASS_IMPORT CSO2ErrorLog();
	DLL_CLASS_IMPORT ~CSO2ErrorLog();

	DLL_CLASS_IMPORT void AddErrorID( unsigned int iErrorID );
	DLL_CLASS_IMPORT unsigned int GetErrorID();

	DLL_CLASS_IMPORT void AddMsg( const char* szFormat, ... );
	DLL_CLASS_IMPORT const char* GetMsg( unsigned int iMsgIndex );
	DLL_CLASS_IMPORT const wchar_t* GetWMsg( unsigned int iMsgIndex );

	DLL_CLASS_IMPORT CSO2ErrorLog& operator=( const CSO2ErrorLog& other );

private:
	unsigned int m_iErrorID;
	unsigned int m_iCurrentMsgIndex;
	char* m_szMessages[CSO2ERRORLOG_MAX_INDEX];	 	
};

PLATFORM_INTERFACE CSO2ErrorLog g_CSO2ErrorLog;

typedef unsigned long long usersn_t;

// half of this class isnt reversed since i dont need it, maybe used in kr?
abstract_class ICSO2ClientLogWriter
{
public:
	virtual ~ICSO2ClientLogWriter();

	virtual bool LoadGameLogManager( const char* szLogModuleName ) = 0;
	virtual int InitializeGameLogManagerA( int a1, int a2, int a3 ) = 0;
	virtual int WriteStageLogA( int a1, int a2 ) = 0;
	virtual int WriteErrorLogA( int a1, int a2 ) = 0;
	virtual int WriteLogA( int a1, int a2 ) = 0; // not in nxgsm.dll
	virtual int WriteLogOncePerKeyA( int a1, int a2 ) = 0; // not in nxgsm.dll
	virtual int GetSessionIDA( int a1, int a2 ) = 0;
	virtual void SetUserSN( usersn_t iSessionSN ) = 0;
	virtual void FinalizeGameLogManager() = 0;
};

PLATFORM_INTERFACE ICSO2ClientLogWriter* GetCSO2ClientLogWriter();

PLATFORM_INTERFACE void CSO2_WriteStageLog( int iStage, const char* szString );
