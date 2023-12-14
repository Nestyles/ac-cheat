#include <iostream>
#include <Windows.h>
#include "Process.h"

int main()
{
    try {
		uint32_t proc_id = Process::getProcId(L"ac_client.exe");
		//std::cout << "size: " << sizeof(long) << std::endl;
		//std::cout << "ProcId: " << proc_id << std::endl;
		uint32_t ac_addr = Process::getModuleBaseAdress(proc_id, L"ac_client.exe");
		//std::cout << "ac_client.exe module base addr " << std::hex << ac_addr << std::endl;
		Process proc;
		proc.openProcess(proc_id);
		uint32_t local_player = proc.readMemory<uint32_t>(ac_addr + 0x18AC00);
		std::cout << std::dec << proc.readMemory<int32_t>(local_player + 0xEC) << std::endl;
	} catch (std::exception& exception) {
		std::cerr << exception.what() << std::endl;
		return 84;
	}
	return 0;
}
