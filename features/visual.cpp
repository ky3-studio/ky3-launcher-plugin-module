#include "visual.h"
#include "../core/state.h"
#include "../core/config.h"
#include "../core/log.h"
#include "../core/safe.h"
#include "../core/scanner.h"
#include "../core/signatures.h"
#include "../MinHook/MinHook.h"
#include <cstring>

__declspec(align(16)) static uint8_t g_fakeFogData[256];

__int64 __fastcall HookDisplayFog(__int64 a1, __int64 a2) {
    static bool logged = false;
    if (!logged) { Log("[HOOK] Fog first call"); logged = true; }
    if (g_config.disableFog && a2) {
        __try {
            memset(g_fakeFogData, 0, sizeof(g_fakeFogData));
            memcpy(g_fakeFogData, (void*)a2, 64);
            *(int*)g_fakeFogData = 0;
            if (IsValidPtr(g_oDisplayFog)) return g_oDisplayFog(a1, (__int64)g_fakeFogData);
        } __except(EXCEPTION_EXECUTE_HANDLER) {}
    }
    if (IsValidPtr(g_oDisplayFog)) return g_oDisplayFog(a1, a2);
    return 0;
}

void __fastcall HookShowDamage(void* a, int b, int c, int d, float e, void* f, void* g, void* h, int i) {
    if (g_config.disableDamageText) return;
    if (IsValidPtr(g_oShowDamage)) g_oShowDamage(a, b, c, d, e, f, g, h, i);
}

bool __fastcall HookEventCamera(void* a, void* b) {
    if (g_config.disableCameraAnim) return true;
    if (IsValidPtr(g_oEventCamera)) return g_oEventCamera(a, b);
    return true;
}

void* __fastcall HookPlayerPerspective(void* RCX, float Display, void* R8) {
    if (g_config.disableCharFade) {
        if (IsValidPtr(g_oPlayer_Perspective)) return g_oPlayer_Perspective(RCX, 1.0f, R8);
    }
    if (IsValidPtr(g_oPlayer_Perspective)) return g_oPlayer_Perspective(RCX, Display, R8);
    return nullptr;
}

void InitVisual() {
    using namespace PatternScanner;
    uintptr_t addr;

    addr = ScanLog(Signatures::Fog, "Fog");
    if (addr) MH_CreateHook((void*)addr, HookDisplayFog, (void**)&g_oDisplayFog);

    addr = ScanLog(Signatures::DamageText, "DamageText");
    if (addr) MH_CreateHook((void*)addr, HookShowDamage, (void**)&g_oShowDamage);

    addr = ScanLog(Signatures::EventCamera, "EventCamera");
    if (addr) MH_CreateHook((void*)addr, HookEventCamera, (void**)&g_oEventCamera);

    addr = ScanLog(Signatures::Perspective, "Perspective");
    if (addr) {
        addr = ResolveCallLog(addr, "Perspective");
        if (addr) MH_CreateHook((void*)addr, HookPlayerPerspective, (void**)&g_oPlayer_Perspective);
    }
}
