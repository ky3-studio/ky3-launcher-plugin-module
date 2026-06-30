#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#include <Windows.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <atomic>
#include <cstdio>
#include <intrin.h>
#include <share.h>
#include <Psapi.h>
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "gdi32.lib")

#include "MinHook/MinHook.h"

#include "core/types.h"
#include "core/state.h"
#include "core/config.h"
#include "core/log.h"
#include "core/safe.h"
#include "core/scanner.h"
#include "core/signatures.h"

#include "input.h"
#include "init.h"
#include "gui.h"

DWORD WINAPI MainThread(LPVOID) {
    while (!g_GameWindow && g_running) {
        g_GameWindow = FindWindowA("UnityWndClass", nullptr);
        Sleep(100);
    }
    if (!g_running) return 0;
    Sleep(3000);

    MH_Initialize();
    LogInit();
    Log("MainThread started, game window found");
    LoadConfig();
    InitGameHooks();
    InitGui();
    MH_EnableHook(MH_ALL_HOOKS);
    Log("All hooks enabled");

    CreateThread(nullptr, 0, InputThread, nullptr, 0, nullptr);

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

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        g_hModule = hModule;
        CreateThread(nullptr, 0, MainThread, hModule, 0, nullptr);
        break;
    case DLL_PROCESS_DETACH:
        g_running = false;
        Sleep(200);
        ShutdownGui();
        if (g_mouseHook) { UnhookWindowsHookEx(g_mouseHook); g_mouseHook = nullptr; }
        MH_DisableHook(MH_ALL_HOOKS);
        MH_Uninitialize();
        LogClose();
        break;
    }
    return TRUE;
}
