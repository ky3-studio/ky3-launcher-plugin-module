#include "fps.h"
#include "../core/state.h"
#include "../core/config.h"
#include "../core/log.h"
#include "../core/safe.h"
#include "../core/scanner.h"
#include "../core/signatures.h"
#include "../MinHook/MinHook.h"

int __fastcall HookGetFrameCount() {
    static bool logged = false;
    if (!logged) { Log("[HOOK] GetFrameCount first call"); logged = true; }
    auto orig = g_oGetFrameCount;
    if (!IsValidPtr(orig)) return 60;
    int ret = 60;
    __try { ret = orig(); } __except(EXCEPTION_EXECUTE_HANDLER) { return 60; }
    if (ret >= 60) return 60;
    if (ret >= 45) return 45;
    if (ret >= 30) return 30;
    return ret;
}

void InitFps() {
    using namespace PatternScanner;
    uintptr_t addr;

    addr = ScanLog(Signatures::SetFrameCount, "SetFrameCount");
    if (addr) {
        uintptr_t target = ResolveCallLog(addr, "SetFrameCount");
        if (target) g_Set_FrameCount = (Set_FrameCount_t)target;
    }

    addr = ScanLog(Signatures::GetFrameCount, "GetFrameCount");
    if (addr) {
        uintptr_t target = ResolveCallLog(addr, "GetFrameCount");
        if (target) MH_CreateHook((void*)target, HookGetFrameCount, (void**)&g_oGetFrameCount);
    }

    addr = ScanLog(Signatures::SyncCount, "SyncCount");
    if (addr) {
        addr = ResolveCallLog(addr, "SyncCount");
        if (addr) g_Set_SyncCount = (Set_SyncCount_t)addr;
    }
}
