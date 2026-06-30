#include "config.h"
#include "state.h"
#include <string>

Config g_config;

static FILETIME g_lastConfigWriteTime = {};
static DWORD g_lastConfigSaveTickCount = 0;

static std::wstring GetConfigPath() {
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(g_hModule, path, MAX_PATH);
    std::wstring p = path;
    return p.substr(0, p.find_last_of(L"\\/")) + L"\\dll_config.ini";
}

static void WriteIniInt(const char* key, int value, const char* path) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", value);
    WritePrivateProfileStringA("Settings", key, buf, path);
}

void SaveConfig() {
    std::wstring wpath = GetConfigPath();
    char pathA[MAX_PATH];
    WideCharToMultiByte(CP_ACP, 0, wpath.c_str(), -1, pathA, MAX_PATH, nullptr, nullptr);

    WriteIniInt("targetFov", (int)g_config.targetFov, pathA);
    WriteIniInt("disableVSync", g_config.disableVSync ? 1 : 0, pathA);
    WriteIniInt("enableFov", g_config.enableFov ? 1 : 0, pathA);
    WriteIniInt("disableFog", g_config.disableFog ? 1 : 0, pathA);
    WriteIniInt("disableCharFade", g_config.disableCharFade ? 1 : 0, pathA);
    WriteIniInt("hideUID", g_config.hideUID ? 1 : 0, pathA);
    WriteIniInt("hideMenuUID", g_config.hideMenuUID ? 1 : 0, pathA);
    WriteIniInt("hideQuestBanner", g_config.hideQuestBanner ? 1 : 0, pathA);
    WriteIniInt("disableDamageText", g_config.disableDamageText ? 1 : 0, pathA);
    WriteIniInt("disableCameraAnim", g_config.disableCameraAnim ? 1 : 0, pathA);
    WriteIniInt("touchScreen", g_config.touchScreen ? 1 : 0, pathA);
    WriteIniInt("enablePortableCraft", g_config.enableCraft ? 1 : 0, pathA);
    WriteIniInt("redirectCraft", g_config.redirectCraft ? 1 : 0, pathA);
    WriteIniInt("craftKey", g_config.craftKey, pathA);
    WriteIniInt("craftModifier", g_config.craftModifier, pathA);
    WriteIniInt("removeTeamAnim", g_config.removeTeamAnim ? 1 : 0, pathA);
    WriteIniInt("enableFps", g_config.enableFps ? 1 : 0, pathA);
    WriteIniInt("targetFps", g_config.targetFps, pathA);
    WriteIniInt("enableDispatch", g_config.enableDispatch ? 1 : 0, pathA);
    WriteIniInt("redirectDispatch", g_config.redirectDispatch ? 1 : 0, pathA);
    WriteIniInt("dispatchKey", g_config.dispatchKey, pathA);
    WriteIniInt("dispatchModifier", g_config.dispatchModifier, pathA);
    WriteIniInt("enableCooking", g_config.enableCooking ? 1 : 0, pathA);
    WriteIniInt("cookingKey", g_config.cookingKey, pathA);
    WriteIniInt("cookingModifier", g_config.cookingModifier, pathA);
    WriteIniInt("enableForge", g_config.enableForge ? 1 : 0, pathA);
    WriteIniInt("forgeKey", g_config.forgeKey, pathA);
    WriteIniInt("forgeModifier", g_config.forgeModifier, pathA);
    WriteIniInt("enableNoGrass", g_config.enableNoGrass ? 1 : 0, pathA);
    WriteIniInt("enableGui", g_config.enableGui ? 1 : 0, pathA);
    WriteIniInt("guiKey", g_config.guiKey, pathA);
    WriteIniInt("guiModifier", g_config.guiModifier, pathA);

    g_lastConfigSaveTickCount = GetTickCount();
    HANDLE hFile = CreateFileW(wpath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
    if (hFile != INVALID_HANDLE_VALUE) {
        GetFileTime(hFile, nullptr, nullptr, &g_lastConfigWriteTime);
        CloseHandle(hFile);
    }
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
    g_config.enableNoGrass = GetPrivateProfileIntA("Settings", "enableNoGrass", 0, pathA) != 0;
    g_config.enableGui = GetPrivateProfileIntA("Settings", "enableGui", 1, pathA) != 0;
    g_config.guiKey = GetPrivateProfileIntA("Settings", "guiKey", 0x2D, pathA);
    g_config.guiModifier = GetPrivateProfileIntA("Settings", "guiModifier", 0, pathA);

    HANDLE hFile = CreateFileW(wpath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
    if (hFile != INVALID_HANDLE_VALUE) {
        GetFileTime(hFile, nullptr, nullptr, &g_lastConfigWriteTime);
        CloseHandle(hFile);
    }
}

void ReloadConfigIfChanged() {
    std::wstring wpath = GetConfigPath();
    HANDLE hFile = CreateFileW(wpath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) return;

    FILETIME ft;
    GetFileTime(hFile, nullptr, nullptr, &ft);
    CloseHandle(hFile);

    if (CompareFileTime(&ft, &g_lastConfigWriteTime) > 0) {
        DWORD elapsed = GetTickCount() - g_lastConfigSaveTickCount;
        if (elapsed > 1000) {
            g_lastConfigWriteTime = ft;
            LoadConfig();
        }
    }
}
