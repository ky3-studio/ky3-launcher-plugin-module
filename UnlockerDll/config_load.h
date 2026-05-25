#pragma once

std::wstring GetConfigPath() {
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(g_hModule, path, MAX_PATH);
    std::wstring p = path;
    return p.substr(0, p.find_last_of(L"\\/")) + L"\\dll_config.ini";
}

void LoadConfig() {
    std::wstring wpath = GetConfigPath();
    char pathA[MAX_PATH];
    WideCharToMultiByte(CP_ACP, 0, wpath.c_str(), -1, pathA, MAX_PATH, nullptr, nullptr);

    g_config.targetFov = (float)GetPrivateProfileIntA("Settings", "targetFov", 45, pathA);
    g_config.disableVSync = GetPrivateProfileIntA("Settings", "disableVSync", 0, pathA) != 0;
    g_config.enableFov = GetPrivateProfileIntA("Settings", "enableFov", 0, pathA) != 0;
    g_config.disableFog = GetPrivateProfileIntA("Settings", "disableFog", 0, pathA) != 0;
    g_config.disableCharFade = GetPrivateProfileIntA("Settings", "disableCharFade", 0, pathA) != 0;
    g_config.hideUID = GetPrivateProfileIntA("Settings", "hideUID", 0, pathA) != 0;
    g_config.hideMenuUID = GetPrivateProfileIntA("Settings", "hideMenuUID", 0, pathA) != 0;
    g_config.hideQuestBanner = GetPrivateProfileIntA("Settings", "hideQuestBanner", 0, pathA) != 0;
    g_config.disableDamageText = GetPrivateProfileIntA("Settings", "disableDamageText", 0, pathA) != 0;
    g_config.disableCameraAnim = GetPrivateProfileIntA("Settings", "disableCameraAnim", 0, pathA) != 0;
    g_config.touchScreen = GetPrivateProfileIntA("Settings", "touchScreen", 0, pathA) != 0;
    g_config.redirectCraft = GetPrivateProfileIntA("Settings", "redirectCraft", 0, pathA) != 0;
    g_config.removeTeamAnim = GetPrivateProfileIntA("Settings", "removeTeamAnim", 0, pathA) != 0;
    g_config.craftKey = GetPrivateProfileIntA("Settings", "craftKey", 0x36, pathA);
    g_config.craftModifier = GetPrivateProfileIntA("Settings", "craftModifier", 0, pathA);

    g_config.enableFps = GetPrivateProfileIntA("Settings", "enableFps", 0, pathA) != 0;
    g_config.targetFps = GetPrivateProfileIntA("Settings", "targetFps", 120, pathA);
    g_config.enableCraft = GetPrivateProfileIntA("Settings", "enablePortableCraft", 0, pathA) != 0;
    if (!g_config.enableCraft)
        g_config.enableCraft = GetPrivateProfileIntA("Settings", "enableCraftRedirect", 0, pathA) != 0;
    if (!g_config.enableCraft)
        g_config.enableCraft = GetPrivateProfileIntA("Settings", "enableCraft", 0, pathA) != 0;

    g_config.enableDispatch = GetPrivateProfileIntA("Settings", "enableDispatch", 0, pathA) != 0;
    g_config.redirectDispatch = GetPrivateProfileIntA("Settings", "redirectDispatch", 0, pathA) != 0;
    g_config.dispatchKey = GetPrivateProfileIntA("Settings", "dispatchKey", 0x76, pathA);
    g_config.dispatchModifier = GetPrivateProfileIntA("Settings", "dispatchModifier", 0, pathA);
    GetPrivateProfileStringA("Settings", "dispatchPageName", "ExpeditionPage", g_config.dispatchPageName, sizeof(g_config.dispatchPageName), pathA);

    g_config.enableCooking = GetPrivateProfileIntA("Settings", "enableCooking", 0, pathA) != 0;
    g_config.cookingKey = GetPrivateProfileIntA("Settings", "cookingKey", 0x77, pathA);
    g_config.cookingModifier = GetPrivateProfileIntA("Settings", "cookingModifier", 0, pathA);
    g_config.enableForge = GetPrivateProfileIntA("Settings", "enableForge", 0, pathA) != 0;
    g_config.forgeKey = GetPrivateProfileIntA("Settings", "forgeKey", 0x78, pathA);
    g_config.forgeModifier = GetPrivateProfileIntA("Settings", "forgeModifier", 0, pathA);
}

static HHOOK g_mouseHook = nullptr;

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
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

DWORD WINAPI FpsThread(LPVOID) {
    DWORD lastCraftKeyTime = 0;

    while (g_running) {
        DWORD now = GetTickCount();

        bool needMouseHook = (g_config.redirectCraft && (g_config.craftKey == 5 || g_config.craftKey == 6))
                           || (g_config.redirectDispatch && (g_config.dispatchKey == 5 || g_config.dispatchKey == 6))
                           || (g_config.enableCooking && (g_config.cookingKey == 5 || g_config.cookingKey == 6))
                           || (g_config.enableForge && (g_config.forgeKey == 5 || g_config.forgeKey == 6));
        if (needMouseHook && !g_mouseHook) {
            g_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc, g_hModule, 0);
        } else if (!needMouseHook && g_mouseHook) {
            UnhookWindowsHookEx(g_mouseHook);
            g_mouseHook = nullptr;
            g_xbutton1Pressed.store(false);
            g_xbutton2Pressed.store(false);
        }

        if (g_config.redirectCraft && g_config.craftKey != 0) {
            bool keyPressed = false;
            if (g_config.craftKey == 5) keyPressed = g_xbutton1Pressed.load();
            else if (g_config.craftKey == 6) keyPressed = g_xbutton2Pressed.load();
            else keyPressed = (GetAsyncKeyState(g_config.craftKey) & 0x8000) != 0;

            if (keyPressed) {
                bool modifierOk = true;
                switch (g_config.craftModifier) {
                    case 1: modifierOk = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0; break;
                    case 2: modifierOk = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0; break;
                    case 3: modifierOk = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0; break;
                    default: break;
                }
                if (modifierOk && now - lastCraftKeyTime > 300) {
                    g_triggerCraft.store(true);
                    lastCraftKeyTime = now;
                }
            }
        }

        if (g_config.redirectDispatch && g_config.dispatchKey != 0) {
            bool keyPressed = false;
            if (g_config.dispatchKey == 5) keyPressed = g_xbutton1Pressed.load();
            else if (g_config.dispatchKey == 6) keyPressed = g_xbutton2Pressed.load();
            else keyPressed = (GetAsyncKeyState(g_config.dispatchKey) & 0x8000) != 0;

            if (keyPressed) {
                bool modifierOk = true;
                switch (g_config.dispatchModifier) {
                    case 1: modifierOk = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0; break;
                    case 2: modifierOk = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0; break;
                    case 3: modifierOk = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0; break;
                    default: break;
                }
                if (modifierOk && now - g_lastDispatchTime > 300) {
                    g_triggerDispatch.store(true);
                    g_lastDispatchTime = now;
                }
            }
        }

        if (g_config.enableCooking && g_config.cookingKey != 0) {
            bool keyPressed = false;
            if (g_config.cookingKey == 5) keyPressed = g_xbutton1Pressed.load();
            else if (g_config.cookingKey == 6) keyPressed = g_xbutton2Pressed.load();
            else keyPressed = (GetAsyncKeyState(g_config.cookingKey) & 0x8000) != 0;

            if (keyPressed) {
                bool modifierOk = true;
                switch (g_config.cookingModifier) {
                    case 1: modifierOk = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0; break;
                    case 2: modifierOk = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0; break;
                    case 3: modifierOk = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0; break;
                    default: break;
                }
                if (modifierOk && now - g_lastCookingTime > 300) {
                    g_triggerCooking.store(true);
                    g_lastCookingTime = now;
                }
            }
        }

        if (g_config.enableForge && g_config.forgeKey != 0) {
            bool keyPressed = false;
            if (g_config.forgeKey == 5) keyPressed = g_xbutton1Pressed.load();
            else if (g_config.forgeKey == 6) keyPressed = g_xbutton2Pressed.load();
            else keyPressed = (GetAsyncKeyState(g_config.forgeKey) & 0x8000) != 0;

            if (keyPressed) {
                bool modifierOk = true;
                switch (g_config.forgeModifier) {
                    case 1: modifierOk = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0; break;
                    case 2: modifierOk = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0; break;
                    case 3: modifierOk = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0; break;
                    default: break;
                }
                if (modifierOk && now - g_lastForgeTime > 300) {
                    g_triggerForge.store(true);
                    g_lastForgeTime = now;
                }
            }
        }

        Sleep(16);
    }
    if (g_mouseHook) { UnhookWindowsHookEx(g_mouseHook); g_mouseHook = nullptr; }
    return 0;
}

DWORD WINAPI MainThread(LPVOID lpParameter) {
    while (!g_GameWindow && g_running) {
        g_GameWindow = FindWindowA("UnityWndClass", nullptr);
        Sleep(100);
    }
    if (!g_running) return 0;
    Sleep(3000);

    MH_Initialize();
    LoadConfig();
    InitGameHooks();

    CreateThread(nullptr, 0, FpsThread, nullptr, 0, nullptr);

    while (g_running) {
        if (!IsWindow(g_GameWindow)) {
            g_running = false;
            Sleep(500);
            TerminateProcess(GetCurrentProcess(), 0);
            break;
        }
        Sleep(1000);
    }

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        g_hModule = hModule;
        CreateThread(nullptr, 0, MainThread, hModule, 0, nullptr);
        break;
    case DLL_PROCESS_DETACH:
        g_running = false;
        Sleep(200);
        if (g_mouseHook) { UnhookWindowsHookEx(g_mouseHook); g_mouseHook = nullptr; }
        MH_DisableHook(MH_ALL_HOOKS);
        MH_Uninitialize();
        break;
    }
    return TRUE;
}
