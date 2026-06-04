#pragma once

static float g_lastFov = 0;

int __fastcall HookChangeFOV(__int64 a1, float fov) {
    if (!g_inGameWorld) g_inGameWorld = true;

    if (g_config.enableFov) {
        if (fov >= 35.0f && fov <= 70.0f) {
            return g_oChangeFOV(a1, g_config.targetFov);
        }
    }

    return g_oChangeFOV(a1, fov);
}

__declspec(align(16)) static uint8_t g_fakeFogData[512];
int __fastcall HookDisplayFog(__int64 a1, __int64 a2) {
    if (g_config.disableFog && a2) {
        memcpy(g_fakeFogData, (void*)a2, 404);
        for (int i = 8; i <= 400; i += 4)
            *(float*)(g_fakeFogData + i) = 0.0f;
        return g_oDisplayFog(a1, (uintptr_t)g_fakeFogData);
    }
    return g_oDisplayFog(a1, a2);
}

static bool g_perspectiveLogged = false;
void* __fastcall HookPlayer_Perspective(void* RCX, float Display, void* R8) {
    if (g_config.disableCharFade) Display = 1.0f;
    return g_oPlayer_Perspective(RCX, Display, R8);
}

typedef void(__fastcall *ShowDamage_t)(void*, int, int, int, float, void*, void*, void*, int);
ShowDamage_t g_oShowDamage = nullptr;
void __fastcall HookShowDamage(void* a, int b, int c, int d, float e, void* f, void* g, void* h, int i) {
    if (g_config.disableDamageText) return;
    if (g_oShowDamage) g_oShowDamage(a, b, c, d, e, f, g, h, i);
}

typedef bool(__fastcall *EventCamera_t)(void*, void*);
EventCamera_t g_oEventCamera = nullptr;
bool __fastcall HookEventCamera(void* a, void* b) {
    if (g_config.disableCameraAnim) return true;
    if (g_oEventCamera) return g_oEventCamera(a, b);
    return true;
}

void InitVisual() {
    using namespace PatternScanner;
    uintptr_t addr;

    addr = ScanNullTerminated(Signatures::FOV);
    if (addr && IsValidFunctionPrologue(addr)) {
        MH_CreateHook((void*)addr, HookChangeFOV, (void**)&g_oChangeFOV);
    }

    addr = ScanNullTerminated(Signatures::Fog);
    if (addr) MH_CreateHook((void*)addr, HookDisplayFog, (void**)&g_oDisplayFog);

    addr = ScanNullTerminated(Signatures::Perspective);
    if (addr) {
        uintptr_t funcAddr = ResolveCall(addr);
        if (funcAddr) MH_CreateHook((void*)funcAddr, HookPlayer_Perspective, (void**)&g_oPlayer_Perspective);
    }

    addr = ScanNullTerminated(Signatures::DamageText);
    if (addr) MH_CreateHook((void*)addr, HookShowDamage, (void**)&g_oShowDamage);

    addr = ScanNullTerminated(Signatures::EventCamera);
    if (addr) MH_CreateHook((void*)addr, HookEventCamera, (void**)&g_oEventCamera);
}
