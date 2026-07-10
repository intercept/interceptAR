export module ScriptModule;

import std;
import DllInterface;
import EnfusionTypes;

namespace Intercept {

    extern "C++" export class ClassType;
    extern "C++" export class FunctionType;

    export extern "C++" class ScriptModule {
    public:


        //ClassType* FindType(std::string_view name) {
        //    return GDllInterface.Type_ScriptModule_FindType(this, name);
        //}

        static ClassType* FindType(std::string_view name) {
            return (ClassType*)GDllInterface.Type_ScriptModule_FindType(nullptr, name);
        }

        std::string_view GetName() {
            return GDllInterface.Type_ScriptModule_GetName(this);
        }

        std::span<ENF_Variable*> GetVariables() const {
            auto varSpan = GDllInterface.Type_ScriptModule_GetVariables(this);

            return std::span<ENF_Variable*>((ENF_Variable**)varSpan.data(), varSpan.size());
        }

        std::span<FunctionType*> GetFunctions() const {
            auto varSpan = GDllInterface.Type_ScriptModule_GetFunctions(this);

            return std::span<FunctionType*>((FunctionType**)varSpan.data(), varSpan.size());
        }

        std::span<ClassType*> GetTypes() const {
            auto varSpan = GDllInterface.Type_ScriptModule_GetTypes(this);

            return std::span<ClassType*>((ClassType**)varSpan.data(), varSpan.size());
        }


    };


}
