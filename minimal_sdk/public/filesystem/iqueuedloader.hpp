#pragma once

#include "appframework/iappsystem.hpp"

class CFunctor;
class ILoaderProgress;
class IResourcePreload;
class LoaderJob_t;

enum LoaderError_t
{
    LOADERERROR_NONE = 0,
    LOADERERROR_FILEOPEN = -1,
    LOADERERROR_READING = -2,
};

enum ResourcePreload_t
{
    RESOURCEPRELOAD_UNKNOWN,
    RESOURCEPRELOAD_SOUND,
    RESOURCEPRELOAD_MATERIAL,
    RESOURCEPRELOAD_MODEL,
    RESOURCEPRELOAD_CUBEMAP,
    RESOURCEPRELOAD_STATICPROPLIGHTING,
    RESOURCEPRELOAD_ANONYMOUS,
    RESOURCEPRELOAD_COUNT
};

using QueuedLoaderCallback_t = void ( * )( void* pContext, void* pContext2,
                                           const void* pData, int nSize,
                                           LoaderError_t loaderError );

using DynamicResourceCallback_t = void ( * )( const char* pFilename,
                                              void* pContext, void* pContext2 );

constexpr const char* QUEUEDLOADER_INTERFACE_VERSION = "QueuedLoaderVersion004";

class IQueuedLoader : public IAppSystem
{
public:
    virtual void InstallLoader( ResourcePreload_t type,
                                IResourcePreload* pLoader ) = 0;
    virtual void InstallProgress( ILoaderProgress* pProgress ) = 0;

    // Set bOptimizeReload if you want appropriate data (such as static prop
    // lighting) to persist - rather than being purged and reloaded - when going
    // from map A to map A.
    virtual bool BeginMapLoading( const char* pMapName, bool bLoadForHDR,
                                  bool bOptimizeMapReload ) = 0;
    virtual void EndMapLoading( bool bAbort ) = 0;
    virtual bool AddJob( const LoaderJob_t* pLoaderJob ) = 0;

    // injects a resource into the map's reslist, rejected if not understood
    virtual void AddMapResource( const char* pFilename ) = 0;

    // dynamically load a map resource
    virtual void DynamicLoadMapResource( const char* pFilename,
                                         DynamicResourceCallback_t pCallback,
                                         void* pContext, void* pContext2 ) = 0;
    virtual void QueueDynamicLoadFunctor( CFunctor* pFunctor ) = 0;
    virtual bool CompleteDynamicLoad() = 0;

    // callback is asynchronous
    virtual bool ClaimAnonymousJob( const char* pFilename,
                                    QueuedLoaderCallback_t pCallback,
                                    void* pContext,
                                    void* pContext2 = nullptr ) = 0;
    // provides data if loaded, caller owns data
    virtual bool ClaimAnonymousJob( const char* pFilename, void** pData,
                                    int* pDataSize,
                                    LoaderError_t* pError = nullptr ) = 0;

    virtual bool IsMapLoading() const = 0;
    virtual bool IsSameMapLoading() const = 0;
    virtual bool IsFinished() const = 0;

    // callers can expect that jobs are not immediately started when batching
    virtual bool IsBatching() const = 0;

    virtual bool IsDynamic() const = 0;

    // callers can conditionalize operational spew
    virtual int GetSpewDetail() const = 0;

    virtual void PurgeAll() = 0;
};

extern IQueuedLoader* g_pQueuedLoader;
