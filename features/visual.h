#pragma once
#include <cstdint>

void InitVisual();
__int64 __fastcall HookDisplayFog(__int64 a1, __int64 a2);
void __fastcall HookShowDamage(void* a, int b, int c, int d, float e, void* f, void* g, void* h, int i);
bool __fastcall HookEventCamera(void* a, void* b);
