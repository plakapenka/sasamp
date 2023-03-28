//
// Created by plaka on 28.03.2023.
//

#ifndef LIVERUSSIA_KEYGEN_H
#define LIVERUSSIA_KEYGEN_H

#include <cstdint>

class CKeyGen {
public:
    static const uint32_t keyTable[256];

public:
    static void InjectHooks();

    [[nodiscard]] static uint32_t AppendStringToKey(uint32_t key, const char* str);
    [[nodiscard]] static uint32_t GetKey(const char* str);
    [[nodiscard]] static uint32_t GetKey(const char* str, int32_t size);
    [[nodiscard]] static uint32_t GetUppercaseKey(const char* str);
};


#endif //LIVERUSSIA_KEYGEN_H
