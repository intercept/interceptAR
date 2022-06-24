#pragma once

#include "interceptTypes.hpp"
import <span>;

export template <typename Type>
class ENF_Array {
public:
    Type* data;
    uint32_t bufferSize;
    uint32_t size;


    std::span<Type> AsSpan() const {
        return std::span(data, size);
    }
};
