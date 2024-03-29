#ifndef COMMON_H
#define COMMON_H
#include <cstdint>
#include "../WindowsLib/WindowsUi.h"

using u64 = std::uint64_t;
using s64 = std::int64_t;
using u32 = std::uint32_t;
using s32 = std::int32_t;
using u16 = std::uint16_t;
using s16 = std::int16_t;
using u8 = std::uint8_t;
using s8 = std::int8_t;

static constexpr int DefaultDpi = 96;

inline int DpiScale(int coordinate, int dpi)
{
    return MulDiv(coordinate, dpi, DefaultDpi);
}

#endif