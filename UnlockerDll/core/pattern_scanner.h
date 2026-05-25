#pragma once

namespace PatternScanner {

    inline void ParsePattern(const char* pattern, std::vector<int>& parsed) {
        const char* p = pattern;
        while (*p) {
            while (*p == ' ') p++;
            if (*p == 0) break;
            if (*p == '?') {
                parsed.push_back(-1);
                p++;
                if (*p == '?') p++;
            } else {
                char* end;
                parsed.push_back((int)strtoul(p, &end, 16));
                p = end;
            }
        }
    }

    inline uintptr_t ScanMainModule(const char* pattern) {
        std::vector<int> parsedPattern;
        ParsePattern(pattern, parsedPattern);
        if (parsedPattern.empty()) return 0;

        HMODULE hModule = GetModuleHandleW(NULL);
        if (!hModule) return 0;

        MODULEINFO modinfo = {};
        if (!K32GetModuleInformation(GetCurrentProcess(), hModule, &modinfo, sizeof(modinfo)))
            return 0;

        BYTE* moduleBase = (BYTE*)modinfo.lpBaseOfDll;
        BYTE* moduleEnd = moduleBase + modinfo.SizeOfImage;
        size_t patternSize = parsedPattern.size();

        MEMORY_BASIC_INFORMATION mbi = {};
        BYTE* currentAddr = moduleBase;

        while (currentAddr < moduleEnd) {
            if (VirtualQuery(currentAddr, &mbi, sizeof(mbi)) == 0) break;

            if (mbi.State == MEM_COMMIT &&
                ((mbi.Protect & 0x10) || (mbi.Protect & 0x20) || (mbi.Protect & 0x40) ||
                 (mbi.Protect & 0x02) || (mbi.Protect & 0x04))) {

                SIZE_T regionSize = mbi.RegionSize;
                if ((BYTE*)mbi.BaseAddress + regionSize > moduleEnd)
                    regionSize = moduleEnd - (BYTE*)mbi.BaseAddress;

                if (regionSize >= patternSize) {
                    SIZE_T maxOffset = regionSize - patternSize;
                    for (SIZE_T offset = 0; offset <= maxOffset; ++offset) {
                        bool found = true;
                        for (size_t j = 0; j < patternSize; ++j) {
                            int patternByte = parsedPattern[j];
                            if (patternByte != -1 &&
                                patternByte != (int)((BYTE*)mbi.BaseAddress)[offset + j]) {
                                found = false;
                                break;
                            }
                        }
                        if (found) {
                            return (uintptr_t)((BYTE*)mbi.BaseAddress + offset);
                        }
                    }
                }
            }
            currentAddr = (BYTE*)mbi.BaseAddress + mbi.RegionSize;
        }
        return 0;
    }

    inline uintptr_t Scan(const char* pattern) {
        return ScanMainModule(pattern);
    }

    inline uintptr_t ResolveRelativeAddress(uintptr_t instructionAddr, size_t offset = 1, size_t instructionSize = 5) {
        if (!instructionAddr) return 0;
        __try {
            int32_t rel = *(int32_t*)(instructionAddr + offset);
            return instructionAddr + instructionSize + rel;
        } __except(EXCEPTION_EXECUTE_HANDLER) {
            return 0;
        }
    }

    inline uintptr_t ResolveCall(uintptr_t addr) {
        return ResolveRelativeAddress(addr, 1, 5);
    }

    inline uintptr_t ScanMulti(const char* patterns[], int count, const char* funcName = nullptr) {
        for (int i = 0; i < count; i++) {
            if (!patterns[i] || !patterns[i][0]) continue;
            uintptr_t addr = ScanMainModule(patterns[i]);
            if (addr) return addr;
        }
        return 0;
    }

    inline uintptr_t ScanNullTerminated(const char* patterns[]) {
        for (int i = 0; patterns[i] != nullptr; i++) {
            uintptr_t addr = ScanMainModule(patterns[i]);
            if (addr) return addr;
        }
        return 0;
    }

    inline bool IsValidFunctionPrologue(uintptr_t addr) {
        if (!addr) return false;
        __try {
            BYTE* p = (BYTE*)addr;
            if (p[0] == 0x40 && (p[1] >= 0x50 && p[1] <= 0x57)) return true;
            if (p[0] == 0x41 && (p[1] >= 0x54 && p[1] <= 0x57)) return true;
            if (p[0] == 0x48 && p[1] == 0x83 && p[2] == 0xEC) return true;
            if (p[0] == 0x48 && p[1] == 0x81 && p[2] == 0xEC) return true;
            if (p[0] == 0x48 && p[1] == 0x89 && p[2] == 0x5C && p[3] == 0x24) return true;
            if (p[0] == 0x55 || p[0] == 0x56 || p[0] == 0x57 || p[0] == 0x53) return true;
            if (p[-1] == 0xCC || p[-2] == 0xCC) return true;
        } __except(EXCEPTION_EXECUTE_HANDLER) {
            return false;
        }
        return false;
    }

    inline uintptr_t ScanWithValidation(const char* patterns[], int count, const char* funcName = nullptr) {
        for (int i = 0; i < count; i++) {
            if (!patterns[i] || !patterns[i][0]) continue;
            uintptr_t addr = ScanMainModule(patterns[i]);
            if (addr && IsValidFunctionPrologue(addr)) return addr;
        }
        return 0;
    }
}
