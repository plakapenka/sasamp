//
// Created by plaka on 20.03.2023.
//

#ifndef LIVERUSSIA_CSTACKTRACE_H
#define LIVERUSSIA_CSTACKTRACE_H

#include <dlfcn.h>
#include "../main.h"

#define PRINT_CRASH_STATES(context) \
	CrashLog("register states:"); \
	CrashLog("r0: 0x%X, r1: 0x%X, r2: 0x%X, r3: 0x%X", (context)->uc_mcontext.arm_r0, (context)->uc_mcontext.arm_r1, (context)->uc_mcontext.arm_r2, (context)->uc_mcontext.arm_r3); \
	CrashLog("r4: 0x%x, r5: 0x%x, r6: 0x%x, r7: 0x%x", (context)->uc_mcontext.arm_r4, (context)->uc_mcontext.arm_r5, (context)->uc_mcontext.arm_r6, (context)->uc_mcontext.arm_r7); \
	CrashLog("r8: 0x%x, r9: 0x%x, sl: 0x%x, fp: 0x%x", (context)->uc_mcontext.arm_r8, (context)->uc_mcontext.arm_r9, (context)->uc_mcontext.arm_r10, (context)->uc_mcontext.arm_fp); \
	CrashLog("ip: 0x%x, sp: 0x%x, lr: 0x%x, pc: 0x%x", (context)->uc_mcontext.arm_ip, (context)->uc_mcontext.arm_sp, (context)->uc_mcontext.arm_lr, (context)->uc_mcontext.arm_pc);

void CrashLog(const char* fmt, ...);

class CStackTrace
{
public:
    static void printBacktrace(ucontext_t *uContext)
    {
        CrashLog("------------ START BACKTRACE ------------");
        for (auto i = 0; i < 100; ++i)
        {
            const auto address = *reinterpret_cast<uintptr_t*>(uContext->uc_mcontext.arm_sp + 4 * i);

            CStackTrace::printAddressBacktrace(address, (void*)(uContext->uc_mcontext.arm_pc + 4 * i), (void*)(uContext->uc_mcontext.arm_lr + 4 * i));
        }
        CrashLog("------------ END BACKTRACE ------------");
    }

private:
    static void printAddressBacktrace(const unsigned address, void* pc, void* lr)
    {
        Dl_info info_pc, info_lr;

        dladdr(pc, &info_pc);
        dladdr(lr, &info_lr);

        char filename[0xFF] = { 0 },
                buffer[2048] = { 0 };

        sprintf(filename, "/proc/%d/maps", getpid());

        auto* const fp = fopen(filename, "rt");
        if (fp == nullptr)
        {
            Log("ERROR: can't open file %s", filename);
            return;
        }

        while (fgets(buffer, sizeof(buffer), fp))
        {
            const auto start_address = strtoul(buffer, nullptr, 16);
            const auto end_address = strtoul(strchr(buffer, '-') + 1, nullptr, 16);

            if (start_address <= address && end_address > address)
            {
                if (*(strchr(buffer, ' ') + 3) == 'x')
                {
                    CrashLog("Call: %x (GTA: %x PC: %s LR: %s) (SAMP: %x)",
                             address,
                             address - g_libGTASA,
                             info_pc.dli_sname,
                             info_lr.dli_sname,
                             address - CUtil::FindLibrary("libsamp.so")
                             );
                    break;
                }
            }
        }

        fclose(fp);
    }
};

#endif //LIVERUSSIA_CSTACKTRACE_H
