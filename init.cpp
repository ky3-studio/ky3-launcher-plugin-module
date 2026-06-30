#include "init.h"
#include "core/state.h"
#include "core/log.h"
#include "features/fps.h"
#include "features/visual.h"
#include "features/fov.h"
#include "features/no_grass.h"
#include "features/ui_hide.h"
#include "features/craft.h"
#include "features/cooking.h"
#include "features/expedition.h"
#include "features/team.h"
#include "features/mobile_ui.h"
#include <Windows.h>
#include <Psapi.h>

bool InitGameHooks() {
    HMODULE hModule = GetModuleHandleW(NULL);
    if (hModule) {
        MODULEINFO modinfo = {};
        if (K32GetModuleInformation(GetCurrentProcess(), hModule, &modinfo, sizeof(modinfo))) {
            g_gameModBase = (uintptr_t)modinfo.lpBaseOfDll;
            g_gameModSize = modinfo.SizeOfImage;
            Log("[INIT] Game module: base=0x%llX size=0x%X", (unsigned long long)g_gameModBase, (unsigned int)g_gameModSize);
        }
    }

    Log("[INIT] --- Begin signature scanning ---");
    InitFps();
    InitVisual();
    InitFov();
    InitNoGrass();
    InitUIHiding();
    InitCraft();
    InitCooking();
    InitExpedition();
    InitTeam();
    InitMobileUI();
    Log("[INIT] --- Signature scanning complete ---");

    Log("[INIT] Results: SetFrame=%s GetFrame=%s FOV=%s",
        g_Set_FrameCount ? "OK" : "FAIL",
        g_oGetFrameCount ? "OK" : "FAIL",
        g_oChangeFOV ? "OK" : "FAIL");

    return (g_Set_FrameCount || g_oGetFrameCount || g_oChangeFOV);
}
