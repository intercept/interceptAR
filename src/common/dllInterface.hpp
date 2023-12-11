#pragma once

class VariableDataHolder;
class FunctionArgumentsHandler;
class FunctionResultHandler;
class ENF_Variable;


export typedef void (*ScriptFunc)(FunctionArgumentsHandler& args, FunctionResultHandler& result);
class IScriptClassBaseSimple;
export typedef void (*registerClassT)(IScriptClassBaseSimple* self, std::string_view name, bool doReg);

// Same as Enforce Script
export enum class LogLevel : uint8_t {
    Spam,
    Verbose,
    Debug,
    Normal,
    Warning,
    Error,
    Fatal
};

//#TODO move into internal namespace, need exported for host, but normal users don't access this
export class DllInterface {
public:
    static const uint64_t CurrentVersion = 1;
    uint64_t version = CurrentVersion;

    void (*printLogMessage)(LogLevel level, std::string_view msg);

    // To not add/remove entries above here, even with changing CurrentVersion, thats not allowed.

    registerClassT regClass;
    bool (*varIsNull)(const VariableDataHolder* ivar);

    // Important, for now inputString needs to be null terminated, real std::string_view is not supported. That needs a change on host side, we still take std::string_view here so we don't need to change API version when this gets fixed
    void (*copyStringIntoVariable)(std::string_view inputString, const ENF_Variable* targetVariableType, VariableDataHolder* targetVariableDataHolder);
};

export inline
#ifndef WIN32
    __attribute__((visibility("default")))
    //__attribute__((__symver__("GDllInterface@1025")))
#endif
DllInterface GDllInterface;

//#TODO move this

export class Intercept {

public:
    static void Print(LogLevel level, std::string_view msg)
    {
        GDllInterface.printLogMessage(level, msg);
    }

};

// Entry point in client DLL


#ifdef _WIN32

inline uint8_t PtrHash(uintptr_t input) { //#TODO move to Util?
    // auto bytes = std::bit_cast<std::array<uint8_t, sizeof(uintptr_t)>>(input); // MSVC I have no fucking idea whats wrong with you
    auto bytes = std::span<uint8_t>(reinterpret_cast<uint8_t*>(&input) + 1, 7);
    uint8_t res = 0;
    for (const auto& byte : bytes) {
        res ^= byte;
    }

    return res;
}

#include <Windows.h>

extern "C" {
extern BOOL WINAPI _DllMainCRTStartup(
    HINSTANCE const instance,
    DWORD const reason,
    LPVOID const reserved);
}

extern "C" IMAGE_DOS_HEADER __ImageBase;

export extern "C" BOOL InterceptEntryPoint(HINSTANCE const instance,
                                           DWORD const reason,
                                           LPVOID const reserved) {

    if (reason != DLL_PROCESS_ATTACH) // We only do special handling at first load
        return _DllMainCRTStartup(instance, reason, reserved);

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

    if (!hLoadedLibrary) {
        Util::BreakToDebuggerIfPresent(); // No host?! Bad
        return false;
    }

    auto& hostDllInterface = *reinterpret_cast<const DllInterface*>(GetProcAddress(hLoadedLibrary, MAKEINTRESOURCEA(1025)));

    if (hostDllInterface.version != DllInterface::CurrentVersion) {

        // Get our name
        char dllName[_MAX_PATH] {};
        ::GetModuleFileNameA((HINSTANCE)&__ImageBase, dllName, _MAX_PATH);
        
        // printLogMessage should be here anyway even if version is wrong

        // Would like to use std::filesystem::path::filename, but we cannot allocate yet. That means we also cannot use normal std::format
        char logMsgBuffer[512] {};
        #ifdef __cpp_lib_format
            std::format_to_n(logMsgBuffer, std::size(logMsgBuffer) - 1, "Intercept plugin \"{}\" is outdated relative to intercept host and refuses to load", dllName);
        #else
            std::snprintf(logMsgBuffer, std::size(logMsgBuffer) - 1, "Intercept plugin \"%s\" is outdated relative to intercept host and refuses to load", dllName);
        #endif

        hostDllInterface.printLogMessage(LogLevel::Error, logMsgBuffer);
        return false;
    }
        
    
    GDllInterface = hostDllInterface;

    return _DllMainCRTStartup(instance, reason, reserved);
}

#else

// Linux entry point

void __attribute__((constructor(0))) InterceptEntryPoint(void) {

    // find host
    Util::BreakToDebuggerIfPresent();
    auto hostRef = dlopen("InterceptHost.so", RTLD_NOLOAD);

    // https://anadoxin.org/blog/control-over-symbol-exports-in-gcc.html/
    //  -fvisibility=hidden compile option?

    auto& hostDllInterface = *reinterpret_cast<const DllInterface*>(dlsym(hostRef, "GDllInterface@1025"));

    if (hostDllInterface.version != DllInterface::CurrentVersion) {

        // printLogMessage should be here anyway even if version is wrong

        // Would like to use std::filesystem::path::filename, but we cannot allocate yet. That means we also cannot use normal std::format
        char logMsgBuffer[512]{};
#ifdef __cpp_lib_format
        std::format_to_n(logMsgBuffer, std::size(logMsgBuffer) - 1, "Intercept plugin \"{}\" is outdated relative to intercept host and refuses to load", dllName);
#else
        std::snprintf(logMsgBuffer, std::size(logMsgBuffer) - 1, "Intercept plugin \"%s\" is outdated relative to intercept host and refuses to load", "#TODO");
#endif

        hostDllInterface.printLogMessage(LogLevel::Error, logMsgBuffer);
        return;
    }


    GDllInterface = hostDllInterface;
}



#endif
