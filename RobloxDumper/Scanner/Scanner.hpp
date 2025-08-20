#pragma once
#define ZYDIS_STATIC_BUILD
#include <Windows.h>
#include <string>
#include <vector>
#include <Zydis/Zydis.h>

struct Process;

struct Patterns {
	const std::string& name;
	const std::string& pattern;
	bool isFunction;
};

struct Results {
	std::string name;
	uintptr_t value;
};

inline std::vector<Results> ObtainedAddresses{};
inline std::vector<Results> ObtainedXAddresses{};
inline std::vector<Results> ObtainedHAddresses{};

class Scanner {
private:

public:
	static uintptr_t PatternScan(const std::string& pattern, Process process, const std::string& module = "Player");
	static uintptr_t FindNext(uintptr_t start, Process process, const std::string& module, ZydisMnemonic targetMnemonic, int skip, ZydisOperandType operandTypeFilter = ZYDIS_OPERAND_TYPE_UNUSED);
	static uintptr_t GetLEAValue(uintptr_t instructionAddr, Process proc);
	static uintptr_t GetCallValue(uintptr_t callAddr, Process proc);
	static uintptr_t ResolveDataReference(uintptr_t start, Process process, const std::string& module = "Player", int skip = 0);
	static uintptr_t GetDisplacementAddress(uintptr_t start, Process process, int mode = 2, const std::string& module = "Player");
	static uintptr_t Rebase(uintptr_t address, Process process, const std::string& module = "Player");
	static std::string StringAt(uintptr_t address, Process process, size_t maxLength = 256);
};
