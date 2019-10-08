#include "convar.h"
#include "hooks.hpp"

void BytePatchServer( const uintptr_t dwServerBase )
{
    // remove FCVAR_CHEAT from ent_create until sv_cheats is fixed
    /*
    ConCommand* ent_create =
        reinterpret_cast<ConCommand*>( dwServerBase + 0xA0875C );

    if ( ent_create->IsFlagSet( FCVAR_CHEAT ) )
        ent_create->RemoveFlags( FCVAR_CHEAT );
    */
}

void OnServerLoaded( const uintptr_t dwServerBase )
{
    static bool bHasLoaded = false;

    if ( bHasLoaded )
    {
        return;
    }

    bHasLoaded = true;

    BytePatchServer( dwServerBase );
}
