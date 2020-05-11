#pragma once

#include "appframework/iappsystem.hpp"
#include "datacache/imdlcache.hpp"

class CUtlBuffer;
class IMaterial;
class IMaterialSystem;
class ITexture;
class Vector;
class Vector4D;
class VMatrix;

struct FlashlightState_t;
struct LightDesc_t;
struct Ray_t;
struct studiohdr_t;
struct studiohwdata_t;
struct matrix3x4_t;
struct vertexFileHeader_t;

struct DrawModelResults_t;
struct DrawModelInfo_t;
struct GetTriangles_Output_t;
struct model_array_instance_t;
struct StudioRenderConfig_t;

typedef struct
{
    int unused;
} * StudioDecalHandle_t;

#define STUDIORENDER_DECAL_INVALID ( (StudioDecalHandle_t)0 )

enum
{
    ADDDECAL_TO_ALL_LODS = -1
};

enum
{
    STUDIORENDER_DRAW_ENTIRE_MODEL = 0,
    STUDIORENDER_DRAW_OPAQUE_ONLY = 0x01,
    STUDIORENDER_DRAW_TRANSLUCENT_ONLY = 0x02,
    STUDIORENDER_DRAW_GROUP_MASK = 0x03,

    STUDIORENDER_DRAW_NO_FLEXES = 0x04,
    STUDIORENDER_DRAW_STATIC_LIGHTING = 0x08,

    STUDIORENDER_DRAW_ACCURATETIME =
        0x10,  // Use accurate timing when drawing the model.
    STUDIORENDER_DRAW_NO_SHADOWS = 0x20,
    STUDIORENDER_DRAW_GET_PERF_STATS = 0x40,

    STUDIORENDER_DRAW_WIREFRAME = 0x80,

    STUDIORENDER_DRAW_ITEM_BLINK = 0x100,

    STUDIORENDER_SHADOWDEPTHTEXTURE = 0x200,

    STUDIORENDER_SSAODEPTHTEXTURE = 0x1000,

    STUDIORENDER_GENERATE_STATS = 0x8000,
};

enum OverrideType_t
{
    OVERRIDE_NORMAL = 0,
    OVERRIDE_BUILD_SHADOWS,
    OVERRIDE_DEPTH_WRITE,
    OVERRIDE_SSAO_DEPTH_WRITE,
};

constexpr const char* STUDIO_DATA_CACHE_INTERFACE_VERSION =
    "VStudioDataCache005";

class IStudioDataCache : public IAppSystem
{
public:
    virtual bool VerifyHeaders( studiohdr_t* pStudioHdr ) = 0;
    virtual vertexFileHeader_t* CacheVertexData( studiohdr_t* pStudioHdr ) = 0;
};

constexpr const char* STUDIO_RENDER_INTERFACE_VERSION = "VStudioRender025";

class IStudioRender : public IAppSystem
{
public:
    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;

    // Used for the mat_stub console command.
    virtual void Mat_Stub( IMaterialSystem* pMatSys ) = 0;

    // Updates the rendering configuration
    virtual void UpdateConfig( const StudioRenderConfig_t& config ) = 0;
    virtual void GetCurrentConfig( StudioRenderConfig_t& config ) = 0;

    // Load, unload model data
    virtual bool LoadModel( studiohdr_t* pStudioHdr, void* pVtxData,
                            studiohwdata_t* pHardwareData ) = 0;
    virtual void UnloadModel( studiohwdata_t* pHardwareData ) = 0;

    // Refresh the studiohdr since it was lost...
    virtual void RefreshStudioHdr( studiohdr_t* pStudioHdr,
                                   studiohwdata_t* pHardwareData ) = 0;

    // This is needed to do eyeglint and calculate the correct texcoords for the
    // eyes.
    virtual void SetEyeViewTarget( const studiohdr_t* pStudioHdr,
                                   int nBodyIndex,
                                   const Vector& worldPosition ) = 0;

    // Methods related to lighting state
    // NOTE: SetAmbientLightColors assumes that the arraysize is the same as
    // returned from GetNumAmbientLightSamples
    virtual int GetNumAmbientLightSamples() = 0;
    virtual const Vector* GetAmbientLightDirections() = 0;
    virtual void SetAmbientLightColors(
        const Vector4D* pAmbientOnlyColors ) = 0;
    virtual void SetAmbientLightColors( const Vector* pAmbientOnlyColors ) = 0;
    virtual void SetLocalLights( int numLights,
                                 const LightDesc_t* pLights ) = 0;

    // Sets information about the camera location + orientation
    virtual void SetViewState( const Vector& viewOrigin,
                               const Vector& viewRight, const Vector& viewUp,
                               const Vector& viewPlaneNormal ) = 0;

    // Allocates flex weights for use in rendering
    // NOTE: Pass in a non-null second parameter to lock delayed flex weights
    virtual void LockFlexWeights( int nWeightCount, float** ppFlexWeights,
                                  float** ppFlexDelayedWeights = nullptr ) = 0;
    virtual void UnlockFlexWeights() = 0;

    // Used to allocate bone matrices to be used to pass into DrawModel
    virtual matrix3x4_t* LockBoneMatrices( int nBoneCount ) = 0;
    virtual void UnlockBoneMatrices() = 0;

    // LOD stuff
    virtual int GetNumLODs( const studiohwdata_t& hardwareData ) const = 0;
    virtual float GetLODSwitchValue( const studiohwdata_t& hardwareData,
                                     int lod ) const = 0;
    virtual void SetLODSwitchValue( studiohwdata_t& hardwareData, int lod,
                                    float switchValue ) = 0;

    // Sets the color/alpha modulation
    virtual void SetColorModulation( float const* pColor ) = 0;
    virtual void SetAlphaModulation( float flAlpha ) = 0;

    // Draws the model
    virtual void DrawModel( DrawModelResults_t* pResults,
                            const DrawModelInfo_t& info,
                            matrix3x4_t* pBoneToWorld, float* pFlexWeights,
                            float* pFlexDelayedWeights,
                            const Vector& modelOrigin,
                            int flags = STUDIORENDER_DRAW_ENTIRE_MODEL ) = 0;

    // Methods related to static prop rendering
    virtual void DrawModelStaticProp(
        const DrawModelInfo_t& drawInfo, const matrix3x4_t& modelToWorld,
        int flags = STUDIORENDER_DRAW_ENTIRE_MODEL ) = 0;
    virtual void DrawStaticPropDecals( const DrawModelInfo_t& drawInfo,
                                       const matrix3x4_t& modelToWorld ) = 0;
    virtual void DrawStaticPropShadows( const DrawModelInfo_t& drawInfo,
                                        const matrix3x4_t& modelToWorld,
                                        int flags ) = 0;

    // Causes a material to be used instead of the materials the model was
    // compiled with
    virtual void ForcedMaterialOverride(
        IMaterial* newMaterial,
        OverrideType_t nOverrideType = OVERRIDE_NORMAL ) = 0;

    // Create, destroy list of decals for a particular model
    virtual StudioDecalHandle_t CreateDecalList(
        studiohwdata_t* pHardwareData ) = 0;
    virtual void DestroyDecalList( StudioDecalHandle_t handle ) = 0;

    // Add decals to a decal list by doing a planar projection along the ray
    // The BoneToWorld matrices must be set before this is called
    virtual void AddDecal( StudioDecalHandle_t handle, studiohdr_t* pStudioHdr,
                           matrix3x4_t* pBoneToWorld, const Ray_t& ray,
                           const Vector& decalUp, IMaterial* pDecalMaterial,
                           float radius, int body, bool noPokethru = false,
                           int maxLODToDecal = ADDDECAL_TO_ALL_LODS ) = 0;

    // Compute the lighting at a point and normal
    virtual void ComputeLighting( const Vector* pAmbient, int lightCount,
                                  LightDesc_t* pLights, const Vector& pt,
                                  const Vector& normal, Vector& lighting ) = 0;

    // Compute the lighting at a point, constant directional component is passed
    // as flDirectionalAmount
    virtual void ComputeLightingConstDirectional(
        const Vector* pAmbient, int lightCount, LightDesc_t* pLights,
        const Vector& pt, const Vector& normal, Vector& lighting,
        float flDirectionalAmount ) = 0;

    // Shadow state (affects the models as they are rendered)
    virtual void AddShadow( IMaterial* pMaterial, void* pProxyData,
                            FlashlightState_t* m_pFlashlightState = nullptr,
                            VMatrix* pWorldToTexture = nullptr,
                            ITexture* pFlashlightDepthTexture = nullptr ) = 0;
    virtual void ClearAllShadows() = 0;

    // Gets the model LOD; pass in the screen size in pixels of a sphere
    // of radius 1 that has the same origin as the model to get the LOD out...
    virtual int ComputeModelLod( studiohwdata_t* pHardwareData,
                                 float unitSphereSize,
                                 float* pMetric = nullptr ) = 0;

    // Return a number that is usable for budgets, etc.
    // Things that we care about:
    // 1) effective triangle count (factors in batch sizes, state changes, etc)
    // 2) texture memory usage
    // Get Triangles returns the LOD used
    virtual void GetPerfStats( DrawModelResults_t* pResults,
                               const DrawModelInfo_t& info,
                               CUtlBuffer* pSpewBuf = nullptr ) const = 0;
    virtual void GetTriangles( const DrawModelInfo_t& info,
                               matrix3x4_t* pBoneToWorld,
                               GetTriangles_Output_t& out ) = 0;

    // Returns materials used by a particular model
    virtual int GetMaterialList( studiohdr_t* pStudioHdr, int count,
                                 IMaterial** ppMaterials ) = 0;
    virtual int GetMaterialListFromBodyAndSkin(
        MDLHandle_t studio, int nSkin, int nBody, int nCountOutputMaterials,
        IMaterial** ppOutputMaterials ) = 0;
    // draw an array of models with the same state
    virtual void DrawModelArray(
        const DrawModelInfo_t& drawInfo, int arrayCount,
        model_array_instance_t* pInstanceData, int instanceStride,
        int flags = STUDIORENDER_DRAW_ENTIRE_MODEL ) = 0;
};

extern IStudioRender* g_pStudioRender;
