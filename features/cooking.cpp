#include "cooking.h"
#include "../core/state.h"
#include "../core/config.h"
#include "../core/log.h"
#include "../core/safe.h"
#include "../core/scanner.h"
#include "../core/signatures.h"
#include "../MinHook/MinHook.h"
#include <cstring>

static uintptr_t g_CookingHandlerAddr = 0;
static uintptr_t g_CookPatch_EntityVal = 0;
static uintptr_t g_CookPatch_PathB     = 0;
static uintptr_t g_CookPatch_BplSkip   = 0;
static uintptr_t g_CookPatch_NullChk1  = 0;
static uintptr_t g_CookPatch_NullChk2  = 0;
static uintptr_t g_CookPatch_NullTgt1  = 0;
static uintptr_t g_CookPatch_NullTgt2  = 0;
static uintptr_t g_CookPatch_FireWrite = 0;
static uint32_t  g_CookV2_FireState    = 0;
static uint32_t  g_CookV2_FireParam    = 0;
static uint32_t  g_CookV2_EntityRef    = 0;
static bool      g_CookPatchReady      = false;
static volatile LONG g_cookPatchLock   = 0;

static BYTE g_CookSnap_EntityVal[9]    = {};
static BYTE g_CookSnap_BplSkip[1]      = {};
static BYTE g_CookSnap_NullChk1[6]     = {};
static BYTE g_CookSnap_NullChk2[6]     = {};
static BYTE g_CookHandlerPrologue[8]   = {};

static uintptr_t g_CookingShowPageAddr = 0;
static volatile bool g_CookingHookActive = false;

typedef __int64(__fastcall *tCookingShowPage)(__int64, __int64);
static tCookingShowPage g_oCookingShowPage = nullptr;

static __int64 __fastcall HookCookingShowPage(__int64 a1, __int64 a2) {
    if (g_CookingHookActive && a1) {
        g_CookingHookActive = false;
        __try {
            uintptr_t v1 = *(uintptr_t*)(a1 + 0x20);
            if (v1) {
                uintptr_t v2 = *(uintptr_t*)(v1 + 0x10);
                if (v2) {
                    uint32_t oFS = g_CookV2_FireState  ? g_CookV2_FireState  : 0x248;
                    uint32_t oFP = g_CookV2_FireParam  ? g_CookV2_FireParam  : 0x250;
                    uint32_t oER = g_CookV2_EntityRef  ? g_CookV2_EntityRef  : 0xA0;
                    *(uint32_t*) (v2 + oFS) = 0x3F800000;
                    *(uint32_t*) (v2 + oFP) = 0x3F800000;
                    *(uintptr_t*)(v2 + oER) = 0;
                }
            }
        } __except(EXCEPTION_EXECUTE_HANDLER) {}
    }
    return g_oCookingShowPage(a1, a2);
}

void DoCookingLogic() {
    if (!g_CookingHandlerAddr || !g_CookPatchReady) return;
    if (InterlockedCompareExchange(&g_cookPatchLock, 1, 0) != 0) return;

    if (memcmp((void*)g_CookingHandlerAddr, g_CookHandlerPrologue, 8) != 0) {
        g_CookPatchReady = false;
        InterlockedExchange(&g_cookPatchLock, 0);
        return;
    }

    if (memcmp((void*)g_CookPatch_EntityVal, g_CookSnap_EntityVal, 9) != 0 ||
        memcmp((void*)g_CookPatch_BplSkip,   g_CookSnap_BplSkip,   1) != 0 ||
        memcmp((void*)g_CookPatch_NullChk1,  g_CookSnap_NullChk1,  6) != 0 ||
        memcmp((void*)g_CookPatch_NullChk2,  g_CookSnap_NullChk2,  6) != 0) {
        g_CookPatchReady = false;
        InterlockedExchange(&g_cookPatchLock, 0);
        return;
    }

    BYTE orig_ev[9], orig_bpl[1], orig_nc1[6], orig_nc2[6];
    memcpy(orig_ev,  (void*)g_CookPatch_EntityVal, 9);
    memcpy(orig_bpl, (void*)g_CookPatch_BplSkip,   1);
    memcpy(orig_nc1, (void*)g_CookPatch_NullChk1,  6);
    memcpy(orig_nc2, (void*)g_CookPatch_NullChk2,  6);

    uintptr_t lo  = g_CookPatch_EntityVal;
    uintptr_t hi  = g_CookPatch_FireWrite + 19;
    SIZE_T    len = (SIZE_T)(hi - lo);
    DWORD     oldProt;

    VirtualProtect((void*)lo, len, PAGE_EXECUTE_READWRITE, &oldProt);

    {
        int32_t disp = (int32_t)(g_CookPatch_PathB - (g_CookPatch_EntityVal + 5));
        BYTE patch[9] = { 0xE9, 0, 0, 0, 0, 0x90, 0x90, 0x90, 0x90 };
        memcpy(patch + 1, &disp, 4);
        memcpy((void*)g_CookPatch_EntityVal, patch, 9);
    }

    *(BYTE*)g_CookPatch_BplSkip = 0xEB;

    {
        int32_t disp = (int32_t)(g_CookPatch_NullTgt2 - (g_CookPatch_NullChk2 + 6));
        BYTE patch[6] = { 0x0F, 0x84, 0, 0, 0, 0 };
        memcpy(patch + 2, &disp, 4);
        memcpy((void*)g_CookPatch_NullChk2, patch, 6);
    }

    {
        int32_t disp = (int32_t)(g_CookPatch_NullTgt1 - (g_CookPatch_NullChk1 + 6));
        BYTE patch[6] = { 0x0F, 0x84, 0, 0, 0, 0 };
        memcpy(patch + 2, &disp, 4);
        memcpy((void*)g_CookPatch_NullChk1, patch, 6);
    }

    VirtualProtect((void*)lo, len, oldProt, &oldProt);
    FlushInstructionCache(GetCurrentProcess(), (void*)lo, len);

    typedef bool(__fastcall *tCookingHandler)(__int64, __int64);
    auto handler = (tCookingHandler)g_CookingHandlerAddr;
    static BYTE dummyCtx[4096]  = {};
    static BYTE dummyData[4096] = {};

    g_CookingHookActive = true;
    __try {
        handler((__int64)dummyCtx, (__int64)dummyData);
    } __except(EXCEPTION_EXECUTE_HANDLER) {}
    g_CookingHookActive = false;

    VirtualProtect((void*)lo, len, PAGE_EXECUTE_READWRITE, &oldProt);
    memcpy((void*)g_CookPatch_EntityVal, orig_ev,  9);
    memcpy((void*)g_CookPatch_BplSkip,   orig_bpl, 1);
    memcpy((void*)g_CookPatch_NullChk1,  orig_nc1, 6);
    memcpy((void*)g_CookPatch_NullChk2,  orig_nc2, 6);
    VirtualProtect((void*)lo, len, oldProt, &oldProt);
    FlushInstructionCache(GetCurrentProcess(), (void*)lo, len);

    InterlockedExchange(&g_cookPatchLock, 0);
}

static bool InitCookingPatchSites(uintptr_t h) {
    uintptr_t hEnd = h + 0x600;

    for (uintptr_t s = h + 0x200; s < hEnd - 30; ++s) {
        if (*(BYTE*)s        == 0x48 && *(BYTE*)(s + 1)  == 0x8B && *(BYTE*)(s + 2)  == 0x0D &&
            *(BYTE*)(s + 7)  == 0xE8 &&
            *(BYTE*)(s + 12) == 0x48 && *(BYTE*)(s + 13) == 0x89 && *(BYTE*)(s + 14) == 0xC3 &&
            *(BYTE*)(s + 15) == 0x48 && *(BYTE*)(s + 16) == 0x8B && *(BYTE*)(s + 17) == 0x0D &&
            *(BYTE*)(s + 22) == 0xE8 &&
            *(BYTE*)(s + 27) == 0x48 && *(BYTE*)(s + 28) == 0x89 && *(BYTE*)(s + 29) == 0xC6) {
            g_CookPatch_PathB = s;
            break;
        }
    }
    if (!g_CookPatch_PathB) return false;

    for (uintptr_t s = h + 0x100; s < g_CookPatch_PathB; ++s) {
        if (*(BYTE*)s       == 0x48 && *(BYTE*)(s + 1) == 0x85 && *(BYTE*)(s + 2) == 0xDB &&
            *(BYTE*)(s + 3) == 0x0F && *(BYTE*)(s + 4) == 0x84) {
            g_CookPatch_EntityVal = s;
        }
    }
    if (!g_CookPatch_EntityVal) return false;

    for (uintptr_t s = g_CookPatch_PathB; s < hEnd - 19; ++s) {
        if (*(BYTE*)s        == 0x89 && *(BYTE*)(s + 1)  == 0x86 &&
            *(BYTE*)(s + 4)  == 0x00 && *(BYTE*)(s + 5)  == 0x00 &&
            *(BYTE*)(s + 6)  == 0x89 && *(BYTE*)(s + 7)  == 0x8E &&
            *(BYTE*)(s + 10) == 0x00 && *(BYTE*)(s + 11) == 0x00 &&
            *(BYTE*)(s + 12) == 0x4C && *(BYTE*)(s + 13) == 0x89 && *(BYTE*)(s + 14) == 0xB6 &&
            *(BYTE*)(s + 18) == 0x00) {
            g_CookPatch_FireWrite = s;
            g_CookV2_FireState = *(uint16_t*)(s + 2);
            g_CookV2_FireParam = *(uint16_t*)(s + 8);
            g_CookV2_EntityRef = *(uint16_t*)(s + 15);
            break;
        }
    }
    if (!g_CookPatch_FireWrite) return false;

    for (uintptr_t s = g_CookPatch_FireWrite - 1; s > g_CookPatch_FireWrite - 0x20 && s > h; --s) {
        if (*(BYTE*)s       == 0x40 && *(BYTE*)(s + 1) == 0x84 &&
            *(BYTE*)(s + 2) == 0xED && *(BYTE*)(s + 3) == 0x75) {
            g_CookPatch_BplSkip = s + 3;
            break;
        }
    }
    if (!g_CookPatch_BplSkip) return false;

    int nullCount = 0;
    for (uintptr_t s = g_CookPatch_PathB; s < g_CookPatch_FireWrite; ++s) {
        if (*(BYTE*)s       == 0x48 && *(BYTE*)(s + 1) == 0x85 && *(BYTE*)(s + 2) == 0xC0 &&
            *(BYTE*)(s + 3) == 0x0F && *(BYTE*)(s + 4) == 0x84) {
            ++nullCount;
            if (nullCount == 1) {
                g_CookPatch_NullChk1 = s + 3;
                for (uintptr_t t = s + 9; t < g_CookPatch_FireWrite; ++t) {
                    if (*(BYTE*)t       == 0x48 && *(BYTE*)(t + 1) == 0x8B && *(BYTE*)(t + 2) == 0x86 &&
                        *(BYTE*)(t + 5) == 0x00 && *(BYTE*)(t + 6) == 0x00) {
                        g_CookPatch_NullTgt1 = t;
                        break;
                    }
                }
            } else if (nullCount == 2) {
                g_CookPatch_NullChk2 = s + 3;
                for (uintptr_t t = s + 9; t < g_CookPatch_FireWrite; ++t) {
                    if (*(BYTE*)t == 0x48 && *(BYTE*)(t + 1) == 0x85 && *(BYTE*)(t + 2) == 0xDB) {
                        g_CookPatch_NullTgt2 = t;
                        break;
                    }
                }
                break;
            }
        }
    }
    if (!g_CookPatch_NullTgt1 || !g_CookPatch_NullTgt2) return false;

    for (uintptr_t s = h + 0x300; s < h + 0x600; ++s) {
        if (*(BYTE*)s       == 0xE8 && *(BYTE*)(s + 5) == 0x40 &&
            *(BYTE*)(s + 6) == 0xB6 && *(BYTE*)(s + 7) == 0x01) {
            int32_t rel = *(int32_t*)(s + 1);
            g_CookingShowPageAddr = s + 5 + rel;
            break;
        }
    }

    memcpy(g_CookHandlerPrologue, (void*)h, 8);
    memcpy(g_CookSnap_EntityVal, (void*)g_CookPatch_EntityVal, 9);
    memcpy(g_CookSnap_BplSkip,   (void*)g_CookPatch_BplSkip,   1);
    memcpy(g_CookSnap_NullChk1,  (void*)g_CookPatch_NullChk1,  6);
    memcpy(g_CookSnap_NullChk2,  (void*)g_CookPatch_NullChk2,  6);

    g_CookPatchReady = true;
    return true;
}

void InitCooking() {
    using namespace PatternScanner;

    if (!g_InnerDispatcherAddr) {
        for (int i = 0; Signatures::InnerDispatcher[i] && !g_InnerDispatcherAddr; i++) {
            uintptr_t a = Scan(Signatures::InnerDispatcher[i]);
            if (a) {
                g_InnerDispatcherAddr = a;
                Log("[SCAN] InnerDispatcher(cooking): matched pattern[%d] at 0x%llX | sig: %s", i, (unsigned long long)a, Signatures::InnerDispatcher[i]);
            }
        }
    }

    if (!g_InnerDispatcherAddr) {
        Log("[SCAN] InnerDispatcher(cooking): FAILED - no pattern matched");
        return;
    }

    uintptr_t idBase = g_InnerDispatcherAddr;
    uintptr_t hashAddr = 0;

    for (uintptr_t scan = idBase; scan < idBase + 0x20000 - 4; scan++) {
        __try {
            if (memcmp((void*)scan, Signatures::CookingHash, 4) != 0) continue;

            uintptr_t cmpStart = 0;
            uintptr_t afterCmp = 0;
            if (*(BYTE*)(scan - 2) == 0x81 && *(BYTE*)(scan - 1) == 0xF9) {
                cmpStart = scan - 2;
                afterCmp = scan + 4;
            } else if (*(BYTE*)(scan - 1) == 0x3D) {
                cmpStart = scan - 1;
                afterCmp = scan + 4;
            } else continue;

            BYTE b0 = *(BYTE*)afterCmp;
            BYTE b1 = *(BYTE*)(afterCmp + 1);
            bool isCondJmp = (b0 == 0x74) || (b0 == 0x75)
                          || (b0 == 0x0F && (b1 == 0x84 || b1 == 0x85));
            if (isCondJmp) {
                hashAddr = cmpStart;
                break;
            }
        } __except(EXCEPTION_EXECUTE_HANDLER) { break; }
    }

    if (hashAddr) {
        for (uintptr_t scan = hashAddr; scan < hashAddr + 0x200; scan++) {
            __try {
                if (*(BYTE*)scan == 0x41 && *(BYTE*)(scan + 1) == 0x5F && *(BYTE*)(scan + 2) == 0xE9) {
                    int32_t rel = *(int32_t*)(scan + 3);
                    g_CookingHandlerAddr = (scan + 2) + 5 + rel;
                    break;
                }
            } __except(EXCEPTION_EXECUTE_HANDLER) { break; }
        }
    }

    if (g_CookingHandlerAddr) {
        Log("[SCAN] CookingHandler: located at 0x%llX", (unsigned long long)g_CookingHandlerAddr);
        InitCookingPatchSites(g_CookingHandlerAddr);
        if (g_CookingShowPageAddr) {
            Log("[SCAN] CookingShowPage: located at 0x%llX", (unsigned long long)g_CookingShowPageAddr);
            MH_CreateHook((LPVOID)g_CookingShowPageAddr, HookCookingShowPage, (LPVOID*)&g_oCookingShowPage);
        }
    } else {
        Log("[SCAN] CookingHandler: FAILED - hash not found");
    }
}
