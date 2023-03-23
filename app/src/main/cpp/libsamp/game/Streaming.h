//
// Created by plaka on 11.01.2023.
//

#ifndef LIVERUSSIA_STREAMING_H
#define LIVERUSSIA_STREAMING_H


class CStreaming {

public:
    static int AddImageToList(const char *name, bool b);

    static void RemoveAllUnusedModels();

    static void RequestModel(int index, int flags);

    static void LoadAllRequestedModels(bool bPriorityRequestsOnly);
};


#endif //LIVERUSSIA_STREAMING_H
