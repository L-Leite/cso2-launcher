#pragma once

#include <map>

#define ON_LOAD_LIB( module )                                            \
    static void _On##module##Loaded( uintptr_t dwModuleBase );           \
    static CLoadLibCallbacks _##module##Callback( #module ".dll",        \
                                                  _On##module##Loaded ); \
    static void _On##module##Loaded( uintptr_t dwModuleBase )

#define ON_LOAD_LIB_EXTENSION( module, ext )                       \
    void _On##module##Loaded( uintptr_t dwModuleBase );            \
    CLoadLibCallbacks _##module##Callback( #module #ext,           \
                                           &_On##module##Loaded ); \
    void _On##module##Loaded( uintptr_t dwModuleBase )

#define GET_LOAD_LIB_MODULE() dwModuleBase

using LoadLibCallbackfn_t = void ( * )( uintptr_t );

class CLoadLibCallbacks
{
public:
    CLoadLibCallbacks( const std::string& szModuleName,
                       LoadLibCallbackfn_t pCallback )
    {
        GetCallbacks()[szModuleName] = pCallback;
    }

    ~CLoadLibCallbacks() = default;
    CLoadLibCallbacks( CLoadLibCallbacks&& ) = delete;

    // forces the static map to be initialized when needed
    inline static std::map<const std::string, LoadLibCallbackfn_t>&
    GetCallbacks()
    {
        static std::map<const std::string, LoadLibCallbackfn_t> s_Callbacks;
        return s_Callbacks;
    }

    inline static void OnLoadLibrary( const std::string& szModuleName,
                                      uintptr_t dwModuleBase )
    {
        auto& callbacks = GetCallbacks();

        if ( callbacks.find( szModuleName ) != callbacks.end() )
        {
            callbacks[szModuleName]( dwModuleBase );
            callbacks.erase( szModuleName );
        }
    }
};
