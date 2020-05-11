#pragma once

#include "appframework/iappsystem.hpp"

class DataCacheStatus_t;
class IDataCacheClient;
class IDataCacheSection;

enum DataCacheReportType_t
{
    DC_SUMMARY_REPORT,
    DC_DETAIL_REPORT,
    DC_DETAIL_REPORT_LRU,
};

struct DataCacheLimits_t
{
    DataCacheLimits_t( unsigned int _nMaxBytes = -1,
                       unsigned int _nMaxItems = -1,
                       unsigned int _nMinBytes = 0,
                       unsigned int _nMinItems = 0 )
        : nMaxBytes( _nMaxBytes ), nMaxItems( _nMaxItems ),
          nMinBytes( _nMinBytes ), nMinItems( _nMinItems )
    {
    }

    // Maximum levels permitted
    unsigned int nMaxBytes;
    unsigned int nMaxItems;

    // Minimum levels permitted
    unsigned int nMinBytes;
    unsigned int nMinItems;
};

constexpr const char* DATACACHE_INTERFACE_VERSION = "VDataCache003";

class IDataCache : public IAppSystem
{
public:
    //--------------------------------------------------------
    // Purpose: Controls cache size.
    //--------------------------------------------------------
    virtual void SetSize( int nMaxBytes ) = 0;
    virtual void SetOptions( unsigned options ) = 0;
    virtual void SetSectionLimits( const char* pszSectionName,
                                   const DataCacheLimits_t& limits ) = 0;

    //--------------------------------------------------------
    // Purpose: Get the current state of the cache
    //--------------------------------------------------------
    virtual void GetStatus( DataCacheStatus_t* pStatus,
                            DataCacheLimits_t* pLimits = nullptr ) = 0;

    //--------------------------------------------------------
    // Purpose: Add a section to the cache
    //--------------------------------------------------------
    virtual IDataCacheSection* AddSection(
        IDataCacheClient* pClient, const char* pszSectionName,
        const DataCacheLimits_t& limits = DataCacheLimits_t(),
        bool bSupportFastFind = false ) = 0;

    //--------------------------------------------------------
    // Purpose: Remove a section from the cache
    //--------------------------------------------------------
    virtual void RemoveSection( const char* pszClientName,
                                bool bCallFlush = true ) = 0;

    //--------------------------------------------------------
    // Purpose: Find a section of the cache
    //--------------------------------------------------------
    virtual IDataCacheSection* FindSection( const char* pszClientName ) = 0;

    //--------------------------------------------------------
    // Purpose: Dump the oldest items to free the specified amount of memory.
    // Returns amount actually freed
    //--------------------------------------------------------
    virtual unsigned Purge( unsigned nBytes ) = 0;

    //--------------------------------------------------------
    // Purpose: Empty the cache. Returns bytes released, will remove locked
    // items if force specified
    //--------------------------------------------------------
    virtual unsigned Flush( bool bUnlockedOnly = true,
                            bool bNotify = true ) = 0;

    //--------------------------------------------------------
    // Purpose: Output the state of the cache
    //--------------------------------------------------------
    virtual void OutputReport(
        DataCacheReportType_t reportType = DC_SUMMARY_REPORT,
        const char* pszSection = nullptr ) = 0;
};
