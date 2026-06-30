#include "no_grass.h"
#include "../core/state.h"
#include "../core/config.h"
#include "../core/log.h"
#include "../core/safe.h"
#include "../core/scanner.h"
#include "../core/signatures.h"

void __fastcall HookSetActive(void* pThis, bool active) {
    if (g_config.enableNoGrass && active && g_GetName) {
        __try {
            Il2CppString* name = g_GetName(pThis);
            if (name && name->length > 0) {
                if (wcsstr(name->chars, L"Grass") && !wcsstr(name->chars, L"Eff") && !wcsstr(name->chars, L"Monster")) {
                    return;
                }
            }
        } __except(EXCEPTION_EXECUTE_HANDLER) {}
    }
    if (g_oSetActiveOrig) g_oSetActiveOrig(pThis, active);
}

void UpdateNoGrass() {
}

void InitNoGrass() {
    using namespace PatternScanner;

    uintptr_t addr = ScanLog(Signatures::GetName, "GetName");
    if (addr) {
        g_GetName = (GetName_t)addr;
    }
}
