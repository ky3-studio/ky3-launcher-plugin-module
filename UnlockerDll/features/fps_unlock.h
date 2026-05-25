#pragma once

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
