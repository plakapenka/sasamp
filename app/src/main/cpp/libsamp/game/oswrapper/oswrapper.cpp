//
// Created by plaka on 24.03.2023.
//

#include "oswrapper.h"
#include "../../util/patch.h"

uint64_t GetOSWPerformanceTime()
{
    return (uint64_t)(OS_TimeAccurate() * 1000000.0);
}

double OS_TimeAccurate() {
    CHook::CallFunction<double>(g_libGTASA + 0x0026A4E8 + 1);
}
