#pragma once

enum ImageFormat
{
    IMAGE_FORMAT_UNKNOWN = -1,
    IMAGE_FORMAT_RGBA8888 = 0,
    IMAGE_FORMAT_ABGR8888,
    IMAGE_FORMAT_RGB888,
    IMAGE_FORMAT_BGR888,
    IMAGE_FORMAT_RGB565,
    IMAGE_FORMAT_I8,
    IMAGE_FORMAT_IA88,
    IMAGE_FORMAT_P8,
    IMAGE_FORMAT_A8,
    IMAGE_FORMAT_RGB888_BLUESCREEN,
    IMAGE_FORMAT_BGR888_BLUESCREEN,
    IMAGE_FORMAT_ARGB8888,
    IMAGE_FORMAT_BGRA8888,
    IMAGE_FORMAT_DXT1,
    IMAGE_FORMAT_DXT3,
    IMAGE_FORMAT_DXT5,
    IMAGE_FORMAT_BGRX8888,
    IMAGE_FORMAT_BGR565,
    IMAGE_FORMAT_BGRX5551,
    IMAGE_FORMAT_BGRA4444,
    IMAGE_FORMAT_DXT1_ONEBITALPHA,
    IMAGE_FORMAT_BGRA5551,
    IMAGE_FORMAT_UV88,
    IMAGE_FORMAT_UVWQ8888,
    IMAGE_FORMAT_RGBA16161616F,
    IMAGE_FORMAT_RGBA16161616,
    IMAGE_FORMAT_UVLX8888,
    IMAGE_FORMAT_R32F,  // Single-channel 32-bit floating point
    IMAGE_FORMAT_RGB323232F,
    IMAGE_FORMAT_RGBA32323232F,

    // Depth-stencil texture formats for shadow depth mapping
    IMAGE_FORMAT_NV_DST16,   //
    IMAGE_FORMAT_NV_DST24,   //
    IMAGE_FORMAT_NV_INTZ,    // Vendor-specific depth-stencil texture
    IMAGE_FORMAT_NV_RAWZ,    // formats for shadow depth mapping
    IMAGE_FORMAT_ATI_DST16,  //
    IMAGE_FORMAT_ATI_DST24,  //
    IMAGE_FORMAT_NV_NULL,    // Dummy format which takes no video memory

    // Compressed normal map formats
    IMAGE_FORMAT_ATI2N,  // One-surface ATI2N / DXN format
    IMAGE_FORMAT_ATI1N,  // Two-surface ATI1N format

    IMAGE_FORMAT_DXT1_RUNTIME,
    IMAGE_FORMAT_DXT5_RUNTIME,

    NUM_IMAGE_FORMATS
};