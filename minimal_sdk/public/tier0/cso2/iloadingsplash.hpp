#pragma once

class ICSO2LoadingSplash
{
public:
    virtual ~ICSO2LoadingSplash();

    virtual void* StartSplashThread( void* hInstance ) = 0;
    virtual void Unknown00() = 0;  // retn
    virtual void EndSplashThread() = 0;
    virtual void SetLoadingText( const char* szText ) = 0;
    virtual bool IsVisible() = 0;
    virtual void SetLoadingProgress( int iProgress ) = 0;
    virtual void InvalidateScreenRect() = 0;
};

ICSO2LoadingSplash* GetCSO2LoadingSplash();