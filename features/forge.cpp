#include "forge.h"
#include "../core/state.h"
#include "../core/config.h"
#include "../core/log.h"
#include "../core/safe.h"

void DoForgeLogic() {
    if (!g_FindStringAddr || !g_CraftPartnerAddr) return;

    typedef void*(__fastcall *tFindString)(const char*);
    typedef bool(__fastcall *tCraftPartner)(void*, void*, void*, void*, void*);

    auto findString = (tFindString)g_FindStringAddr;
    auto craftPartner = (tCraftPartner)g_CraftPartnerAddr;

    __try {
        void* pageStr = findString("ForgingPage");
        if (pageStr) craftPartner(pageStr, nullptr, nullptr, nullptr, nullptr);
    } __except(EXCEPTION_EXECUTE_HANDLER) {}
}
