#pragma once


#pragma region Refcounting

export class refcount_base {
public:
    constexpr refcount_base() noexcept : _refcount(0) {}
    constexpr refcount_base(const refcount_base&) noexcept : _refcount(0) {}
    constexpr void operator=(const refcount_base&) const noexcept {}

    constexpr int add_ref() const noexcept {
        return ++_refcount;
    }
    constexpr int dec_ref() const noexcept {
        return --_refcount;
    }
    constexpr int ref_count() const noexcept {
        return _refcount;
    }
    mutable int _refcount;
};

// refcount has to be the first element in a class. Use refcount_vtable instead if refcount is preceded by a vtable
export class refcount : public refcount_base {
public:
    virtual ~refcount() = default;
    int release() const {
        const auto rcount = dec_ref();
        if (rcount == 0) {
            // this->~refcount();
            lastRefDeleted();
            // rv_allocator<refcount>::deallocate(const_cast<refcount *>(this), 0);
        }
        return rcount;
    }
    void destruct() const noexcept {
        delete const_cast<refcount*>(this);
    }
    virtual void lastRefDeleted() const { destruct(); }

private:
    virtual int __dummy_refcount_func() const noexcept { return 0; }
};


export template <class Type>
class ref {
    friend class game_value_static; // Overrides _ref to nullptr in destructor when Arma is exiting
    static_assert(std::is_base_of<refcount_base, Type>::value, "Type must inherit refcount_base");
    Type* _ref{nullptr};

public:
    using baseType = Type;

    constexpr ref() noexcept = default;
    ~ref() { free(); }

    // Construct from Pointer
    constexpr ref(Type* other_) noexcept {
        if (other_)
            other_->add_ref();
        _ref = other_;
    }
    // Copy from pointer
    ref& operator=(Type* source_) noexcept {
        Type* old = _ref;
        if (source_)
            source_->add_ref();
        _ref = source_;
        if (old)
            old->release(); // decrement reference and delete object if refcount == 0
        return *this;
    }

    // Construct from reference
    constexpr ref(const ref& source_ref_) noexcept {
        Type* source = source_ref_._ref;
        if (source)
            source->add_ref();
        _ref = source;
    }
    // Copy from reference.
    ref& operator=(const ref& other_) {
        Type* source = other_._ref;
        Type* old = _ref;
        if (source)
            source->add_ref();
        _ref = source;
        if (old)
            old->release(); // decrement reference and delete object if refcount == 0
        return *this;
    }

    // Construct from reference and convert
    template <typename T>
    constexpr ref(const ref<T>& source_ref_) noexcept {
        static_assert(std::is_constructible_v<Type*, T*> || std::is_base_of_v<T, Type>, "Cannot convert intercept::types::ref to incompatible type");
        T* source = source_ref_.get();
        if (source)
            source->add_ref();
        _ref = static_cast<Type*>(source);
    }
    // Copy from reference.
    template <class T>
    ref& operator=(const ref<T>& other_) {
        static_assert(std::is_constructible_v<Type*, T*> || std::is_base_of_v<T, Type>, "Cannot convert intercept::types::ref to incompatible type");
        T* source = other_.get();
        Type* old = _ref;
        if (source)
            source->add_ref();
        _ref = source;
        if (old)
            old->release(); // decrement reference and delete object if refcount == 0
        return *this;
    }

    void swap(ref& other_) noexcept {
        auto temp = other_._ref;
        other_._ref = _ref;
        _ref = temp;
    }
    constexpr bool is_null() const noexcept { return _ref == nullptr; }
    void free() noexcept {
        if (!_ref)
            return;
        _ref->release();
        _ref = nullptr;
    }
    // This returns a pointer to the underlying object. Use with caution!

    [[deprecated]] constexpr Type* getRef() const noexcept { return _ref; }
    /// This returns a pointer to the underlying object. Use with caution!
    constexpr Type* get() const noexcept { return _ref; }
    constexpr Type* operator->() const noexcept { return _ref; }
    operator Type*() const noexcept { return _ref; }
    bool operator!=(const ref<Type>& other_) const noexcept { return _ref != other_._ref; }
    size_t ref_count() const noexcept { return _ref ? _ref->ref_count() : 0; }
};

// specialization for I_debug_value. You should not be using this.
export template <class Type>
class i_ref {
    Type* _ref{nullptr};

public:
    using baseType = Type;
    constexpr i_ref() noexcept = default;
    ~i_ref() { free(); }

    // Construct from Pointer
    constexpr i_ref(Type* other_) noexcept {
        if (other_)
            other_->IaddRef();
        _ref = other_;
    }
    // Copy from pointer
    i_ref& operator=(Type* source_) noexcept {
        Type* old = _ref;
        if (source_)
            source_->IaddRef();
        _ref = source_;
        if (old)
            old->release(); // decrement reference and delete object if refcount == 0
        return *this;
    }

    // Construct from reference
    constexpr i_ref(const i_ref& source_ref_) noexcept {
        Type* source = source_ref_._ref;
        if (source)
            source->IaddRef();
        _ref = source;
    }
    // Copy from reference.
    i_ref& operator=(const i_ref& other_) {
        Type* source = other_._ref;
        Type* old = _ref;
        if (source)
            source->IaddRef();
        _ref = source;
        if (old)
            old->Irelease(); // decrement reference and delete object if refcount == 0
        return *this;
    }
    void swap(i_ref& other_) noexcept {
        auto temp = other_._ref;
        other_._ref = _ref;
        _ref = temp;
    }
    constexpr bool is_null() const noexcept { return _ref == nullptr; }
    void free() noexcept {
        if (!_ref)
            return;
        _ref->Irelease();
        _ref = nullptr;
    }
    // This returns a pointer to the underlying object. Use with caution!

    [[deprecated]] constexpr Type* getRef() const noexcept { return _ref; }
    /// This returns a pointer to the underlying object. Use with caution!
    constexpr Type* get() const noexcept { return _ref; }
    constexpr Type* operator->() const noexcept { return _ref; }
    operator Type*() const noexcept { return _ref; }
    bool operator!=(const ref<Type>& other_) const noexcept { return _ref != other_._ref; }
    size_t ref_count() const noexcept { return _ref ? _ref->ref_count() : 0; }
};

/// When this goes out of scope. The pointer is deleted. This takes ownership of the pointer
export template <class Type>
class unique_ref {
protected:
    Type* _ref;

public:
    unique_ref() { _ref = NULL; }
    unique_ref(Type* source) {
        _ref = source;
    }
    unique_ref(const unique_ref& other) {
        _ref = other._ref;
        other._ref = nullptr; // We take ownership
    }
    ~unique_ref() { clear(); }

    unique_ref& operator=(Type* other) {
        if (_ref == other)
            return *this;
        clear();
        _ref = other;
        return *this;
    }

    unique_ref& operator=(const unique_ref& other) {
        if (other._ref == _ref)
            return *this;
        clear();
        _ref = other._ref;
        other._ref = nullptr; // We take ownership
        return *this;
    }

    bool is_null() const { return _ref == nullptr; }
    void clear() {
        if (_ref)
            delete _ref;
        _ref = nullptr;
    }
    Type* operator->() const { return _ref; }
    operator Type*() const { return _ref; }
    Type* get() const { return _ref; }
};

#pragma endregion
