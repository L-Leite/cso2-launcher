#pragma once

#include <cstddef>
#include <cstdint>

#include "containers/utlsymbol.hpp"
#include "math/vector.hpp"

#include "inputsystem/ButtonCode.hpp"

class AudioState_t;
class CAudioSource;
class CGamestatsData;
class client_textmessage_t;
class color32;
class CPhysCollide;
class CSentence;
class IAchievementMgr;
class IMaterial;
class IMaterialSystem;
class INetChannelInfo;
class ISpatialQuery;
class KeyValues;
class matrix3x4_t;
class model_t;
class ModelEntry_t;
class OcclusionParams_t;
class player_info_t;
class QAngle;
class SurfInfo;
class VMatrix;
class vmode_s;

class ICSO2LogSystem;
class ICSO2GameManager;
class ICSO2LobbyStateManager;
class ICSO2FullDataDownloader;
class ICSO2Exp;
class ICSO2NMManager;

enum SkyboxVisibility_t
{
    SKYBOX_NOT_VISIBLE = 0,
    SKYBOX_3DSKYBOX_VISIBLE,
    SKYBOX_2DSKYBOX_VISIBLE,
};

enum CSO2MapLoadingProgress_t
{
    CSO2_MAP_LOADING_START = 0,
    CSO2_MAP_LOADING_UPDATE,
    CSO2_MAP_LOADING_FINISHED,
    CSO2_MAP_LOADING_UNK00,
    CSO2_MAP_LOADING_UNK01,
    CSO2_MAP_LOADING_UNK02
};

constexpr const char* VENGINE_CLIENT_INTERFACE_VERSION = "VEngineClient014";
constexpr const char* VENGINE_CLIENT_INTERFACE_VERSION_13 = "VEngineClient013";

class IVEngineClient013
{
public:
    // Find the model's surfaces that intersect the given sphere.
    // Returns the number of surfaces filled in.
    virtual int GetIntersectingSurfaces(
        const model_t* model, const Vector& vCenter, const float radius,
        const bool
            bOnlyVisibleSurfaces,  // Only return surfaces visible to vCenter.
        SurfInfo* pInfos, const int nMaxInfos ) = 0;

    // Get the lighting intensivty for a specified point
    // If bClamp is specified, the resulting Vector is restricted to the 0.0
    // to 1.0 for each element
    virtual Vector GetLightForPoint( const Vector& pos, bool bClamp ) = 0;

    // Traces the line and reports the material impacted as well as the lighting
    // information for the impact point
    virtual IMaterial* TraceLineMaterialAndLighting( const Vector& start,
                                                     const Vector& end,
                                                     Vector& diffuseLightColor,
                                                     Vector& baseColor ) = 0;

    // Given an input text buffer data pointer, parses a single token into the
    // variable token and returns the new
    //  reading position
    virtual const char* ParseFile( const char* data, char* token,
                                   int maxlen ) = 0;
    virtual bool CopyLocalFile( const char* source,
                                const char* destination ) = 0;

    // Gets the dimensions of the game window
    virtual void GetScreenSize( int& width, int& height ) = 0;

    // Forwards szCmdString to the server, sent reliably if bReliable is set
    virtual void ServerCmd( bool a2, const char* szCmdString,
                            bool bReliable = true ) = 0;
    virtual void ServerCmd( const char* szCmdString,
                            bool bReliable = true ) = 0;
    // Inserts szCmdString into the command buffer as if it was typed by the
    // client to his/her console. Note: Calls to this are checked against
    // FCVAR_CLIENTCMD_CAN_EXECUTE (if that bit is not set, then this function
    // can't change it).
    //       Call ClientCmd_Unrestricted to have access to
    //       FCVAR_CLIENTCMD_CAN_EXECUTE vars.
    virtual void ClientCmd( const char* szCmdString ) = 0;
    virtual void ClientCmd_AddCommand( bool a2, const char* szCmdString ) = 0;

    // Fill in the player info structure for the specified player index (name,
    // model, etc.)
    virtual bool GetPlayerInfo( int ent_num, player_info_t* pinfo ) = 0;

    // Retrieve the player entity number for a specified userID
    virtual int GetPlayerForUserID( int userID ) = 0;

    // Retrieves text message system information for the specified message by
    // name
    virtual client_textmessage_t* TextMessageGet( const char* pName ) = 0;

    // Returns true if the console is visible
    virtual bool Con_IsVisible( void ) = 0;

    virtual bool IFileSystem14() = 0;  // returns false

    // Get the entity index of the local player
    virtual int GetLocalPlayer( void ) = 0;

    // Client DLL is hooking a model, loads the model into memory and returns
    // pointer to the model_t
    virtual const model_t* LoadModel( const char* pName,
                                      bool bProp = false ) = 0;

    // From CSO2
    virtual const ModelEntry_t* FindModelEntry(
        FileNameHandle_t& hModelFile ) = 0;
    virtual FileNameHandle_t GetFileNameHandleByIndex(
        unsigned short iIndex ) = 0;
    virtual void FileSystem19( void* a2 ) = 0;

    // Get accurate, sub-frame clock ( profiling use )
    virtual float Time( void ) = 0;

    // Get the exact server timesstamp ( server time ) from the last message
    // received from the server
    virtual float GetLastTimeStamp( void ) = 0;

    // Given a CAudioSource (opaque pointer), retrieve the underlying CSentence
    // object ( stores the words, phonemes, and close
    //  captioning data )
    virtual CSentence* GetSentence( CAudioSource* pAudioSource ) = 0;
    // Given a CAudioSource, determines the length of the underlying audio file
    // (.wav, .mp3, etc.)
    virtual float GetSentenceLength( CAudioSource* pAudioSource ) = 0;
    // Returns true if the sound is streaming off of the hard disk (instead of
    // being memory resident)
    virtual bool IsStreaming( CAudioSource* pAudioSource ) const = 0;

    // Copy current view orientation into va
    virtual void GetViewAngles( QAngle& va ) = 0;
    // Set current view orientation from va
    virtual void SetViewAngles( QAngle& va ) = 0;

    // Retrieve the current game's maxclients setting
    virtual int GetMaxClients( void ) = 0;

    // Given the string pBinding which may be bound to a key,
    //  returns the string name of the key to which this string is bound.
    //  Returns NULL if no such binding exists
    virtual const char* Key_LookupBinding( const char* pBinding ) = 0;

    // Given the name of the key "mouse1", "e", "tab", etc., return the string
    // it is bound to "+jump", "impulse 50", etc.
    virtual const char* Key_BindingForKey( ButtonCode_t code ) = 0;

    virtual bool IsKeyDown( ButtonCode_t code ) = 0;

    // key trapping (for binding keys)
    virtual void StartKeyTrapMode( void ) = 0;
    virtual bool CheckDoneKeyTrapping( ButtonCode_t& code ) = 0;

    // Returns true if the player is fully connected and active in game (i.e,
    // not still loading)
    virtual bool IsInGame( void ) = 0;
    // Returns true if the player is connected, but not necessarily active in
    // game (could still be loading)
    virtual bool IsConnected( void ) = 0;
    // Returns true if the loading plaque should be drawn
    virtual bool IsDrawingLoadingImage( void ) = 0;

    // Prints the formatted string to the notification area of the screen ( down
    // the right hand edge
    //  numbered lines starting at position 0
    virtual void Con_NPrintf( int pos, const char* fmt, ... ) = 0;
    // Similar to Con_NPrintf, but allows specifying custom text color and
    // duration information
    virtual void Con_NXPrintf( const struct con_nprint_s* info, const char* fmt,
                               ... ) = 0;

    // Is the specified world-space bounding box inside the view frustum?
    virtual int IsBoxVisible( const Vector& mins, const Vector& maxs ) = 0;

    // Is the specified world-space boudning box in the same PVS cluster as the
    // view origin?
    virtual int IsBoxInViewCluster( const Vector& mins,
                                    const Vector& maxs ) = 0;

    // Returns true if the specified box is outside of the view frustum and
    // should be culled
    virtual bool CullBox( const Vector& mins, const Vector& maxs ) = 0;

    // Allow the sound system to paint additional data (during lengthy rendering
    // operations) to prevent stuttering sound.
    virtual void Sound_ExtraUpdate( void ) = 0;

    // Get the current game directory ( e.g., hl2, tf2, cstrike, hl1 )
    virtual const char* GetGameDirectory( void ) = 0;

    // Get access to the world to screen transformation matrix
    virtual const VMatrix& WorldToScreenMatrix() = 0;

    // Get the matrix to move a point from world space into view space
    // (translate and rotate so the camera is at the origin looking down X).
    virtual const VMatrix& WorldToViewMatrix() = 0;

    // The .bsp file can have mod-specified data lumps. These APIs are for
    // working with such game lumps.

    // Get mod-specified lump version id for the specified game data lump
    virtual int GameLumpVersion( int lumpId ) const = 0;
    // Get the raw size of the specified game data lump.
    virtual int GameLumpSize( int lumpId ) const = 0;
    // Loads a game lump off disk, writing the data into the buffer pointed to
    // bye pBuffer Returns false if the data can't be read or the destination
    // buffer is too small
    virtual bool LoadGameLump( int lumpId, void* pBuffer, int size ) = 0;

    // Returns the number of leaves in the level
    virtual int LevelLeafCount() const = 0;

    virtual int FileSystem49( int a2 ) = 0;

    // Gets a way to perform spatial queries on the BSP tree
    virtual ISpatialQuery* GetBSPTreeQuery() = 0;

    // Convert texlight to gamma...
    virtual void LinearToGamma( float* linear, float* gamma ) = 0;

    // Get the lightstyle value
    virtual float LightStyleValue( int style ) = 0;

    // Computes light due to dynamic lighting at a point
    // If the normal isn't specified, then it'll return the maximum lighting
    virtual void ComputeDynamicLighting( const Vector& pt,
                                         const Vector* pNormal,
                                         Vector& color ) = 0;

    // Returns the color of the ambient light
    virtual void GetAmbientLightColor( Vector& color ) = 0;

    // Returns the dx support level
    virtual int GetDXSupportLevel() = 0;

    // GR - returns the HDR support status
    virtual bool SupportsHDR() = 0;

    // Replace the engine's material system pointer.
    virtual void Mat_Stub( IMaterialSystem* pMatSys ) = 0;

    // Get the name of the current map
    virtual void GetChapterName( char* pchBuff, int iMaxLength ) = 0;
    virtual const char* GetLevelName( void ) = 0;
    virtual int GetLevelVersion( void ) = 0;
#if !defined( NO_VOICE )
    // Obtain access to the voice tweaking API
    virtual struct IVoiceTweak_s* GetVoiceTweakAPI( void ) = 0;
#endif

    virtual void SetMapLoadingProgress(
        CSO2MapLoadingProgress_t iMapLoadProgress, int iLoadPercentage,
        int a3 ) = 0;

    // Tell engine stats gathering system that the rendering frame is
    // beginning/ending
    virtual void EngineStats_BeginFrame( void ) = 0;
    virtual void EngineStats_EndFrame( void ) = 0;

    // This tells the engine to fire any events (temp entity messages) that it
    // has queued up this frame. It should only be called once per frame.
    virtual void FireEvents() = 0;

    // Returns an area index if all the leaves are in the same area. If they
    // span multple areas, then it returns -1.
    virtual int GetLeavesArea( int* pLeaves, int nLeaves ) = 0;

    // Returns true if the box touches the specified area's frustum.
    virtual bool DoesBoxTouchAreaFrustum( const Vector& mins,
                                          const Vector& maxs, int iArea ) = 0;

    // Sets the hearing origin (i.e., the origin and orientation of the listener
    // so that the sound system can spatialize
    //  sound appropriately ).
    virtual void SetAudioState( const AudioState_t& state ) = 0;

    // Sentences / sentence groups
    virtual int SentenceGroupPick( int groupIndex, char* name,
                                   int nameBufLen ) = 0;
    virtual int SentenceGroupPickSequential( int groupIndex, char* name,
                                             int nameBufLen, int sentenceIndex,
                                             int reset ) = 0;
    virtual int SentenceIndexFromName( const char* pSentenceName ) = 0;
    virtual const char* SentenceNameFromIndex( int sentenceIndex ) = 0;
    virtual int SentenceGroupIndexFromName( const char* pGroupName ) = 0;
    virtual const char* SentenceGroupNameFromIndex( int groupIndex ) = 0;
    virtual float SentenceLength( int sentenceIndex ) = 0;

    // Computes light due to dynamic lighting at a point
    // If the normal isn't specified, then it'll return the maximum lighting
    // If pBoxColors is specified (it's an array of 6), then it'll copy the
    // light contribution at each box side.
    virtual void ComputeLighting( const Vector& pt, const Vector* pNormal,
                                  bool bClamp, Vector& color,
                                  Vector* pBoxColors = nullptr ) = 0;

    // Activates/deactivates an occluder...
    virtual void ActivateOccluder( int nOccluderIndex, bool bActive ) = 0;
    virtual bool IsOccluded( const Vector& vecAbsMins,
                             const Vector& vecAbsMaxs ) = 0;

    // The save restore system allocates memory from a shared memory pool, use
    // this allocator to allocate/free saverestore
    //  memory.
    virtual void* SaveAllocMemory( size_t num, size_t size ) = 0;
    virtual void SaveFreeMemory( void* pSaveMem ) = 0;

    // returns info interface for client netchannel
    virtual INetChannelInfo* GetNetChannelInfo( void ) = 0;

    // Debugging functionality:
    // Very slow routine to draw a physics model
    virtual void DebugDrawPhysCollide( const CPhysCollide* pCollide,
                                       IMaterial* pMaterial,
                                       matrix3x4_t& transform,
                                       const color32& color ) = 0;
    // This can be used to notify test scripts that we're at a particular spot
    // in the code.
    virtual void CheckPoint( const char* pName ) = 0;
    // Draw portals if r_DrawPortals is set (Debugging only)
    virtual void DrawPortals() = 0;
    // Determine whether the client is playing back or recording a demo
    virtual bool IsPlayingDemo( void ) = 0;
    virtual bool IsRecordingDemo( void ) = 0;
    virtual bool IsPlayingTimeDemo( void ) = 0;
    virtual int GetDemoRecordingTick( void ) = 0;
    virtual int GetDemoPlaybackTick( void ) = 0;
    virtual int GetDemoPlaybackStartTick( void ) = 0;
    virtual float GetDemoPlaybackTimeScale( void ) = 0;
    virtual int GetDemoPlaybackTotalTicks( void ) = 0;
    // Is the game paused?
    virtual bool IsPaused( void ) = 0;
    // Is the game currently taking a screenshot?
    virtual bool IsTakingScreenshot( void ) = 0;
    // Is this a HLTV broadcast ?
    virtual bool IsHLTV( void ) = 0;

    virtual bool FileSystem95() = 0;  // returns false

    // is this level loaded as just the background to the main menu? (active,
    // but unplayable)
    virtual bool IsLevelMainMenuBackground( void ) = 0;
    // returns the name of the background level
    virtual void GetMainMenuBackgroundName( char* dest, int destlen ) = 0;

    // Get video modes
    virtual void GetVideoModes( int& nCount, vmode_s*& pModes ) = 0;

    // Occlusion system control
    virtual void SetOcclusionParameters( const OcclusionParams_t& params ) = 0;

    // What language is the user expecting to hear .wavs in, "english" or
    // another...
    virtual void GetUILanguage( char* dest, int destlen ) = 0;

    // Can skybox be seen from a particular point?
    virtual SkyboxVisibility_t IsSkyboxVisibleFromPoint(
        const Vector& vecPoint ) = 0;

    // Get the pristine map entity lump string.  (e.g., used by CS to reload the
    // map entities when restarting a round.)
    virtual const char* GetMapEntitiesString() = 0;

    // Is the engine in map edit mode ?
    virtual bool IsInEditMode( void ) = 0;

    // current screen aspect ratio (eg. 4.0f/3.0f, 16.0f/9.0f)
    virtual float GetScreenAspectRatio() = 0;

    virtual float GetDemoOverrideFOV() = 0;

    // allow the game UI to login a user
    virtual bool REMOVED_SteamRefreshLogin( const char* password,
                                            bool isSecure ) = 0;
    virtual bool REMOVED_SteamProcessCall( bool& finished ) = 0;

    // allow other modules to know about engine versioning (one use is a proxy
    // for network compatability)
    virtual unsigned int GetEngineBuildNumber() = 0;  // engines build
    virtual const char*
    GetProductVersionString() = 0;  // mods version number (steam.inf)

    // Communicates to the color correction editor that it's time to grab the
    // pre-color corrected frame Passes in the actual size of the viewport
    virtual void GrabPreColorCorrectedFrame( int x, int y, int width,
                                             int height ) = 0;

    virtual bool IsHammerRunning() const = 0;

    // Inserts szCmdString into the command buffer as if it was typed by the
    // client to his/her console. And then executes the command string
    // immediately (vs ClientCmd() which executes in the next frame)
    //
    // Note: this is NOT checked against the FCVAR_CLIENTCMD_CAN_EXECUTE vars.
    virtual void ExecuteClientCmd( const char* szCmdString ) = 0;

    // returns if the loaded map was processed with HDR info. This will be set
    // regardless of what HDR mode the player is in.
    virtual bool MapHasHDRLighting( void ) = 0;

    virtual int GetAppID() = 0;

    // Just get the leaf ambient light - no caching, no samples
    virtual Vector GetLightForPointFast( const Vector& pos, bool bClamp ) = 0;

    // This version does NOT check against FCVAR_CLIENTCMD_CAN_EXECUTE.
    virtual void ClientCmd_Unrestricted( const char* szCmdString ) = 0;

    // This used to be accessible through the cl_restrict_server_commands cvar.
    // By default, Valve games restrict the server to only being able to execute
    // commands marked with FCVAR_SERVER_CAN_EXECUTE. By default, mods are
    // allowed to execute any server commands, and they can restrict the
    // server's ability to execute client commands with this function.
    virtual void SetRestrictServerCommands( bool bRestrict ) = 0;

    // If set to true (defaults to true for Valve games and false for others),
    // then IVEngineClient::ClientCmd can only execute things marked with
    // FCVAR_CLIENTCMD_CAN_EXECUTE.
    virtual void SetRestrictClientCommands( bool bRestrict ) = 0;

    // Sets the client renderable for an overlay's material proxy to bind to
    virtual void SetOverlayBindProxy( int iOverlayID, void* pBindProxy ) = 0;

    virtual bool CopyFrameBufferToMaterial( const char* pMaterialName ) = 0;

    // Matchmaking
    virtual void ChangeTeam( const char* pTeamName ) = 0;

    // Causes the engine to read in the user's configuration on disk
    virtual void ReadConfiguration( const bool readDefault = false ) = 0;

    virtual void SetAchievementMgr( IAchievementMgr* pAchievementMgr ) = 0;
    virtual IAchievementMgr* GetAchievementMgr() = 0;

    virtual bool MapLoadFailed( void ) = 0;
    virtual void SetMapLoadFailed( bool bState ) = 0;

    virtual bool IsLowViolence() = 0;
    virtual const char* GetMostRecentSaveGame( void ) = 0;
    virtual void SetMostRecentSaveGame( const char* lpszFilename ) = 0;

    virtual void StartXboxExitingProcess() = 0;
    virtual bool IsSaveInProgress() = 0;
    virtual uint32_t OnStorageDeviceAttached( void ) = 0;
    virtual void OnStorageDeviceDetached( void ) = 0;

    virtual void ResetDemoInterpolation( void ) = 0;

    // Methods to set/get a gamestats data container so client & server running
    // in same process can send combined data
    virtual void SetGamestatsData( CGamestatsData* pGamestatsData ) = 0;
    virtual CGamestatsData* GetGamestatsData() = 0;

    virtual bool FileSystem138( bool a2 ) = 0;
    virtual int FileSystem139() = 0;

    virtual ICSO2LogSystem* GetCSO2LogSystem() = 0;
    virtual ICSO2GameManager* GetCSO2GameManager() = 0;
    virtual ICSO2LobbyStateManager* GetCSO2LobbyStateManager() = 0;
    virtual ICSO2FullDataDownloader* GetCSO2FullDataDownloader() = 0;
    virtual ICSO2Exp* GetCSO2Exp() = 0;
    virtual ICSO2NMManager* GetCSO2NMManager() = 0;

    virtual int SetLobbyMusic(
        const char* szMusicPath ) = 0;  // maybe, maybe not; untested
    virtual int ResetLobbyMusic() = 0;  // again, untested
    virtual bool PlaySound( const char* szSoundPath ) = 0;
    virtual void SomethingWithSound( const char* szSoundPath ) = 0;
    virtual int FileSystem150( void* a1, int a2, char a3, int a4, char a5 ) = 0;
    virtual int FileSystem151( const char* a1 ) = 0;
    virtual int FileSystem152() = 0;
    virtual int FileSystem153() = 0;

#if defined( USE_SDL )
    // we need to pull delta's from the cocoa mgr, the engine vectors this for
    // us
    virtual void GetMouseDelta( int& x, int& y,
                                bool bIgnoreNextMouseDelta = false ) = 0;
#endif

    virtual void FileSystem154() = 0;                  // does nothing
    virtual void FileSystem155( int a2, int a3 ) = 0;  // does nothing

    virtual void* GetWindowHandle() = 0;

    // Sends a key values server command, not allowed from scripts execution
    // Params:
    //	pKeyValues	- key values to be serialized and sent to server
    //				  the pointer is deleted inside the function:
    // pKeyValues->deleteThis()
    virtual void ServerCmdKeyValues( KeyValues* pKeyValues ) = 0;

    virtual bool FileSystem158( int a1, int a2, int a3,
                                int a4 ) = 0;  // returns false
    virtual bool FileSystem159() = 0;          // returns false

    virtual bool IsSkippingPlayback( void ) = 0;
    virtual bool IsLoadingDemo( void ) = 0;

    // Returns true if the engine is playing back a "locally recorded" demo,
    // which includes both SourceTV and replay demos, since they're recorded
    // locally (on servers), as opposed to a client recording a demo while
    // connected to a remote server.
    virtual bool IsPlayingDemoALocallyRecordedDemo() = 0;

    // Given the string pBinding which may be bound to a key,
    //  returns the string name of the key to which this string is bound.
    //  Returns NULL if no such binding exists
    // Unlike Key_LookupBinding, leading '+' characters are not stripped from
    // bindings.
    virtual const char* Key_LookupBindingExact( const char* pBinding ) = 0;

    // virtual void				AddPhonemeFile( const char *pszPhonemeFile ) =
    // 0;

    virtual void SaveVolumeValuesToRegistry() = 0;
    virtual void FileSystem165(
        void* a2 ) = 0;  // calls something from CCSO2CacheMgr
    virtual void FileSystem166( void* a2, bool a3 ) = 0;         // bsp related?
    virtual void FileSystem167( void* a2, int a3, int a4 ) = 0;  // bsp related?

    virtual bool Is32BitsOS() = 0;
};

class IVEngineClient : public IVEngineClient013
{
public:
    virtual uint32_t GetProtocolVersion() = 0;
    virtual bool IsWindowedMode() = 0;

    // Flash the window (os specific)
    virtual void FlashWindow() = 0;

    // Client version from the steam.inf, this will be compared to the GC
    // version
    virtual int GetClientVersion() const = 0;  // engines build

    // Is App Active
    virtual bool IsActiveApp() = 0;

    virtual void DisconnectInternal() = 0;

    virtual int GetInstancesRunningCount() = 0;

    virtual bool IsInCommentaryMode() = 0;
};