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
};

Config g_config;
std::atomic<bool> g_running(true);
HWND g_GameWindow = nullptr;
uintptr_t g_FpsAddr = 0;
HMODULE g_hModule = nullptr;

typedef int(*GameUpdate_t)(__int64 a1, const char* a2);
typedef int(*Set_FrameCount_t)(int value);
typedef int(*Get_FrameCount_t)();
typedef int(*Set_SyncCount_t)(int value);
typedef int(*ChangeFOV_t)(__int64 a1, float a2);
typedef int(*DisplayFog_t)(__int64 a1, __int64 a2);
typedef void*(*Player_Perspective_t)(void* RCX, float Display, void* R8);
typedef void(__fastcall *CraftEntry_t)(void*);
typedef void(*OpenTeam_t)();
typedef void(*ChangeMobileUI_t)(uint8_t* buffer);
typedef void(*QuestBanner_t)(__int64);
typedef void(__fastcall *ComponentInit_t)(void* thisPtr);
typedef void*(__fastcall *FindGameObject_t)(void*);

GameUpdate_t g_oGameUpdate = nullptr;
Set_FrameCount_t g_Set_FrameCount = nullptr;
Get_FrameCount_t g_oGetFrameCount = nullptr;
Set_SyncCount_t g_Set_SyncCount = nullptr;
ChangeFOV_t g_oChangeFOV = nullptr;
DisplayFog_t g_oDisplayFog = nullptr;
Player_Perspective_t g_oPlayer_Perspective = nullptr;
CraftEntry_t g_oCraftEntry = nullptr;
OpenTeam_t g_oOpenTeam = nullptr;
QuestBanner_t g_oQuestBanner = nullptr;
ComponentInit_t g_oComponentInit = nullptr;
uintptr_t g_ComponentInitAddr = 0;
FindGameObject_t g_oFindGameObject = nullptr;
uintptr_t g_CraftPartnerAddr = 0;
static uintptr_t g_InnerDispatcherAddr = 0;
uintptr_t g_FindStringAddr = 0;
uintptr_t g_CheckCanEnterAddr = 0;
uintptr_t g_OpenTeamPageAddr = 0;
uintptr_t g_ChangeMobileUIAddr = 0;
uintptr_t g_FindGameObjectAddr = 0;
uintptr_t g_SetActiveAddr = 0;

static uintptr_t g_gameModBase = 0;
static uintptr_t g_gameModSize = 0;

static volatile bool g_inGameWorld = false;

static bool g_mobileUIApplied = false;
static std::atomic<bool> g_craftKeyPressed(false);
static std::atomic<bool> g_triggerCraft(false);
static std::atomic<bool> g_craftPageOpen(false);
static std::atomic<bool> g_triggerCloseCraft(false);
static DWORD g_lastCraftTime = 0;
static uintptr_t g_GameAssemblyBase = 0;
static std::atomic<bool> g_triggerDispatch(false);
static DWORD g_lastDispatchTime = 0;
static std::atomic<bool> g_triggerCooking(false);
static DWORD g_lastCookingTime = 0;
static std::atomic<bool> g_triggerForge(false);
static DWORD g_lastForgeTime = 0;
static std::atomic<bool> g_xbutton1Pressed(false);
static std::atomic<bool> g_xbutton2Pressed(false);

struct alignas(8) StdString {
    union {
        char buf[16];
        char* ptr;
    };
    size_t size;
    size_t capacity;
};

void LoadConfig();
