#pragma once

#include "rwlpcore.h"
#include "rwcore.h"

/* skeleton.h */
typedef struct RsGlobalType RsGlobalType;
struct RsGlobalType
{
    const RwChar *appName;
    RwInt32 maximumWidth;
    RwInt32 maximumHeight;
    // useless stuff
    // ...
};

#define RWRGBALONG(r,g,b,a) ((unsigned int) (((a) << 24) | ((r) << 16) | ((g) << 8) | (b)))
#define RWRSTATE(a) (reinterpret_cast<void *>(a))
#define TRUE true
#define FALSE false

/* rtpng.h */
extern RwImage* (*RtPNGImageWrite)(RwImage* image, const RwChar* imageName);
extern RwImage* (*RtPNGImageRead)(const RwChar* imageName);

extern RwBool (*RwTextureDestroy)(RwTexture* texture);

extern RsGlobalType* RsGlobal;