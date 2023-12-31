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

uintptr_t TrampHook(void *hook_addr, void* func, int stolen_len)
{
    if (stolen_len < 5) {
        throw std::runtime_error("Wrong len for hook");
    }

    void* old_code = VirtualAlloc(nullptr, stolen_len + 5, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (old_code == nullptr) {
        throw std::runtime_error("Failed to virtual alloc tramphook");
    }
    memcpy(old_code, hook_addr, stolen_len);
    uintptr_t relative_addr = (reinterpret_cast<uintptr_t>(hook_addr) - reinterpret_cast<uintptr_t>(old_code)) - 5;
    reinterpret_cast<char *>(old_code)[stolen_len] = 0xE9;
    *reinterpret_cast<uintptr_t*>((uintptr_t)old_code + stolen_len + 1) = relative_addr; // + 5 to dodge the jmp

    Hook(hook_addr, func, stolen_len);
    return reinterpret_cast<uintptr_t>(old_code);
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
