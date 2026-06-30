#include "team.h"
#include "../core/state.h"
#include "../core/config.h"
#include "../core/log.h"
#include "../core/safe.h"
#include "../core/scanner.h"
#include "../core/signatures.h"
#include "../MinHook/MinHook.h"

typedef bool (*CheckCanEnter_fn)();
typedef void (*OpenTeamPage_fn)(bool);

void __fastcall HookOpenTeam() {
    static bool logged = false;
    if (!logged) { Log("[HOOK] OpenTeam first call"); logged = true; }
    if (g_config.removeTeamAnim) {
        auto check = (CheckCanEnter_fn)g_CheckCanEnterAddr;
        auto openPage = (OpenTeamPage_fn)g_OpenTeamPageAddr;
        if (IsValidPtr(check) && IsValidPtr(openPage)) {
            bool canEnter = false;
            __try { canEnter = check(); } __except(EXCEPTION_EXECUTE_HANDLER) {}
            if (canEnter) {
                __try { openPage(false); } __except(EXCEPTION_EXECUTE_HANDLER) {}
                return;
            }
        }
    }
    if (IsValidPtr(g_oOpenTeam)) g_oOpenTeam();
}

void InitTeam() {
    using namespace PatternScanner;

    uintptr_t addr = ScanLog(Signatures::OpenTeam, "OpenTeam");
    if (addr) {
        MH_CreateHook((void*)addr, HookOpenTeam, (void**)&g_oOpenTeam);
    }

    for (int i = 0; Signatures::CheckCanEnter[i] && !g_CheckCanEnterAddr; i++) {
        uintptr_t a = Scan(Signatures::CheckCanEnter[i]);
        if (a) {
            g_CheckCanEnterAddr = a;
            Log("[SCAN] CheckCanEnter: matched pattern[%d] at 0x%llX | sig: %s", i, (unsigned long long)a, Signatures::CheckCanEnter[i]);
        }
    }
    if (!g_CheckCanEnterAddr) Log("[SCAN] CheckCanEnter: FAILED - no pattern matched");

    for (int i = 0; Signatures::OpenTeamPage[i] && !g_OpenTeamPageAddr; i++) {
        uintptr_t a = Scan(Signatures::OpenTeamPage[i]);
        if (a) {
            g_OpenTeamPageAddr = a;
            Log("[SCAN] OpenTeamPage: matched pattern[%d] at 0x%llX | sig: %s", i, (unsigned long long)a, Signatures::OpenTeamPage[i]);
        }
    }
    if (!g_OpenTeamPageAddr) Log("[SCAN] OpenTeamPage: FAILED - no pattern matched");
}
