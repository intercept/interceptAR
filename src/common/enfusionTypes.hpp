#pragma once

#include "interceptTypes.hpp"

import <cstdint>;
import <functional>;
import <typeinfo>;
import <optional>;



// Generic

export struct Vector3 { //#TODO
    Vector3(float* p) { memcpy(d, p, sizeof(float) * 3); };
    Vector3(std::initializer_list<float> list) { memcpy(d, list.begin(), sizeof(float) * 3); };
    float d[3]{};
};

// No export, only used for variable getter
template<class T>
class ENF_ArrayInstance {

    void* dummy1;
    void* dummy2;
    void* dummy3;
    void* dummy4;
    void* dummy5;
    ENF_Array<T> array;
public: 
    const ENF_Array<T>& GetArray() const { return array; }
};

















export class VariableDataHolder {
    alignas(8) char buffer[8] {};
public:
    template <typename Type>
    const Type& GetAs() const {
        static_assert(sizeof(Type) <= 8, "Unsupported type, is larger than possible buffer");
        return reinterpret_cast<const Type&>(buffer);
    }

    template <typename Type>
    Type& GetAs() {
        static_assert(sizeof(Type) <= 8, "Unsupported type, is larger than possible buffer");
        return reinterpret_cast<Type&>(buffer);
    }
};

export enum class VariableType {
    NullT = 0,
    BoolT,
    IntT, // 32 or 64bit?
    FloatT,
    VectorT,
    StringT,
    ClassT,
    VoidT
};

//#TODO enfusionTypesTodo.hpp VariableSubType
export enum class VariableSubType : uint32_t {
    Pointer = 1 << 6
};

class VariableTypeInfo {
    uint32_t type{0}; // Type is only the first 4 bits, the rest is smth else
    VariableSubType subtype{0};
public:
    VariableType GetType() const { return (VariableType)(type >> 28); }
    BitflagEnum<VariableSubType> getSubType() const { return subtype; }
};

export class ENF_Variable : public VariableDataHolder {
private:
    const char* name;
    VariableTypeInfo type;
    void* placeholder; //#TODO minor
    uint16_t arraySizeMagic[2]{}; //#TODO Wtf?
public:
    const char* GetName() const {
        return name;
    }
    VariableType GetVariableType() const {
        return type.GetType();
    }
    BitflagEnum<VariableSubType> GetVariableSubType() const {
        return type.getSubType();
    }

    // Only works for arrays
    uint32_t GetSize() const {
        return arraySizeMagic[0] * (arraySizeMagic[1] ? arraySizeMagic[1] : 1); //#TODO wtf?
    }
};


export class VariableHelper {
    VariableDataHolder& holder;
    const ENF_Variable& typeThing;
public:

    VariableHelper(VariableDataHolder& holder, const ENF_Variable& typeThing) : holder(holder), typeThing(typeThing) {};

    template <typename Type>
    Type GetAs() const { return holder.GetAs<Type>(); };

    template <typename Type>
    void SetAs(Type value) {if (!IsNull()) holder.GetAs<Type>() = value;};

    template <typename Type>
    static bool VerifyTypeMatch(const ENF_Variable* var) { __debugbreak(); return false; };


    //#TODO this may be easier to read if implemented as classes but eeeh

    template <> bool GetAs<bool>() const { return holder.GetAs<bool>();};
    template <> void SetAs<bool>(bool value) { if (!IsNull()) holder.GetAs<bool>() = value; };
    template <> static bool VerifyTypeMatch<bool>(const ENF_Variable* var) { return var && var->GetVariableType() == VariableType::IntT && !var->GetVariableSubType().isSet(VariableSubType::Pointer); };

    template <> float GetAs<float>() const { return holder.GetAs<float>();};
    template <> void SetAs<float>(float value) { if (!IsNull()) holder.GetAs<float>() = value; };
    template <> static bool VerifyTypeMatch<float>(const ENF_Variable* var) { return var && var->GetVariableType() == VariableType::FloatT && !var->GetVariableSubType().isSet(VariableSubType::Pointer); };

    template <> int GetAs<int>() const { return holder.GetAs<int>();};
    template <> void SetAs<int>(int value) { if (!IsNull()) holder.GetAs<int>() = value; };
    template <> static bool VerifyTypeMatch<int>(const ENF_Variable* var) { return var && var->GetVariableType() == VariableType::IntT && !var->GetVariableSubType().isSet(VariableSubType::Pointer); };

    template <> uint32_t GetAs<uint32_t>() const { return holder.GetAs<uint32_t>();};
    template <> void SetAs<uint32_t>(uint32_t value) { if (!IsNull()) holder.GetAs<uint32_t>() = value; };
    template <> static bool VerifyTypeMatch<uint32_t>(const ENF_Variable* var) { return var && var->GetVariableType() == VariableType::IntT && !var->GetVariableSubType().isSet(VariableSubType::Pointer); };

    //#TODO is this incompatible with script? script doesn't have int64?
    template <> uint64_t GetAs<uint64_t>() const { return holder.GetAs<uint64_t>();};
    template <> void SetAs<uint64_t>(uint64_t value) { if (!IsNull()) holder.GetAs<uint64_t>() = value; };
    template <> static bool VerifyTypeMatch<uint64_t>(const ENF_Variable* var) { return var && var->GetVariableType() == VariableType::IntT && !var->GetVariableSubType().isSet(VariableSubType::Pointer); };

    // Color == Int
    // Enum


    template <> const char* GetAs<const char*>() const { return holder.GetAs<const char*>();};
    template <> void SetAs<const char*>(const char* value) { if (!IsNull()) GDllInterface.copyStringIntoVariable(value, &typeThing, &holder); };
    template <> static bool VerifyTypeMatch<const char*>(const ENF_Variable* var) { return var && var->GetVariableType() == VariableType::StringT &&! var->GetVariableSubType().isSet(VariableSubType::Pointer); };

    template <typename T>
    struct assert_false : std::false_type {};



    template <> std::string_view GetAs<std::string_view>() const { return std::string_view(holder.GetAs<const char*>());};
    //template <> void SetAs<std::string_view>(std::string_view value) { if (!IsNull()) GDllInterface.copyStringIntoVariable(value, &typeThing, &holder); };
    //  Cannot do string_view, Enfusion code requires string to be nullterminated when we pass to copyStringIntoVariable. This needs to be improved on host side //#TODO Minor
    template <> void SetAs<std::string_view>(std::string_view value) = delete;
    template <> static bool VerifyTypeMatch<std::string_view>(const ENF_Variable* var) { return var && var->GetVariableType() == VariableType::StringT && !var->GetVariableSubType().isSet(VariableSubType::Pointer); };

    // arrays of floats
    template <> Vector3 GetAs<Vector3>() const { return holder.GetAs<float*>(); };
    template <> void SetAs<Vector3>(Vector3 value) { memcpy(holder.GetAs<float*>(), value.d, sizeof(float) * 3); };
    template <> static bool VerifyTypeMatch<Vector3>(const ENF_Variable* var) { return var && var->GetVariableType() == VariableType::VectorT && var->GetVariableSubType().isSet(VariableSubType::Pointer) && var->GetSize() == 3; };

    //#TODO Vector4, Quaternion, Matrix33, Matrix43, what else?

    //#TODO enum

    // Array... meh... //#TODO make this a bit nicer? somehow?

#define ARRAYTYPE(TYPE) \
    template <> std::span<TYPE> GetAs<std::span<TYPE>>() const { return holder.GetAs<ENF_ArrayInstance<TYPE>*>()->GetArray().AsSpan(); }; \
    template <> void SetAs<std::span<TYPE>>(std::span<TYPE> value) = delete; \
    template <> static bool VerifyTypeMatch<std::span<TYPE>>(const ENF_Variable* var) { return var && var->GetVariableType() == VariableType::ClassT && var->GetVariableSubType().isSet(VariableSubType::Pointer); };

    /*
     Enforce Script defines these:
        typedef array<string> TStringArray;
        typedef array<float> TFloatArray;
        typedef array<int> TIntArray;
        typedef array<bool> TBoolArray;
        typedef array<Class> TClassArray;
        typedef array<Managed> TManagedArray;
        typedef array<ref Managed> TManagedRefArray;
        typedef array<vector> TVectorArray;
        typedef array<pointer> TPointerArray;
        typedef array<ResourceName> TResourceNameArray;

     We currently don't support Instances, so Class/Managed/pointer is not available. Not sure about ResourceName, that might just be alias for string?
     
     */



    ARRAYTYPE(Vector3)
    ARRAYTYPE(const char*)
    ARRAYTYPE(float)
    ARRAYTYPE(int)
    ARRAYTYPE(bool)
#undef ARRAYTYPE


    //#TODO enfusionTypesTodo.hpp VariableHelper

    bool IsNull() const {
        return GDllInterface.varIsNull(&holder);
    }
};



export class FunctionResultHandler {
public:
    template <typename Type>
    Type GetAs() const { return getHelper().GetAs<Type>(); };

    template <typename Type>
    bool VerifyType() const {
        return VariableHelper::VerifyTypeMatch<Type>(varInternal);
    }

    VariableType GetType() const {
        return varInternal->GetVariableType();
    }

    template <typename Type>
    void SetAs(const Type& value) {
        if (VerifyType<Type>()) {
            getHelper().SetAs<Type>(value);
        } else
            __debugbreak();
    };

    bool IsNull() const {
        return getHelper().IsNull();
    }


    bool IsValid() const {
        return varInternal != nullptr;
    }

protected:
    ENF_Variable* varInternal;

    
    VariableHelper getHelper() {
        return VariableHelper(*varInternal, *varInternal);
    }

    const VariableHelper getHelper() const {
        return VariableHelper(*varInternal, *varInternal);
    }

};

export class FunctionArgumentsHandler {
public:
    template <typename Type>
    Type GetAs(uint32_t index) const { return getHelper(index).GetAs<Type>(); };

    // Variables arguments are read only
    //#TODO minor, out parameters are not! Can easily add verification here that its out param type
    //template <typename Type>
    //void SetAs(const Type& value) {
    //    if (VerifyType<Type>()) {
    //        getHelper()->SetAs<Type>(value);
    //    } else
    //        __debugbreak();
    //};


    template <typename Type>
    bool VerifyType(uint32_t index) const {
        return VariableHelper::VerifyTypeMatch<Type>(getVarTypeThing(index));
    }

    VariableType GetType(uint32_t index) const {
        return getVarTypeThing(index)->GetVariableType();
    }

    template <typename Type>
    void SetAs(uint32_t index, const Type& value) {
        if (VerifyType<Type>(index)) {
            getHelper(index).SetAs<Type>(value);
        } else
            __debugbreak();
    };

    bool IsNull(uint32_t index) const {
        return getHelper(index).IsNull();
    }

     //#TODO major?
    //uint32_t GetSize(uint32_t index) {
    //    return getVarTypeThing(index)->GetSize();
    //}

protected:
    VariableDataHolder** variablesArray;
    const ENF_Variable* const* variableTypesThing;

    VariableHelper getHelper(uint32_t index) {
        return VariableHelper(*variablesArray[index], *variableTypesThing[index]);
    }

    const VariableHelper getHelper(uint32_t index) const {
        return VariableHelper(*variablesArray[index], *variableTypesThing[index]);
    }

    const ENF_Variable* getVarTypeThing(uint32_t index) const {
        return variableTypesThing[index];
    }
};

export class ClassInstanceVariable {
public:
    ClassInstanceVariable(VariableHelper* varData, ENF_Variable* varTypeStuff) : varData(varData), variableTypeThing(varTypeStuff) {}


    template <typename Type>
    Type GetAs() const {

        if constexpr (std::_Is_span_v<Type>) {
            return std::span(getHelper()->GetAs<Type::pointer>(), variableTypeThing->GetSize());
        } else
            return getHelper()->GetAs<Type>();
    };

    template <typename Type>
    void SetAs(const Type& value) {
        if (VerifyType<Type>()) {
            getHelper()->SetAs<Type>(value);
        } else
            __debugbreak();
    };

    template <typename Type>
    bool VerifyType() const {
        return VariableHelper::VerifyTypeMatch<Type>(variableTypeThing);
    }

    VariableType GetType() const {
        return variableTypeThing->GetVariableType();
    }

    bool IsNull() const {
        return getHelper()->IsNull();
    }

    bool IsValid() const {
        return variableTypeThing != nullptr;
    }



protected:
    VariableHelper* varData;
    ENF_Variable* variableTypeThing;

    VariableHelper* getHelper() {
        return reinterpret_cast<VariableHelper*>(varData);
    }

    const VariableHelper* getHelper() const {
        return reinterpret_cast<const VariableHelper*>(varData);
    }
};

export class ClassInstance {
    void* dummy1;
    struct {
        void* dummy;
        VariableTypeInfo type;
        const char* name;
    } *classInfo;
    void* dummy2;

    // offset
    //VariableDataHolder data[1];

    VariableHelper* GetDataHolders() const {
        uint32_t size = *(uint32_t*)((uintptr_t)classInfo + 0xB8);

        return (VariableHelper*)(((uintptr_t)this) + size);
    }

    class Variable : public ENF_Variable { // Inherits from ENF_Variable, but data holder here is actually empty, data is after class instance by offset
    public:
        uint16_t offset;
        uint16_t pos;
    };

    uint32_t GetVariableIndex(std::string_view name) const {
        auto& vars = *(ENF_Array<Variable*>*)((uintptr_t)classInfo + 0x58);
        auto found = std::ranges::find_if(vars.AsSpan(), [name](const Variable* var) { return name.compare(var->GetName()) == 0; });
        if (found == vars.AsSpan().end())
            return -1;
        return std::distance(vars.AsSpan().begin(), found);
    }

    Variable* GetVariableRawAt(uint32_t index) const {
        auto& vars = *(ENF_Array<Variable*>*)((uintptr_t)classInfo + 0x58);
        auto var = vars.AsSpan()[index];
        return var;
    }

    VariableHelper* GetVariableAt(uint32_t index) const {
        auto var = GetVariableRawAt(index);
        auto offset = var->offset * 4;

        return (VariableHelper*)((uintptr_t)GetDataHolders() + offset);
    }

public:

    std::optional<ClassInstanceVariable> GetVariable(std::string_view name) const {
        auto index = GetVariableIndex(name);
        if (index == -1)
            return {};

        return ClassInstanceVariable(GetVariableAt(index), GetVariableRawAt(index));
    }
    // windows mess, just get this windows.h include outta here please wtf..
    #undef GetClassName
    std::string_view GetClassName() const {
        return classInfo->name;
    }

    auto GetClassType() const {
        return classInfo->type;
    }


};
















// P2


class IScriptClassBaseSimple {
    uint64_t internalBuffer[8] {};
protected:

    IScriptClassBaseSimple(std::string_view classname) {
        // Register
        GDllInterface.regClass(this, classname, true);
    }

    virtual ~IScriptClassBaseSimple() {
        // UnRegister
        __debugbreak(); // Cannot do that currently
        //GDllInterface.regClass(this, {}, false);
    }

public:
    using RegisterFuncHandler = std::function<void(std::string_view name, ScriptFunc fn)>;
    virtual void DoSetup(RegisterFuncHandler) = 0;
};

export template <FixedString classname>
class ScriptClassBaseSimple : public IScriptClassBaseSimple {
public:
    ScriptClassBaseSimple() : IScriptClassBaseSimple(std::string_view(classname)) {}
};






















