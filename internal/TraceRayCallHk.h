#pragma once

extern uintptr_t jump_back_traceray;
extern uintptr_t current_crosshair_ent_addr;

void hookTracerayCall();
void unHookTracerayCall();
