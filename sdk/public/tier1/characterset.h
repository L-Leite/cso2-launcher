//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Shared code for parsing / searching for characters in a string
//			using lookup tables
//
// $Workfile:     $
// $Date:         $
// $NoKeywords: $
//===========================================================================//

#ifndef CHARACTERSET_H
#define CHARACTERSET_H

#ifdef _WIN32
#pragma once
#endif


struct characterset_t
{
	char set[256];
};


// This is essentially a strpbrk() using a precalculated lookup table
//-----------------------------------------------------------------------------
// Purpose: builds a simple lookup table of a group of important characters
// Input  : *pSetBuffer - pointer to the buffer for the group
//			*pSetString - list of characters to flag
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Purpose: builds a simple lookup table of a group of important characters
// Input  : *pParseGroup - pointer to the buffer for the group
//			*pGroupString - null terminated list of characters to flag
//-----------------------------------------------------------------------------
inline void CharacterSetBuild( characterset_t* pSetBuffer, const char* pszSetString )
{
    int i = 0;

    // Test our pointers
    if ( !pSetBuffer || !pszSetString )
        return;

    memset( pSetBuffer->set, 0, sizeof( pSetBuffer->set ) );

    while ( pszSetString[i] )
    {
        pSetBuffer->set[(unsigned)pszSetString[i]] = 1;
        i++;
    }
}


//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pSetBuffer - pre-build group buffer
//			character - character to lookup
// Output : int - 1 if the character was in the set
//-----------------------------------------------------------------------------
#define IN_CHARACTERSET( SetBuffer, character )		((SetBuffer).set[ (unsigned char) (character) ])


#endif // CHARACTERSET_H
