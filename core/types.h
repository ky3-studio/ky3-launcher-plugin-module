#pragma once
#include <Windows.h>
#include <cstdint>
#include <atomic>

typedef int(*Set_FrameCount_t)(int value);
typedef int(*Get_FrameCount_t)();
typedef int(*Set_SyncCount_t)(int value);
typedef int(*ChangeFOV_t)(__int64 a1, float a2);
typedef __int64(*DisplayFog_t)(__int64 a1, __int64 a2);
typedef void*(*Player_Perspective_t)(void* RCX, float Display, void* R8);
typedef void(__fastcall *CraftEntry_t)(void*);
typedef void(*OpenTeam_t)();
typedef void(*ChangeMobileUI_t)(uint8_t* buffer);
typedef void(*QuestBanner_t)(__int64);
typedef void(*ShowDamage_t)(void*, int, int, int, float, void*, void*, void*, int);
typedef bool(*EventCamera_t)(void*, void*);
typedef void(__fastcall *ComponentInit_t)(void* thisPtr);
typedef void*(__fastcall *FindGameObject_t)(void*);
typedef void(__fastcall *SetActive_t)(void*, bool);

struct Il2CppString {
    void* klass;
    void* monitor;
    int32_t length;
    wchar_t chars[1];
};
typedef Il2CppString*(__fastcall *GetName_t)(void*);

struct alignas(8) StdString {
    union {
        char buf[16];
        char* ptr;
    };
    size_t size;
    size_t capacity;
};
