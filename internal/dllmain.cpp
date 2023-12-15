#include "pch.h"
#include <cstdio>
#include <iostream>
#include <thread>

#define _CRT_SECURE_NO_WARNINGS

enum class ModuleOffsets : uintptr_t {
    LocalPlayer = 0x18AC00,
    EntityList = 0x18ac04,
    PlayerCount = 0x18ac0c
};

class Vector3 {
public:
    float x, y, z;
    friend std::ostream& operator<<(std::ostream& os, const Vector3& vec) {
        os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
        return os;
    }
};

class Player
{
public:
    char pad_0000[4]; //0x0000
    Vector3 head_pos; //0x0004
    char pad_0010[24]; //0x0010
    Vector3 feet_pos; //0x0028
    float yaw; //0x0034
    float pitch; //0x0038
    char pad_003C[176]; //0x003C
    int32_t health; //0x00EC
    int32_t armor; //0x00F0
    char pad_00F4[156]; //0x00F4
}; //Size: 0x0190

void SetupConsole()
{
    //Allocate a console and make sure I can write to it
    AllocConsole();
    freopen("CONOUT$", "wb", stdout);
    freopen("CONOUT$", "wb", stderr);
    freopen("CONIN$", "rb", stdin);
    SetConsoleTitle(L"PoC Innovation ;)");
}

DWORD WINAPI MainEntry(LPVOID module)
{
    uintptr_t module_base_addr = reinterpret_cast<uintptr_t>(GetModuleHandle(NULL)); // get the moduleBaseAdress of current module (ac_client.exe)
    std::cout << "ac_client.exe: " << std::hex << module_base_addr << std::dec << std::endl;
    uintptr_t local_player_ptr = module_base_addr + static_cast<uintptr_t>(ModuleOffsets::LocalPlayer);

    Player* local_player = *reinterpret_cast<Player**>(local_player_ptr);
    std::cout << "health: " << local_player->health << std::endl;
    local_player->health = 10000;

    std::cout << local_player->head_pos << std::endl;

    size_t *player_count = reinterpret_cast<size_t*>(module_base_addr + static_cast<uintptr_t>(ModuleOffsets::PlayerCount));
    std::cout << "player_count: " << *player_count << std::endl; // doesn't work on empty map
    uintptr_t entity_list_ptr = module_base_addr + static_cast<uintptr_t>(ModuleOffsets::EntityList);
    std::cout << "entity_list: " << entity_list_ptr << std::endl;
    uintptr_t entity_list = *reinterpret_cast<uintptr_t*>(entity_list_ptr);
    Player* first_enemy = *reinterpret_cast<Player**>(entity_list + 4);
    std::cout << "first_enemy addr: " << first_enemy << std::endl;
    while (!(GetAsyncKeyState(VK_ESCAPE) & 0x01)) {
        if (local_player != nullptr && first_enemy != nullptr) {
			std::cout << first_enemy->head_pos << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

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

