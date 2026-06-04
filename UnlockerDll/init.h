#pragma once

bool InitGameHooks() {
    HMODULE hModule = GetModuleHandleW(NULL);
    if (hModule) {
        MODULEINFO modinfo = {};
        if (K32GetModuleInformation(GetCurrentProcess(), hModule, &modinfo, sizeof(modinfo))) {
            g_gameModBase = (uintptr_t)modinfo.lpBaseOfDll;
            g_gameModSize = modinfo.SizeOfImage;
        }
    }

    InitFps();
    InitVisual();
    InitUIHiding();
    InitCraft();
    InitCooking();
    InitExpedition();
    InitTeam();
    InitMobileUI();
    InitNoGrass();

    MH_EnableHook(MH_ALL_HOOKS);

    return (g_oGameUpdate || g_FpsAddr || g_TargetFrameRate || g_Set_FrameCount || g_oGetFrameCount || g_oChangeFOV);
}
