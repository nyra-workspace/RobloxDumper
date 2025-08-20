#include "Hyperion.hpp"
#include "../ProcessManager/Manager.hpp"
#include "../Scanner/Scanner.hpp"
#include <iostream>



bool HyperionDump::DumpHyperion(Process proc) {

    uintptr_t ControlFlowGuard = Scanner::PatternScan("49 BB FF FF FF FF FF 7F 00 00", proc, "Hyperion");
    if (ControlFlowGuard != 0x0) {
        uintptr_t instr = Scanner::FindNext(ControlFlowGuard, proc, "Hyperion", ZYDIS_MNEMONIC_MOV, 1);
        uintptr_t BitMap = Scanner::GetDisplacementAddress(instr, proc, 1, "Hyperion");
        ObtainedHAddresses.push_back({ "BitMap", BitMap });
        ObtainedHAddresses.push_back({ "ControlFlowGuard", ControlFlowGuard });
    }
    else {
        ObtainedHAddresses.push_back({ "BitMap", 0x0 });
        ObtainedHAddresses.push_back({ "ControlFlowGuard", 0x0 });
    }

    uintptr_t yarastart = Scanner::PatternScan("55 41 57 41 56 56 57 53 48 83 EC ? 48 8D 6C 24 ? 48 89 D6", proc, "Hyperion");
    if (yarastart != 0x0) {
        uintptr_t yaraof = Scanner::FindNext(yarastart, proc, "Hyperion", ZYDIS_MNEMONIC_ADD, 2);
        uintptr_t yaradetectionstrucptr = Scanner::GetDisplacementAddress(yaraof, proc, 1, "Hyperion");
        ObtainedHAddresses.push_back({ "YaraDetectionStructPtr", yaradetectionstrucptr });
    }
    else {
        ObtainedHAddresses.push_back({ "YaraDetectionStructPtr", 0x0 });
    }
	return true;
}