#include "../../main.h"
#include "RenderWare.h"
#include "util/patch.h"

RsGlobalType* RsGlobal;

/* rwcore.h */
RwCamera* (*RwCameraBeginUpdate)(RwCamera* camera);
RwCamera* (*RwCameraEndUpdate)(RwCamera* camera);
RwCamera* (*RwCameraShowRaster)(RwCamera * camera, void *pDev, RwUInt32 flags);

RwRaster* 	(*RwRasterCreate)(RwInt32 width, RwInt32 height, RwInt32 depth, RwInt32 flags);
RwBool 		(*RwRasterDestroy)(RwRaster * raster);
RwRaster* 	(*RwRasterGetOffset)(RwRaster *raster, RwInt16 *xOffset, RwInt16 *yOffset);
RwInt32 	(*RwRasterGetNumLevels)(RwRaster * raster);
RwRaster* 	(*RwRasterSubRaster)(RwRaster * subRaster, RwRaster * raster, RwRect * rect);
RwRaster* 	(*RwRasterRenderFast)(RwRaster * raster, RwInt32 x, RwInt32 y);
RwRaster* 	(*RwRasterRender)(RwRaster * raster, RwInt32 x, RwInt32 y);
RwRaster* 	(*RwRasterRenderScaled)(RwRaster * raster, RwRect * rect);
RwRaster* 	(*RwRasterPushContext)(RwRaster * raster);
RwRaster* 	(*RwRasterPopContext)(void);
RwRaster* 	(*RwRasterGetCurrentContext)(void);
RwBool 		(*RwRasterClear)(RwInt32 pixelValue);
RwBool 		(*RwRasterClearRect)(RwRect * rpRect, RwInt32 pixelValue);
RwRaster* 	(*RwRasterShowRaster)(RwRaster * raster, void *dev, RwUInt32 flags);
RwUInt8* 	(*RwRasterLock)(RwRaster * raster, RwUInt8 level, RwInt32 lockMode);
RwRaster* 	(*RwRasterUnlock)(RwRaster * raster);
RwUInt8* 	(*RwRasterLockPalette)(RwRaster * raster, RwInt32 lockMode);
RwRaster* 	(*RwRasterUnlockPalette)(RwRaster * raster);
RwImage* 	(*RwImageCreate)(RwInt32 width, RwInt32 height, RwInt32 depth);
RwBool 		(*RwImageDestroy)(RwImage * image);
RwImage* 	(*RwImageAllocatePixels)(RwImage * image);
RwImage* 	(*RwImageFreePixels)(RwImage * image);
RwImage* 	(*RwImageCopy)(RwImage * destImage, const RwImage * sourceImage);
RwImage* 	(*RwImageResize)(RwImage * image, RwInt32 width, RwInt32 height);
RwImage* 	(*RwImageApplyMask)(RwImage * image, const RwImage * mask);
RwImage* 	(*RwImageMakeMask)(RwImage * image);
RwImage* 	(*RwImageReadMaskedImage)(const RwChar * imageName, const RwChar * maskname);
RwImage* 	(*RwImageRead)(const RwChar * imageName);
RwImage* 	(*RwImageWrite)(RwImage * image, const RwChar * imageName);
RwImage* 	(*RwImageSetFromRaster)(RwImage *image, RwRaster *raster);
RwRaster* 	(*RwRasterSetFromImage)(RwRaster *raster, RwImage *image);
RwRaster* 	(*RwRasterRead)(const RwChar *filename);
RwRaster* 	(*RwRasterReadMaskedRaster)(const RwChar *filename, const RwChar *maskname);
RwImage* 	(*RwImageFindRasterFormat)(RwImage *ipImage, RwInt32 nRasterType, RwInt32 *npWidth, RwInt32 *npHeight, RwInt32 *npDepth,RwInt32 *npFormat);

/* rwlpcore.h */
RwReal 		(*RwIm2DGetNearScreenZ)(void);
RwReal 		(*RwIm2DGetFarScreenZ)(void);
RwBool 		(*RwRenderStateGet)(RwRenderState state, void *value);
RwBool 		(*RwRenderStateSet)(RwRenderState state, void *value);
RwBool 		(*RwIm2DRenderLine)(RwIm2DVertex *vertices, RwInt32 numVertices, RwInt32 vert1, RwInt32 vert2);
RwBool 		(*RwIm2DRenderTriangle)(RwIm2DVertex *vertices, RwInt32 numVertices, RwInt32 vert1, RwInt32 vert2, RwInt32 vert3 );
RwBool 		(*RwIm2DRenderPrimitive)(RwPrimitiveType primType, RwIm2DVertex *vertices, RwInt32 numVertices);
RwBool 		(*RwIm2DRenderIndexedPrimitive)(RwPrimitiveType primType, RwIm2DVertex *vertices, RwInt32 numVertices, RwImVertexIndex *indices, RwInt32 numIndices);

/* rtpng.h */
RwImage*	(*RtPNGImageWrite)(RwImage* image, const RwChar* imageName);
RwImage* 	(*RtPNGImageRead)(const RwChar* imageName);

RwBool (*RwTextureDestroy)(RwTexture* texture);
void WriteMemory(uintptr_t dest, uintptr_t src, size_t size);
void InitRenderWareFunctions()
{
	Log("Initializing RenderWare..");

	/* skeleton.h */
	RsGlobal = (RsGlobalType*)(g_libGTASA+0x009FC8FC);

	/* rwCore.h */
	*(void**)(&RwCameraBeginUpdate) 			= (void*)(g_libGTASA+0x001D5A18+1);
	*(void**)(&RwCameraEndUpdate) 				= (void*)(g_libGTASA+0x001D5A14+1);
	*(void**)(&RwCameraShowRaster)				= (void*)(g_libGTASA+0x001D5D14+1);

	*(void **)(&RwRasterCreate) 				= (void*)(g_libGTASA+0x001DA9D0+1);
	*(void **)(&RwRasterDestroy) 				= (void*)(g_libGTASA+0x001DA7D0+1);
	*(void **)(&RwRasterGetOffset) 				= (void*)(g_libGTASA+0x001DA6AC+1);
	*(void **)(&RwRasterGetNumLevels) 			= (void*)(g_libGTASA+0x001DA900+1);
	*(void **)(&RwRasterSubRaster) 				= (void*)(g_libGTASA+0x001DA974+1);
	*(void **)(&RwRasterRenderFast)				= (void*)(g_libGTASA+0x001DA734+1);
	*(void **)(&RwRasterRender)					= (void*)(g_libGTASA+0x001DA780+1);
	*(void **)(&RwRasterRenderScaled)			= (void*)(g_libGTASA+0x001DA68C+1);
	*(void **)(&RwRasterPushContext)			= (void*)(g_libGTASA+0x001DA818+1);
	*(void **)(&RwRasterPopContext)				= (void*)(g_libGTASA+0x001DA8B8+1);
	*(void **)(&RwRasterGetCurrentContext)		= (void*)(g_libGTASA+0x001DA66C+1);
	*(void **)(&RwRasterClear)					= (void*)(g_libGTASA+0x001DA6DC+1);
	*(void **)(&RwRasterClearRect)				= (void*)(g_libGTASA+0x001DA760+1);
	*(void **)(&RwRasterShowRaster)				= (void*)(g_libGTASA+0x001DA93C+1);
	*(void **)(&RwRasterLock)					= (void*)(g_libGTASA+0x001DAA74+1);
	*(void **)(&RwRasterUnlock)					= (void*)(g_libGTASA+0x001DA6B8+1);
	*(void **)(&RwRasterLockPalette)			= (void*)(g_libGTASA+0x001DA88C+1);
	*(void **)(&RwRasterUnlockPalette)			= (void*)(g_libGTASA+0x001DA7AC+1);
	*(void **)(&RwImageCreate)					= (void*)(g_libGTASA+0x001D8E20+1);
	*(void **)(&RwImageDestroy)					= (void*)(g_libGTASA+0x001D8E78+1);
	*(void **)(&RwImageAllocatePixels)			= (void*)(g_libGTASA+0x001D8F04+1);
	*(void **)(&RwImageFreePixels)				= (void*)(g_libGTASA+0x001D8ED8+1);
	*(void **)(&RwImageCopy)					= (void*)(g_libGTASA+0x001D9560+1);
	*(void **)(&RwImageResize)					= (void*)(g_libGTASA+0x001D8FA0+1);
	*(void **)(&RwImageApplyMask)				= (void*)(g_libGTASA+0x001D9280+1);
	*(void **)(&RwImageMakeMask)				= (void*)(g_libGTASA+0x001D90C8+1);
	*(void **)(&RwImageReadMaskedImage)			= (void*)(g_libGTASA+0x001D9DDC+1);
	*(void **)(&RwImageRead)					= (void*)(g_libGTASA+0x001D97AC+1);
	*(void **)(&RwImageWrite)					= (void*)(g_libGTASA+0x001D9D40+1);
	*(void **)(&RwImageSetFromRaster)			= (void*)(g_libGTASA+0x001DA454+1);
	*(void **)(&RwRasterSetFromImage)			= (void*)(g_libGTASA+0x001DA478+1);
	*(void **)(&RwRasterRead)					= (void*)(g_libGTASA+0x001DA574+1);
	*(void **)(&RwRasterReadMaskedRaster)		= (void*)(g_libGTASA+0x001DA614+1);
	*(void **)(&RwImageFindRasterFormat)		= (void*)(g_libGTASA+0x001DA49C+1);

	/* rwlpcore.h */
	*(void **)(&RwIm2DGetNearScreenZ)			= (void*)(g_libGTASA+0x001E2874+1);
	*(void **)(&RwIm2DGetFarScreenZ)			= (void*)(g_libGTASA+0x001E2884+1);
	*(void **)(&RwRenderStateGet)				= (void*)(g_libGTASA+0x001E28C8+1);
	*(void **)(&RwRenderStateSet)				= (void*)(g_libGTASA+0x001E2894+1);
	*(void **)(&RwIm2DRenderLine)				= (void*)(g_libGTASA+0x001E28D8+1);
	*(void **)(&RwIm2DRenderTriangle)			= (void*)(g_libGTASA+0x001E28F0+1);
	*(void **)(&RwIm2DRenderPrimitive)			= (void*)(g_libGTASA+0x001E2908+1);
	*(void **)(&RwIm2DRenderIndexedPrimitive)	= (void*)(g_libGTASA+0x001E2918+1);

	/* rtpng.h */
	*(void **)(&RtPNGImageWrite)				= (void*)(g_libGTASA+0x0020A144+1);
	*(void **)(&RtPNGImageRead)					= (void*)(g_libGTASA+0x0020A3F4+1);

	*(void**)(&RwTextureDestroy) = (void*)(g_libGTASA + 0x001DB6E4 + 1);

//	CHook::WriteMemory(g_libGTASA + 0x001A7EF2, "\x4F\xF4\x40\x10\x4F\xF4\x40\x10", 8);
//	CHook::WriteMemory(g_libGTASA + 0x001A7F34, "\x4F\xF4\x40\x10\x4F\xF4\x40\x10", 8);
}