#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#include <Windows.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <atomic>
#include <cstdio>
#include <intrin.h>
#include <share.h>

#include "MinHook/MinHook.h"

#pragma comment(lib, "dwmapi.lib")
#include <Psapi.h>
#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "gdi32.lib")

#include "core/config.h"
#include "core/signatures.h"
#include "core/pattern_scanner.h"


#include "features/fps_unlock.h"
#include "features/visual_mods.h"
#include "features/ui_hiding.h"
#include "features/craft.h"
#include "features/cooking.h"
#include "features/expedition.h"
#include "features/team.h"
#include "features/mobile_ui.h"
#include "features/forge.h"

#include "game_loop.h"
#include "init.h"
#include "config_load.h"
