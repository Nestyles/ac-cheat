#include <cstddef>
#include "pch.h"
#include "Utils.h"
#include "TraceRayCallHk.h"
#include <iostream>

uintptr_t jump_back_traceray = 0;
uintptr_t current_crosshair_ent_addr = 0;

void __declspec(naked) getCrossHairEnt() {
    __asm {
        mov current_crosshair_ent_addr, eax
        jmp jump_back_traceray
    }
}

void hookTracerayCall()
{
    int stolen_len = 7;
    uintptr_t traceray_hook = static_cast<uintptr_t>(0x45F573);
    jump_back_traceray = traceray_hook + stolen_len;
    std::cout << jump_back_traceray << std::endl;

    jump_back_traceray = TrampHook(reinterpret_cast<void *>(traceray_hook), getCrossHairEnt, stolen_len);
}

void unHookTracerayCall()
{
    uintptr_t traceray_hook = static_cast<uintptr_t>(0x45F573);

    Patch((void*)traceray_hook, "\x83\xC4\x10\x89\x44\x24\x10", 7);
}
