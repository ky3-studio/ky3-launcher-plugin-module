#pragma once

typedef __int64(__fastcall *tParentRouter)(__int64 a1, __int64 a2, __int64 a3, __int64 a4, __int64 a5, int a6, __int64 a7);
static tParentRouter g_oParentRouter = nullptr;

static __int64 g_expArgs[7] = {0};
static bool g_expCaptured = false;

static uintptr_t g_ExpHandlerAddr = 0;
static BYTE g_ExpHandlerPrologue[8] = {0};

typedef void(__fastcall *tOpenPage)(__int64, __int64, int, int);
static tOpenPage g_oOpenPage = nullptr;

void __fastcall HookOpenPage(__int64 mgr, __int64 pageCtx, int flag1, int flag2) {
    g_oOpenPage(mgr, pageCtx, flag1, flag2);
}

typedef __int64(__fastcall *tCraftPartnerHook)(__int64 a1, __int64 a2, void* a3, __int64 a4, __int64 a5);
static tCraftPartnerHook g_oCraftPartner = nullptr;
static std::atomic<bool> g_isOurDispatchCall(false);

__int64 __fastcall HookParentRouter(__int64 a1, __int64 a2, __int64 a3, __int64 a4, __int64 a5, int a6, __int64 a7) {
    return g_oParentRouter(a1, a2, a3, a4, a5, a6, a7);
}

__int64 __fastcall HookCraftPartner(__int64 a1, __int64 a2, void* a3, __int64 a4, __int64 a5) {
    return g_oCraftPartner(a1, a2, a3, a4, a5);
}

void DoDispatchLogic() {
    uintptr_t handlerAddr = g_ExpHandlerAddr;
    if (!handlerAddr) return;

    __try {
        if (memcmp((void*)handlerAddr, g_ExpHandlerPrologue, 8) != 0) {
            g_ExpHandlerAddr = 0;
            return;
        }
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        g_ExpHandlerAddr = 0;
        return;
    }

    typedef bool(__fastcall *tExpHandler)(__int64 param1, __int64 param2);
    static BYTE dummyBuf[4096] = {0};

    auto handler = (tExpHandler)handlerAddr;
    __try {
        handler(0, (__int64)dummyBuf);
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        __try {
            handler(0, 0);
        } __except(EXCEPTION_EXECUTE_HANDLER) {
            g_ExpHandlerAddr = 0;
        }
    }
}
