#pragma once

__int64 __fastcall HookGameUpdate(__int64 a1, const char* a2) {
    auto orig = g_oGameUpdate;
    return orig ? orig(a1, a2) : 0;
}
