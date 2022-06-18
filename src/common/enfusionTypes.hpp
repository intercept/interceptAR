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
    VariableType getType() const { return (VariableType)(type >> 28); }
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
        return type.getType();
    }
    BitflagEnum<VariableSubType> GetVariableSubType() const {
        return type.getSubType();
    }

    // Only works for arrays
    uint32_t GetSize() const {
        return arraySizeMagic[0] * (arraySizeMagic[1] ? arraySizeMagic[1] : 1); //#TODO wtf?
    }
};


export class VariableHelper : private VariableDataHolder {
public:
    template <typename Type>
    Type GetAs() const { return VariableDataHolder::GetAs<Type>(); };

    template <typename Type>
    void SetAs(Type value) {if (!isNull()) VariableDataHolder::GetAs<Type>() = value;};

    template <typename Type>
    static bool verifyTypeMatch(const ENF_Variable* var) { __debugbreak(); return false; };


    //#TODO this may be easier to read if implemented as classes but eeeh

    template <> bool GetAs<bool>() const { return VariableDataHolder::GetAs<bool>();};
    template <> void SetAs<bool>(bool value) { if (!isNull()) VariableDataHolder::GetAs<bool>() = value; };
    template <> static bool verifyTypeMatch<bool>(const ENF_Variable* var) { return var && var->GetVariableType() == VariableType::IntT && !var->GetVariableSubType().isSet(VariableSubType::Pointer); };

    template <> float GetAs<float>() const { return VariableDataHolder::GetAs<float>();};
    template <> void SetAs<float>(float value) { if (!isNull()) VariableDataHolder::GetAs<float>() = value; };
    template <> static bool verifyTypeMatch<float>(const ENF_Variable* var) { return var && var->GetVariableType() == VariableType::FloatT && !var->GetVariableSubType().isSet(VariableSubType::Pointer); };

    template <> int GetAs<int>() const { return VariableDataHolder::GetAs<int>();};
    template <> void SetAs<int>(int value) { if (!isNull()) VariableDataHolder::GetAs<int>() = value; };
    template <> static bool verifyTypeMatch<int>(const ENF_Variable* var) { return var && var->GetVariableType() == VariableType::IntT && !var->GetVariableSubType().isSet(VariableSubType::Pointer); };

    template <> uint32_t GetAs<uint32_t>() const { return VariableDataHolder::GetAs<uint32_t>();};
    template <> void SetAs<uint32_t>(uint32_t value) { if (!isNull()) VariableDataHolder::GetAs<uint32_t>() = value; };
    template <> static bool verifyTypeMatch<uint32_t>(const ENF_Variable* var) { return var && var->GetVariableType() == VariableType::IntT && !var->GetVariableSubType().isSet(VariableSubType::Pointer); };

    //#TODO is this incompatible with script? script doesn't have int64?
    template <> uint64_t GetAs<uint64_t>() const { return VariableDataHolder::GetAs<uint64_t>();};
    template <> void SetAs<uint64_t>(uint64_t value) { if (!isNull()) VariableDataHolder::GetAs<uint64_t>() = value; };
    template <> static bool verifyTypeMatch<uint64_t>(const ENF_Variable* var) { return var && var->GetVariableType() == VariableType::IntT && !var->GetVariableSubType().isSet(VariableSubType::Pointer); };

    // Color == Int
    // Enum


    template <> const char* GetAs<const char*>() const { return VariableDataHolder::GetAs<const char*>();};
    template <> void SetAs<const char*>(const char* value) { 
         __debugbreak(); //#TODO major
        // if (!isNull())
        //     CopyString(value.cstr(), def, &GetAs<char*>(), forceCopy);
    };
    template <> static bool verifyTypeMatch<const char*>(const ENF_Variable* var) { return var && var->GetVariableType() == VariableType::IntT && var->GetVariableSubType().isSet(VariableSubType::Pointer) && var->GetSize() == 0; };

    template <typename T>
    struct assert_false : std::false_type {};

    template <> std::string_view GetAs<std::string_view>() const { return std::string_view(VariableDataHolder::GetAs<const char*>());};
    template <>
    void SetAs<std::string_view>(std::string_view value) {
        __debugbreak(); // Cannot set a string_view value, thats const :U :angery:
        //static_assert(assert_false<std::string_view>::value);
    };
    template <> static bool verifyTypeMatch<std::string_view>(const ENF_Variable* var) { return var && var->GetVariableType() == VariableType::IntT && var->GetVariableSubType().isSet(VariableSubType::Pointer) && var->GetSize() == 0; };



    // arrays of floats
    template <> Vector3 GetAs<Vector3>() const { return VariableDataHolder::GetAs<float*>(); };
    template <> void SetAs<Vector3>(Vector3 value) {

        memcpy(VariableDataHolder::GetAs<float*>(), value.d, sizeof(float) * 3);

        //__debugbreak();
        //static_assert(assert_false<Vector3>::value);
    };
    template <>
    static bool verifyTypeMatch<Vector3>(const ENF_Variable* var) { return var && var->GetVariableType() == VariableType::VectorT && var->GetVariableSubType().isSet(VariableSubType::Pointer) && var->GetSize() == 3; }; //#RENAME IsPtr

    //#TODO Vector4, Quaternion, Matrix33, Matrix43, what else?

    //#TODO enum


    //#TODO enfusionTypesTodo.hpp VariableHelper


    bool isNull() const {
        //#TODO check if this works, I think it doesn't, it checks this ptr, but it should probably check actual value stored inside
        __debugbreak();
        return GDllInterface.varIsNull(this);
    }
};



export class FunctionResultHandler {
public:
    template <typename Type>
    Type GetAs() const { return getHelper()->GetAs<Type>(); };

    template <typename Type>
    bool verifyType() const {
        return VariableHelper::verifyTypeMatch<Type>(varInternal);
    }

    VariableType getType() const {
        return varInternal->GetVariableType();
    }

    template <typename Type>
    void SetAs(const Type& value) {
        if (verifyType<Type>()) {
            getHelper()->SetAs<Type>(value);
        } else
            __debugbreak();
    };

    bool isNull() const {
        return getHelper()->isNull();
    }


    bool IsValid() const {
        return varInternal != nullptr;
    }

protected:
    ENF_Variable* varInternal;

    
    VariableHelper* getHelper() {
        return reinterpret_cast<VariableHelper*>(varInternal);
    }

    const VariableHelper* getHelper() const {
        return reinterpret_cast<const VariableHelper*>(varInternal);
    }

};

export class FunctionArgumentsHandler {
public:
    template <typename Type>
    Type GetAs(uint32_t index) const { return getHelper(index)->GetAs<Type>(); };

    // Variables arguments are read only
    //#TODO minor, out parameters are not! Can easily add verification here that its out param type
    //template <typename Type>
    //void SetAs(const Type& value) {
    //    if (verifyType<Type>()) {
    //        getHelper()->SetAs<Type>(value);
    //    } else
    //        __debugbreak();
    //};


    template <typename Type>
    bool verifyType(uint32_t index) const {
        return VariableHelper::verifyTypeMatch<Type>(getVarTypeThing(index));
    }

    VariableType getType(uint32_t index) const {
        return getVarTypeThing(index)->GetVariableType();
    }

    template <typename Type>
    void SetAs(uint32_t index, const Type& value) {
        if (verifyType<Type>(index)) {
            getHelper(index)->SetAs<Type>(value);
        } else
            __debugbreak();
    };

    bool isNull(uint32_t index) const {
        return getHelper(index)->isNull();
    }

     //#TODO major?
    //uint32_t GetSize(uint32_t index) {
    //    return getVarTypeThing(index)->GetSize();
    //}

protected:
    VariableDataHolder** variablesArray;
    const ENF_Variable* const* variableTypesThing;

    VariableHelper* getHelper(uint32_t index) {
        return reinterpret_cast<VariableHelper*>(variablesArray[index]);
    }

    const VariableHelper* getHelper(uint32_t index) const {
        return reinterpret_cast<const VariableHelper*>(variablesArray[index]);
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
        if (verifyType<Type>()) {
            getHelper()->SetAs<Type>(value);
        } else
            __debugbreak();
    };

    template <typename Type>
    bool verifyType() const {
        return VariableHelper::verifyTypeMatch<Type>(variableTypeThing);
    }

    VariableType getType() const {
        return variableTypeThing->GetVariableType();
    }

    bool isNull() const {
        return getHelper()->isNull();
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






















