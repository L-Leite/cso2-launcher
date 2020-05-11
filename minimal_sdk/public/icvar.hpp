#pragma once

#include "appframework/iappsystem.hpp"

class Color;
class ConCommandBase;
class ConCommand;
class ConVar;
class IConsoleDisplayFunc;
class IConVar;
class ICvarQuery;

using CVarDLLIdentifier_t = int;
using FnChangeCallback_t = void ( * )( IConVar* var, const char* pOldValue,
                                       float flOldValue );

constexpr const char* CVAR_INTERFACE_VERSION = "VEngineCvar004";

class ICvar : public IAppSystem
{
public:
    // Allocate a unique DLL identifier
    virtual CVarDLLIdentifier_t AllocateDLLIdentifier() = 0;

    // Register, unregister commands
    virtual void RegisterConCommand( ConCommandBase* pCommandBase ) = 0;
    virtual void UnregisterConCommand( ConCommandBase* pCommandBase ) = 0;
    virtual void UnregisterConCommands( CVarDLLIdentifier_t id ) = 0;

    // If there is a +<varname> <value> on the command line, this returns the
    // value. Otherwise, it returns NULL.
    virtual const char* GetCommandLineValue( const char* pVariableName ) = 0;

    // Try to find the cvar pointer by name
    virtual ConCommandBase* FindCommandBase( const char* name ) = 0;
    virtual const ConCommandBase* FindCommandBase( const char* name ) const = 0;
    virtual ConVar* FindVar( const char* var_name ) = 0;
    virtual const ConVar* FindVar( const char* var_name ) const = 0;
    virtual ConCommand* FindCommand( const char* name ) = 0;
    virtual const ConCommand* FindCommand( const char* name ) const = 0;

    // Get first ConCommandBase to allow iteration
    virtual ConCommandBase* GetCommands( void ) = 0;
    virtual const ConCommandBase* GetCommands( void ) const = 0;

    // Install a global change callback (to be called when any convar changes)
    virtual void InstallGlobalChangeCallback( FnChangeCallback_t callback ) = 0;
    virtual void RemoveGlobalChangeCallback( FnChangeCallback_t callback ) = 0;
    virtual void CallGlobalChangeCallbacks( ConVar* var, const char* pOldString,
                                            float flOldValue ) = 0;

    // Install a console printer
    virtual void InstallConsoleDisplayFunc(
        IConsoleDisplayFunc* pDisplayFunc ) = 0;
    virtual void RemoveConsoleDisplayFunc(
        IConsoleDisplayFunc* pDisplayFunc ) = 0;
    virtual void ConsoleColorPrintf( const Color& clr, const char* pFormat,
                                     ... ) = 0;
    virtual void ConsolePrintf( const char* pFormat, ... ) const = 0;
    virtual void ConsoleDPrintf( const char* pFormat, ... ) const = 0;

    // Reverts cvars which contain a specific flag
    virtual void RevertFlaggedConVars( int nFlag ) = 0;

    // Method allowing the engine ICvarQuery interface to take over
    // A little hacky, owing to the fact the engine is loaded
    // well after ICVar, so we can't use the standard connect pattern
    virtual void InstallCVarQuery( ICvarQuery* pQuery ) = 0;

    virtual bool IsMaterialThreadSetAllowed() const = 0;
    virtual void QueueMaterialThreadSetValue( ConVar* pConVar,
                                              const char* pValue ) = 0;
    virtual void QueueMaterialThreadSetValue( ConVar* pConVar, int nValue ) = 0;
    virtual void QueueMaterialThreadSetValue( ConVar* pConVar,
                                              float flValue ) = 0;
    virtual bool HasQueuedMaterialThreadConVarSets() const = 0;
    virtual int ProcessQueuedMaterialThreadConVarSets() = 0;

protected:
    class ICVarIteratorInternal;

public:
    /// Iteration over all cvars.
    /// (THIS IS A SLOW OPERATION AND YOU SHOULD AVOID IT.)
    /// usage:
    /// { ICVar::Iterator iter(g_pCVar);
    ///   for ( iter.SetFirst() ; iter.IsValid() ; iter.Next() )
    ///   {
    ///       ConCommandBase *cmd = iter.Get();
    ///   }
    /// }
    /// The Iterator class actually wraps the internal factory methods
    /// so you don't need to worry about new/delete -- scope takes care
    //  of it.
    /// We need an iterator like this because we can't simply return a
    /// pointer to the internal data type that contains the cvars --
    /// it's a custom, protected class with unusual semantics and is
    /// prone to change.
    class Iterator
    {
    public:
        inline Iterator( ICvar* icvar );
        inline ~Iterator( void );
        inline void SetFirst( void );
        inline void Next( void );
        inline bool IsValid( void );
        inline ConCommandBase* Get( void );

    private:
        ICVarIteratorInternal* m_pIter;
    };

protected:
    // internals for  ICVarIterator
    class ICVarIteratorInternal
    {
    public:
        // warning: delete called on 'ICvar::ICVarIteratorInternal' that is
        // abstract but has non-virtual destructor [-Wdelete-non-virtual-dtor]
        virtual ~ICVarIteratorInternal() {}
        virtual void SetFirst( void ) = 0;
        virtual void Next( void ) = 0;
        virtual bool IsValid( void ) = 0;
        virtual ConCommandBase* Get( void ) = 0;
    };

    virtual ICVarIteratorInternal* FactoryInternalIterator( void ) = 0;
    friend class Iterator;
};