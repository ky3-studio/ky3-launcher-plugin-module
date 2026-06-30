#pragma once
#include <Windows.h>

extern HHOOK g_mouseHook;

DWORD WINAPI InputThread(LPVOID);
