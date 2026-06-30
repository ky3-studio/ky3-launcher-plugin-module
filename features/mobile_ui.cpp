#include "mobile_ui.h"
#include "../core/state.h"
#include "../core/config.h"
#include "../core/log.h"
#include "../core/safe.h"
#include "../core/scanner.h"
#include "../core/signatures.h"

void InitMobileUI() {
    using namespace PatternScanner;

    for (int i = 0; Signatures::MobileUI[i] && !g_ChangeMobileUIAddr; i++) {
        uintptr_t a = Scan(Signatures::MobileUI[i]);
        if (a) {
            uintptr_t resolved = ResolveCall(a);
            if (resolved) {
                g_ChangeMobileUIAddr = resolved;
                Log("[SCAN] MobileUI: matched pattern[%d] at 0x%llX -> resolved 0x%llX | sig: %s", i, (unsigned long long)a, (unsigned long long)resolved, Signatures::MobileUI[i]);
            }
        }
    }
    if (!g_ChangeMobileUIAddr) Log("[SCAN] MobileUI: FAILED - no pattern matched");
}
