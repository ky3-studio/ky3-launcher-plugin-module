#pragma once

static bool g_gameFullyLoaded = true;
static uintptr_t g_openTeamScanAddr = 0;

typedef bool (*CheckCanEnter_fn)();
typedef void (*OpenTeamPage_fn)(bool);

void __fastcall HookOpenTeam() {
    if (g_config.removeTeamAnim && g_OpenTeamPageAddr) {
        __try {
            auto openTeamPage = (OpenTeamPage_fn)g_OpenTeamPageAddr;
            openTeamPage(false);
        } __except(EXCEPTION_EXECUTE_HANDLER) {}
        return;
    }
    if (g_oOpenTeam) g_oOpenTeam();
}
