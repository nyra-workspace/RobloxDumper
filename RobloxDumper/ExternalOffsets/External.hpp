#pragma once
#include <Windows.h>
#include <string>


struct Process;

class ExternalDump {
private:

public:
	static void DumpEx(Process proc);
	static void DumpXternal(Process proc);

};