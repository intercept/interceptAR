#pragma once

//#include "interceptTypes.hpp"
//#include <span>

export template <typename Type>
class ENF_Array {
public:
    Type* data;
    uint32_t bufferSize;
    uint32_t size;


    auto AsSpan() const {
        return std::span(data, size);
    }
};
