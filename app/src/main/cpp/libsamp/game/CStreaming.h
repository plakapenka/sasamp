//
// Created by plaka on 11.01.2023.
//

#ifndef LIVERUSSIA_CSTREAMING_H
#define LIVERUSSIA_CSTREAMING_H


class CStreaming {

public:
    static int AddImageToList(const char *name, bool b);

    static void RemoveAllUnusedModels();

    static void RequestModel(int index, int flags = 0xC);
};


#endif //LIVERUSSIA_CSTREAMING_H
