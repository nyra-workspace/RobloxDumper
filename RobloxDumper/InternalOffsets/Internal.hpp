#pragma once
#include <Windows.h>
#include <iostream>
struct Process;





class InternalDump {
private:

public:
	static void DumpXref(Process proc);
	static bool DumpPattern(Process proc);
};