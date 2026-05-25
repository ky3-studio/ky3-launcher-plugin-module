#pragma once

static bool g_questBannerHidden = false;

void TryHideQuestBannerUI() {
    if (!g_SetActiveAddr || !g_FindGameObjectAddr || !g_FindStringAddr) return;

    static void* cached_banner = nullptr;
    static DWORD lastSearchTime = 0;

    auto setActive = (void(__fastcall*)(void*, bool))g_SetActiveAddr;
    auto findString = (void*(__fastcall*)(const char*))g_FindStringAddr;
    auto findGameObject = (void*(__fastcall*)(void*))g_FindGameObjectAddr;

    DWORD now = GetTickCount();

    if (!cached_banner && (now - lastSearchTime > 1000)) {
        lastSearchTime = now;
        __try {
            void* strObj = findString("Canvas/Pages/InLevelMainPage/GrpMainPage/GrpTopInfo/QuestBanner");
            if (strObj) {
                void* bannerObj = findGameObject(strObj);
                if (bannerObj) cached_banner = bannerObj;
            }
        } __except(EXCEPTION_EXECUTE_HANDLER) {
            cached_banner = nullptr;
        }
    }

    if (!cached_banner) {
        g_questBannerHidden = false;
        return;
    }

    __try {
        bool shouldHide = g_config.hideQuestBanner;
        if (shouldHide && !g_questBannerHidden) {
            setActive(cached_banner, false);
            g_questBannerHidden = true;
        } else if (!shouldHide && g_questBannerHidden) {
            setActive(cached_banner, true);
            g_questBannerHidden = false;
        }
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        cached_banner = nullptr;
        g_questBannerHidden = false;
    }
}

static bool g_gameUIDHidden = false;

void TryHideGameUID() {
    if (!g_SetActiveAddr) return;

    static void* cached_uid_obj = nullptr;
    static int frameSkip = 0;

    auto _SetActive = (void(__fastcall*)(void*, bool))g_SetActiveAddr;

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
    if (++frameSkip < 60) return;
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
    if (!g_SetActiveAddr) return;

    static void* cached_obj = nullptr;
    static int frameSkip = 0;

    if (cached_obj) {
        __try {
            auto setActive = (void(__fastcall*)(void*, bool))g_SetActiveAddr;
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
    if (++frameSkip < 120) return;
    frameSkip = 0;

    __try {
        auto findStr = (void*(__fastcall*)(const char*))g_FindStringAddr;
        auto findObj = (void*(__fastcall*)(void*))g_FindGameObjectAddr;
        void* strObj = findStr("/Canvas/Pages/PlayerProfilePage/GrpProfile/Right/GrpPlayerCard/UID");
        if (strObj) cached_obj = findObj(strObj);
    } __except(EXCEPTION_EXECUTE_HANDLER) {}
}

void HookQuestBanner(__int64 a1) {
    if (g_config.hideQuestBanner) return;
    if (g_oQuestBanner) g_oQuestBanner(a1);
}
