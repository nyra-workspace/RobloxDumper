#include "Internal.hpp"
#include "../ProcessManager/Manager.hpp"
#include "../Scanner/Scanner.hpp"



Patterns RobloxPatterns[]{
        {"FireMouseHoverEnter", "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8B FA 48 8B F1 33 ED 89 AC 24 ? ? ? ? 48 85 D2", false},
        {"FireMouseHoverLeave", "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8B FA 48 8B F1 33 ED 89 AC 24 ? ? ? ? 48 8B 01", false},
        {"FireTouchInterest", "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8B 79 ? 41 0F B6 F1", false},
        {"FireClickDetector", "48 89 5C 24 ? 55 56 57 48 83 EC ? 49 8B F8 48 8B F1 33 ED 89 AC 24 ? ? ? ? F3 0F 10 81 ? ? ? ? 0F 2F C1 0F 86", false},
        {"GetContextObject", "48 89 54 24 ? 48 83 EC ? 4C 8B D1 44 0F B6 CA", false},
        {"GetFFlag", "48 2B D1 49 83 F8", false},
        {"GetGlobalState", "48 89 5C 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B FA 48 8B D9", false},
        {"GetIdentityptr", "48 8B 0D ? ? ? ? E8 ? ? ? ? 48 8B F0 48 89 45 ? 44 0F 10 00", true},
        {"GetLuaState", "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 49 8B F8 48 8B F2 48 8B D9 8B 81", false},
        {"GetModuleFromVMStateMap", "48 89 54 24 ? 53 56 57 48 83 EC ? 48 8B 81", false},
        {"GetProperty", "40 57 48 8B 39 4C 8B 41 ? 49 3B F8 75 ? 45 33 C9 41 8B C9", false},
        {"GetValues", "48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 45 0F B6 D1", false},

        {"Idenityptr", "48 8B 0D ? ? ? ? E8 ? ? ? ? 48 8B F0 48 89 45 ? 44 0F 10 00", true},
        {"IdentityStruct", "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8B F9 E8 ? ? ? ? 90", false},
        {"Impersonator", "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8B F9 49 8B E9", false},
        {"KTable", "48 8d 0d ? ? ? ? 48 8d 54 24 ? 48 8b 04 c1", true},
        {"LockViolationInstanceCrash", "48 8D 0D ? ? ? ? E8 ? ? ? ? 0F B6 C0 85 C0 74 ? 4C 8B 84 24 ? ? ? ? 48 8B 94 24 ? ? ? ? 48 8D 4C 24 ? E8 ? ? ? ? 48 89 44 24 ? 48 8B 44 24 ? 48 89 44 24 ? 48 8B 4C 24 ? E8 ? ? ? ? 48 8B D0 48 8D 0D ? ? ? ? E8 ? ? ? ? 90 48 8D 4C 24 ? E8 ? ? ? ? EB", true},
        {"LuaC_step", "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 56 41 57 48 83 EC ? 48 8B 59 ? B8", false},
        {"LuaD_throw", "48 83 EC ? 44 8B C2 48 8B D1 48 8D 4C 24", false},
        {"LuaL_register", "40 56 41 54 41 55 48 83 EC ?? 48", false},
        {"LuaO_NilObject", "48 8d 3d ? ? ? ? 48 3b d7", true},
        {"luaA_toobject", "48 83 EC ? 4C 8D 15 ? ? ? ? 85 D2", false},
        {"luaL_checklstring", "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 63 FA 49 8B F0", false},
        {"luaL_getmetafield", "40 53 41 56 41 57 48 83 EC ? 0F B6 41", false},
        {"luaM_visitgco", "40 56 41 54 41 55 48 83 EC", false},
        {"luaH_Dummynode", "48 8d 3d ? ? ? ? 48 8B D9 48 39", true},
        {"Luau_execute", "80 79 ? ? 0F 85 ? ? ? ? E9", false},
        {"Pseudo2addr", "41 B9 ? ? ? ? 4C 8B C1 41 3B D1", false},
        {"Print", "48 89 54 24 ? 4C 89 44 24 ? 4C 89 4C 24 ? 55 53 56 57 41 54 41 55", false},
        {"PushInstance", "48 89 5C 24 ? 57 48 83 EC ? 48 8B FA 48 8B D9 E8 ? ? ? ? 84 C0 74 ? 48 8B D7 48 8B CB 48 8B 5C 24", false},
        {"RaiseEventInvocation", "48 89 5C 24 ? 55 56 57 48 83 EC ? 49 8B F1 49 8B E8 48 8B FA 48 8B D9 48 83 79", false},
        {"RawTaskScheduler", "48 89 05 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 3D ? ? ? ? EB ? 48 8B 08 8B 04 0B", true},
        {"RequestCode", "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8B EA 48 8B F9 48 85 D2", false},
        {"SetFFlag", "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC ? 41 0F B6 F1 49 8B E8 48 8B F9", false},
        {"ScriptContextResume", "48 8B C4 44 89 48 ? 4C 89 40 ? 48 89 50 ? 48 89 48 ? 53", false},
        {"SetProtoCapabilities", "48 89 5C 24 ? 48 89 6C 24 ? 56 48 83 EC ? 33 DB", false},
        {"Task_Defer", "48 89 5C 24 ? 55 56 57 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B F9 45 33 FF 44 38 3D", false},
        {"VariantCastInt", "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8B 19 48 8B F9 0F 29 74 24", false},
        {"VariantCastInt64", "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8B 19 48 8B F9 E8 ? ? ? ? 33 F6", false},
};

bool InternalDump::DumpPattern(Process proc) {
    for (const auto& pats : RobloxPatterns) {

        if (!pats.isFunction) {
            uintptr_t result = Scanner::PatternScan(pats.pattern, proc, "Player");
            ObtainedAddresses.push_back({ pats.name, result });
        }
        else {
            uintptr_t resultstart = Scanner::PatternScan(pats.pattern, proc, "Player");
            uintptr_t result = Scanner::GetLEAValue(resultstart, proc);
            ObtainedAddresses.push_back({ pats.name, result });
        }
    }
    return true;
}


void InternalDump::DumpXref(Process proc) {

    uintptr_t callAddr = Scanner::PatternScan("E8 ? ? ? ? C7 43 ? ? ? ? ? 48 8B 4C 24", proc);
    if (callAddr != 0x0) {
        uintptr_t FireProximityPrompt = Scanner::GetCallValue(callAddr, proc);
        ObtainedAddresses.push_back({ "FireProximityPrompt", FireProximityPrompt });
    }
    uintptr_t FireRightMouseClickStart = Scanner::PatternScan("85 FF 74 ? 83 EF ? 74 ? 83 EF ? 74 ? 83 FF ? 75 ? 80 BB", proc);
    if (FireRightMouseClickStart != 0x0) {
        uintptr_t FireRightMouseClick = Scanner::ResolveDataReference(Scanner::FindNext(FireRightMouseClickStart, proc, "Player", ZYDIS_MNEMONIC_CALL, 0), proc, "Player", 0);
        ObtainedAddresses.push_back({ "FireRightMouseClick", FireRightMouseClick });
    }

    uintptr_t Fakedmstart = Scanner::PatternScan("48 8D 1D ? ? ? ? 8B 07 39 05 ? ? ? ? 7F", proc);
    if (Fakedmstart != 0x0) {
        FakeDataModelPointer = Scanner::GetLEAValue(Fakedmstart, proc) + 0x8;
        ObtainedAddresses.push_back({ "FakeDataModelPointer", FakeDataModelPointer });
    }
}
