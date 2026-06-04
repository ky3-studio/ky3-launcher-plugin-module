#pragma once

static bool g_configDirty = false;
static DWORD g_lastSaveTick = 0;

static void DrawMenu() {
    float dt = ImGui::GetIO().DeltaTime;
    float target = g_showMenu ? 1.f : 0.f;
    g_menuSlide += (target - g_menuSlide) * ImClamp(dt * 12.f, 0.f, 1.f);
    if (g_menuSlide < 0.005f && !g_showMenu) { g_menuSlide = 0.f; return; }

    ImGuiIO& io = ImGui::GetIO();
    float menuW = 580.f;
    float menuH = ImClamp(io.DisplaySize.y * 0.70f, 380.f, io.DisplaySize.y - 40.f);
    float posX = io.DisplaySize.x - menuW * g_menuSlide;
    float posY = (io.DisplaySize.y - menuH) * 0.5f;

    ImGui::SetNextWindowPos(ImVec2(posX, posY));
    ImGui::SetNextWindowSize(ImVec2(menuW, menuH));
    ImGuiWindowFlags wf = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoTitleBar;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12.f);
    if (!ImGui::Begin("##unlocker_menu", nullptr, wf)) {
        ImGui::End();
        ImGui::PopStyleVar();
        return;
    }

    const float sideW = 120.f;
    ImVec2 region = ImGui::GetContentRegionAvail();

    ImGui::BeginChild("##nav", ImVec2(sideW, region.y), true);
    ImGui::TextColored({0.94f, 0.94f, 0.98f, 0.5f}, "KY3-LAUNCHER");
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    const char* tabNames[] = {
        "\xe6\x80\xa7\xe8\x83\xbd",
        "\xe8\xa7\x86\xe8\xa7\x89",
        "\xe5\xbf\xab\xe6\x8d\xb7",
        "UI",
    };
    for (int i = 0; i < 4; ++i) {
        bool sel = (g_menuTab == i);
        if (sel) {
            ImGui::PushStyleColor(ImGuiCol_Button, {0.06f, 0.06f, 0.08f, 1.f});
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0.08f, 0.08f, 0.10f, 1.f});
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, {0.10f, 0.10f, 0.12f, 1.f});
        }
        if (ImGui::Button(tabNames[i], ImVec2(sideW - 16.f, 30.f))) g_menuTab = i;
        if (sel) ImGui::PopStyleColor(3);
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("##content", ImVec2(0, region.y), true);

    bool changed = false;

    if (g_menuTab == 0) {
        changed |= ImGui::Checkbox("\xe8\xa7\xa3\xe9\x94\x81\xe5\xb8\xa7\xe7\x8e\x87", &g_config.enableFps);
        if (g_config.enableFps) {
            changed |= ImGui::SliderInt("FPS", &g_config.targetFps, 30, 360);
        }
        changed |= ImGui::Checkbox("\xe5\x85\xb3\xe9\x97\xad\xe5\x9e\x82\xe7\x9b\xb4\xe5\x90\x8c\xe6\xad\xa5", &g_config.disableVSync);
    } else if (g_menuTab == 1) {
        changed |= ImGui::Checkbox("\xe8\x87\xaa\xe5\xae\x9a\xe4\xb9\x89 FOV", &g_config.enableFov);
        if (g_config.enableFov) {
            changed |= ImGui::SliderFloat("FOV", &g_config.targetFov, 20.0f, 120.0f, "%.0f");
        }
        changed |= ImGui::Checkbox("\xe5\x85\xb3\xe9\x97\xad\xe9\x9b\xbe\xe6\x95\x88", &g_config.disableFog);
        changed |= ImGui::Checkbox("\xe5\x85\xb3\xe9\x97\xad\xe8\xa7\x92\xe8\x89\xb2\xe6\xb7\xa1\xe5\x87\xba", &g_config.disableCharFade);
        changed |= ImGui::Checkbox("\xe5\x85\xb3\xe9\x97\xad\xe4\xbc\xa4\xe5\xae\xb3\xe6\x95\xb0\xe5\xad\x97", &g_config.disableDamageText);
        changed |= ImGui::Checkbox("\xe5\x85\xb3\xe9\x97\xad\xe9\x95\x9c\xe5\xa4\xb4\xe5\x8a\xa8\xe7\x94\xbb", &g_config.disableCameraAnim);
        changed |= ImGui::Checkbox("\xe5\x85\xb3\xe9\x97\xad\xe8\x8d\x89\xe5\x9c\xb0", &g_config.enableNoGrass);
    } else if (g_menuTab == 2) {
        changed |= ImGui::Checkbox("\xe4\xbe\xbf\xe6\x90\xba\xe5\x90\x88\xe6\x88\x90", &g_config.redirectCraft);
        changed |= ImGui::Checkbox("\xe4\xbe\xbf\xe6\x90\xba\xe6\xb4\xbe\xe9\x81\xa3", &g_config.redirectDispatch);
        changed |= ImGui::Checkbox("\xe4\xbe\xbf\xe6\x90\xba\xe7\x83\xb9\xe9\xa5\xaa", &g_config.enableCooking);
        changed |= ImGui::Checkbox("\xe4\xbe\xbf\xe6\x90\xba\xe9\x94\xbb\xe9\x80\xa0", &g_config.enableForge);
        ImGui::Separator();
        changed |= ImGui::Checkbox("\xe5\xbf\xab\xe9\x80\x9f\xe7\xbb\x84\xe9\x98\x9f", &g_config.removeTeamAnim);
    } else if (g_menuTab == 3) {
        changed |= ImGui::Checkbox("\xe9\x9a\x90\xe8\x97\x8f UID", &g_config.hideUID);
        changed |= ImGui::Checkbox("\xe9\x9a\x90\xe8\x97\x8f\xe8\x8f\x9c\xe5\x8d\x95 UID", &g_config.hideMenuUID);
        changed |= ImGui::Checkbox("\xe9\x9a\x90\xe8\x97\x8f\xe4\xbb\xbb\xe5\x8a\xa1\xe6\xa8\xaa\xe5\xb9\x85", &g_config.hideQuestBanner);
    }

    if (changed) g_configDirty = true;

    DWORD now = GetTickCount();
    if (g_configDirty && (now - g_lastSaveTick > 300)) {
        SaveConfig();
        g_configDirty = false;
        g_lastSaveTick = now;
    }

    ImGui::EndChild();
    ImGui::End();
    ImGui::PopStyleVar();
}
