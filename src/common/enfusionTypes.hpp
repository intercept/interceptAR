#pragma once

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


// I would put the get templates into Variable Helper, but cannot use templates here, not supported in GCC https://gcc.gnu.org/bugzilla/show_bug.cgi?id=85282

class VariableHelper;

namespace internal {
    template<typename Type>
    struct VarTraits;
}



export class VariableHelper {
    template <typename Type>
    friend struct internal::VarTraits;

    VariableDataHolder& holder;
    const ENF_Variable& typeThing;
public:

    VariableHelper(VariableDataHolder& holder, const ENF_Variable& typeThing) : holder(holder), typeThing(typeThing) {};

    template <typename Type>
    Type GetAs() const {
        return internal::VarTraits<Type>::GetAs(*this);
    };

    template <typename Type>
    void SetAs(Type value) {
        internal::VarTraits<Type>::SetAs(*this, value);
    };

    template <typename Type>
    static bool VerifyTypeMatch(const ENF_Variable* var) {
        return internal::VarTraits<Type>::VerifyTypeMatch(var);
    };

    // Color == Int
    // Enum

    template <typename T>
    struct assert_false : std::false_type {};



    //#TODO enum

    // Array... meh... //#TODO make this a bit nicer? somehow?


    //#TODO enfusionTypesTodo.hpp VariableHelper

    bool IsNull() const {
        return GDllInterface.varIsNull(&holder);
    }
};


namespace internal {

    // Checks if input is either A or A& or A&& or const A&, we don't care, use whatever
    template <typename A, typename T>
    concept IsMatchingUnderlyingType = std::is_same_v<A, std::remove_cvref_t<T>>;

    template <typename Type>
    struct VarTraits {

        static Type GetAs(const VariableHelper& holder) {
            if constexpr (std::is_pointer_v<Type>)
                return holder.holder.GetAs<Type>();
          
            static_assert(sizeof(Type) < 0 || std::is_pointer_v<Type>, "Invalid Type used in GetAs");
        }
        template <IsMatchingUnderlyingType<Type> NewT>
        static void SetAs(const VariableHelper& holder, NewT newValue) { static_assert(sizeof(Type) < 0, "Invalid Type used in SetAs"); }
        static bool VerifyTypeMatch(const ENF_Variable* var) {
            static_assert(sizeof(Type) < 0, "Invalid Type used in VerifyTypeMatch");
            return false;
        }
    };

    // Implementations are below VariableHelper

    template <>
    struct VarTraits<bool> {
        using Type = bool;

        static Type GetAs(const VariableHelper& holder) { return holder.holder.GetAs<Type>(); }

        template <IsMatchingUnderlyingType<Type> NewT>
        static void SetAs(const VariableHelper& holder, NewT newValue) {
            if (!holder.IsNull())
                holder.holder.GetAs<std::remove_cvref_t<Type>&>() = newValue;
        }
        static bool VerifyTypeMatch(const ENF_Variable* var) {
            return var && var->GetVariableType() == VariableType::IntT && !var->GetVariableSubType().isSet(VariableSubType::Pointer);
        }
    };

    template <>
    struct VarTraits<float> {
        using Type = float;

        static Type GetAs(const VariableHelper& holder) { return holder.holder.GetAs<Type>(); }

        template <IsMatchingUnderlyingType<Type> NewT>
        static void SetAs(const VariableHelper& holder, NewT newValue) {
            if (!holder.IsNull())
                holder.holder.GetAs<std::remove_cvref_t<Type>&>() = newValue;
        }
        static bool VerifyTypeMatch(const ENF_Variable* var) {
            return var && var->GetVariableType() == VariableType::FloatT && !var->GetVariableSubType().isSet(VariableSubType::Pointer);
        }
    };

    template <>
    struct VarTraits<int> {
        using Type = int;

        static Type GetAs(const VariableHelper& holder) { return holder.holder.GetAs<Type>(); }

        template <IsMatchingUnderlyingType<Type> NewT>
        static void SetAs(const VariableHelper& holder, NewT newValue) {
            if (!holder.IsNull())
                holder.holder.GetAs<std::remove_cvref_t<Type>&>() = newValue;
        }
        static bool VerifyTypeMatch(const ENF_Variable* var) {
            return var && var->GetVariableType() == VariableType::IntT && !var->GetVariableSubType().isSet(VariableSubType::Pointer);
        }
    };

    template <>
    struct VarTraits<uint32_t> {
        using Type = uint32_t;

        static Type GetAs(const VariableHelper& holder) { return holder.holder.GetAs<Type>(); }

        template <IsMatchingUnderlyingType<Type> NewT>
        static void SetAs(const VariableHelper& holder, NewT newValue) {
            if (!holder.IsNull())
                holder.holder.GetAs<std::remove_cvref_t<Type>&>() = newValue;
        }
        static bool VerifyTypeMatch(const ENF_Variable* var) {
            return var && var->GetVariableType() == VariableType::IntT && !var->GetVariableSubType().isSet(VariableSubType::Pointer);
        }
    };

    //#TODO is this incompatible with script? script doesn't have int64?
    // template <>
    // struct VarTraits<uint64_t> {
    //    using Type = uint64_t;
    //
    //    Type GetAs(const VariableHelper& holder) const { return holder.GetAs<Type>(); }
    //
    //    template <IsMatchingUnderlyingType<Type> NewT>
    //    void SetAs(const VariableHelper& holder, NewT newValue) const {
    //        if (!holder.IsNull())
    //            holder.GetAs<Type>() = value;
    //    }
    //    bool VerifyTypeMatch(const ENF_Variable* var) {
    //        return var && var->GetVariableType() == VariableType::IntT && !var->GetVariableSubType().isSet(VariableSubType::Pointer);
    //    }
    //};


    template <>
    struct VarTraits<const char*> {
        using Type = const char*;

        static Type GetAs(const VariableHelper& holder) { return holder.GetAs<Type>(); }

        template <IsMatchingUnderlyingType<Type> NewT>
        static void SetAs(const VariableHelper& holder, NewT newValue) {
            if (!holder.IsNull())
                GDllInterface.copyStringIntoVariable(newValue, &holder.typeThing, &holder.holder);
        }
        static bool VerifyTypeMatch(const ENF_Variable* var) {
            return var && var->GetVariableType() == VariableType::StringT && !var->GetVariableSubType().isSet(VariableSubType::Pointer);
        }
    };

    template <>
    struct VarTraits<std::string_view> {
        using Type = std::string_view;

        static Type GetAs(const VariableHelper& holder) { return holder.GetAs<const char*>(); }

        template <IsMatchingUnderlyingType<Type> NewT>
        static void SetAs(const VariableHelper& holder, NewT newValue) = delete; // Cannot set a string_view :D Engine doesn't support non null terminated string so this would need a bit more work
        static bool VerifyTypeMatch(const ENF_Variable* var) {
            return var && var->GetVariableType() == VariableType::StringT && !var->GetVariableSubType().isSet(VariableSubType::Pointer);
        }
    };

    // Array of floats
    template <>
    struct VarTraits<Vector3> {
        using Type = Vector3;

        static Type GetAs(const VariableHelper& holder) { return holder.GetAs<float*>(); }

        template <IsMatchingUnderlyingType<Type> NewT>
        static void SetAs(const VariableHelper& holder, NewT newValue) {
            if (!holder.IsNull())
                memcpy(holder.GetAs<float*>(), newValue.d, sizeof(float) * 3);
        }
        static bool VerifyTypeMatch(const ENF_Variable* var) {
            return var && var->GetVariableType() == VariableType::VectorT && var->GetVariableSubType().isSet(VariableSubType::Pointer) && var->GetSize() == 3;
        }
    };

    //#TODO Vector4, Quaternion, Matrix33, Matrix43, what else?



#define ARRAYTYPE(TYPE)                                                                                                                                          \
    template <>                                                                                                                                                  \
    struct VarTraits<std::span<TYPE>> {                                                                                                                           \
        static std::span<TYPE> GetAs(const VariableHelper& holder) { return holder.GetAs<ENF_ArrayInstance<TYPE>*>()->GetArray().AsSpan(); }                     \
        template <IsMatchingUnderlyingType<std::span<TYPE>> NewT>                                                                                                \
        static void SetAs(const VariableHelper& holder, NewT newValue) = delete; /* Cannot set arrays currently, need to implement writing to ArrayInstance */   \
        static bool VerifyTypeMatch(const ENF_Variable* var) {                                                                                                   \
            return var && var->GetVariableType() == VariableType::ClassT /*Class type is "Array"*/ && var->GetVariableSubType().isSet(VariableSubType::Pointer); \
        }                                                                                                                                                        \
    };

#ifndef __linux__ // GCC internal compiler error https://gcc.gnu.org/bugzilla/show_bug.cgi?id=100583

    ARRAYTYPE(Vector3)
    ARRAYTYPE(const char*)
    ARRAYTYPE(float)
    ARRAYTYPE(int)
    ARRAYTYPE(bool)

#endif
#undef ARRAYTYPE


}




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
            Util::BreakToDebuggerIfPresent(); // You passed a invalid type, the variable stored here is not the type you are trying to set it ass
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
            Util::BreakToDebuggerIfPresent(); // You passed a invalid type, the variable stored here is not the type you are trying to set it ass
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

namespace internal {
    template <class, template <class> class>
    struct is_instance : public std::false_type {};

    template <class T, template <class> class U>
    struct is_instance<U<T>, U> : public std::true_type {};
}

export class ClassInstanceVariable {
public:
    ClassInstanceVariable(VariableHelper* varData, ENF_Variable* varTypeStuff) : varData(varData), variableTypeThing(varTypeStuff) {}


    template <typename Type>
    Type GetAs() const {

        if constexpr (internal::is_instance<Type, std::span>::value) {
            return std::span(getHelper()->GetAs<Type::pointer>(), variableTypeThing->GetSize());
        } else
            return getHelper()->GetAs<Type>();
    };

    template <typename Type>
    void SetAs(const Type& value) {
        if (VerifyType<Type>()) {
            getHelper()->SetAs<Type>(value);
        } else
            Util::BreakToDebuggerIfPresent(); // You passed a invalid type, the variable stored here is not the type you are trying to set it ass
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
        Util::BreakToDebuggerIfPresent();
        //__debugbreak(); // Cannot do that currently
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






















