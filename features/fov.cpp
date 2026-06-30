#include "fov.h"
#include "no_grass.h"
#include "ui_hide.h"
#include "craft.h"
#include "cooking.h"
#include "expedition.h"
#include "forge.h"
#include "mobile_ui.h"
#include "../core/state.h"
#include "../core/config.h"
#include "../core/log.h"
#include "../core/safe.h"
#include "../core/scanner.h"
#include "../core/signatures.h"
#include "../MinHook/MinHook.h"
#include <cstring>

static void __declspec(noinline) SafeCall_SetFrameCount(int fps) {
    __try { g_Set_FrameCount(fps); } __except(EXCEPTION_EXECUTE_HANDLER) {}
}

static void __declspec(noinline) SafeCall_SetSyncCount() {
    __try { g_Set_SyncCount(0); } __except(EXCEPTION_EXECUTE_HANDLER) {}
}

static void __declspec(noinline) SafeCall_Craft() {
    __try { DoCraftLogic(); } __except(EXCEPTION_EXECUTE_HANDLER) {}
}

static void __declspec(noinline) SafeCall_Dispatch() {
    __try { DoDispatchLogic(); } __except(EXCEPTION_EXECUTE_HANDLER) {}
}

static void __declspec(noinline) SafeCall_Cooking() {
    __try { DoCookingLogic(); } __except(EXCEPTION_EXECUTE_HANDLER) {}
}

static void __declspec(noinline) SafeCall_Forge() {
    __try { DoForgeLogic(); } __except(EXCEPTION_EXECUTE_HANDLER) {}
}

static void __declspec(noinline) SafeCall_NoGrass() {
    __try { UpdateNoGrass(); } __except(EXCEPTION_EXECUTE_HANDLER) {}
}

static void __declspec(noinline) SafeCall_HideUID() {
    __try { TryHideGameUID(); } __except(EXCEPTION_EXECUTE_HANDLER) {}
}

static void __declspec(noinline) SafeCall_HideMenuUID() {
    __try { TryHideMenuUID(); } __except(EXCEPTION_EXECUTE_HANDLER) {}
}

static void __declspec(noinline) SafeCall_HideQuest() {
    __try { TryHideQuestBannerUI(); } __except(EXCEPTION_EXECUTE_HANDLER) {}
}

static void __declspec(noinline) SafeCall_MobileUI(uint8_t* buf, uint32_t offset) {
    __try {
        memset(buf, 0, 0x1000);
        *reinterpret_cast<uint8_t**>(buf + offset) = buf;
        ChangeMobileUI_t changeMobileUI = (ChangeMobileUI_t)g_ChangeMobileUIAddr;
        changeMobileUI(buf);
    } __except(EXCEPTION_EXECUTE_HANDLER) {}
}

int __fastcall HookChangeFOV(__int64 a1, float fov) {
    if (!g_inGameWorld) g_inGameWorld = true;

    static bool loggedFirst = false;
    if (!loggedFirst) { Log("[RUN] ChangeFOV hook first call, fov=%.1f", fov); loggedFirst = true; }

    static int frameCounter = 0;
    frameCounter++;

    static DWORD lastConfigTime = 0;
    DWORD now = GetTickCount();
    if (now - lastConfigTime > 2000) {
        ReloadConfigIfChanged();
        lastConfigTime = now;
    }

    if (g_config.enableFps && g_config.targetFps > 0 && IsValidPtr(g_Set_FrameCount)) {
        static bool fpsLogged = false;
        if (!fpsLogged) { Log("[RUN] FPS unlock active, target=%d", g_config.targetFps); fpsLogged = true; }
        SafeCall_SetFrameCount(g_config.targetFps);
    }

    if (g_config.disableVSync && IsValidPtr(g_Set_SyncCount)) {
        static bool vsyncLogged = false;
        if (!vsyncLogged) { Log("[RUN] VSync disabled"); vsyncLogged = true; }
        SafeCall_SetSyncCount();
    }

    static int mobileUIDelay = 0;
    static uint8_t localMobileUIBuffer[0x1000] = {0};
    static bool lastTouchScreenState = false;

    if (g_config.touchScreen != lastTouchScreenState) {
        if (g_config.touchScreen && g_ChangeMobileUIAddr && !g_mobileUIApplied) {
            if (++mobileUIDelay > 300) {
                Log("[RUN] MobileUI applying...");
                const uint32_t offsets[] = { 0x210, 0x218, 0x220, 0x228, 0x118, 0x120, 0x128, 0x130, 0x138, 0x140, 0x148, 0x150 };
                bool success = false;
                for (int i = 0; i < sizeof(offsets)/sizeof(offsets[0]) && !success; i++) {
                    SafeCall_MobileUI(localMobileUIBuffer, offsets[i]);
                    if (localMobileUIBuffer[0] != 0 || *reinterpret_cast<uint8_t**>(localMobileUIBuffer + offsets[i]) == localMobileUIBuffer) {
                        success = true;
                        Log("[RUN] MobileUI success at offset 0x%X", offsets[i]);
                    }
                }
                if (!success) Log("[RUN] MobileUI FAILED all offsets");
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
        Log("[RUN] Craft triggered");
        SafeCall_Craft();
    }
    if (g_triggerDispatch.exchange(false)) {
        Log("[RUN] Dispatch triggered");
        SafeCall_Dispatch();
    }
    if (g_triggerCooking.exchange(false)) {
        Log("[RUN] Cooking triggered");
        SafeCall_Cooking();
    }
    if (g_triggerForge.exchange(false)) {
        Log("[RUN] Forge triggered");
        SafeCall_Forge();
    }

    SafeCall_NoGrass();
    SafeCall_HideUID();
    SafeCall_HideMenuUID();
    SafeCall_HideQuest();

    if (frameCounter >= 100) {
        frameCounter = 0;
    }

    if (g_config.enableFov && fov >= 35.0f && fov <= 70.0f) {
        if (IsValidPtr(g_oChangeFOV)) return g_oChangeFOV(a1, g_config.targetFov);
    }

    if (IsValidPtr(g_oChangeFOV)) return g_oChangeFOV(a1, fov);
    return 0;
}

void InitFov() {
    using namespace PatternScanner;
    uintptr_t addr;

    addr = ScanLogValidated(Signatures::FOV, "FOV");
    if (addr) {
        MH_CreateHook((void*)addr, HookChangeFOV, (void**)&g_oChangeFOV);
    }
}
