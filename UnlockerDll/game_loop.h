#pragma once

__int64 __fastcall HookGameUpdate(__int64 a1, const char* a2) {
    if (!g_running) return g_oGameUpdate(a1, a2);

    __int64 result = g_oGameUpdate(a1, a2);

    static DWORD lastConfigTime = 0;
    DWORD now = GetTickCount();
    if (now - lastConfigTime > 100) {
        LoadConfig();
        lastConfigTime = now;
    }

    if (g_config.enableFps && g_config.targetFps > 0) {
        if (g_Set_FrameCount) g_Set_FrameCount(g_config.targetFps);
        if (g_TargetFrameRate) {
            __try { *(volatile int*)g_TargetFrameRate = g_config.targetFps; } __except(EXCEPTION_EXECUTE_HANDLER) {}
        }
    }

    static bool vsyncApplied = false;
    if (g_config.disableVSync && g_Set_SyncCount && !vsyncApplied) {
        g_Set_SyncCount(0);
        vsyncApplied = true;
    } else if (!g_config.disableVSync && vsyncApplied) {
        if (g_Set_SyncCount) g_Set_SyncCount(1);
        vsyncApplied = false;
    }

    static int mobileUIDelay = 0;
    static uint8_t localMobileUIBuffer[0x1000] = {0};
    static bool lastTouchScreenState = false;

    if (g_config.touchScreen != lastTouchScreenState) {
        if (g_config.touchScreen && g_ChangeMobileUIAddr && !g_mobileUIApplied) {
            if (++mobileUIDelay > 300) {
                const uint32_t offsets[] = { 0x210, 0x218, 0x220, 0x228, 0x118, 0x120, 0x128, 0x130, 0x138, 0x140, 0x148, 0x150 };
                bool success = false;
                for (int i = 0; i < sizeof(offsets)/sizeof(offsets[0]) && !success; i++) {
                    __try {
                        memset(localMobileUIBuffer, 0, sizeof(localMobileUIBuffer));
                        *reinterpret_cast<uint8_t**>(localMobileUIBuffer + offsets[i]) = localMobileUIBuffer;
                        ChangeMobileUI_t changeMobileUI = (ChangeMobileUI_t)g_ChangeMobileUIAddr;
                        changeMobileUI(localMobileUIBuffer);
                        success = true;
                    } __except(EXCEPTION_EXECUTE_HANDLER) {}
                }
                g_mobileUIApplied = true;
                lastTouchScreenState = true;
            }
        } else if (!g_config.touchScreen && g_mobileUIApplied) {
            g_mobileUIApplied = false;
            mobileUIDelay = 0;
            lastTouchScreenState = false;
        } else if (!g_config.touchScreen) {
            lastTouchScreenState = false;
        }
    }

    if (g_triggerCraft.exchange(false)) {
        DoCraftLogic();
    }

    if (g_triggerDispatch.exchange(false)) {
        DoDispatchLogic();
    }

    if (g_triggerCooking.exchange(false)) {
        DoCookingLogic();
    }

    if (g_triggerForge.exchange(false)) {
        DoForgeLogic();
    }

    TryHideGameUID();
    TryHideMenuUID();
    TryHideQuestBannerUI();

    return result;
}
