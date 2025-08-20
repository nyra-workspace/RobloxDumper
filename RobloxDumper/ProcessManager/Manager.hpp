#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include <tuple>
#include <unordered_map>
typedef std::unordered_map<std::string, uintptr_t> ModuleList;


struct Process {
    DWORD pid;
    HWND windowhandle;
    HANDLE processhandle;
    ModuleList Modules;
};

enum AttachMode {
    ALL = PROCESS_ALL_ACCESS,
    RW = PROCESS_VM_READ | PROCESS_VM_WRITE,
    RWO = PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION,
};


class ProcessManager {
private:

public:
	static DWORD GetProcessId(const std::string& windowname);
    static HANDLE AttachProcess(DWORD pid, AttachMode mode);
    static HWND GetWindowHandle(const std::string& windowname);
    static uintptr_t GetModuleBaseAddress(const std::wstring& ModuleName, DWORD pid);

    static bool SuspendProcess(Process proc);

    static Process SetupProcess(const std::string& windowname);


    template <typename T>
    static inline T Read(uintptr_t address, HANDLE hproc) {
        T buffer{};
        ReadProcessMemory(hproc, (LPCVOID)address, &buffer, sizeof(T), nullptr);

        return buffer;
    }
    template <typename T>
    static inline void Write(uintptr_t address, T value, HANDLE hproc) {
        WriteProcessMemory(hproc, (LPVOID)address, &value, sizeof(T), nullptr);
    }

    static inline std::string readstring(std::uintptr_t address, Process proc) {
        std::string buffer;
        buffer.reserve(204);

        for (int i = 0; i < 200; i++) {
            char c = Read<char>(address + i, proc.processhandle);
            if (c == 0) break;
            buffer.push_back(c);
        }

        return buffer;
    }
};


inline uintptr_t FakeDataModelPointer;