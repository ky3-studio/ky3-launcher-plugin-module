#pragma once
#include "types.h"
#include <atomic>

extern Set_FrameCount_t g_Set_FrameCount;
extern Get_FrameCount_t g_oGetFrameCount;
extern Set_SyncCount_t g_Set_SyncCount;
extern ChangeFOV_t g_oChangeFOV;
extern DisplayFog_t g_oDisplayFog;
extern Player_Perspective_t g_oPlayer_Perspective;
extern CraftEntry_t g_oCraftEntry;
extern OpenTeam_t g_oOpenTeam;
extern QuestBanner_t g_oQuestBanner;
extern ShowDamage_t g_oShowDamage;
extern EventCamera_t g_oEventCamera;
extern GetName_t g_GetName;
extern SetActive_t g_oSetActive;
extern SetActive_t g_oSetActiveOrig;

extern uintptr_t g_CraftPartnerAddr;
extern uintptr_t g_InnerDispatcherAddr;
extern uintptr_t g_FindStringAddr;
extern uintptr_t g_CheckCanEnterAddr;
extern uintptr_t g_OpenTeamPageAddr;
extern uintptr_t g_ChangeMobileUIAddr;
extern uintptr_t g_FindGameObjectAddr;

extern uintptr_t g_gameModBase;
extern uintptr_t g_gameModSize;

extern volatile bool g_inGameWorld;
extern bool g_mobileUIApplied;

extern std::atomic<bool> g_triggerCraft;
extern std::atomic<bool> g_triggerDispatch;
extern std::atomic<bool> g_triggerCooking;
extern std::atomic<bool> g_triggerForge;
extern std::atomic<bool> g_xbutton1Pressed;
extern std::atomic<bool> g_xbutton2Pressed;

extern DWORD g_lastDispatchTime;
extern DWORD g_lastCookingTime;
extern DWORD g_lastForgeTime;
extern DWORD g_lastGuiToggleTime;

extern std::atomic<bool> g_running;
extern std::atomic<bool> g_showMenu;
extern HWND g_GameWindow;
extern HMODULE g_hModule;
