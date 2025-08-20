#pragma once
#include <Windows.h>
#include <string>
struct Process;


class HyperionDump {
private:

public:
	static bool DumpHyperion(Process proc);
};