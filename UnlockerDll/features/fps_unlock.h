#pragma once

__int64 __fastcall HookGameUpdate(__int64 a1, const char* a2);

uintptr_t g_TargetFrameRate = 0;
uintptr_t g_FpsInstructionAddr = 0;

typedef void(*set_targetFrameRate_t)(int value);
set_targetFrameRate_t g_set_targetFrameRate = nullptr;
set_targetFrameRate_t g_o_set_targetFrameRate = nullptr;

void Hook_set_targetFrameRate(int value) {
    if (!g_o_set_targetFrameRate) return;
    if (g_config.enableFps && g_config.targetFps > 0) {
        g_o_set_targetFrameRate(g_config.targetFps);
    } else {
        g_o_set_targetFrameRate(value);
    }
}

int __fastcall HookGetFrameCount() {
    if (g_config.enableFps) {
        if (g_oGetFrameCount) {
            int ret = g_oGetFrameCount();
            if (ret >= 60) return 60;
            if (ret >= 45) return 45;
            if (ret >= 30) return 30;
            return ret;
        }
        return 60;
    }
    return g_oGetFrameCount ? g_oGetFrameCount() : 60;
}

void InitFps() {
    using namespace PatternScanner;
    uintptr_t addr;

    addr = ScanNullTerminated(Signatures::GameUpdate);
    if (addr) {
        addr = ResolveCall(addr);
        if (addr) MH_CreateHook((void*)addr, HookGameUpdate, (void**)&g_oGameUpdate);
    }

    addr = ScanNullTerminated(Signatures::SetFrameCount);
    if (addr) {
        uintptr_t target = ResolveCall(addr);
        if (target) g_Set_FrameCount = (Set_FrameCount_t)target;
    }

    addr = ScanNullTerminated(Signatures::GetFrameCount);
    if (addr) {
        uintptr_t target = ResolveCall(addr);
        if (target) MH_CreateHook((void*)target, HookGetFrameCount, (void**)&g_oGetFrameCount);
    }

    {
        uintptr_t patternAddr = Scan(Signatures::FPS);
        if (patternAddr) {
            g_FpsInstructionAddr = patternAddr;

            uintptr_t setTargetFR = ResolveCall(patternAddr + 5);
            if (setTargetFR) {
                __try {
                    while (*reinterpret_cast<uint8_t*>(setTargetFR) == 0xE9) {
                        int32_t jmpDisp = *reinterpret_cast<int32_t*>(setTargetFR + 1);
                        setTargetFR = setTargetFR + jmpDisp + 5;
                    }
                } __except(EXCEPTION_EXECUTE_HANDLER) {}
                MH_CreateHook((void*)setTargetFR, Hook_set_targetFrameRate, (void**)&g_o_set_targetFrameRate);
            }

            uintptr_t rip = patternAddr + 5;
            while (*reinterpret_cast<uint8_t*>(rip) == 0xE8 || *reinterpret_cast<uint8_t*>(rip) == 0xE9) {
                int32_t disp = *reinterpret_cast<int32_t*>(rip + 1);
                rip = rip + disp + 5;
            }

            int32_t disp = *reinterpret_cast<int32_t*>(rip + 2);
            g_TargetFrameRate = rip + disp + 6;

            MEMORY_BASIC_INFORMATION mbi{};
            if (VirtualQuery((void*)g_TargetFrameRate, &mbi, sizeof(mbi))) {
                if (mbi.Protect != PAGE_READWRITE &&
                    mbi.Protect != PAGE_WRITECOPY &&
                    mbi.Protect != PAGE_EXECUTE_READWRITE &&
                    mbi.Protect != PAGE_EXECUTE_WRITECOPY) {
                    g_TargetFrameRate = 0;
                }
            }
        }
    }

    addr = ScanNullTerminated(Signatures::SyncCount);
    if (addr) {
        addr = ResolveCall(addr);
        if (addr) g_Set_SyncCount = (Set_SyncCount_t)addr;
    }
}
