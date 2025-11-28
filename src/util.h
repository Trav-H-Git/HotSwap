#include <type_traits>
#include <concepts>
#include <cstddef>

namespace Util
{
    template <typename T>
    concept Integral = std::is_integral_v<T>;

    template <Integral T>
    constexpr bool IsPowerOf2(const T& value)
    {
        using UnsignedT = std::make_unsigned_t<T>;
        UnsignedT v = static_cast<UnsignedT>(value);
        return v != 0 && (v & (v - 1)) == 0;
    }

    template <Integral T>
    constexpr T NextPowerOf2(const T& value)
    {
        using UnsignedT = std::make_unsigned_t<T>;
        UnsignedT v = static_cast<UnsignedT>(value);

        if (v == 0)
            return static_cast<T>(1);

        if (IsPowerOf2(value))
            return static_cast<T>(value * 2);

        v--;
        if constexpr (sizeof(UnsignedT) >= 8) v |= v >> 32;
        if constexpr (sizeof(UnsignedT) >= 4) v |= v >> 16;
        if constexpr (sizeof(UnsignedT) >= 2) v |= v >> 8;
        if constexpr (sizeof(UnsignedT) >= 1) v |= v >> 4;
        v |= v >> 2;
        v |= v >> 1;
        v++;
    }
}