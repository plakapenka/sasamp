//
// Created by plaka on 24.01.2023.
//

#ifndef LIVERUSSIA_CLOADER_H
#define LIVERUSSIA_CLOADER_H


static class CLoader {

public:
    static void loadBassLib();

    static void redirectDirection(const char *path);

    static void initCrashLytics();
};


#endif //LIVERUSSIA_CLOADER_H
