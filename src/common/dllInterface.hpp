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

import <span>;
uint8_t PtrHash(uintptr_t input) {
    // auto bytes = std::bit_cast<std::array<uint8_t, sizeof(uintptr_t)>>(input); // MSVC I have no fucking idea whats wrong with you
    auto bytes = std::span<uint8_t>(reinterpret_cast<uint8_t*>(&input)+1, 7);
    uint8_t res = 0;
    for (const auto& byte : bytes) {
        res ^= byte;
    }

    return res;
}



export extern "C" BOOL InterceptEntryPoint(HINSTANCE const instance,
                                           DWORD const reason,
                                           LPVOID const reserved) {
    // First code of a Intercept plugin that runs
    // We find our host, and grab its DLL interface struct
    // The host cannot set it for us because it cannot run code before our DllMain runs, but we need it to be initialized before crt init


    // Old interface, cannot use this anymore because we don't know host dll's name
    // auto hLoadedLibrary = GetModuleHandleA("NPClient64.dll");

    // New, grab our base pointer from peb
    auto pebPtr = *reinterpret_cast<PVOID**>(__readgsqword(0x30) + 0x60);

    HMODULE hLoadedLibrary = nullptr;
    // possible locations of our host pointer, find the correct one.
    for (auto& it : { 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6D }) {

        auto ptr = reinterpret_cast<uint64_t>(DecodePointer(reinterpret_cast<PVOID*>(pebPtr)[it]));
        ptr = ptr ^ static_cast<uint64_t>(PtrHash(ptr));
        if (!(ptr & 0xFF)) { // found it
            hLoadedLibrary = reinterpret_cast<decltype(hLoadedLibrary)>(ptr);
            break;
        }
    }

    auto& hostDllInterface = *reinterpret_cast<const DllInterface*>(GetProcAddress(hLoadedLibrary, MAKEINTRESOURCEA(1)));

    if (hostDllInterface.version != DllInterface::CurrentVersion)
        return false;
    
    GDllInterface = hostDllInterface;

    return _DllMainCRTStartup(instance, reason, reserved);
}
