#pragma once
#ifndef UTF8_TO_CP1251_H_20170402_195220
#define UTF8_TO_CP1251_H_20170402_195220

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
    extern "C" {
#endif

int convertUtf8ToCp1251(const char * utf8, char * cp1251);

typedef struct {
    uint8_t cp1251;
    uint32_t unicode;
} Cp1251;

void setCustomCp1251Table(const Cp1251 * table, const size_t count);

#ifdef __cplusplus
    }
#endif

#endif /* #ifndef UTF8_TO_CP1251_H_20170402_195220 */
