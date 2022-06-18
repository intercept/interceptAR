#pragma once


export template<typename EnumType>
class BitflagEnum {
    using BaseT = std::underlying_type_t<EnumType>;
    EnumType data = 0;

 public:
    BitflagEnum() {}
    BitflagEnum(EnumType data) : data(data) {}

    bool isSet(EnumType flag) const { return ((BaseT)data & (BaseT)flag) != 0; }
    void set(EnumType flag) { data |= flag; }
    void unset(EnumType flag) { data &= ~flag; }

    void operator|=(EnumType other) { data |= other; }
    void operator&=(EnumType other) { data &= other; }
    EnumType operator|(EnumType other) const { return data | other; }
    EnumType operator&(EnumType other) const { return data & other; }

    operator EnumType() const { return data; }
};
