#include "../ProcessManager/Manager.hpp"
#include "../Scanner/Scanner.hpp"
#include "External.hpp"


void ExternalDump::DumpEx(Process proc) {

    uintptr_t DataModel = 0x0;
    uintptr_t FakeToReal = 0x1C0;

    uintptr_t Name = 0x0;
    uintptr_t GameLoaded = 0x0;
    uintptr_t Children = 0x0;
    uintptr_t Workspace = 0x0;
    uintptr_t Gravity = 0x0;
    uintptr_t Parent = 0x0;
    uintptr_t ClassDescriptor = 0x0;
    uintptr_t Camera = 0x0;
    uintptr_t LocalPlayer = 0x0;
    uintptr_t LocalCharacter = 0x0;
    uintptr_t LocalHumanoid = 0x0;
    uintptr_t JumpPower = 0x0;
    uintptr_t Walkspeed = 0x0;
    uintptr_t WalkspeedCheck = 0x0;
    uintptr_t Health = 0x0;
    uintptr_t FOV = 0x0;
    uintptr_t ScriptContext = 0x0;




    for (Name = 0x0; Name < 0x1000; ++Name) {
        uintptr_t FakeDataModel = ProcessManager::Read<uintptr_t>(FakeDataModelPointer, proc.processhandle); DataModel = ProcessManager::Read<uintptr_t>(FakeDataModel + FakeToReal, proc.processhandle); uintptr_t nameptr = ProcessManager::Read<uintptr_t>(DataModel + Name, proc.processhandle); std::string name = ProcessManager::readstring(nameptr, proc); if (name == "Ugc") {
            ObtainedXAddresses.push_back({ "Name", Name }); break;
        }
    }

    // Gameloaded
    for (GameLoaded = 0x0; GameLoaded < 0x1000; ++GameLoaded) {
        int64_t _gameloaded = ProcessManager::Read<int64_t>(DataModel + GameLoaded, proc.processhandle);
        if (_gameloaded == 31) {

            ObtainedXAddresses.push_back({ "GameLoaded", GameLoaded });
            break;
        }
    }

    uintptr_t players;
    // Children
    for (Children = 0x0; Children < 0x1000; ++Children) {
        uintptr_t start = ProcessManager::Read<uintptr_t>(DataModel + Children, proc.processhandle);
        uintptr_t instances = ProcessManager::Read<uintptr_t>(start, proc.processhandle);
        bool found = false;

        for (int i = 0; i < 30; i++) {
            uintptr_t instance = ProcessManager::Read<uintptr_t>(instances + i * 0x10, proc.processhandle);
            uintptr_t nameptr = ProcessManager::Read<uintptr_t>(instance + Name, proc.processhandle);
            std::string name = ProcessManager::readstring(nameptr, proc);

            if (name == "Players") {

                found = true;
                players = instance;
                ObtainedXAddresses.push_back({ "Children", Children });
                break;
            }
        }

        if (found)
            break;
    }


    // Workspace
    uintptr_t WorkspacePtr;
    for (Workspace = 0x0; Workspace < 0x1000; ++Workspace) {
        WorkspacePtr = ProcessManager::Read<uintptr_t>(DataModel + Workspace, proc.processhandle);
        uintptr_t nameptr = ProcessManager::Read<uintptr_t>(WorkspacePtr + Name, proc.processhandle);
        std::string name = ProcessManager::readstring(nameptr, proc);
        if (name == "Workspace") {
            ObtainedXAddresses.push_back({ "Workspace", Workspace });
            break;
        }
    }



    // Parent
    for (Parent = 0x0; Parent < 0x1000; ++Parent) {
        uintptr_t Start = ProcessManager::Read<uintptr_t>(Workspace + Parent, proc.processhandle);
        uintptr_t nameptr = ProcessManager::Read<uintptr_t>(Start + Name, proc.processhandle);
        std::string name = ProcessManager::readstring(nameptr, proc);

        if (name == "Ugc") {

            ObtainedXAddresses.push_back({ "Parent", Parent });
            break;
        }
    }



    // ClassDescriptor
    for (ClassDescriptor = 0x0; ClassDescriptor < 0x1000; ++ClassDescriptor) {
        uintptr_t classdescriptorptr = ProcessManager::Read<uintptr_t>(DataModel + ClassDescriptor, proc.processhandle);
        uintptr_t classnameptr = ProcessManager::Read<uintptr_t>(classdescriptorptr + 0x8, proc.processhandle);
        std::string classname = ProcessManager::readstring(classnameptr, proc);

        if (classname == "DataModel") {

            ObtainedXAddresses.push_back({ "ClassDescriptor", ClassDescriptor });
            break;
        }
    }


    // Camera
    for (Camera = 0x0; Camera < 0x1000; ++Camera) {
        uintptr_t cameraptr = ProcessManager::Read<uintptr_t>(WorkspacePtr + Camera, proc.processhandle);
        uintptr_t nameptr = ProcessManager::Read<uintptr_t>(cameraptr + Name, proc.processhandle);
        std::string name = ProcessManager::readstring(nameptr, proc);

        if (name == "Camera") {

            ObtainedXAddresses.push_back({ "Camera", Camera });
            break;
        }
    }

    std::string PlayerName;
    // LocalPlayer
    for (LocalPlayer = 0x0; LocalPlayer < 0x1000; ++LocalPlayer) {
        uintptr_t Start = ProcessManager::Read<uintptr_t>(players + LocalPlayer, proc.processhandle);
        uintptr_t CDlp = ProcessManager::Read<uintptr_t>(Start + ClassDescriptor, proc.processhandle);
        uintptr_t nameptr = ProcessManager::Read<uintptr_t>(CDlp + 0x8, proc.processhandle);
        std::string name = ProcessManager::readstring(nameptr, proc);

        if (name == "Player") {

            ObtainedXAddresses.push_back({ "LocalPlayer", LocalPlayer });

            uintptr_t localname = ProcessManager::Read<uintptr_t>(Start + Name, proc.processhandle);
            PlayerName = ProcessManager::readstring(localname, proc);
            break;
        }
    }
    // Character
    for (LocalCharacter = 0x0; LocalCharacter < 0x1000; ++LocalCharacter) {
        uintptr_t WorkspaceChildrens = ProcessManager::Read<uintptr_t>(
            ProcessManager::Read<uintptr_t>(WorkspacePtr + Children, proc.processhandle), proc.processhandle);
        uintptr_t localcharacterptr = ProcessManager::Read<uintptr_t>(WorkspaceChildrens + LocalCharacter, proc.processhandle);
        uintptr_t localcharacternameptr = ProcessManager::Read<uintptr_t>(localcharacterptr + Name, proc.processhandle);
        std::string localcharactername = ProcessManager::readstring(localcharacternameptr, proc);

        if (localcharactername == PlayerName) {
            LocalCharacter = localcharacterptr;
            break;
        }
    }

    // Humanoid
    for (LocalHumanoid = 0x0; LocalHumanoid < 0x100000; ++LocalHumanoid) {
        uintptr_t humanoidptr = ProcessManager::Read<uintptr_t>(LocalCharacter + LocalHumanoid, proc.processhandle);
        uintptr_t cdptr = ProcessManager::Read<uintptr_t>(humanoidptr + ClassDescriptor, proc.processhandle);
        uintptr_t cnptr = ProcessManager::Read<uintptr_t>(cdptr + 0x8, proc.processhandle);
        std::string name = ProcessManager::readstring(cnptr, proc);

        if (name == "Humanoid") {
            LocalHumanoid = humanoidptr;
            break;
        }
    }

    // JumpPower
    for (JumpPower = 0x0; JumpPower < 0x1000; ++JumpPower) {
        float JP = ProcessManager::Read<float>(LocalHumanoid + JumpPower, proc.processhandle);
        if (fabs(JP - 50.0f) < 0.001f) {

            ObtainedXAddresses.push_back({ "JumpPower", JumpPower });
            break;
        }
    }

    // WalkSpeed
    for (Walkspeed = 0x0; Walkspeed < 0x1000; ++Walkspeed) {
        float ws = ProcessManager::Read<float>(LocalHumanoid + Walkspeed, proc.processhandle);
        if (fabs(ws - 16.0f) < 0.001f) {

            ObtainedXAddresses.push_back({ "Walkspeed", Walkspeed });
            break;
        }
    }

    // WalkSpeedCheck
    for (WalkspeedCheck = 0x0; WalkspeedCheck < 0x1000; ++WalkspeedCheck) {
        float ws = ProcessManager::Read<float>(LocalHumanoid + WalkspeedCheck, proc.processhandle);
        if (fabs(ws - 16.0f) < 0.001f && WalkspeedCheck != Walkspeed) {

            ObtainedXAddresses.push_back({ "WalkspeedCheck", WalkspeedCheck });
            break;
        }
    }

    // Gravity
    for (Gravity = 0x0; Gravity < 0x1000; ++Gravity) {
        float gravityptr = ProcessManager::Read<float>(Workspace + Gravity, proc.processhandle);
        if (fabs(gravityptr - 196.2f) < 0.001f) {

            ObtainedXAddresses.push_back({ "Gravity", Gravity });
            break;
        }
    }

    // Health
    for (Health = 0x0; Health < 0x1000; ++Health) {
        float HealthValue = ProcessManager::Read<float>(LocalHumanoid + Health, proc.processhandle);
        if (fabs(HealthValue - 100.0f) < 0.001f) {

            ObtainedXAddresses.push_back({ "Health", Health });
            break;
        }
    }

    // FOV
    for (FOV = 0x0; FOV < 0x1000; ++FOV) {
        float FovValue = ProcessManager::Read<float>(Camera + FOV, proc.processhandle);
        if (fabs(FovValue - (70.0f * (3.1415926f / 180.0f))) < 0.001f) {

            ObtainedXAddresses.push_back({ "FOV", FOV });
            break;
        }
    }

    // ScriptContext
    for (ScriptContext = 0x0; ScriptContext < 0x1000; ++ScriptContext) {
        uintptr_t childrens = ProcessManager::Read<uintptr_t>(
            ProcessManager::Read<uintptr_t>(DataModel + Children, proc.processhandle), proc.processhandle);
        uintptr_t ScriptContextptr = ProcessManager::Read<uintptr_t>(childrens + ScriptContext, proc.processhandle);
        uintptr_t nameptr = ProcessManager::Read<uintptr_t>(ScriptContextptr + Name, proc.processhandle);
        std::string name = ProcessManager::readstring(nameptr, proc);

        if (name == "Script Context") {

            ObtainedXAddresses.push_back({ "ScriptContext", ScriptContext });
            ScriptContext = ScriptContextptr;
            break;
        }
    }

}



void ExternalDump::DumpXternal(Process proc) {
    uintptr_t PropertyDescriptorOffStart = Scanner::PatternScan("48 89 5C 24 ? 48 89 4C 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8B EC 48 83 EC ? 48 8B D9 45 33 E4", proc);
    if (PropertyDescriptorOffStart != 0x0) {
        uintptr_t PropertyDescriptorOff = Scanner::GetDisplacementAddress(Scanner::FindNext(Scanner::FindNext(PropertyDescriptorOffStart, proc, "Player", ZYDIS_MNEMONIC_JMP, 0), proc, "Player", ZYDIS_MNEMONIC_LEA, 1), proc);
        ObtainedXAddresses.push_back({ "PropertyDescriptor", PropertyDescriptorOff });
    }

    uintptr_t OverlapStart = Scanner::PatternScan("48 8B 80 88 02 00 00", proc);
    if (OverlapStart != 0x0) {
        uintptr_t Overlap = Scanner::GetDisplacementAddress(OverlapStart, proc);
        ObtainedXAddresses.push_back({ "Overlap", Overlap });
    }

    uintptr_t GlobalStateStart = Scanner::PatternScan("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 49 8B F8 48 8B F2 48 8B D9 8B 81", proc);
    if (GlobalStateStart != 0x0) {
        uintptr_t GlobalStateLEA = Scanner::FindNext(GlobalStateStart, proc, "Player", ZYDIS_MNEMONIC_LEA, 1);
        uintptr_t GlobalState = Scanner::GetDisplacementAddress(GlobalStateLEA, proc);
        ObtainedXAddresses.push_back({ "GlobalState", GlobalState });
    }



    uintptr_t IsCoreScriptStart = Scanner::PatternScan("80 B8 ? ? ? ? ? 0F 84 ? ? ? ? EB", proc);
    if (IsCoreScriptStart != 0x0) {
        uintptr_t IsCoreScript = Scanner::GetDisplacementAddress(IsCoreScriptStart, proc);
        ObtainedXAddresses.push_back({ "IsCoreScript", IsCoreScript });
    }

    uintptr_t RequireBypassStart = Scanner::PatternScan("80 BF ? ? ? ? ? 75 ? 48 8B 43", proc);
    if (RequireBypassStart != 0x0) {
        uintptr_t RequireBypassCMP = Scanner::FindNext(RequireBypassStart, proc, "Player", ZYDIS_MNEMONIC_CMP, 0);
        uintptr_t RequireBypass = Scanner::GetDisplacementAddress(RequireBypassCMP, proc);
        ObtainedXAddresses.push_back({ "RequireBypass", RequireBypass });
    }


    uintptr_t GetSetImplStart = Scanner::PatternScan("88 44 24 ? 48 8B 01 48 8B 88", proc);
    if (GetSetImplStart != 0x0) {
        uintptr_t GetSetImplMOV = Scanner::FindNext(GetSetImplStart, proc, "Player", ZYDIS_MNEMONIC_MOV, 2);
        uintptr_t GetSetImpl = Scanner::GetDisplacementAddress(GetSetImplMOV, proc);
        ObtainedXAddresses.push_back({ "GetSetImplementation", GetSetImpl });
    }
}
