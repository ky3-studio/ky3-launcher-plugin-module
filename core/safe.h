#pragma once
#include <Windows.h>
#include <cstdint>
#include "state.h"

template <typename T>
inline bool IsValidPtr(T ptr) {
    if (!ptr) return false;
    return (uintptr_t)ptr > 0x10000 && (uintptr_t)ptr < 0x7FFFFFFFFFFF;
}

inline bool CheckWindowFocused() {
    HWND fg = GetForegroundWindow();
    if (!fg || !g_GameWindow) return false;
    return (fg == g_GameWindow);
}
