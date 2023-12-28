#include "pch.h"
#include "Utils.h"
#include "Player.h"
#include <cstdio>
#include <iostream>
#include <thread>
#include <numbers>

#define _CRT_SECURE_NO_WARNINGS

enum class ModuleOffsets : uintptr_t {
    LocalPlayer = 0x18AC00,
    EntityList = 0x18ac04,
    PlayerCount = 0x18ac0c
};

uintptr_t jumpBack = 0;
uintptr_t currentCrossHairEntityAddr = 0;

void __declspec(naked) getCrossHairEnt() {
    __asm {
        mov currentCrossHairEntityAddr, eax
        add esp, [10h]
        mov [esp + 10h], eax
        jmp jumpBack
    }
}


void SetupConsole()
{
    //Allocate a console and make sure I can write to it
    AllocConsole();
    freopen("CONOUT$", "wb", stdout);
    freopen("CONOUT$", "wb", stderr);
    freopen("CONIN$", "rb", stdin);
    SetConsoleTitle(L"PoC Innovation ;)");
}

void moveViewToEnemy(Player& local, Player& enemy)
{
    auto to_deg = [](const float rad) {
        return rad * 180.f / std::numbers::pi_v<float>;
	};
    float abs_x = enemy.head_pos.x - local.head_pos.x;
    float abs_y = enemy.head_pos.y - local.head_pos.y;
    float abs_z = enemy.head_pos.z - local.head_pos.z;

    //float len = std::sqrt(abs_x * abs_x + abs_y * abs_y + abs_z * abs_z);
    local.yaw = to_deg(std::atan2(abs_y, abs_x)) + 90;
    local.pitch = to_deg(std::asin(abs_z / std::hypot(abs_x, abs_y, abs_z)));
}

float euclidianDistance(float x, float y)
{
    return std::sqrtf(x * x + y * y);
}

inline void clickMouse()
{
    INPUT input = { 0 };

    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1, &input, sizeof(INPUT));
}

inline void releaseMouse()
{
    INPUT input = { 0 };

    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, &input, sizeof(INPUT));
}

DWORD WINAPI MainEntry(LPVOID module)
{
    uintptr_t module_base_addr = reinterpret_cast<uintptr_t>(GetModuleHandle(NULL)); // get the moduleBaseAdress of current module (ac_client.exe)
    std::cout << "ac_client.exe: " << std::hex << module_base_addr << std::dec << std::endl;
    uintptr_t local_player_ptr = module_base_addr + static_cast<uintptr_t>(ModuleOffsets::LocalPlayer);
    Player* local_player = *reinterpret_cast<Player**>(local_player_ptr);
    size_t *player_count = reinterpret_cast<size_t*>(module_base_addr + static_cast<uintptr_t>(ModuleOffsets::PlayerCount));
    std::cout << "player_count: " << *player_count << std::endl; // doesn't work on empty map
    uintptr_t entity_list_ptr = module_base_addr + static_cast<uintptr_t>(ModuleOffsets::EntityList);
    std::cout << "entity_list: " << entity_list_ptr << std::endl;
    uintptr_t entity_list = *reinterpret_cast<uintptr_t*>(entity_list_ptr);

    int stolen_len = 7;
    DWORD traceray_hook = 0x45F573;
    jumpBack = (DWORD)(traceray_hook + stolen_len);
    Hook((void*)traceray_hook, getCrossHairEnt, stolen_len);
    while (!(GetAsyncKeyState(VK_ESCAPE) & 0x01)) {
        if (!(local_player != nullptr && *player_count != 0))
            break;
        float closest_distance = 100000000.0;
        size_t closest_player_id = 0;
        for (size_t i = 1; i < *player_count; ++i) {
			Player* enemy = *reinterpret_cast<Player**>(entity_list + (4 * i));
            if (enemy->health <= 0) continue;
			float abs_x = enemy->head_pos.x - local_player->head_pos.x;
			float abs_y = enemy->head_pos.y - local_player->head_pos.y;
            auto temp_closest_distance = euclidianDistance(abs_x, abs_y);
            if (temp_closest_distance < closest_distance) {
                closest_distance = temp_closest_distance;
                closest_player_id = i;
            }
        }
        if (closest_player_id != 0 && GetAsyncKeyState(VK_XBUTTON2)) {
			Player* enemy = *reinterpret_cast<Player**>(entity_list + (4 * closest_player_id));
            moveViewToEnemy(*local_player, *enemy);
        }
        if (currentCrossHairEntityAddr) {
            clickMouse();
        } else {
            releaseMouse();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    Patch((void*)traceray_hook, "\x83\xC4\x10\x89\x44\x24\x10", 7);

    std::cout << "Cheat closed\n";

    //std::cin.ignore();
    //MessageBox(NULL, L"coucou", L"test", MB_OK);

    FreeConsole(); // the console doesn't close automaticaly (windows 11 problem ?)
    FreeLibraryAndExitThread(reinterpret_cast<HMODULE>(module), 0);
    return 0;
}

//https://learn.microsoft.com/fr-fr/windows/win32/dlls/dllmain
BOOL APIENTRY DllMain( HMODULE module,
                       DWORD  ul_reason_for_call,
                       LPVOID lp_reserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        SetupConsole();
        CreateThread(NULL, 0, MainEntry, module, 0, nullptr);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

