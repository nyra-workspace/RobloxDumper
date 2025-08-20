#include "Manager.hpp"
#include <TlHelp32.h>



DWORD ProcessManager::GetProcessId(const std::string& windowname) {
	HWND h = FindWindowA(nullptr, windowname.c_str());
	if (!h) return 0;
	DWORD pid;
	GetWindowThreadProcessId(h, &pid);

	return pid;
}

HWND ProcessManager::GetWindowHandle(const std::string& windowname) {
	return FindWindowA(nullptr, windowname.c_str());
}

HANDLE ProcessManager::AttachProcess(DWORD pid, AttachMode mode) {
	HANDLE phandle = OpenProcess(mode, 0, pid);

	return phandle;
}

bool ProcessManager::SuspendProcess(Process proc) {

    DWORD pid = proc.pid;

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return false;
    }

    THREADENTRY32 te32{};
    te32.dwSize = sizeof(THREADENTRY32);

    if (!Thread32First(snapshot, &te32)) {
        CloseHandle(snapshot);
        return false;
    }

    do {
        if (te32.th32OwnerProcessID == pid) {
            HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te32.th32ThreadID);
            if (hThread) {
                SuspendThread(hThread);
                CloseHandle(hThread);
            }
        }
    } while (Thread32Next(snapshot, &te32));
    

    CloseHandle(snapshot);
    return true;
}

uintptr_t ProcessManager::GetModuleBaseAddress(const std::wstring& ModuleName, DWORD pid) {
    MODULEENTRY32 me32{ sizeof(me32) };
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
    if (hSnapshot == INVALID_HANDLE_VALUE) return 0;
    if (Module32First(hSnapshot, &me32)) {
        do {
            if (ModuleName == me32.szModule) {
                CloseHandle(hSnapshot);
                return (uintptr_t)me32.modBaseAddr;
            }
        } while (Module32Next(hSnapshot, &me32));
    }
    CloseHandle(hSnapshot);
    return 0;
}



Process ProcessManager::SetupProcess(const std::string& windowname) {
    ModuleList list;
    DWORD pid = GetProcessId(windowname);
    HWND hwnd = GetWindowHandle(windowname);
    HANDLE hproc = AttachProcess(pid, ALL);

    list["Player"] = GetModuleBaseAddress(L"RobloxPlayerBeta.exe", pid);;
    list["Hyperion"] = GetModuleBaseAddress(L"RobloxPlayerBeta.dll", pid);;

    return Process{ pid, hwnd, hproc, list};
}