#include "log.h"
#include "state.h"
#include <Windows.h>
#include <cstdio>
#include <share.h>
#include <string>

static FILE* g_logFile = nullptr;

static std::wstring GetLogPath() {
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(g_hModule, path, MAX_PATH);
    std::wstring p = path;
    return p.substr(0, p.find_last_of(L"\\/")) + L"\\dll_log.txt";
}

void LogInit() {
    std::wstring wpath = GetLogPath();
    g_logFile = _wfsopen(wpath.c_str(), L"w", _SH_DENYNO);
    if (g_logFile) {
        SYSTEMTIME st;
        GetLocalTime(&st);
        fprintf(g_logFile, "=== KY3 Plugin Log Started %04d-%02d-%02d %02d:%02d:%02d ===\n",
                st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
        fflush(g_logFile);
    }
}

void Log(const char* fmt, ...) {
    if (!g_logFile) return;
    SYSTEMTIME st;
    GetLocalTime(&st);
    fprintf(g_logFile, "[%02d:%02d:%02d.%03d] ", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    va_list args;
    va_start(args, fmt);
    vfprintf(g_logFile, fmt, args);
    va_end(args);
    fprintf(g_logFile, "\n");
    fflush(g_logFile);
}

void LogClose() {
    if (g_logFile) {
        Log("=== Log Closed ===");
        fclose(g_logFile);
        g_logFile = nullptr;
    }
}
