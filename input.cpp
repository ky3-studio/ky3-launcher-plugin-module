#include "input.h"
#include "core/state.h"
#include "core/config.h"
#include "core/safe.h"

HHOOK g_mouseHook = nullptr;

static LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        MSLLHOOKSTRUCT* pMouse = (MSLLHOOKSTRUCT*)lParam;
        if (wParam == WM_XBUTTONDOWN) {
            WORD xbutton = HIWORD(pMouse->mouseData);
            if (xbutton == XBUTTON1) g_xbutton1Pressed.store(true);
            if (xbutton == XBUTTON2) g_xbutton2Pressed.store(true);
        } else if (wParam == WM_XBUTTONUP) {
            WORD xbutton = HIWORD(pMouse->mouseData);
            if (xbutton == XBUTTON1) g_xbutton1Pressed.store(false);
            if (xbutton == XBUTTON2) g_xbutton2Pressed.store(false);
        }
    }
    return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
}

static bool CheckKeyWithModifier(int key, int modifier) {
    bool keyPressed = false;
    if (key == 5) keyPressed = g_xbutton1Pressed.load();
    else if (key == 6) keyPressed = g_xbutton2Pressed.load();
    else keyPressed = (GetAsyncKeyState(key) & 0x8000) != 0;

    if (!keyPressed) return false;

    switch (modifier) {
        case 1: return (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
        case 2: return (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
        case 3: return (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
        default: return true;
    }
}

DWORD WINAPI InputThread(LPVOID) {
    DWORD lastCraftKeyTime = 0;
    DWORD lastReloadCheck = 0;

    while (g_running) {
        DWORD now = GetTickCount();

        if (now - lastReloadCheck > 2000) {
            ReloadConfigIfChanged();
            lastReloadCheck = now;
        }

        bool needMouseHook = (g_config.redirectCraft && (g_config.craftKey == 5 || g_config.craftKey == 6))
                           || (g_config.redirectDispatch && (g_config.dispatchKey == 5 || g_config.dispatchKey == 6))
                           || (g_config.enableCooking && (g_config.cookingKey == 5 || g_config.cookingKey == 6))
                           || (g_config.enableForge && (g_config.forgeKey == 5 || g_config.forgeKey == 6))
                           || (g_config.enableGui && (g_config.guiKey == 5 || g_config.guiKey == 6));
        if (needMouseHook && !g_mouseHook) {
            g_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc, g_hModule, 0);
        } else if (!needMouseHook && g_mouseHook) {
            UnhookWindowsHookEx(g_mouseHook);
            g_mouseHook = nullptr;
            g_xbutton1Pressed.store(false);
            g_xbutton2Pressed.store(false);
        }

        if (!CheckWindowFocused()) {
            Sleep(16);
            continue;
        }

        if (g_config.redirectCraft && g_config.craftKey != 0) {
            if (CheckKeyWithModifier(g_config.craftKey, g_config.craftModifier) && now - lastCraftKeyTime > 300) {
                g_triggerCraft.store(true);
                lastCraftKeyTime = now;
            }
        }

        if (g_config.redirectDispatch && g_config.dispatchKey != 0) {
            if (CheckKeyWithModifier(g_config.dispatchKey, g_config.dispatchModifier) && now - g_lastDispatchTime > 300) {
                g_triggerDispatch.store(true);
                g_lastDispatchTime = now;
            }
        }

        if (g_config.enableCooking && g_config.cookingKey != 0) {
            if (CheckKeyWithModifier(g_config.cookingKey, g_config.cookingModifier) && now - g_lastCookingTime > 300) {
                g_triggerCooking.store(true);
                g_lastCookingTime = now;
            }
        }

        if (g_config.enableForge && g_config.forgeKey != 0) {
            if (CheckKeyWithModifier(g_config.forgeKey, g_config.forgeModifier) && now - g_lastForgeTime > 300) {
                g_triggerForge.store(true);
                g_lastForgeTime = now;
            }
        }

        if (g_config.enableGui && g_config.guiKey != 0) {
            if (CheckKeyWithModifier(g_config.guiKey, g_config.guiModifier) && now - g_lastGuiToggleTime > 300) {
                g_showMenu = !g_showMenu;
                g_lastGuiToggleTime = now;
            }
        }

        Sleep(16);
    }
    if (g_mouseHook) { UnhookWindowsHookEx(g_mouseHook); g_mouseHook = nullptr; }
    return 0;
}
