#pragma once

bool InitGameHooks() {
    using namespace PatternScanner;

    HMODULE hModule = GetModuleHandleW(NULL);
    if (hModule) {
        MODULEINFO modinfo = {};
        if (K32GetModuleInformation(GetCurrentProcess(), hModule, &modinfo, sizeof(modinfo))) {
            g_gameModBase = (uintptr_t)modinfo.lpBaseOfDll;
            g_gameModSize = modinfo.SizeOfImage;
        }
    }

    uintptr_t addr = 0;

    addr = ScanNullTerminated(Signatures::GameUpdate);
    if (addr) {
        addr = ResolveCall(addr);
        if (addr) MH_CreateHook((void*)addr, HookGameUpdate, (void**)&g_oGameUpdate);
    }

    addr = ScanNullTerminated(Signatures::SetFrameCount);
    if (addr) {
        uintptr_t target = ResolveCall(addr);
        if (target) g_Set_FrameCount = (Set_FrameCount_t)target;
    }

    addr = ScanNullTerminated(Signatures::GetFrameCount);
    if (addr) {
        uintptr_t target = ResolveCall(addr);
        if (target) MH_CreateHook((void*)target, HookGetFrameCount, (void**)&g_oGetFrameCount);
    }

    {
        uintptr_t patternAddr = Scan(Signatures::FPS);
        if (patternAddr) {
            g_FpsInstructionAddr = patternAddr;

            uintptr_t setTargetFR = ResolveCall(patternAddr + 5);
            if (setTargetFR) {
                __try {
                    while (*reinterpret_cast<uint8_t*>(setTargetFR) == 0xE9) {
                        int32_t jmpDisp = *reinterpret_cast<int32_t*>(setTargetFR + 1);
                        setTargetFR = setTargetFR + jmpDisp + 5;
                    }
                } __except(EXCEPTION_EXECUTE_HANDLER) {}
                MH_CreateHook((void*)setTargetFR, Hook_set_targetFrameRate, (void**)&g_o_set_targetFrameRate);
            }

            uintptr_t rip = patternAddr + 5;
            while (*reinterpret_cast<uint8_t*>(rip) == 0xE8 || *reinterpret_cast<uint8_t*>(rip) == 0xE9) {
                int32_t disp = *reinterpret_cast<int32_t*>(rip + 1);
                rip = rip + disp + 5;
            }

            int32_t disp = *reinterpret_cast<int32_t*>(rip + 2);
            g_TargetFrameRate = rip + disp + 6;

            MEMORY_BASIC_INFORMATION mbi{};
            if (VirtualQuery((void*)g_TargetFrameRate, &mbi, sizeof(mbi))) {
                if (mbi.Protect != PAGE_READWRITE &&
                    mbi.Protect != PAGE_WRITECOPY &&
                    mbi.Protect != PAGE_EXECUTE_READWRITE &&
                    mbi.Protect != PAGE_EXECUTE_WRITECOPY) {
                    g_TargetFrameRate = 0;
                }
            }
        }
    }

    addr = ScanNullTerminated(Signatures::SyncCount);
    if (addr) {
        addr = ResolveCall(addr);
        if (addr) g_Set_SyncCount = (Set_SyncCount_t)addr;
    }

    addr = ScanNullTerminated(Signatures::FOV);
    if (addr && IsValidFunctionPrologue(addr)) {
        MH_CreateHook((void*)addr, HookChangeFOV, (void**)&g_oChangeFOV);
    }

    addr = ScanNullTerminated(Signatures::Fog);
    if (addr) MH_CreateHook((void*)addr, HookDisplayFog, (void**)&g_oDisplayFog);

    addr = ScanNullTerminated(Signatures::Perspective);
    if (addr) {
        uintptr_t funcAddr = ResolveCall(addr);
        if (funcAddr) MH_CreateHook((void*)funcAddr, HookPlayer_Perspective, (void**)&g_oPlayer_Perspective);
    }

    {
        addr = 0;
        for (int i = 0; Signatures::DamageText[i] && !addr; i++) {
            addr = Scan(Signatures::DamageText[i]);
        }
        if (addr) MH_CreateHook((void*)addr, HookShowDamage, (void**)&g_oShowDamage);
    }

    {
        addr = 0;
        for (int i = 0; Signatures::EventCamera[i] && !addr; i++) {
            addr = Scan(Signatures::EventCamera[i]);
        }
        if (addr) MH_CreateHook((void*)addr, HookEventCamera, (void**)&g_oEventCamera);
    }

    {
        addr = 0;
        for (int i = 0; Signatures::BurstAnimDispatch[i] && !addr; i++) {
            addr = Scan(Signatures::BurstAnimDispatch[i]);
        }
        if (addr) {
            MH_CreateHook((void*)addr, HookBurstAnimDispatcher, (void**)&g_oBurstAnimDispatcher);
            uint8_t* pCmp = (uint8_t*)(addr + 0x0D);
            if (pCmp[0] == 0x80 && pCmp[1] == 0x3D) {
                int32_t ripOffset = *(int32_t*)(pCmp + 2);
                g_pBurstSkipFlag = (volatile uint8_t*)(pCmp + 7 + ripOffset);
            }
        }
    }

    {
        addr = 0;
        for (int i = 0; Signatures::QuestBanner[i] && !addr; i++) {
            addr = Scan(Signatures::QuestBanner[i]);
        }
        if (addr) MH_CreateHook((void*)addr, HookQuestBanner, (void**)&g_oQuestBanner);
    }

    for (int i = 0; Signatures::FindGameObject[i] && !g_FindGameObjectAddr; i++) {
        uintptr_t a = Scan(Signatures::FindGameObject[i]);
        if (a) {
            if (i == 1) a = ResolveCall(a);
            if (a) g_FindGameObjectAddr = a;
        }
    }

    for (int i = 0; Signatures::SetActive[i] && !g_SetActiveAddr; i++) {
        uintptr_t a = Scan(Signatures::SetActive[i]);
        if (a) {
            if (i == 0) a = ResolveCall(a);
            if (a) g_SetActiveAddr = a;
        }
    }

    for (int i = 0; Signatures::FindString[i] && !g_FindStringAddr; i++) {
        uintptr_t a = Scan(Signatures::FindString[i]);
        if (a) g_FindStringAddr = a;
    }

    for (int i = 0; Signatures::CraftPartner[i] && !g_CraftPartnerAddr; i++) {
        uintptr_t a = Scan(Signatures::CraftPartner[i]);
        if (a) g_CraftPartnerAddr = a;
    }

    for (int i = 0; Signatures::InnerDispatcher[i] && !g_InnerDispatcherAddr; i++) {
        uintptr_t a = Scan(Signatures::InnerDispatcher[i]);
        if (a) g_InnerDispatcherAddr = a;
    }

    if (g_InnerDispatcherAddr) {
        uintptr_t idBase = g_InnerDispatcherAddr;
        uintptr_t hashAddr = 0;

        for (uintptr_t scan = idBase; scan < idBase + 0x20000 - 4; scan++) {
            __try {
                if (memcmp((void*)scan, Signatures::CookingHash, 4) != 0) continue;

                uintptr_t cmpStart = 0;
                uintptr_t afterCmp = 0;
                if (*(BYTE*)(scan - 2) == 0x81 && *(BYTE*)(scan - 1) == 0xF9) {
                    cmpStart = scan - 2;
                    afterCmp = scan + 4;
                } else if (*(BYTE*)(scan - 1) == 0x3D) {
                    cmpStart = scan - 1;
                    afterCmp = scan + 4;
                } else continue;

                BYTE b0 = *(BYTE*)afterCmp;
                BYTE b1 = *(BYTE*)(afterCmp + 1);
                bool isCondJmp = (b0 == 0x74) || (b0 == 0x75)
                              || (b0 == 0x0F && (b1 == 0x84 || b1 == 0x85));
                if (isCondJmp) {
                    hashAddr = cmpStart;
                    break;
                }
            } __except(EXCEPTION_EXECUTE_HANDLER) { break; }
        }

        if (hashAddr) {
            for (uintptr_t scan = hashAddr; scan < hashAddr + 0x200; scan++) {
                __try {
                    if (*(BYTE*)scan == 0x41 && *(BYTE*)(scan + 1) == 0x5F && *(BYTE*)(scan + 2) == 0xE9) {
                        int32_t rel = *(int32_t*)(scan + 3);
                        g_CookingHandlerAddr = (scan + 2) + 5 + rel;
                        break;
                    }
                } __except(EXCEPTION_EXECUTE_HANDLER) { break; }
            }
        }

        if (g_CookingHandlerAddr) {
            InitCookingPatchSites(g_CookingHandlerAddr);
            if (g_CookingShowPageAddr) {
                MH_CreateHook((LPVOID)g_CookingShowPageAddr, HookCookingShowPage, (LPVOID*)&g_oCookingShowPage);
            }
        }
    }

    if (g_InnerDispatcherAddr) {
        uintptr_t idBase = g_InnerDispatcherAddr;
        uintptr_t leafJeTarget = 0;

        for (uintptr_t scan = idBase; scan < idBase + 0x20000 - 4; scan++) {
            __try {
                if (memcmp((void*)scan, Signatures::ExpeditionHash, 4) != 0) continue;
                uintptr_t cmpAddr = 0;
                uintptr_t afterCmp = 0;
                if (*(BYTE*)(scan - 2) == 0x81 && *(BYTE*)(scan - 1) == 0xF9) {
                    cmpAddr = scan - 2;
                    afterCmp = scan + 4;
                } else if (*(BYTE*)(scan - 1) == 0x3D) {
                    cmpAddr = scan - 1;
                    afterCmp = scan + 4;
                }
                if (!cmpAddr) continue;

                BYTE b0 = *(BYTE*)afterCmp;
                BYTE b1 = *(BYTE*)(afterCmp + 1);
                bool isCondJmp = (b0 == 0x74) || (b0 == 0x75)
                              || (b0 == 0x0F && (b1 == 0x84 || b1 == 0x85));
                if (!isCondJmp) continue;

                uintptr_t ac = (*(BYTE*)cmpAddr == 0x81) ? cmpAddr + 6 : cmpAddr + 5;
                for (uintptr_t s = ac; s < ac + 10; s++) {
                    if (*(BYTE*)s == 0x0F && *(BYTE*)(s+1) == 0x84) {
                        int32_t d = *(int32_t*)(s + 2);
                        leafJeTarget = s + 6 + d;
                        break;
                    }
                    if (*(BYTE*)s == 0x0F && *(BYTE*)(s+1) == 0x85) {
                        leafJeTarget = s + 6;
                        break;
                    }
                    if (*(BYTE*)s == 0x74) {
                        int8_t d = *(int8_t*)(s + 1);
                        leafJeTarget = s + 2 + d;
                        break;
                    }
                    if (*(BYTE*)s == 0x75) {
                        leafJeTarget = s + 2;
                        break;
                    }
                }
                if (leafJeTarget) break;
            } __except(EXCEPTION_EXECUTE_HANDLER) { break; }
        }

        if (leafJeTarget) {
            for (uintptr_t scan = leafJeTarget; scan < leafJeTarget + 0x200; scan++) {
                __try {
                    if (*(BYTE*)scan == 0x41 && *(BYTE*)(scan+1) == 0x5F && *(BYTE*)(scan+2) == 0xE9) {
                        int32_t rel = *(int32_t*)(scan + 3);
                        g_ExpHandlerAddr = (scan + 2) + 5 + rel;
                        break;
                    }
                } __except(EXCEPTION_EXECUTE_HANDLER) { break; }
            }
        }

        if (g_ExpHandlerAddr) {
            __try {
                memcpy(g_ExpHandlerPrologue, (void*)g_ExpHandlerAddr, 8);
            } __except(EXCEPTION_EXECUTE_HANDLER) {}
        }
    }

    {
        addr = ScanNullTerminated(Signatures::CraftEntry);
        if (addr && g_FindStringAddr && g_CraftPartnerAddr) {
            MH_CreateHook((void*)addr, HookCraftEntry, (void**)&g_oCraftEntry);
        }
    }

    {
        addr = ScanNullTerminated(Signatures::OpenTeam);
        if (addr) {
            g_openTeamScanAddr = addr;
            MH_CreateHook((void*)addr, HookOpenTeam, (void**)&g_oOpenTeam);
        }
    }

    for (int i = 0; Signatures::CheckCanEnter[i] && !g_CheckCanEnterAddr; i++) {
        uintptr_t a = Scan(Signatures::CheckCanEnter[i]);
        if (a) g_CheckCanEnterAddr = a;
    }

    for (int i = 0; Signatures::OpenTeamPage[i] && !g_OpenTeamPageAddr; i++) {
        uintptr_t a = Scan(Signatures::OpenTeamPage[i]);
        if (a) g_OpenTeamPageAddr = a;
    }

    for (int i = 0; Signatures::MobileUI[i] && !g_ChangeMobileUIAddr; i++) {
        uintptr_t a = Scan(Signatures::MobileUI[i]);
        if (a) {
            uintptr_t resolved = ResolveCall(a);
            if (resolved) g_ChangeMobileUIAddr = resolved;
        }
    }

    if (g_CraftPartnerAddr) {
        MH_CreateHook((LPVOID)g_CraftPartnerAddr, HookCraftPartner, (LPVOID*)&g_oCraftPartner);
    }

    if (g_InnerDispatcherAddr) {
        MH_CreateHook((LPVOID)g_InnerDispatcherAddr, HookParentRouter, (LPVOID*)&g_oParentRouter);
    }

    MH_EnableHook(MH_ALL_HOOKS);

    return (g_oGameUpdate || g_FpsAddr || g_TargetFrameRate || g_Set_FrameCount || g_oGetFrameCount || g_oChangeFOV);
}
