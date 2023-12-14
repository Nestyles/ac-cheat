#include "Process.h"
#include <TlHelp32.h>
#include <stdexcept>

uint32_t Process::getProcId(const wchar_t *process_name)
{
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (snap != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 proc_entry;
        proc_entry.dwSize = sizeof(proc_entry);

        if (Process32First(snap, &proc_entry) == false)
			throw Exception(Exception::Type::ProcIdNotFound);
        do
        {
            if (!wcscmp(proc_entry.szExeFile, process_name)) {
                CloseHandle(snap);
                return proc_entry.th32ProcessID;
            }
        } while (Process32Next(snap, &proc_entry));
    }
    CloseHandle(snap);
	throw Exception(Exception::Type::ProcIdNotFound);
}

uint32_t Process::getModuleBaseAdress(uint32_t proc_id, const wchar_t* module_name)
{
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, proc_id);

    if (snap != INVALID_HANDLE_VALUE) {
        MODULEENTRY32 module_entry;
        module_entry.dwSize = sizeof(module_entry);

        if (Module32First(snap, &module_entry) == false)
			throw Exception(Exception::Type::ModuleNotFound);
        do
        {
            if (!wcscmp(module_entry.szModule, module_name)) {
                CloseHandle(snap);
                return reinterpret_cast<uint32_t>(module_entry.modBaseAddr);
            }
        } while (Module32Next(snap, &module_entry));
    }
    CloseHandle(snap);
    throw Exception(Exception::Type::ModuleNotFound);
}

void Process::openProcess(uint32_t proc_id)
{
    _process_handle = OpenProcess(PROCESS_ALL_ACCESS, false, proc_id);

    if (_process_handle == INVALID_HANDLE_VALUE) {
        throw Exception(Exception::Type::OpenProcess);
    }
}

Process::~Process()
{
    if (_process_handle != INVALID_HANDLE_VALUE) {
        CloseHandle(_process_handle);
    }
}

uint32_t Process::resolveMLPtr(uint32_t first_addr, const std::vector<unsigned>& offsets) const
{
    auto size = offsets.size();
    uintptr_t addr = first_addr;

    for (auto& offset : offsets) {
        addr = readMemory<uintptr_t>(addr);
        addr += offset;
    }
    return addr;
}
