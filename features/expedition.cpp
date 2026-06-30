#include "expedition.h"
#include "../core/state.h"
#include "../core/config.h"
#include "../core/log.h"
#include "../core/safe.h"
#include "../core/scanner.h"
#include "../core/signatures.h"
#include <cstring>

static uintptr_t g_ExpHandlerAddr = 0;
static BYTE g_ExpHandlerPrologue[8] = {0};

void DoDispatchLogic() {
    uintptr_t handlerAddr = g_ExpHandlerAddr;
    if (!handlerAddr) return;

    __try {
        if (memcmp((void*)handlerAddr, g_ExpHandlerPrologue, 8) != 0) {
            g_ExpHandlerAddr = 0;
            return;
        }
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        g_ExpHandlerAddr = 0;
        return;
    }

    typedef bool(__fastcall *tExpHandler)(__int64 param1, __int64 param2);
    static BYTE dummyBuf[4096] = {0};

    auto handler = (tExpHandler)handlerAddr;
    __try {
        handler(0, (__int64)dummyBuf);
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        __try {
            handler(0, 0);
        } __except(EXCEPTION_EXECUTE_HANDLER) {
            g_ExpHandlerAddr = 0;
        }
    }
}

void InitExpedition() {
    using namespace PatternScanner;

    if (!g_InnerDispatcherAddr) {
        for (int i = 0; Signatures::InnerDispatcher[i] && !g_InnerDispatcherAddr; i++) {
            uintptr_t a = Scan(Signatures::InnerDispatcher[i]);
            if (a) {
                g_InnerDispatcherAddr = a;
                Log("[SCAN] InnerDispatcher(expedition): matched pattern[%d] at 0x%llX | sig: %s", i, (unsigned long long)a, Signatures::InnerDispatcher[i]);
            }
        }
    }

    if (!g_InnerDispatcherAddr) {
        Log("[SCAN] InnerDispatcher(expedition): FAILED - no pattern matched");
        return;
    }

    uintptr_t idBase = g_InnerDispatcherAddr;
    uintptr_t leafJeTarget = 0;

    for (uintptr_t scan = idBase; scan < idBase + 0x20000 - 4; scan++) {
        __try {
            if (memcmp((void*)scan, Signatures::ExpeditionHash, 4) != 0) continue;
            uintptr_t cmpAddr = 0;
            uintptr_t afterCmp = 0;
            if (*(BYTE*)(scan - 2) == 0x81 && *(BYTE*)(scan - 1) == 0xF9) {
                cmpAddr = scan - 2;
                afterCmp = scan + 4;
            } else if (*(BYTE*)(scan - 1) == 0x3D) {
                cmpAddr = scan - 1;
                afterCmp = scan + 4;
            }
            if (!cmpAddr) continue;

            BYTE b0 = *(BYTE*)afterCmp;
            BYTE b1 = *(BYTE*)(afterCmp + 1);
            bool isCondJmp = (b0 == 0x74) || (b0 == 0x75)
                          || (b0 == 0x0F && (b1 == 0x84 || b1 == 0x85));
            if (!isCondJmp) continue;

            uintptr_t ac = (*(BYTE*)cmpAddr == 0x81) ? cmpAddr + 6 : cmpAddr + 5;
            for (uintptr_t s = ac; s < ac + 10; s++) {
                if (*(BYTE*)s == 0x0F && *(BYTE*)(s+1) == 0x84) {
                    int32_t d = *(int32_t*)(s + 2);
                    leafJeTarget = s + 6 + d;
                    break;
                }
                if (*(BYTE*)s == 0x0F && *(BYTE*)(s+1) == 0x85) {
                    leafJeTarget = s + 6;
                    break;
                }
                if (*(BYTE*)s == 0x74) {
                    int8_t d = *(int8_t*)(s + 1);
                    leafJeTarget = s + 2 + d;
                    break;
                }
                if (*(BYTE*)s == 0x75) {
                    leafJeTarget = s + 2;
                    break;
                }
            }
            if (leafJeTarget) break;
        } __except(EXCEPTION_EXECUTE_HANDLER) { break; }
    }

    if (leafJeTarget) {
        for (uintptr_t scan = leafJeTarget; scan < leafJeTarget + 0x200; scan++) {
            __try {
                if (*(BYTE*)scan == 0x41 && *(BYTE*)(scan+1) == 0x5F && *(BYTE*)(scan+2) == 0xE9) {
                    int32_t rel = *(int32_t*)(scan + 3);
                    g_ExpHandlerAddr = (scan + 2) + 5 + rel;
                    break;
                }
            } __except(EXCEPTION_EXECUTE_HANDLER) { break; }
        }
    }

    if (g_ExpHandlerAddr) {
        Log("[SCAN] ExpHandler: located at 0x%llX", (unsigned long long)g_ExpHandlerAddr);
        __try {
            memcpy(g_ExpHandlerPrologue, (void*)g_ExpHandlerAddr, 8);
        } __except(EXCEPTION_EXECUTE_HANDLER) {}
    } else {
        Log("[SCAN] ExpHandler: FAILED - hash not found");
    }
}
