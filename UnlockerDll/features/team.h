#pragma once

static bool g_gameFullyLoaded = true;
static uintptr_t g_openTeamScanAddr = 0;

typedef bool (*CheckCanEnter_fn)();
typedef void (*OpenTeamPage_fn)(bool);

void __fastcall HookOpenTeam() {
    if (g_config.removeTeamAnim) {
        auto check = (CheckCanEnter_fn)g_CheckCanEnterAddr;
        auto openPage = (OpenTeamPage_fn)g_OpenTeamPageAddr;
        if (check && openPage) {
            bool canEnter = false;
            __try { canEnter = check(); } __except(EXCEPTION_EXECUTE_HANDLER) {}
            if (canEnter) {
                __try { openPage(false); } __except(EXCEPTION_EXECUTE_HANDLER) {}
                return;
            }
        }
    }
    if (g_oOpenTeam) g_oOpenTeam();
}

void InitTeam() {
    using namespace PatternScanner;

    uintptr_t addr = ScanNullTerminated(Signatures::OpenTeam);
    if (addr) {
        g_openTeamScanAddr = addr;
        MH_CreateHook((void*)addr, HookOpenTeam, (void**)&g_oOpenTeam);
    }

    for (int i = 0; Signatures::CheckCanEnter[i] && !g_CheckCanEnterAddr; i++) {
        uintptr_t a = Scan(Signatures::CheckCanEnter[i]);
        if (a) g_CheckCanEnterAddr = a;
    }

    for (int i = 0; Signatures::OpenTeamPage[i] && !g_OpenTeamPageAddr; i++) {
        uintptr_t a = Scan(Signatures::OpenTeamPage[i]);
        if (a) g_OpenTeamPageAddr = a;
    }
}
