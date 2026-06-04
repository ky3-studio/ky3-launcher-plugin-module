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

void __fastcall HookCraftEntry(void* __this) {
    if (g_config.enableCraft && g_FindStringAddr && g_CraftPartnerAddr) {
        DoCraftLogic();
        return;
    }
    if (g_oCraftEntry) g_oCraftEntry(__this);
}

void InitCraft() {
    using namespace PatternScanner;

    for (int i = 0; Signatures::CraftPartner[i] && !g_CraftPartnerAddr; i++) {
        uintptr_t a = Scan(Signatures::CraftPartner[i]);
        if (a) g_CraftPartnerAddr = a;
    }

    uintptr_t addr = ScanNullTerminated(Signatures::CraftEntry);
    if (addr && g_FindStringAddr && g_CraftPartnerAddr) {
        MH_CreateHook((void*)addr, HookCraftEntry, (void**)&g_oCraftEntry);
    }
}
