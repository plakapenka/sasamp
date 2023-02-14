//
// Created by plaka on 24.01.2023.
//

#ifndef LIVERUSSIA_CLOADER_H
#define LIVERUSSIA_CLOADER_H


static class CLoader {

public:
    static void loadBassLib();

    static void initCrashLytics();

    static void *loadSettingThread(void *p);

    static void loadSetting();

    static void initJavaClasses(JavaVM *pjvm);
};


#endif //LIVERUSSIA_CLOADER_H
