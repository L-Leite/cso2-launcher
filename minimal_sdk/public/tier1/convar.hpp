#pragma once

//
// NOTE: these concommand/convar definitions do not register themselves
// into the commands table when created
//

#include <stdexcept>

#include "containers/utlstring.hpp"
#include "containers/utlvector.hpp"
#include "icvar.hpp"
#include "tier1/iconvar.hpp"

class CCommand;
class ICommandCallback;
class ICommandCompletionCallback;

using FnCommandCallbackVoid_t = void ( * )();
using FnCommandCallback_t = void ( * )( const CCommand& command );

#define COMMAND_COMPLETION_MAXITEMS 64
#define COMMAND_COMPLETION_ITEM_LENGTH 64

using FnCommandCompletionCallback = int ( * )(
    const char* partial, char commands[COMMAND_COMPLETION_MAXITEMS]
                                      [COMMAND_COMPLETION_ITEM_LENGTH] );

class ConCommandBase
{
public:
    ConCommandBase()
        : m_pNext( nullptr ), m_bRegistered( false ), m_pszName( nullptr ),
          m_pszHelpString( nullptr ), m_nFlags( FCVAR_NONE )
    {
        throw std::runtime_error(
            "ConCommandBase's constructor is not implemented" );
    }

    virtual ~ConCommandBase() {}

    virtual bool IsCommand() const { return true; }

    virtual bool IsFlagSet( int flag ) const
    {
        return ( flag & this->m_nFlags ) ? true : false;
    }

    virtual void AddFlags( int flags ) { m_nFlags |= flags; }
    inline void RemoveFlags( int flags ) { m_nFlags &= ~flags; }

    virtual const char* GetName() const { return m_pszName; }
    virtual const char* GetHelpText() const { return m_pszHelpString; }

    const ConCommandBase* GetNext() const { return m_pNext; }
    ConCommandBase* GetNext() { return m_pNext; }

    virtual bool IsRegistered() const { return m_bRegistered; }

    virtual CVarDLLIdentifier_t GetDLLIdentifier() const { return -1; }

protected:
    virtual void CreateBase( const char* pName,
                             const char* pHelpString = nullptr, int flags = 0 )
    {
    }

    virtual void Init() {}

protected:
    // Next ConVar in chain
    // Prior to register, it points to the next convar in the DLL.
    // Once registered, though, m_pNext is reset to point to the next
    // convar in the global list
    ConCommandBase* m_pNext;

    // Has the cvar been added to the global list?
    bool m_bRegistered;

    // Static data
    const char* m_pszName;
    const char* m_pszHelpString;

    // ConVar flags
    int m_nFlags;
};

class ConCommand : public ConCommandBase
{
public:
    typedef ConCommandBase BaseClass;

    virtual ~ConCommand(){};

    virtual bool IsCommand() const { return true; };

    virtual int AutoCompleteSuggest( const char* partial,
                                     CUtlVector<CUtlString>& commands )
    {
        return 0;
    }

    virtual bool CanAutoComplete() { return false; }

    virtual void Dispatch( const CCommand& command ) {}

private:
    // Call this function when executing the command
    union {
        FnCommandCallbackVoid_t m_fnCommandCallbackV1;
        FnCommandCallback_t m_fnCommandCallback;
        ICommandCallback* m_pCommandCallback;
    };

    union {
        FnCommandCompletionCallback m_fnCompletionCallback;
        ICommandCompletionCallback* m_pCommandCompletionCallback;
    };

    bool m_bHasCompletionCallback : 1;
    bool m_bUsingNewCommandCallback : 1;
    bool m_bUsingCommandCallbackInterface : 1;
};

class ConVar : public ConCommandBase, public IConVar
{
public:
    typedef ConCommandBase BaseClass;

    virtual ~ConVar();

    virtual bool IsFlagSet( int flag ) const
    {
        return ( flag & m_pParent->m_nFlags ) ? true : false;
    }

    virtual const char* GetHelpText() const
    {
        return m_pParent->m_pszHelpString;
    }

    virtual bool IsRegistered() const { return m_pParent->m_bRegistered; }
    virtual const char* GetName() const { return m_pParent->m_pszName; }
    inline int GetFlags() const { return m_pParent->m_nFlags; }
    virtual void AddFlags( int flags ) { m_pParent->m_nFlags |= flags; }
    inline void RemoveFlags( int flags ) { m_pParent->m_nFlags &= ~flags; }
    virtual bool IsCommand() const { return false; }

    // Retrieve value
    inline float GetFloat() const { return m_pParent->m_fValue; }
    inline int GetInt() const { return m_pParent->m_nValue; }
    inline bool GetBool() const { return !!GetInt(); }
    inline const char* GetString() const
    {
        if ( m_nFlags & FCVAR_NEVER_AS_STRING )
        {
            return "FCVAR_NEVER_AS_STRING";
        }

        return ( m_pParent->m_pszString ) ? m_pParent->m_pszString : "";
    }

    inline bool GetMin( float& minVal ) const
    {
        minVal = m_pParent->m_fMinVal;
        return m_pParent->m_bHasMin;
    }

    inline bool GetMax( float& maxVal ) const
    {
        maxVal = m_pParent->m_fMaxVal;
        return m_pParent->m_bHasMax;
    }

    inline const char* GetDefault( void ) const
    {
        return m_pParent->m_pszDefaultValue;
    }

    inline void SetDefault( const char* pszDefault )
    {
        m_pszDefaultValue = pszDefault ? pszDefault : "";
    }

    virtual void SetValue( const char* value ) {}
    virtual void SetValue( float value ) {}
    virtual void SetValue( int value ) {}

private:
    virtual void InternalSetValue( const char* value ) {}
    virtual void InternalSetFloatValue( float fNewValue ) {}
    virtual void InternalSetIntValue( int nValue ) {}

    virtual bool ClampValue( float& value ) { return false; }
    virtual void ChangeStringValue( const char* tempVal, float flOldValue ) {}

    virtual void Create( const char* pName, const char* pDefaultValue,
                         int flags = 0, const char* pHelpString = 0,
                         bool bMin = false, float fMin = 0.0, bool bMax = false,
                         float fMax = false, FnChangeCallback_t callback = 0 )
    {
    }

    // Used internally by OneTimeInit to initialize.
    virtual void Init() {}

private:
    // This either points to "this" or it points to the original declaration of
    // a ConVar. This allows ConVars to exist in separate modules, and they all
    // use the first one to be declared. m_pParent->m_pParent must equal
    // m_pParent (ie: m_pParent must be the root, or original, ConVar).
    ConVar* m_pParent;

    int unk;

    // Static data
    const char* m_pszDefaultValue;

    // Value
    // Dynamically allocated
    char* m_pszString;
    int m_StringLength;

    // Values
    float m_fValue;
    int m_nValue;

    // Min/Max values
    bool m_bHasMin;
    float m_fMinVal;
    bool m_bHasMax;
    float m_fMaxVal;

    // Call this function when ConVar changes
    FnChangeCallback_t m_fnChangeCallback;
};