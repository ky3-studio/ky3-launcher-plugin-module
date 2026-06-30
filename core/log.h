#pragma once
#include <cstdarg>

void LogInit();
void Log(const char* fmt, ...);
void LogClose();
