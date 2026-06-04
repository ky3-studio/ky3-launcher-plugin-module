#pragma once

static void ApplyTheme() {
    ImGuiStyle& s = ImGui::GetStyle();
    s.WindowRounding    = 12.0f;
    s.ChildRounding     = 10.0f;
    s.FrameRounding     = 8.0f;
    s.GrabRounding      = 6.0f;
    s.PopupRounding     = 8.0f;
    s.ScrollbarRounding = 12.0f;
    s.TabRounding       = 6.0f;
    s.WindowBorderSize  = 0.0f;
    s.FrameBorderSize   = 0.0f;
    s.WindowTitleAlign  = {0.5f, 0.5f};
    s.WindowPadding     = {14, 12};
    s.FramePadding      = {10, 5};
    s.ItemSpacing       = {8, 6};
    s.ScrollbarSize     = 10.0f;
    s.GrabMinSize       = 10.0f;
    s.IndentSpacing     = 18.0f;

    ImVec4* c = s.Colors;
    c[ImGuiCol_WindowBg]             = {0.005f, 0.005f, 0.006f, 0.97f};
    c[ImGuiCol_ChildBg]              = {0.007f, 0.007f, 0.008f, 0.98f};
    c[ImGuiCol_PopupBg]              = {0.010f, 0.010f, 0.012f, 0.99f};
    c[ImGuiCol_Border]               = {0.016f, 0.016f, 0.016f, 0.96f};
    c[ImGuiCol_TitleBg]              = {0.005f, 0.005f, 0.006f, 1.00f};
    c[ImGuiCol_TitleBgActive]        = {0.008f, 0.008f, 0.010f, 1.00f};
    c[ImGuiCol_Text]                 = {0.94f, 0.94f, 0.98f, 1.00f};
    c[ImGuiCol_TextDisabled]         = {0.24f, 0.24f, 0.26f, 0.65f};
    c[ImGuiCol_Header]               = {0.016f, 0.016f, 0.020f, 1.00f};
    c[ImGuiCol_HeaderHovered]        = {0.03f, 0.03f, 0.04f, 1.00f};
    c[ImGuiCol_HeaderActive]         = {0.04f, 0.04f, 0.05f, 1.00f};
    c[ImGuiCol_Button]               = {0.022f, 0.022f, 0.028f, 1.00f};
    c[ImGuiCol_ButtonHovered]        = {0.04f, 0.04f, 0.05f, 1.00f};
    c[ImGuiCol_ButtonActive]         = {0.06f, 0.06f, 0.08f, 1.00f};
    c[ImGuiCol_FrameBg]              = {0.016f, 0.016f, 0.020f, 1.00f};
    c[ImGuiCol_FrameBgHovered]       = {0.03f, 0.03f, 0.04f, 1.00f};
    c[ImGuiCol_FrameBgActive]        = {0.04f, 0.04f, 0.05f, 1.00f};
    c[ImGuiCol_SliderGrab]           = {0.40f, 0.40f, 0.40f, 1.00f};
    c[ImGuiCol_SliderGrabActive]     = {0.55f, 0.55f, 0.55f, 1.00f};
    c[ImGuiCol_CheckMark]            = {0.94f, 0.94f, 0.98f, 1.00f};
    c[ImGuiCol_Separator]            = {0.016f, 0.016f, 0.020f, 0.80f};
    c[ImGuiCol_SeparatorHovered]     = {0.04f, 0.04f, 0.05f, 0.80f};
    c[ImGuiCol_SeparatorActive]      = {0.06f, 0.06f, 0.08f, 1.00f};
    c[ImGuiCol_ResizeGrip]           = {0.03f, 0.03f, 0.04f, 0.30f};
    c[ImGuiCol_ResizeGripHovered]    = {0.04f, 0.04f, 0.05f, 0.60f};
    c[ImGuiCol_ResizeGripActive]     = {0.06f, 0.06f, 0.08f, 0.90f};
    c[ImGuiCol_ScrollbarBg]          = {0.005f, 0.005f, 0.006f, 0.50f};
    c[ImGuiCol_ScrollbarGrab]        = {0.03f, 0.03f, 0.04f, 1.00f};
    c[ImGuiCol_ScrollbarGrabHovered] = {0.04f, 0.04f, 0.05f, 1.00f};
    c[ImGuiCol_ScrollbarGrabActive]  = {0.06f, 0.06f, 0.08f, 1.00f};
}
