#include "Shuffles.hpp"
#include "../ProcessManager/Manager.hpp"
#include "../Scanner/Scanner.hpp"
#include <iostream>



uintptr_t ShuffleDump::LuaC_Init5{};
bool found = false;
uintptr_t shuffle7end;
uintptr_t shuffle8end;

uintptr_t ShuffleDump::luaT_eventnames(Process proc) {
	uintptr_t ssss = Scanner::PatternScan("48 89 5C 24 ? 56 57 41 56 48 83 EC ? 49 8B F8", proc, "Player");
	uintptr_t bs = Scanner::FindNext(ssss, proc, "Player", ZYDIS_MNEMONIC_RET, 1);
	uintptr_t step2 = Scanner::FindNext(bs, proc, "Player", ZYDIS_MNEMONIC_CALL, 1);


	uintptr_t step3 = Scanner::ResolveDataReference(step2, proc);
	uintptr_t luaTeventnamesa = Scanner::FindNext(step3, proc, "Player", ZYDIS_MNEMONIC_LEA, 1);
	uintptr_t luaTeventnames = Scanner::ResolveDataReference(luaTeventnamesa, proc, "Player", 0);

	shuffle7end = luaTeventnames + 0x40;
	shuffle8end = luaTeventnames + 0x80;
	return luaTeventnames;
}	


uintptr_t ShuffleDump::Lua_ResetThread(Process proc) {
	uintptr_t hh = Scanner::PatternScan("40 53 41 54 48 83 EC ? 48 8B 51", proc, "Player");
    if (hh == 0x0) {
        MessageBoxA(0, "Failed to find Lua_ResetThread pattern.", 0, 0);
        return 0x0;
	}

    return hh;
}


uintptr_t GetShuffle4Position(uintptr_t addr, Process proc) {

    return ((Scanner::GetDisplacementAddress(addr, proc) / 0x8) + 0x1);
}

uintptr_t GetShufflePos6(uintptr_t addr, Process proc) {

	return (((Scanner::GetDisplacementAddress(addr, proc) - 8) / 0x8) + 0x1);
}

bool ShuffleDump::Shuffle3(Process proc) {
	LuaC_Init5 = Scanner::PatternScan("48 8B 43 ? 4C 8D 25 ? ? ? ? 0F B6 48", proc);
	if (LuaC_Init5 != 0x0) {
		uintptr_t s = Scanner::FindNext(LuaC_Init5, proc, "Player", ZYDIS_MNEMONIC_MOVZX, 0);
		uintptr_t a1 = Scanner::FindNext(s, proc, "Player", ZYDIS_MNEMONIC_MOV, 1);
		uintptr_t a2 = Scanner::FindNext(s, proc, "Player", ZYDIS_MNEMONIC_MOV, 2);

		uintptr_t first = Scanner::GetDisplacementAddress(a1, proc);
		uintptr_t second = Scanner::GetDisplacementAddress(a2, proc);

		int expected = 6;
		int third = expected - first - second;

		std::cout << "#define LUAU_SHUFFLE3(s, a1, a2, a3) a" << first << " s a" << second << " s a" << third << std::endl;
	}
	else {
		std::cout << "// failed to get LuaC_init (skipping shuffle3)\n";

	}

    return true;
}
// TODO: make this go to the next movs that mov [rdx] 
bool ShuffleDump::Shuffle4(Process proc) {
    uintptr_t lua_resetthread = Lua_ResetThread(proc);
	if (lua_resetthread > 0x10) {
		uintptr_t Init = Scanner::FindNext(lua_resetthread, proc, "Player", ZYDIS_MNEMONIC_LEA, 1);
		uintptr_t a1 = GetShuffle4Position(Scanner::FindNext(Init, proc, "Player", ZYDIS_MNEMONIC_MOV, 1), proc); // rax
		uintptr_t a2 = GetShuffle4Position(Scanner::FindNext(Init, proc, "Player", ZYDIS_MNEMONIC_MOV, 2), proc); // random;
		uintptr_t a3 = GetShuffle4Position(Scanner::FindNext(Init, proc, "Player", ZYDIS_MNEMONIC_MOV, 3), proc); // rcx

		int expected = (1 + 2 + 3 + 4);
		int current = a1 + a2 + a3;
		int a4 = expected - current;

		std::string slots[4];

		slots[a1 - 1] = "a1";
		slots[a2 - 1] = "a2";
		slots[a3 - 1] = "a3";
		slots[a4 - 1] = "a4";

		printf("#define LUAU_SHUFFLE4(s, a1, a2, a3, a4) ");
		for (int i = 0; i < 4; ++i) {
			printf("%s", slots[i].c_str());
			if (i != 3)
				printf(" s ");
		}
		printf("\n");
	}
	else {
		MessageBoxA(0, "Failed to dump Shuffle4.", 0, 0);
	}
}

bool ShuffleDump::Shuffle5(Process proc) {


	uintptr_t luaTeventnames = luaT_eventnames(proc);
	std::vector<std::string> Structure = {
		"__lt",
		"__le",
		"__concat",
		"__type",
		"__metatable"
	};
	std::vector<std::string> Found;
	for (uintptr_t ptr = shuffle8end; ptr < shuffle8end + 5 * sizeof(uintptr_t); ptr += 0x8) {
		uintptr_t s1 = ProcessManager::Read<uintptr_t>(ptr, proc.processhandle);
		std::string content = Scanner::StringAt(s1, proc);
		/*std::cout << "Found: " << content << "\n";*/
		Found.push_back(content);
	}

	std::vector<uintptr_t> order;
	for (const std::string& current : Found) {
		size_t i = 0;
		for (const std::string& original : Structure) {
			if (original == current) break;
			i++;
		}
		order.push_back(i + 1);
	}
	std::cout << "#define LUAU_SHUFFLE5(s, a1, a2, a3, a4, a5) ";

	for (int i = 0; i < order.size(); i++) {

		std::cout << "a" << order[i];
		if (i < order.size() - 1)
			std::cout << " s ";
	}
	std::cout << "\n";

}

bool ShuffleDump::Shuffle7(Process proc) {


	uintptr_t luaTeventnames = luaT_eventnames(proc);
    std::vector<std::string> Structure = {
		"__index",
		"__newindex",
		"__mode",
		"__namecall",
		"__call",
		"__iter",
		"__len"
    };
	std::vector<std::string> Found;
	for (uintptr_t ptr = luaTeventnames; ptr < luaTeventnames + 7 * sizeof(uintptr_t); ptr += 0x8) {
		uintptr_t s1 = ProcessManager::Read<uintptr_t>(ptr, proc.processhandle);
		std::string content = Scanner::StringAt(s1, proc);
		//std::cout << "Found: " << content << "\n";
		Found.push_back(content);
	}

	std::vector<uintptr_t> order;
	for (const std::string& current : Found) {
		size_t i = 0;
		for (const std::string& original : Structure) {
			if (original == current) break;
			i++;
		}
		order.push_back(i + 1);
	}
	std::cout << "#define LUAU_SHUFFLE7(s, a1, a2, a3, a4, a5, a6, a7) ";

	for (int i = 0; i < order.size(); i++) {

		std::cout << "a" << order[i];
		if (i < order.size() - 1)
			std::cout << " s ";
	}
	std::cout << "\n";

}

bool ShuffleDump::Shuffle8(Process proc) {


	uintptr_t luaTeventnames = luaT_eventnames(proc);
	std::vector<std::string> Structure = {
		"__add",
		"__sub",
		"__mul",
		"__div",
		"__idiv",
		"__mod",
		"__pow",
		"__unm"
	};
	std::vector<std::string> Found;
	for (uintptr_t ptr = shuffle7end; ptr < shuffle7end + 8 * sizeof(uintptr_t); ptr += 0x8) {
		uintptr_t s1 = ProcessManager::Read<uintptr_t>(ptr, proc.processhandle);
		std::string content = Scanner::StringAt(s1, proc);
		//std::cout << "Found: " << content << "\n";
		Found.push_back(content);
	}

	std::vector<uintptr_t> order;
	for (const std::string& current : Found) {
		size_t i = 0;
		for (const std::string& original : Structure) {
			if (original == current) break;
			i++;
		}
		order.push_back(i + 1);
	}
	std::cout << "#define LUAU_SHUFFLE8(s, a1, a2, a3, a4, a5, a6, a7, a8) ";

	for (int i = 0; i < order.size(); i++) {

		std::cout << "a" << order[i];
		if (i < order.size() - 1)
			std::cout << " s ";
	}
	std::cout << "\n";
}

