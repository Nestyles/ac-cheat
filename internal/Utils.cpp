#include "Utils.h"
#include "pch.h"
#include <Windows.h>
#include <stdexcept>

void Hook(void* hook_addr, void* tFunc, int len)
{
    if (len < 5) {
        throw std::runtime_error("Wrong len for hook");
    }

    DWORD old_protection;
    VirtualProtect(hook_addr, len, PAGE_EXECUTE_READWRITE, &old_protection);
    memset(hook_addr, 0x90, len); // nop

    uintptr_t relative_addr = (reinterpret_cast<uintptr_t>(tFunc) - reinterpret_cast<uintptr_t>(hook_addr)) - 5;

    *(uint8_t *)hook_addr = 0xE9; // jmp opcode
    *(uint32_t*)((uint32_t)hook_addr + 1) = relative_addr; // jmp addr

    DWORD new_protection;
    VirtualProtect(hook_addr, len, old_protection, &new_protection);
}

void Patch(void* addr, const char* bytes, int len)
{
    DWORD old_protection;

    VirtualProtect(addr, len, PAGE_EXECUTE_READWRITE, &old_protection);
    for (auto i = 0; i != len; ++i) {
        reinterpret_cast<char *>(addr)[i] = bytes[i];
    }

    DWORD new_protection;
    VirtualProtect(addr, len, old_protection, &new_protection);
}
