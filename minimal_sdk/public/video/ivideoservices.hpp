#pragma once

#include <cstdint>
#include <limits>

#include "appframework/iappsystem.hpp"

class IVideoMaterial;
class IVideoRecorder;

enum class VideoResult
{
    SUCCESS = 0,

    SYSTEM_NOT_AVAILABLE,
    CODEC_NOT_AVAILABLE,
    FEATURE_NOT_AVAILABLE,

    UNKNOWN_OPERATION,
    ILLEGAL_OPERATION,
    OPERATION_NOT_SUPPORTED,

    BAD_INPUT_PARAMETERS,
    OPERATION_ALREADY_PERFORMED,
    OPERATION_OUT_OF_SEQUENCE,

    VIDEO_ERROR_OCCURED,
    FILE_ERROR_OCCURED,
    AUDIO_ERROR_OCCURED,
    SYSTEM_ERROR_OCCURED,
    INITIALIZATION_ERROR_OCCURED,
    SHUTDOWN_ERROR_OCCURED,

    MATERIAL_NOT_FOUND,
    RECORDER_NOT_FOUND,
    VIDEO_FILE_NOT_FOUND,
    VIDEO_SYSTEM_NOT_FOUND,
};

enum class VideoSystem
{
    ALL_VIDEO_SYSTEMS = -2,
    DETERMINE_FROM_FILE_EXTENSION = -1,
    NONE = 0,

    BINK,
    AVI,
    WMV,
    QUICKTIME,
    WEBM,

    VIDEO_SYSTEM_COUNT,
    VIDEO_SYSTEM_FIRST = 1,
};

enum class VideoSystemStatus
{
    OK = 0,
    NOT_INSTALLED,
    NOT_CURRENT_VERSION,
    NOT_INITIALIZED,
    INITIALIZATION_ERROR,
};

enum class VideoSystemFeature : uint32_t
{
    NO_FEATURES = 0x00000000,
    PLAY_VIDEO_FILE_FULL_SCREEN = 0x00000001,
    PLAY_VIDEO_FILE_IN_MATERIAL = 0x00000002,
    ENCODE_VIDEO_TO_FILE = 0x00000004,
    ENCODE_AUDIO_TO_FILE = 0x00000008,

    FULL_PLAYBACK = PLAY_VIDEO_FILE_FULL_SCREEN | PLAY_VIDEO_FILE_IN_MATERIAL,
    FULL_ENCODE = ENCODE_VIDEO_TO_FILE | ENCODE_AUDIO_TO_FILE,
    ALL_VALID_FEATURES = FULL_PLAYBACK | FULL_ENCODE,

    ESF_FORCE_UINT32 = std::numeric_limits<uint32_t>::max(),
};

enum class VideoSoundDeviceOperation_t
{
    SET_DIRECT_SOUND_DEVICE = 0,  // Windows option
    SET_MILES_SOUND_DEVICE,       // Supported by RAD
    HOOK_X_AUDIO,                 // Xbox Option
    SET_SOUND_MANAGER_DEVICE,     // OSX Option
    SET_LIB_AUDIO_DEVICE,         // PS3 Option
    SET_SDL_SOUND_DEVICE,         // SDL Audio
    SET_SDL_PARAMS,               // SDL Audio params
    SDLMIXER_CALLBACK,            // SDLMixer callback

    OPERATION_COUNT
};

enum class VideoEncodeCodec_t
{
    MPEG2_CODEC,
    MPEG4_CODEC,
    H261_CODEC,
    H263_CODEC,
    H264_CODEC,
    MJPEG_A_CODEC,
    MJPEG_B_CODEC,
    SORENSON3_CODEC,
    CINEPACK_CODEC,
    WEBM_CODEC,

    CODEC_COUNT,

    DEFAULT_CODEC = H264_CODEC,
};

enum class VideoPlaybackFlags : uint32_t  // Options when playing a video file
{
    NO_PLAYBACK_OPTIONS = 0x00000000,

    // Full Screen Playback Options
    FILL_WINDOW = 0x00000001,        // force video fill entire window
    LOCK_ASPECT_RATIO = 0x00000002,  // preserve aspect ratio when scaling
    INTEGRAL_SCALE = 0x00000004,     // scale video only by integral amounts
    CENTER_VIDEO_IN_WINDOW = 0x00000008,  // center output video in window

    FORCE_MIN_PLAY_TIME = 0x00000010,  // play for a minimum amount of time
                                       // before allowing skip or abort

    ABORT_ON_SPACE = 0x00000100,  // Keys to abort fullscreen playback on
    ABORT_ON_ESC = 0x00000200,
    ABORT_ON_RETURN = 0x00000400,
    ABORT_ON_ANY_KEY = 0x00000700,

    PAUSE_ON_SPACE = 0x00001000,  // Keys to pause fullscreen playback on
    PAUSE_ON_ESC = 0x00002000,
    PAUSE_ON_RETURN = 0x00004000,
    PAUSE_ON_ANY_KEY = 0x00007000,

    LOOP_VIDEO = 0x00010000,  // Full Screen and Video material
    NO_AUDIO = 0x00020000,
    PRELOAD_VIDEO = 0x00040000,

    DONT_AUTO_START_VIDEO =
        0x00100000,  // Don't begin playing until told to do so.
    TEXTURES_ACTUAL_SIZE =
        0x00200000,  // Try and use textures the same size as the video frame

    VALID_FULLSCREEN_FLAGS = 0x0007771F,  // Playback Flags that are valid for
                                          // playing videos fullscreen
    VALID_MATERIAL_FLAGS = 0x00370000,    // Playback Flags that are valid for
                                          // playing videos in a material

    DEFAULT_MATERIAL_OPTIONS = NO_PLAYBACK_OPTIONS,
    DEFAULT_FULLSCREEN_OPTIONS =
        CENTER_VIDEO_IN_WINDOW | LOCK_ASPECT_RATIO | ABORT_ON_ANY_KEY,

    EVPF_FORCE_UINT32 = std::numeric_limits<uint32_t>::max(),
};

constexpr const char* VIDEO_SERVICES_INTERFACE_VERSION = "IVideoServices002";

class IVideoServices : public IAppSystem
{
public:
    // Query the available video systems
    virtual int GetAvailableVideoSystemCount() = 0;
    virtual VideoSystem GetAvailableVideoSystem( int n ) = 0;

    virtual bool IsVideoSystemAvailable( VideoSystem videoSystem ) = 0;
    virtual VideoSystemStatus GetVideoSystemStatus(
        VideoSystem videoSystem ) = 0;
    virtual VideoSystemFeature GetVideoSystemFeatures(
        VideoSystem videoSystem ) = 0;
    virtual const char* GetVideoSystemName( VideoSystem videoSystem ) = 0;

    virtual VideoSystem FindNextSystemWithFeature(
        VideoSystemFeature features,
        VideoSystem startAfter = VideoSystem::NONE ) = 0;

    virtual VideoResult GetLastResult() = 0;

    // deal with video file extensions and video system mappings
    virtual int GetSupportedFileExtensionCount( VideoSystem videoSystem ) = 0;
    virtual const char* GetSupportedFileExtension( VideoSystem videoSystem,
                                                   int extNum = 0 ) = 0;
    virtual VideoSystemFeature GetSupportedFileExtensionFeatures(
        VideoSystem videoSystem, int extNum = 0 ) = 0;

    virtual VideoSystem LocateVideoSystemForPlayingFile(
        const char* pFileName,
        VideoSystemFeature playMode =
            VideoSystemFeature::PLAY_VIDEO_FILE_IN_MATERIAL ) = 0;
    virtual VideoResult LocatePlayableVideoFile(
        const char* pSearchFileName, const char* pPathID,
        VideoSystem* pPlaybackSystem, char* pPlaybackFileName,
        int fileNameMaxLen,
        VideoSystemFeature playMode = VideoSystemFeature::FULL_PLAYBACK ) = 0;

    // Create/destroy a video material
    virtual IVideoMaterial* CreateVideoMaterial(
        const char* pMaterialName, const char* pVideoFileName,
        const char* pPathID = nullptr,
        VideoPlaybackFlags playbackFlags =
            VideoPlaybackFlags::DEFAULT_MATERIAL_OPTIONS,
        VideoSystem videoSystem = VideoSystem::DETERMINE_FROM_FILE_EXTENSION,
        bool PlayAlternateIfNotAvailable = true ) = 0;

    virtual VideoResult DestroyVideoMaterial(
        IVideoMaterial* pVideoMaterial ) = 0;
    virtual int GetUniqueMaterialID() = 0;

    // Create/destroy a video encoder
    virtual VideoResult IsRecordCodecAvailable( VideoSystem videoSystem,
                                                VideoEncodeCodec_t codec ) = 0;

    virtual IVideoRecorder* CreateVideoRecorder( VideoSystem videoSystem ) = 0;
    virtual VideoResult DestroyVideoRecorder(
        IVideoRecorder* pVideoRecorder ) = 0;

    // Plays a given video file until it completes or the user presses ESC,
    // SPACE, or ENTER
    virtual VideoResult PlayVideoFileFullScreen(
        const char* pFileName, const char* pPathID, void* mainWindow,
        int windowWidth, int windowHeight, int desktopWidth, int desktopHeight,
        bool windowed, float forcedMinTime,
        VideoPlaybackFlags playbackFlags =
            VideoPlaybackFlags::DEFAULT_FULLSCREEN_OPTIONS,
        VideoSystem videoSystem = VideoSystem::DETERMINE_FROM_FILE_EXTENSION,
        bool PlayAlternateIfNotAvailable = true ) = 0;

    // Sets the sound devices that the video will decode to
    virtual VideoResult SoundDeviceCommand(
        VideoSoundDeviceOperation_t operation, void* pDevice = nullptr,
        void* pData = nullptr,
        VideoSystem videoSystem = VideoSystem::ALL_VIDEO_SYSTEMS ) = 0;

    // Get the (localized) name of a codec as a string
    virtual const wchar_t* GetCodecName( VideoEncodeCodec_t nCodec ) = 0;
};
