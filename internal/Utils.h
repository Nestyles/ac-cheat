#pragma once

void Hook(void* hook_addr, void* tFunc, int len);
void Patch(void* addr, const char* bytes, int len);
uintptr_t TrampHook(void* hook_addr, void* func, int stolen_len);
