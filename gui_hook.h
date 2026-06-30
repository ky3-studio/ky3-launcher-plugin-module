#pragma once

static LRESULT CALLBACK GuiWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (g_showMenu && g_imguiReady) {
        ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);
        switch (msg) {
        case WM_LBUTTONDOWN: case WM_LBUTTONUP: case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDOWN: case WM_RBUTTONUP: case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDOWN: case WM_MBUTTONUP: case WM_MBUTTONDBLCLK:
        case WM_MOUSEWHEEL:  case WM_MOUSEHWHEEL:
        case WM_MOUSEMOVE:
        case WM_KEYDOWN: case WM_KEYUP:
        case WM_SYSKEYDOWN: case WM_SYSKEYUP:
        case WM_CHAR: case WM_SYSCHAR:
        case WM_XBUTTONDOWN: case WM_XBUTTONUP:
        case WM_INPUT:
            return 0;
        }
    }

    return g_oldWndProc ? CallWindowProcW(g_oldWndProc, hwnd, msg, wParam, lParam)
                        : DefWindowProcW(hwnd, msg, wParam, lParam);
}

static void RenderImGuiFrame() {
    __try {
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        ImGuiIO& io = ImGui::GetIO();
        io.MouseDrawCursor = g_showMenu;
        if (g_showMenu) {
            io.ConfigFlags &= ~ImGuiConfigFlags_NoMouseCursorChange;
            ClipCursor(nullptr);
        } else {
            io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
        }
        DrawMenu();
        ImGui::Render();
        if (g_d3dContext && g_rtv) {
            g_d3dContext->OMSetRenderTargets(1, &g_rtv, nullptr);
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        }
    } __except(EXCEPTION_EXECUTE_HANDLER) {}
}

static HRESULT __stdcall hkPresent11(IDXGISwapChain* pSwapChain, UINT sync, UINT flags) {
    if (!pSwapChain || !oPresent11) return E_FAIL;

    if (!g_d3dDevice) {
        ID3D11Device* dev = nullptr;
        if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&dev))) {
            g_d3dDevice = dev;
            g_d3dDevice->GetImmediateContext(&g_d3dContext);
        }
    }

    if (g_d3dDevice) {
        if (g_rtv) { g_rtv->Release(); g_rtv = nullptr; }
        ID3D11Texture2D* backBuf = nullptr;
        if (SUCCEEDED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuf))) {
            g_d3dDevice->CreateRenderTargetView(backBuf, nullptr, &g_rtv);
            backBuf->Release();
        }
    }

    if (g_d3dDevice && g_d3dContext && g_rtv && !g_imguiReady) {
        DXGI_SWAP_CHAIN_DESC desc{};
        if (SUCCEEDED(pSwapChain->GetDesc(&desc))) {
            ImGui::CreateContext();
            ApplyTheme();
            ImGuiIO& io = ImGui::GetIO();
            io.IniFilename = nullptr;
            io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

            char winDir[MAX_PATH]{};
            if (GetWindowsDirectoryA(winDir, MAX_PATH)) {
                char fontPath[MAX_PATH];
                snprintf(fontPath, MAX_PATH, "%s\\Fonts\\msyh.ttc", winDir);
                ImFontGlyphRangesBuilder builder;
                builder.AddRanges(io.Fonts->GetGlyphRangesDefault());
                builder.AddRanges(io.Fonts->GetGlyphRangesChineseFull());
                ImVector<ImWchar> ranges;
                builder.BuildRanges(&ranges);
                io.Fonts->AddFontFromFileTTF(fontPath, 16.0f, nullptr, ranges.Data);
            }
            if (io.Fonts->Fonts.Size == 0)
                io.Fonts->AddFontDefault();

            ImGui_ImplWin32_Init(desc.OutputWindow);
            ImGui_ImplDX11_Init(g_d3dDevice, g_d3dContext);

            g_oldWndProc = (WNDPROC)SetWindowLongPtrW(desc.OutputWindow, GWLP_WNDPROC, (LONG_PTR)GuiWndProc);
            g_imguiReady = true;
        }
    }

    if (g_imguiReady) {
        RenderImGuiFrame();
    }

    return oPresent11(pSwapChain, sync, flags);
}

static HRESULT __stdcall hkResizeBuffers11(IDXGISwapChain* pSwapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT format, UINT flags) {
    if (g_rtv) { g_rtv->Release(); g_rtv = nullptr; }
    if (g_imguiReady) {
        ImGui_ImplDX11_InvalidateDeviceObjects();
    }
    return oResizeBuffers11(pSwapChain, bufferCount, width, height, format, flags);
}

static bool InitDX11Hook() {
    HMODULE hD3D11 = GetModuleHandleA("d3d11.dll");
    if (!hD3D11) return false;

    auto pCreate = (decltype(&D3D11CreateDeviceAndSwapChain))
        GetProcAddress(hD3D11, "D3D11CreateDeviceAndSwapChain");
    if (!pCreate) return false;

    WNDCLASSEX wc{}; wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = DefWindowProc;
    wc.hInstance = GetModuleHandleA(NULL);
    wc.lpszClassName = "KY3GC";
    RegisterClassEx(&wc);
    HWND hw = CreateWindowA(wc.lpszClassName, "KY", WS_OVERLAPPEDWINDOW,
                            0, 0, 100, 100, NULL, NULL, wc.hInstance, NULL);

    const D3D_FEATURE_LEVEL fls[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1 };
    D3D_FEATURE_LEVEL fl;
    DXGI_SWAP_CHAIN_DESC scd{};
    scd.BufferCount = 1;
    scd.BufferDesc.Width = 100; scd.BufferDesc.Height = 100;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = hw; scd.SampleDesc.Count = 1;
    scd.Windowed = TRUE; scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    IDXGISwapChain* sc = nullptr; ID3D11Device* dev = nullptr; ID3D11DeviceContext* ctx = nullptr;
    HRESULT hr = pCreate(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0,
                         fls, 2, D3D11_SDK_VERSION, &scd, &sc, &dev, &fl, &ctx);
    if (FAILED(hr) || !sc) {
        DestroyWindow(hw); UnregisterClassA(wc.lpszClassName, wc.hInstance);
        return false;
    }

    void** vtable = *(void***)sc;
    void* pPresent = vtable[8];
    void* pResize  = vtable[13];
    sc->Release(); dev->Release(); ctx->Release();
    DestroyWindow(hw); UnregisterClassA(wc.lpszClassName, wc.hInstance);

    int mh = MH_CreateHook(pPresent, (void*)hkPresent11, (void**)&oPresent11);
    if (mh != MH_OK) return false;
    int mh2 = MH_CreateHook(pResize, (void*)hkResizeBuffers11, (void**)&oResizeBuffers11);
    if (mh2 != MH_OK) return false;
    return true;
}

static void InitGui() {
    for (int attempt = 0; attempt < 5; ++attempt) {
        if (InitDX11Hook()) return;
        Sleep(3000);
    }
}

static void ShutdownGui() {
    if (g_imguiReady) {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
        g_imguiReady = false;
    }
    if (g_oldWndProc && g_GameWindow)
        SetWindowLongPtrW(g_GameWindow, GWLP_WNDPROC, (LONG_PTR)g_oldWndProc);
    if (g_rtv)       { g_rtv->Release(); g_rtv = nullptr; }
    if (g_d3dContext) { g_d3dContext->Release(); g_d3dContext = nullptr; }
    if (g_d3dDevice)  { g_d3dDevice->Release(); g_d3dDevice = nullptr; }
}
