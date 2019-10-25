#pragma once

abstract_class ICSO2LoadingSplash
{
public:
#ifndef __MINGW32__
    virtual ~ICSO2LoadingSplash();
#else
    virtual _destructor();
#endif

    virtual void* StartLoadingScreenThread( void* hInstance ) = 0;
    virtual void Unknown00() = 0;  // retn
    virtual void EndLoadingScreenThread() = 0;
    virtual void SetLoadingText( const char* szText ) = 0;
    virtual bool IsShowingLoadingScreen() = 0;
    virtual void SetLoadingProgress( int iProgress ) = 0;
    virtual void InvalidateScreenRect() = 0;
};

PLATFORM_INTERFACE ICSO2LoadingSplash* GetCSO2LoadingSplash();