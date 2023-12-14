#pragma once
#include <Windows.h>
#include <cstdint>
#include <vector>

class Process
{
public:
	class Exception : public std::exception {
	public:
		enum class Type {
			ProcIdNotFound,
			ModuleNotFound,
			OpenProcess,
			ReadMemory
		};
	private:
		Type _type;
	public:
		Exception(Type ex) : _type(ex) {}
		const char* what() const
		{
			switch (_type) {
				case Type::ProcIdNotFound: return "Process:getProcId: ProcId not found";
				case Type::ModuleNotFound: return "Process:getModuleBaseAdress: Module not found";
				case Type::OpenProcess: return "Process:openProcess: Process not found";
				case Type::ReadMemory: return "Process::ReadMemory: Tried to read in invalid space";
			}
		}
	};
	static uint32_t getProcId(const wchar_t* process_name);
	static uint32_t getModuleBaseAdress(uint32_t proc_id, const wchar_t* module_name);
	void openProcess(uint32_t proc_id);
	uint32_t resolve_mlptr(uint32_t first_addr, const std::vector<unsigned>& offsets) const;

	template<typename T>
	T readMemory(uint32_t address) const
	{
		T buffer{};
		ReadProcessMemory(_process_handle, reinterpret_cast<LPCVOID>(address), &buffer, sizeof(T), nullptr);
		return buffer;
	}
	~Process();
private:
	HANDLE _process_handle = INVALID_HANDLE_VALUE;
};

