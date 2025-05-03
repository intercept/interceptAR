export module ClassType;

import std;
import DllInterface;
import EnfusionContainers;
import EnfusionTypes;

namespace Intercept {

    export class ScriptModule;

    export class FunctionType {
    public:

        std::string_view GetName() const {
            return GDllInterface.Type_Function_GetName(this);
        }
        ScriptModule* GetModule() const {}
    };


    export class ClassType {
    public:

        std::string_view (*Type_Class_GetName)(void* classType);
        void* (*Type_Class_GetModule)(void* classType);
        void* (*Type_Class_GetBaseClass)(void* classType);
        std::span<void*> (*Type_Class_GetVariables)(void* classType);
        std::span<void*> (*Type_Class_GetFunctions)(void* classType);

        std::string_view GetName() const {
            return GDllInterface.Type_Class_GetName(this);
        }
        ScriptModule* GetModule() const {
            return (ScriptModule*)GDllInterface.Type_Class_GetModule(this);
        }
        ClassType* GetBaseClass() const {
            return (ClassType*)GDllInterface.Type_Class_GetBaseClass(this);
        }

        void* GetVariable() const {}
        void GetFunction(std::string_view name) const {}


        std::span<ENF_Variable*> GetVariables() const {
            auto varSpan = GDllInterface.Type_Class_GetVariables(this);

            return std::span<ENF_Variable*>((ENF_Variable**)varSpan.data(), varSpan.size());
        }
    };







}
