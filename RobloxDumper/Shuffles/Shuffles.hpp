#pragma once
#include <Windows.h>
#include <string>
struct Process;




class ShuffleDump {
private:
	static uintptr_t Lua_ResetThread(Process proc);
	static uintptr_t luaT_eventnames(Process proc);
	static uintptr_t fluaopen(Process proc);
	static uintptr_t LuaC_Init5;
public:
	static bool Shuffle3(Process proc);
	static bool Shuffle4(Process proc);
	static bool Shuffle5(Process proc);
	static bool Shuffle6(Process proc);
	static bool Shuffle7(Process proc);
	static bool Shuffle8(Process proc);
};