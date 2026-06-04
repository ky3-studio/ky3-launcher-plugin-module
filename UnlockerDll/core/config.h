#pragma once

struct Config {
    float targetFov = 45.0f;
    bool disableVSync = false;
    bool enableFov = false;
    bool disableFog = false;
    bool disableCharFade = false;
    bool hideUID = false;
    bool hideMenuUID = false;
    bool hideQuestBanner = false;
    bool disableDamageText = false;
    bool disableCameraAnim = false;
    bool touchScreen = false;
    bool redirectCraft = false;
    bool removeTeamAnim = false;
    int craftKey = 0x36;
    int craftModifier = 0;
    bool enableCraft = false;
    bool enableFps = false;
    int targetFps = 120;
    wchar_t gamePath[MAX_PATH] = {0};
    bool enableDispatch = false;
    bool redirectDispatch = false;
    int dispatchKey = 0x76;
    int dispatchModifier = 0;
    char dispatchPageName[64] = "ExpeditionPage";
    bool enableCooking = false;
    int cookingKey = 0x77;
    int cookingModifier = 0;
    bool enableForge = false;
    int forgeKey = 0x78;
    int forgeModifier = 0;
    bool enableNoGrass = false;
    bool enableGui = true;
    int guiKey = 0xA1;    
    int guiModifier = 0;    
};

Config g_config;
