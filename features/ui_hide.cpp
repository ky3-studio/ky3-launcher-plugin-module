#include "ui_hide.h"
#include "no_grass.h"
#include "../core/state.h"
#include "../core/config.h"
#include "../core/log.h"
#include "../core/safe.h"
#include "../core/scanner.h"
#include "../core/signatures.h"
#include "../MinHook/MinHook.h"

static bool g_questBannerHidden = false;

void TryHideQuestBannerUI() {
    if (!g_config.hideQuestBanner) {
        g_questBannerHidden = false;
        return;
    }
    if (!IsValidPtr(g_oSetActiveOrig) || !g_FindGameObjectAddr || !g_FindStringAddr) return;

    static DWORD lastSearchTime = 0;
    DWORD now = GetTickCount();
    if (now - lastSearchTime < 100) return;
    lastSearchTime = now;

    auto setActive = g_oSetActiveOrig;
    auto findString = (void*(__fastcall*)(const char*))g_FindStringAddr;
    auto findGameObject = (void*(__fastcall*)(void*))g_FindGameObjectAddr;

    static const char* paths[] = {
        "Canvas/Pages/InLevelMainPage/GrpMainPage/GrpTopInfo/QuestBanner",
        "Canvas/Pages/InLevelMapPage/GrpMap/GrpPointTips/Layout/QuestBanner",
        nullptr
    };

    __try {
        for (int i = 0; paths[i]; i++) {
            void* strObj = findString(paths[i]);
            if (strObj) {
                void* bannerObj = findGameObject(strObj);
                if (bannerObj) {
                    setActive(bannerObj, false);
                    g_questBannerHidden = true;
                }
            }
        }
    } __except(EXCEPTION_EXECUTE_HANDLER) {}
}

static bool g_gameUIDHidden = false;

void TryHideGameUID() {
    if (!IsValidPtr(g_oSetActiveOrig)) return;

    static void* cached_uid_obj = nullptr;
    static int frameSkip = 0;

    auto _SetActive = g_oSetActiveOrig;

    if (cached_uid_obj) {
        __try {
            bool shouldHide = g_config.hideUID;
            if (shouldHide && !g_gameUIDHidden) {
                _SetActive(cached_uid_obj, false);
                g_gameUIDHidden = true;
            } else if (!shouldHide && g_gameUIDHidden) {
                _SetActive(cached_uid_obj, true);
                g_gameUIDHidden = false;
            }
        } __except(EXCEPTION_EXECUTE_HANDLER) {
            cached_uid_obj = nullptr;
            g_gameUIDHidden = false;
        }
        return;
    }

    if (!g_config.hideUID) return;
    if (++frameSkip < 10) return;
    frameSkip = 0;

    auto _FindString = (void*(__fastcall*)(const char*))g_FindStringAddr;
    auto _FindGameObject = (void*(__fastcall*)(void*))g_FindGameObjectAddr;
    if (!_FindString || !_FindGameObject) return;

    __try {
        void* str_obj = _FindString("/BetaWatermarkCanvas(Clone)/Panel/TxtUID");
        if (str_obj) cached_uid_obj = _FindGameObject(str_obj);
    } __except(EXCEPTION_EXECUTE_HANDLER) {}
}

static bool g_menuUIDHidden = false;

void TryHideMenuUID() {
    if (!IsValidPtr(g_oSetActiveOrig)) return;

    static void* cached_obj = nullptr;
    static int frameSkip = 0;

    if (cached_obj) {
        __try {
            auto setActive = g_oSetActiveOrig;
            bool shouldHide = g_config.hideMenuUID;
            if (shouldHide && !g_menuUIDHidden) {
                setActive(cached_obj, false);
                g_menuUIDHidden = true;
            } else if (!shouldHide && g_menuUIDHidden) {
                setActive(cached_obj, true);
                g_menuUIDHidden = false;
            }
        } __except(EXCEPTION_EXECUTE_HANDLER) {
            cached_obj = nullptr;
            g_menuUIDHidden = false;
        }
        return;
    }

    if (!g_config.hideMenuUID) return;
    if (!g_FindStringAddr || !g_FindGameObjectAddr) return;
    if (++frameSkip < 20) return;
    frameSkip = 0;

    __try {
        auto findStr = (void*(__fastcall*)(const char*))g_FindStringAddr;
        auto findObj = (void*(__fastcall*)(void*))g_FindGameObjectAddr;
        void* strObj = findStr("/Canvas/Pages/PlayerProfilePage/GrpProfile/Right/GrpPlayerCard/UID");
        if (strObj) cached_obj = findObj(strObj);
    } __except(EXCEPTION_EXECUTE_HANDLER) {}
}

void InitUIHiding() {
    using namespace PatternScanner;
    uintptr_t addr;

    for (int i = 0; Signatures::SetActive[i] && !g_oSetActive; i++) {
        uintptr_t a = Scan(Signatures::SetActive[i]);
        if (a) {
            if (i == 0) a = ResolveCall(a);
            if (a) {
                MH_CreateHook((void*)a, HookSetActive, (void**)&g_oSetActiveOrig);
                g_oSetActive = (SetActive_t)a;
                Log("[SCAN] SetActive: matched pattern[%d] at 0x%llX (hooked) | sig: %s", i, (unsigned long long)a, Signatures::SetActive[i]);
            }
        }
    }
    if (!g_oSetActive) Log("[SCAN] SetActive: FAILED - no pattern matched");

    addr = ScanLog(Signatures::QuestBanner, "QuestBanner");

    for (int i = 0; Signatures::FindGameObject[i] && !g_FindGameObjectAddr; i++) {
        uintptr_t a = Scan(Signatures::FindGameObject[i]);
        if (a) {
            if (i == 1) a = ResolveCall(a);
            if (a) {
                g_FindGameObjectAddr = a;
                Log("[SCAN] FindGameObject: matched pattern[%d] at 0x%llX | sig: %s", i, (unsigned long long)a, Signatures::FindGameObject[i]);
            }
        }
    }
    if (!g_FindGameObjectAddr) Log("[SCAN] FindGameObject: FAILED - no pattern matched");

    for (int i = 0; Signatures::FindString[i] && !g_FindStringAddr; i++) {
        uintptr_t a = Scan(Signatures::FindString[i]);
        if (a) {
            g_FindStringAddr = a;
            Log("[SCAN] FindString: matched pattern[%d] at 0x%llX | sig: %s", i, (unsigned long long)a, Signatures::FindString[i]);
        }
    }
    if (!g_FindStringAddr) Log("[SCAN] FindString: FAILED - no pattern matched");
}
