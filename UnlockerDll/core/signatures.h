#pragma once

namespace Signatures {

    static const char* GameUpdate[] = {
        "E8 ? ? ? ? 48 8D 4C 24 ? 8B F8 FF 15 ? ? ? ? E8 ? ? ? ?",
        "E8 ? ? ? ? 48 8D 4C 24 ? 8B F8 FF 15",
        "E8 ? ? ? ? 48 8D 4C 24 ? 8B F8",
        "E8 ? ? ? ? 48 8D 4C 24",
        nullptr
    };

    static const char* SetFrameCount[] = {
        "E8 ? ? ? ? E8 ? ? ? ? 83 F8 1F 0F 9C 05 ? ? ? ? 48 8B 05 ? ? ? ?",
        "E8 ? ? ? ? E8 ? ? ? ? 83 F8 1F 0F 9C 05",
        "E8 ? ? ? ? E8 ? ? ? ? 83 F8 1F",
        nullptr
    };

    static const char* GetFrameCount[] = {
        "E8 ? ? ? ? 85 C0 7E 0E E8 ? ? ? ? 0F 57 C0 F3 0F 2A C0 EB 08",
        "E8 ? ? ? ? 85 C0 7E 0E E8 ? ? ? ? 0F 57 C0",
        "E8 ? ? ? ? 85 C0 7E 0E E8",
        nullptr
    };

    static const char* FPS = "B9 3C 00 00 00 E8";

    static const char* SyncCount[] = {
        "E8 ? ? ? ? E8 ? ? ? ? 89 C6 E8 ? ? ? ? 31 C9 89 F2 49 89 C0 E8 ? ? ? ? 48 89 C6 48 8B 0D ? ? ? ? 80 B9 ? ? ? ? ? 74 47 48 8B 3D ? ? ? ? 48 85 DF 74 4C",
        "E8 ? ? ? ? E8 ? ? ? ? 89 C6 E8 ? ? ? ? 31 C9 89 F2 49 89 C0 E8 ? ? ? ? 48 89 C6",
        "E8 ? ? ? ? E8 ? ? ? ? 89 C6 E8 ? ? ? ? 31 C9 89 F2 49 89 C0",
        "E8 ? ? ? ? E8 ? ? ? ? 89 C6 E8 ? ? ? ? 31 C9",
        nullptr
    };

    static const char* FOV[] = {
        "40 53 48 83 EC 60 0F 29 74 24 ? 48 8B D9 0F 28 F1 E8 ? ? ? ? 48 85 C0 0F 84 ? ? ? ? E8 ? ? ? ? 48 8B C8",
        "40 53 48 83 EC 60 0F 29 74 24 ? 48 8B D9 0F 28 F1 E8 ? ? ? ? 48 85 C0 0F 84",
        "40 53 48 83 EC 60 0F 29 74 24 ? 48 8B D9 0F 28 F1 E8 ? ? ? ? 48 85 C0",
        "40 53 48 83 EC 60 0F 29 74 24 ? 48 8B D9 0F 28 F1",
        "40 53 48 83 EC 60 0F 29 74 24 ? 48 8B D9",
        nullptr
    };

    static const char* Fog[] = {
        "0F B6 02 88 01 8B 42 04 89 41 04 F3 0F 10 52 ? F3 0F 10 4A ? F3 0F 10 42 ? 8B 42 08",
        "0F B6 02 88 01 8B 42 04 89 41 04 F3 0F 10 52 ? F3 0F 10 4A ? F3 0F 10 42 ?",
        "0F B6 02 88 01 8B 42 04 89 41 04 F3 0F 10 52 ? F3 0F 10 4A ?",
        "0F B6 02 88 01 8B 42 04 89 41 04 F3 0F 10 52 ?",
        nullptr
    };

    static const char* Perspective[] = {
        "E8 ? ? ? ? 48 8B BE ? ? ? ? 80 3D ? ? ? ? ? 0F 85 ? ? ? ? 80 BE ? ? ? ? ? 74 11",
        "E8 ? ? ? ? 48 8B BE ? ? ? ? 80 3D ? ? ? ? ? 0F 85",
        "E8 ? ? ? ? 48 8B BE ? ? ? ? 80 3D",
        nullptr
    };

    static const char* DamageText[] = {
        "41 57 41 56 41 55 41 54 56 57 55 53 48 81 EC ? ? ? ? 44 0F 29 9C 24 ? ? ? ? 44 0F 29 94 24 ? ? ? ? 44 0F 29 8C 24 ? ? ? ? 44 0F 29 84 24 ? ? ? ? 0F 29 BC 24 ? ? ? ? 0F 29 B4 24 ? ? ? ? 44 89 CF 45 89 C4",
        "41 57 41 56 41 55 41 54 56 57 55 53 48 81 EC ? ? ? ? 44 0F 29 9C 24",
        nullptr
    };

    static const char* EventCamera[] = {
        "41 57 41 56 56 57 55 53 48 83 EC ? 48 89 D7 49 89 CE 80 3D ? ? ? ? 00 0F 85 ? ? ? ? 80 3D ? ? ? ? 00",
        "41 57 41 56 56 57 55 53 48 83 EC ? 48 89 D7 49 89 CE 80 3D",
        nullptr
    };

    static const char* BurstAnimDispatch[] = {
        "56 57 53 48 83 EC 40 48 89 D7 48 89 CB 80 3D ? ? ? ? 00 0F 85 ? ? ? ? 80 3D ? ? ? ? 00",
        "56 57 53 48 83 EC 40 48 89 D7 48 89 CB 80 3D",
        nullptr
    };

    static const char* QuestBanner[] = {
        "41 57 41 56 56 57 55 53 48 81 EC ? ? ? ? 0F 29 BC 24 ? ? ? ? 0F 29 B4 24 ? ? ? ? 48 89 CE 80 3D ? ? ? ? 00 0F 85 ? ? ? ? 48 8B 96",
        "41 57 41 56 56 57 55 53 48 81 EC ? ? ? ? 0F 29 BC 24 ? ? ? ? 0F 29 B4 24 ? ? ? ? 48 89 CE 80 3D",
        nullptr
    };

    static const char* FindGameObject[] = {
        "40 53 48 83 EC ? 48 89 4C 24 ? 48 8D 54 24 ? 48 8D 4C 24 ? E8 ? ? ? ? 48 8B 08 48 85 C9 75 ? 48 8D 48 ? E8 ? ? ? ? 48 8B 4C 24 ? 48 8B D8 48 85 C9 74 ? 48 83 7C 24 ? 00 76",
        "E9 ? ? ? ? 66 66 2E 0F 1F 84 00 ? ? ? ? E9 ? ? ? ? 66 66 2E 0F 1F 84 00 ? ? ? ? 48 83 EC",
        "48 83 EC ? C7 44 24 ? 00 00 00 00 48 8D 54 24",
        nullptr
    };

    static const char* SetActive[] = {
        "E8 ? ? ? ? 48 8B 56 ? 48 85 D2 0F 84 ? ? ? ? 80 3D ? ? ? ? 0 0F 85 ? ? ? ? 48 89 D1 E8 ? ? ? ? 48 85 C0 0F 84 ? ? ? ? 48 89 C1",
        "40 53 48 83 EC ? 48 8B D9 84 D2 74 ? 48 8B 49 ? 48 85 C9 74",
        "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8B F9 0F B6 F2",
        "40 53 48 83 EC ? 48 8B D9 0F B6 CA 48 8B 01 FF 50",
        nullptr
    };

    static const char* FindString[] = {
        "56 48 83 EC 20 48 89 CE E8 ? ? ? ? 48 89 F1 89 C2 48 83 C4 20 5E E9 ? ? ? ? CC CC CC CC",
        "56 48 83 EC 20 48 89 CE E8 ? ? ? ? 48 89 F1 89 C2 48 83 C4 20 5E E9",
        "56 48 83 EC 20 48 89 CE E8 ? ? ? ? 48 89 F1 89 C2",
        nullptr
    };

    static const char* CraftPartner[] = {
        "41 57 41 56 41 55 41 54 56 57 55 53 48 81 EC ? ? ? ? 4D 89 ? 4C 89 C6 49 89 D4 49 89 CE",
        "41 57 41 56 41 55 41 54 56 57 55 53 48 81 EC ? ? ? ? 4D 89",
        nullptr
    };

    static const char* InnerDispatcher[] = {
        "41 57 41 56 41 55 41 54 56 57 55 53 48 81 EC E8 00 00 00 4C 89 CB 4C 89 C7 48 89 D5 48 89 CA",
        "41 57 41 56 41 55 41 54 56 57 55 53 48 81 EC E8 00 00 00 4C 89 CB 4C 89 C7",
        "41 57 41 56 41 55 41 54 56 57 55 53 48 81 EC ? 00 00 00 4C 89 CB 4C 89 C7 48 89 D5 48 89 CA",
        nullptr
    };

    static const char* CraftEntry[] = {
        "41 56 56 57 53 48 83 EC 58 49 89 CE 80 3D ? ? ? ? 00 0F 84 ? ? ? ? 80 3D ? ? ? ? 00 48 8B 0D ? ? ? ? 0F 85",
        "41 56 56 57 53 48 83 EC 58 49 89 CE 80 3D ? ? ? ? 00 0F 84",
        "41 56 56 57 53 48 83 EC 58 49 89 CE",
        nullptr
    };

    static const char* OpenTeam[] = {
        "48 83 EC ? 80 3D ? ? ? ? 00 75 ? 48 8B 0D ? ? ? ? 80 B9 ? ? ? ? 00 0F 84 ? ? ? ? B9 ? ? ? ? E8 ? ? ? ? 84 C0 75",
        "48 83 EC ? 80 3D ? ? ? ? 00 75 ? 48 8B 0D ? ? ? ? 80 B9 ? ? ? ? 00 0F 84",
        nullptr
    };

    static const char* CheckCanEnter[] = {
        "56 48 81 EC 80 00 00 00 80 3D ? ? ? ? 00 0F 84 ? ? ? ? 80 3D ? ? ? ? 00",
        "56 48 81 EC 80 00 00 00 80 3D ? ? ? ? 00 0F 84",
        "56 48 81 EC 80 00 00 00 80 3D",
        nullptr
    };

    static const char* OpenTeamPage[] = {
        "56 57 53 48 83 EC 20 89 CB 80 3D ? ? ? ? 00 74 7A 80 3D ? ? ? ? 00 48 8B 05",
        "56 57 53 48 83 EC 20 89 CB 80 3D ? ? ? ? 00 74 ? 80 3D ? ? ? ? 00 48 8B 05",
        "56 57 53 48 83 EC 20 89 CB 80 3D",
        nullptr
    };

    static const char* MobileUI[] = {
        "E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 8B 90 ? ? ? ? 48 85 D2 0F 84 ? ? ? ? 80 3D ? ? ? ? ? 0F 85 ? ? ? ? C6 42 ? ? 48 8B 90",
        "E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 8B 90 ? ? ? ? 48 85 D2 0F 84 ? ? ? ? 80 3D",
        nullptr
    };

    static const BYTE CookingHash[] = { 0x1C, 0xCE, 0x1B, 0x4C };
    static const BYTE ExpeditionHash[] = { 0xE1, 0x73, 0x90, 0x69 };
}
