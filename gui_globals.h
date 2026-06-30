#pragma once

#include <d3d11.h>
#include <dxgi.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

#include "GUI/imgui.h"
#include "GUI/imgui_internal.h"
#include "GUI/imgui_impl_dx11.h"
#include "GUI/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

static ID3D11Device*           g_d3dDevice  = nullptr;
static ID3D11DeviceContext*    g_d3dContext = nullptr;
static ID3D11RenderTargetView* g_rtv        = nullptr;
static WNDPROC                 g_oldWndProc = nullptr;
static bool                    g_imguiReady = false;
extern std::atomic<bool>       g_showMenu;
static float                   g_menuSlide  = 0.f;
static int                     g_menuTab    = 0;
static HRESULT(__stdcall* oPresent11)(IDXGISwapChain*, UINT, UINT) = nullptr;
static HRESULT(__stdcall* oResizeBuffers11)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT) = nullptr;
