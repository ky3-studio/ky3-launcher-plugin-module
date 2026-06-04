#pragma once

static void __fastcall HookSetEnabled(bool enabled) {
    if (g_config.enableNoGrass) {
        if (g_oSetEnabled) g_oSetEnabled(false);
        return;
    }
    if (g_oSetEnabled) g_oSetEnabled(enabled);
}

void InitNoGrass() {
    using namespace PatternScanner;
    uintptr_t addr = ScanNullTerminated(Signatures::NoGrass);
    if (addr) {
        MH_CreateHook((void*)addr, (void*)HookSetEnabled, (void**)&g_oSetEnabled);
    }
}
