#include "pch.h"
#include <cstdio>
#include <iostream>

#define _CRT_SECURE_NO_WARNINGS

enum class ModuleOffsets : uintptr_t {
    LocalPlayer = 0x18AC00
};

class Player {
public:
    char pad[0xEC];
    int health;
};

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
    uintptr_t *local_player_ptr = reinterpret_cast<uintptr_t *>((module_base_addr + static_cast<uintptr_t>(ModuleOffsets::LocalPlayer)));

    Player* local_player = reinterpret_cast<Player*>(*local_player_ptr);
    std::cout << "health: " << local_player->health << std::endl;

    std::cin.ignore();
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
        CloseHandle(CreateThread(NULL, 0, MainEntry, module, 0, nullptr));
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

