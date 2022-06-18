#pragma once

#include <Windows.h>

class VariableDataHolder;
class FunctionArgumentsHandler;
class FunctionResultHandler;


export typedef void (*ScriptFunc)(FunctionArgumentsHandler& args, FunctionResultHandler& result);
class IScriptClassBaseSimple;
export typedef void (*registerClassT)(IScriptClassBaseSimple* self, std::string_view name, bool doReg);

export class DllInterface {
public:
    static const uint64_t CurrentVersion = 1;
    uint64_t version = CurrentVersion;

    registerClassT regClass;
    bool (*varIsNull)(const VariableDataHolder* ivar);
    //#TODO copystring

};

extern DllInterface GDllInterface;
// __declspec(dllexport)
export inline DllInterface GDllInterface;

//#TODO only host DLL should export, doesn't matter much though

#pragma comment(linker, "/export:GDllInterface=?GDllInterface@@3VDllInterface@@A,@1,NONAME,DATA")
//export extern "C" __declspec(dllexport) DllInterface& _InterceptGetDllInterface() { return GDllInterface; }


// Entry point in client DLL

extern "C" {
extern BOOL WINAPI _DllMainCRTStartup(
    HINSTANCE const instance,
    DWORD const reason,
    LPVOID const reserved);
}

export extern "C" BOOL InterceptEntryPoint(HINSTANCE const instance,
                                           DWORD const reason,
                                           LPVOID const reserved) {

    auto hLoadedLibrary = GetModuleHandleA("NPClient64.dll"); //#TODO some better way to find host
    auto& hostDllInterface = *reinterpret_cast<const DllInterface*>(GetProcAddress(hLoadedLibrary, MAKEINTRESOURCEA(1)));

    if (hostDllInterface.version != DllInterface::CurrentVersion)
        return false;

    GDllInterface = hostDllInterface;

    return _DllMainCRTStartup(instance, reason, reserved);
}
