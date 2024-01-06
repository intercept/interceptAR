![Intercept](http://i.imgur.com/J0PC0G0.png)

Intercept is a **C++20** binding interface for Arma Reforger's Enfusion Script.

In a nutshell, Intercept lets you write your own "proto" methods in C++ code.

Intercept works on a host/client based system, in which the host, Intercept itself, hosts client DLLs that implement the Intercept library. The Intercept host handles access to the engine. Client DLLs are then able to be written in a way that can safely ignore most internal nuances of handling data in the Enfusion engine and work with standard C++ STD/STL data types, and only a few specialized objects specific to the game engine.

This project is still in early stages, functionality is limited but it should be sufficient for most use cases.

You can get an overview of what Features are planned or in-progress in our [Project board](https://github.com/orgs/intercept/projects/1)

If you want to request new Features feel free to post on our [Discussions board](https://github.com/intercept/interceptAR/discussions/categories/ideas).

Due to legal reasons the host side code is closed-source.
And for safety reasons Intercept plugins **will only work on a dedicated server or inside workbench**, but not on clients.

InterceptAR requires a compiler with C++20 support that is ABI compatible with the InterceptHost.dll, in short, **Visual Studio 2022**.

Note: If you get **crashes** in your plugins, please don't submit the crash reports in the crash reporter, don't waste Enfusion developers time with crashes that are not their fault.

## Installation

There will be additional installation methods in the future.

### Windows

1. Download the [latest InterceptAR release](https://github.com/intercept/interceptAR/releases/).
2. Take the InterceptHost.dll, rename it to dbgcore.dll and place it next to your ArmaReforgerServer.exe or ArmaReforgerWorkbench.exe
3. Create a Intercept Plugin, you can use the [Plugin Template](https://github.com/intercept/interceptAR-plugin-template) as a guide.
4. Place your plugin's DLL file into the "interceptPlugins" folder next to your desired .exe file.

You should end up with a setup like this
- Arma Reforger Server
  - ArmaReforgerServer.exe
  - dbgcore.dll
  - interceptPlugins
    - examplePlugin.dll

### Linux

!! Only Ubuntu 22.04 has been tested so far.

1. Download the [latest InterceptAR release](https://github.com/intercept/interceptAR/releases/).
2. Take the InterceptHost.so file and place it next to your ArmaReforgerServer.
3. Either use the [Wrapper Script](https://github.com/intercept/interceptAR/blob/master/misc/linuxLaunchScript.sh) to launch your server, or manually use LD to launch the server and load Intercept
```
/lib64/ld-linux-x86-64.so.2 --preload ./InterceptHost.so ./ArmaReforgerServer -config "./serverconfig/ServerCfg.json"
```
4. Create a Intercept Plugin, you can use the [Plugin Template](https://github.com/intercept/interceptAR-plugin-template) as a guide.
5. Place your plugin's SO file into a "interceptPlugins" folder next to your desired ArmaReforgerServer file.



## Debugging

### Windows

To debug your plugin on windows, simply attach your IDE's Debugger to running process, and attach it to your Workbench/Server.

### Linux

Run LD and your Server through gdb
```
gdb --args /lib64/ld-linux-x86-64.so.2 --preload ./InterceptHost.so ./ArmaReforgerServer -config "./serverconfig/ServerCfg.json"
```
You can also do this directly in Visual Studio code via launch.json.
Only abbreviated example here, refer to your own launch.json for the rest
```json
            "name": "(gdb) Launch",
            "type": "cppdbg",
            "request": "launch",
            "program": "/lib64/ld-linux-x86-64.so.2",
            "args": [ 
                "--preload",
                "./InterceptHost.so",
                "./ArmaReforgerServer",
                "-config",
                "\"./serverconfig/ServerCFG.json\""
            ],
            "stopAtEntry": false,
            "cwd": "/home/server/steam/steamapps/common/Arma Reforger Server",
```


## Example

```c++
// the Intercept library, only one import required.
import InterceptCommon;


/*
    class ExampleClass
    {
        static proto float TestFunction(string arg);
    };
*/

// You register your own plugin class by inheriting from ScriptClassBaseSimple and then storing a global instance of that class.
// The Intercept Host will automatically call the DoSetup method, letting you register your functions with Enfusion Script

class ExampleClass : public ScriptClassBaseSimple<"ExampleClass"> {
public:

    // static proto float TestFunction(string arg);
    static void TestFunction(FunctionArgumentsHandler& args, FunctionResultHandler& result) {
        auto myString = args.GetAs<std::string_view>(0);

        result.SetAs<float>(133.7f);
    }

    void DoSetup(ScriptClassBaseSimple::RegisterFuncHandler registerFunction) override {
        // We need to assign Enforce Script function name, to our function implementation here

        registerFunction("TestFunction", &TestFunction);
    }
};

ExampleClass GExampleClass; // Register the class, it needs to be a global that never gets deleted

```

## Contributions

Feel free to contribute as much as you want to this project in terms of time and code. The goal of this project is to be a tool for the community.

## License

Intercept is licensed under [Arma Public License Share Alike](https://www.bohemia.net/community/licenses/arma-public-license-share-alike).
