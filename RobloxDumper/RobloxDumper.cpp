#include <iostream>
#include "ProcessManager/Manager.hpp"
#include "Scanner/Scanner.hpp"
#include "ExternalOffsets/External.hpp"
#include "InternalOffsets/Internal.hpp"
#include "HyperionOffsets/Hyperion.hpp"
#include "Shuffles/Shuffles.hpp"
#include <print>

int main()
{
    Process proc = ProcessManager::SetupProcess("Roblox");
    if (!proc.pid) {
        printf("Failed to attach to process! Is roblox open gang?\n");
        std::cin.get();
        exit(EXIT_FAILURE);
    }
    printf("/*\n");
    printf("    Made by nyra-workspace on github, iocreatesymboliclink on discord\n");
    printf("    PID: %d\n", proc.pid);
    printf("    WindowHandle: %p\n", proc.windowhandle);
    printf("    ProcessHandle: %p\n", proc.processhandle);
    for (const auto& [name, address] : proc.Modules) {
        printf("    %s - 0x%llx\n", name.c_str(), (unsigned long long)address);
    }
    std::cout << "\n\n\n";
    ProcessManager::SuspendProcess(proc);

    InternalDump::DumpPattern(proc);
    InternalDump::DumpXref(proc);
    printf("\n\n// INTERNAL OFFSETS\n");
    for (const auto& addr : ObtainedAddresses) {
		if (addr.value == 0x0) {
			printf("inline uintptr_t %s = REBASE(0xDEADBEEFDEADC0DE); // FALED TO DUMP\n", addr.name.c_str());
			continue;
		}
        else {
            printf("inline uintptr_t %s = REBASE(0x%llX);\n", addr.name.c_str(), Scanner::Rebase(addr.value, proc, "Player"));
        }
    }


    ExternalDump::DumpXternal(proc);
    ExternalDump::DumpEx(proc);
    printf("\n\n// EXTERNAL OFFSETS\n");
    for (const auto& addr : ObtainedXAddresses) {
        if (addr.value == 0x0) {
            printf("inline uintptr_t %s = 0xDEADBEEFDEADC0DE; // FAILED TO DUMP {EXTERNAL}\n", addr.name.c_str());
            continue;
        }
        else {
            printf("inline uintptr_t %s = 0x%llX;\n", addr.name.c_str(), addr.value);
        }
    }
    HyperionDump::DumpHyperion(proc);
    printf("\n\n// HYPERION OFFSETS\n");
    for (const auto& addr : ObtainedHAddresses) {
        if (addr.value == 0x0) {
            printf("inline uintptr_t %s = 0xDEADBEEFDEADC0DE; // FAILED TO DUMP {HYPERION}\n", addr.name.c_str());
            continue;
        }
        else {
            printf("inline uintptr_t %s = REBASEHYPERION(0x%llX);\n", addr.name.c_str(), Scanner::Rebase(addr.value, proc, "Hyperion"));
        }
    }


    std::cin.get();
    TerminateProcess(proc.processhandle, NULL);
    CloseHandle(proc.processhandle);
    exit(EXIT_SUCCESS);
}
