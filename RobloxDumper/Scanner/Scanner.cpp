#include "Scanner.hpp"
#include "../ProcessManager/Manager.hpp"
#include <TlHelp32.h>
#include <Psapi.h>
#include <vector>
#include <iostream>
#include <optional>
#include <string>
#include <sstream>
#include <cstdint>

#pragma comment(lib, "Psapi.lib") 


uintptr_t Scanner::PatternScan(const std::string& pattern, Process process, const std::string& module) {
    auto patternToBytes = [](const std::string& pattern) {
        std::vector<std::optional<uint8_t>> bytes;
        std::istringstream iss(pattern);
        std::string byteStr;
        while (iss >> byteStr) {
            if (byteStr == "?" || byteStr == "??")
                bytes.push_back(std::nullopt);
            else
                bytes.push_back(static_cast<uint8_t>(std::stoi(byteStr, nullptr, 16)));
        }
        return bytes;
        };

    uintptr_t base = process.Modules[module];
    MODULEINFO modInfo = { 0 };

    if (!GetModuleInformation(process.processhandle, (HMODULE)base, &modInfo, sizeof(modInfo))) {
        printf("GetModuleInformation failed: %lu\n", GetLastError());
        return 0;
    }

    auto patternBytes = patternToBytes(pattern);
    const size_t patternSize = patternBytes.size();
    uintptr_t moduleEnd = base + modInfo.SizeOfImage;

    uintptr_t currentAddress = base;

    while (currentAddress < moduleEnd) {
        MEMORY_BASIC_INFORMATION mbi;
        if (!VirtualQueryEx(process.processhandle, (LPCVOID)currentAddress, &mbi, sizeof(mbi))) {
            break;
        }

        if ((mbi.State == MEM_COMMIT) &&
            (mbi.Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY | PAGE_READONLY | PAGE_READWRITE)) &&
            !(mbi.Protect & PAGE_GUARD)) {

            std::vector<uint8_t> buffer(mbi.RegionSize);
            SIZE_T bytesRead = 0;

            if (ReadProcessMemory(process.processhandle, mbi.BaseAddress, buffer.data(), buffer.size(), &bytesRead)) {
                for (size_t i = 0; i <= bytesRead - patternSize; i++) {
                    bool found = true;
                    for (size_t j = 0; j < patternSize; j++) {
                        if (patternBytes[j].has_value() && buffer[i + j] != patternBytes[j].value()) {
                            found = false;
                            break;
                        }
                    }
                    if (found) {
                        return reinterpret_cast<uintptr_t>(mbi.BaseAddress) + i;
                    }
                }
            }
            else {
                printf("ReadProcessMemory failed at 0x%p: %lu\n", mbi.BaseAddress, GetLastError());
            }
        }
        

        currentAddress = reinterpret_cast<uintptr_t>(mbi.BaseAddress) + mbi.RegionSize;
    }

    return 0;
}


uintptr_t Scanner::FindNext(uintptr_t start, Process process, const std::string& module, ZydisMnemonic targetMnemonic, int skip, ZydisOperandType operandTypeFilter) {
    ZydisDecoder decoder;
    ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_STACK_WIDTH_64);

    const size_t maxScan = 0x1000;
    std::vector<uint8_t> buffer(maxScan);
    ReadProcessMemory(process.processhandle, (LPCVOID)start, buffer.data(), buffer.size(), nullptr);

    size_t offset = 0;
    while (offset < buffer.size()) {
        ZydisDecodedInstruction instruction;
        ZydisDecodedOperand operands[ZYDIS_MAX_OPERAND_COUNT];

        if (ZYAN_SUCCESS(ZydisDecoderDecodeFull(&decoder, buffer.data() + offset, buffer.size() - offset, &instruction, operands))) {
            if (instruction.mnemonic == targetMnemonic) {
                bool matchesOperandFilter = true;

                // If operand type filter is specified, check if any operand matches
                if (operandTypeFilter != ZYDIS_OPERAND_TYPE_UNUSED) {
                    matchesOperandFilter = false;
                    for (int i = 0; i < instruction.operand_count; ++i) {
                        if (operands[i].type == operandTypeFilter) {
                            matchesOperandFilter = true;
                            break;
                        }
                    }
                }

                if (matchesOperandFilter) {
                    if (skip == 0) {
                        return start + offset;
                    }
                    else {
                        skip--;
                    }
                }
            }
            offset += instruction.length;
        }
        else {
            offset += 1;
        }
    }
    return 0;
}


uintptr_t Scanner::GetLEAValue(uintptr_t instructionAddr, Process proc) {
    HANDLE processHandle = proc.processhandle;

    int32_t offset = 0;
    SIZE_T bytesRead = 0;
    if (!ReadProcessMemory(processHandle,
        reinterpret_cast<LPCVOID>(instructionAddr + 3),
        &offset,
        sizeof(int32_t),
        &bytesRead)) {
        return 0;
    }
    if (bytesRead != sizeof(int32_t)) {
        return 0;
    }
    return instructionAddr + offset + 7;
}

uintptr_t Scanner::GetCallValue(uintptr_t callAddr, Process proc) {

    HANDLE processHandle = proc.processhandle;

    if (!processHandle || !callAddr) return 0;

    int32_t rel = 0;
    SIZE_T bytesRead = 0;

    if (!ReadProcessMemory(processHandle,
        reinterpret_cast<LPCVOID>(callAddr + 1),
        &rel,
        sizeof(int32_t),
        &bytesRead)) {
        return 0;
    }
    if (bytesRead != sizeof(int32_t)) {
        return 0;
    }
    return callAddr + 5 + rel;
}


uintptr_t Scanner::ResolveDataReference(uintptr_t start, Process process, const std::string& module, int skip) {
    uint8_t buffer[32];
    ReadProcessMemory(process.processhandle, (LPCVOID)start, buffer, sizeof(buffer), nullptr);

    ZydisDecoder decoder;
    ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_STACK_WIDTH_64);

    ZydisDecodedInstruction instruction;
    ZydisDecodedOperand operands[ZYDIS_MAX_OPERAND_COUNT];

    if (ZYAN_SUCCESS(ZydisDecoderDecodeFull(&decoder, buffer, sizeof(buffer), &instruction, operands))) {
        for (int i = 0; i < instruction.operand_count; ++i) {
            // Look for memory operands with RIP-relative addressing
            if (operands[i].type == ZYDIS_OPERAND_TYPE_MEMORY &&
                operands[i].mem.base == ZYDIS_REGISTER_RIP) {
                if (skip == 0) {
                    // Calculate RIP-relative address
                    return start + instruction.length + operands[i].mem.disp.value;
                }
                else {
                    skip--;
                }
            }
            // Fallback: also check for immediate operands (for other instruction types)
            else if (operands[i].type == ZYDIS_OPERAND_TYPE_IMMEDIATE) {
                if (skip == 0) {
                    return start + instruction.length + operands[i].imm.value.s;
                }
                else {
                    skip--;
                }
            }
        }
    }
    return 0;
}


uintptr_t Scanner::GetDisplacementAddress(uintptr_t start, Process process, int mode, const std::string& module) {




    if (mode == 1) {
        uint8_t buffer[32];
        ReadProcessMemory(process.processhandle, (LPCVOID)start, buffer, sizeof(buffer), nullptr);

        ZydisDecoder decoder;
        ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_STACK_WIDTH_64);

        ZydisDecodedInstruction instruction;
        ZydisDecodedOperand operands[ZYDIS_MAX_OPERAND_COUNT];
        ZydisDecoderContext context;

        if (ZYAN_SUCCESS(ZydisDecoderDecodeInstruction(&decoder, &context, buffer, sizeof(buffer), &instruction))) {
            if (ZYAN_SUCCESS(ZydisDecoderDecodeOperands(&decoder, &context, &instruction, operands, instruction.operand_count))) {
                for (int i = 0; i < instruction.operand_count; ++i) {
                    if (operands[i].type == ZYDIS_OPERAND_TYPE_MEMORY && operands[i].mem.disp.size != 0) {
                        return start + instruction.length + (int32_t)operands[i].mem.disp.value;
                    }
                }
            }
        }
        return 0;
    }
    else {
        
        uint8_t buffer[32];
        ReadProcessMemory(process.processhandle, (LPCVOID)start, buffer, sizeof(buffer), nullptr);
        ZydisDecoder decoder;
        ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_STACK_WIDTH_64);
        ZydisDecodedInstruction instruction;
        ZydisDecodedOperand operands[ZYDIS_MAX_OPERAND_COUNT];
        ZydisDecoderContext context;
        if (ZYAN_SUCCESS(ZydisDecoderDecodeInstruction(&decoder, &context, buffer, sizeof(buffer), &instruction))) {
            if (ZYAN_SUCCESS(ZydisDecoderDecodeOperands(&decoder, &context, &instruction, operands, instruction.operand_count))) {
                for (int i = 0; i < instruction.operand_count; ++i) {
                    if (operands[i].type == ZYDIS_OPERAND_TYPE_MEMORY && operands[i].mem.disp.size != 0) {
                        int64_t disp = operands[i].mem.disp.value;
                        switch (operands[i].mem.disp.size)
                        {
                        case 8:  disp = (int8_t)disp;  break;
                        case 16: disp = (int16_t)disp; break;
                        case 32: disp = (int32_t)disp; break;
                        case 64: disp = (int64_t)disp; break;
                        }
                        return disp;
                    }
                }
            }
        }

        return 0;
            
    }

}


uintptr_t Scanner::Rebase(uintptr_t address, Process process, const std::string& module) {
    uintptr_t ModuleBaseAddress = process.Modules[module];
    return address - ModuleBaseAddress;
}

std::string Scanner::StringAt(uintptr_t address, Process process, size_t maxLength)
{
    std::vector<char> buffer(maxLength);
    SIZE_T bytesRead = 0;
    if (!ReadProcessMemory(process.processhandle,
        reinterpret_cast<LPCVOID>(address),
        buffer.data(),
        buffer.size(),
        &bytesRead)) {
        return ""; 
    }

    buffer[bytesRead - 1] = '\0';
    size_t actualLength = strnlen(buffer.data(), bytesRead);

    return std::string(buffer.data(), actualLength);
}