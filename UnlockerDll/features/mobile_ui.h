#pragma once

static uint8_t g_mobileUIBuffer[0x1000];

static uint32_t AnalyzeMobileUIOffset() {
    if (!g_ChangeMobileUIAddr) return 0;

    uint8_t* func = (uint8_t*)g_ChangeMobileUIAddr;

    for (int i = 0; i < 0x200; i++) {
        if (func[i] == 0xCC || func[i] == 0xC3) break;

        if (func[i] == 0x48 && func[i+1] == 0x8B) {
            uint8_t modrm = func[i+2];
            if ((modrm & 0xC7) == 0x86) {
                uint32_t offset = *(uint32_t*)(func + i + 3);
                if (offset >= 0x100 && offset <= 0x300) return offset;
            }
            if ((modrm & 0xC7) == 0x81) {
                uint32_t offset = *(uint32_t*)(func + i + 3);
                if (offset >= 0x100 && offset <= 0x300) return offset;
            }
        }

        if (func[i] == 0x48 && func[i+1] == 0x89) {
            uint8_t modrm = func[i+2];
            if ((modrm & 0xC7) == 0x86) {
                uint32_t offset = *(uint32_t*)(func + i + 3);
                if (offset >= 0x100 && offset <= 0x300) return offset;
            }
        }
    }
    return 0;
}

static bool DoApplyMobileUI() {
    __try {
        uint32_t offset = AnalyzeMobileUIOffset();
        if (offset == 0) {
            const uint32_t defaultOffsets[] = { 0x118, 0x120, 0x128, 0x130, 0x138, 0x140, 0x148, 0x150 };
            for (int i = 0; i < sizeof(defaultOffsets)/sizeof(defaultOffsets[0]); i++) {
                offset = defaultOffsets[i];
                memset(g_mobileUIBuffer, 0, sizeof(g_mobileUIBuffer));
                *reinterpret_cast<uint8_t**>(g_mobileUIBuffer + offset) = g_mobileUIBuffer;
                __try {
                    ChangeMobileUI_t changeMobileUI = (ChangeMobileUI_t)g_ChangeMobileUIAddr;
                    changeMobileUI(g_mobileUIBuffer);
                    return true;
                } __except(EXCEPTION_EXECUTE_HANDLER) { continue; }
            }
            return false;
        }

        memset(g_mobileUIBuffer, 0, sizeof(g_mobileUIBuffer));
        *reinterpret_cast<uint8_t**>(g_mobileUIBuffer + offset) = g_mobileUIBuffer;
        ChangeMobileUI_t changeMobileUI = (ChangeMobileUI_t)g_ChangeMobileUIAddr;
        changeMobileUI(g_mobileUIBuffer);
        return true;
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

void InitMobileUI() {
    using namespace PatternScanner;

    for (int i = 0; Signatures::MobileUI[i] && !g_ChangeMobileUIAddr; i++) {
        uintptr_t a = Scan(Signatures::MobileUI[i]);
        if (a) {
            uintptr_t resolved = ResolveCall(a);
            if (resolved) g_ChangeMobileUIAddr = resolved;
        }
    }
}
