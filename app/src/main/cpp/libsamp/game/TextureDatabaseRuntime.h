//
// Created by plaka on 11.01.2023.
//

#ifndef LIVERUSSIA_TEXTUREDATABASERUNTIME_H
#define LIVERUSSIA_TEXTUREDATABASERUNTIME_H

#include <cstdint>

typedef uintptr_t TextureDatabase;

static class TextureDatabaseRuntime {

public:
    enum TextureDatabaseFormat
    {
        DF_UNC = 0,
        DF_DXT = 1,
        DF_360 = 2,
        DF_PS3 = 3,
        DF_PVR = 4,
        DF_Default = 5,
        DF_ETC = 6,
        DF_ALL = 7
    };

   // static unsigned int *storedTexels;
    static TextureDatabase* Load(const char *withName, bool fullyLoad, TextureDatabaseFormat forcedFormat);

    static void Register(TextureDatabase *thiz);
};


#endif //LIVERUSSIA_TEXTUREDATABASERUNTIME_H
