#pragma once

static bool g_gameFullyLoaded = true;
static uintptr_t g_openTeamScanAddr = 0;

typedef bool (*CheckCanEnter_fn)();
typedef void (*OpenTeamPage_fn)(bool);

void __fastcall HookOpenTeam() {
    if (g_config.removeTeamAnim && g_CheckCanEnterAddr && g_OpenTeamPageAddr) {
        __try {
            auto checkCanEnter = (CheckCanEnter_fn)g_CheckCanEnterAddr;
            auto openTeamPage = (OpenTeamPage_fn)g_OpenTeamPageAddr;
            bool canEnter = false;
            __try { canEnter = checkCanEnter(); } __except(EXCEPTION_EXECUTE_HANDLER) { canEnter = false; }
            if (canEnter) {
                __try { openTeamPage(false); } __except(EXCEPTION_EXECUTE_HANDLER) {}
                return;
            }
        } __except(EXCEPTION_EXECUTE_HANDLER) {}
    }
    if (g_oOpenTeam) g_oOpenTeam();
}
