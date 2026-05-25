#pragma once

void DoCraftLogic() {
    if (!g_FindStringAddr || !g_CraftPartnerAddr) return;

    typedef void*(__fastcall *tFindString)(const char*);
    typedef bool(__fastcall *tCraftPartner)(void*, void*, void*, void*, void*);

    auto findString = (tFindString)g_FindStringAddr;
    auto craftPartner = (tCraftPartner)g_CraftPartnerAddr;

    __try {
        void* pageStr = findString("SynthesisPage");
        if (pageStr) craftPartner(pageStr, nullptr, nullptr, nullptr, nullptr);
    } __except(EXCEPTION_EXECUTE_HANDLER) {}
}

void TriggerCraftDirect() {
    if (!g_FindStringAddr || !g_CraftPartnerAddr) return;

    typedef __int64(*FindString_t)(const char*);
    typedef __int64(*FindStringIl2Cpp_t)(void*);
    typedef void(*CraftPartner_t)(__int64, __int64, __int64, __int64, __int64);

    FindString_t findString = (FindString_t)g_FindStringAddr;
    FindStringIl2Cpp_t findStringIl2Cpp = (FindStringIl2Cpp_t)g_FindStringAddr;
    CraftPartner_t craftPartner = (CraftPartner_t)g_CraftPartnerAddr;

    __try {
        __int64 pageObj = findString("SynthesisPage");

        if (pageObj) {
            craftPartner(pageObj, 0, 0, 0, 0);
            g_craftPageOpen = true;
        }
    } __except(EXCEPTION_EXECUTE_HANDLER) {}
}

void __fastcall HookCraftEntry(void* __this) {
    if (g_config.enableCraft && g_FindStringAddr && g_CraftPartnerAddr) {
        DoCraftLogic();
        return;
    }
    if (g_oCraftEntry) g_oCraftEntry(__this);
}
