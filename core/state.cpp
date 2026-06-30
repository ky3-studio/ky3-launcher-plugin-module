#include "state.h"

Set_FrameCount_t g_Set_FrameCount = nullptr;
Get_FrameCount_t g_oGetFrameCount = nullptr;
Set_SyncCount_t g_Set_SyncCount = nullptr;
ChangeFOV_t g_oChangeFOV = nullptr;
DisplayFog_t g_oDisplayFog = nullptr;
Player_Perspective_t g_oPlayer_Perspective = nullptr;
CraftEntry_t g_oCraftEntry = nullptr;
OpenTeam_t g_oOpenTeam = nullptr;
QuestBanner_t g_oQuestBanner = nullptr;
ShowDamage_t g_oShowDamage = nullptr;
EventCamera_t g_oEventCamera = nullptr;
GetName_t g_GetName = nullptr;
SetActive_t g_oSetActive = nullptr;
SetActive_t g_oSetActiveOrig = nullptr;

uintptr_t g_CraftPartnerAddr = 0;
uintptr_t g_InnerDispatcherAddr = 0;
uintptr_t g_FindStringAddr = 0;
uintptr_t g_CheckCanEnterAddr = 0;
uintptr_t g_OpenTeamPageAddr = 0;
uintptr_t g_ChangeMobileUIAddr = 0;
uintptr_t g_FindGameObjectAddr = 0;

uintptr_t g_gameModBase = 0;
uintptr_t g_gameModSize = 0;

volatile bool g_inGameWorld = false;
bool g_mobileUIApplied = false;

std::atomic<bool> g_triggerCraft(false);
std::atomic<bool> g_triggerDispatch(false);
std::atomic<bool> g_triggerCooking(false);
std::atomic<bool> g_triggerForge(false);
std::atomic<bool> g_xbutton1Pressed(false);
std::atomic<bool> g_xbutton2Pressed(false);

DWORD g_lastDispatchTime = 0;
DWORD g_lastCookingTime = 0;
DWORD g_lastForgeTime = 0;
DWORD g_lastGuiToggleTime = 0;

std::atomic<bool> g_running(true);
std::atomic<bool> g_showMenu(false);
HWND g_GameWindow = nullptr;
HMODULE g_hModule = nullptr;
