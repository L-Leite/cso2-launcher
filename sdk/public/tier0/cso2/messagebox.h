#pragma once

PLATFORM_INTERFACE int CSO2FormatMessageBox( LPCSTR lpText, UINT uType, char* Format, ... );
PLATFORM_INTERFACE int CSO2FormatMessageBoxW( LPCWSTR lpText, UINT uType, wchar_t* Format, ... );

PLATFORM_INTERFACE int CSO2MessageBox( HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType, bool bShouldFlash = true );
PLATFORM_INTERFACE int CSO2MessageBoxW( HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType, bool bShouldFlash = true );