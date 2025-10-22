#pragma once

#include <type_traits>
#include <cstdint>

namespace sz_utils
{
    template <typename T>
    struct enable_bitmask_operators
    {
        static constexpr bool enable = false;
    };
    
    template <typename T>
    typename std::enable_if<enable_bitmask_operators<T>::enable, T>::type
        operator|(T lhs, T rhs) noexcept
    {
        using underlying = typename std::underlying_type<T>::type;
        return static_cast<T>(static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
    }

    template <typename T>
    typename std::enable_if<enable_bitmask_operators<T>::enable, T>::type
        operator&(T lhs, T rhs) noexcept
    {
        using underlying = typename std::underlying_type<T>::type;
        return static_cast<T>(static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
    }

    template <typename T>
    typename std::enable_if<enable_bitmask_operators<T>::enable, T>::type
        operator~(T lhs) noexcept
    {
        using underlying = typename std::underlying_type<T>::type;
        return static_cast<T>(~static_cast<underlying>(lhs));
    }

    template <typename T>
    typename std::enable_if<enable_bitmask_operators<T>::enable, T&>::type
        operator|=(T& lhs, T rhs) noexcept
    {
        lhs = lhs | rhs;
        return lhs;
    }

    template <typename T>
    typename std::enable_if<enable_bitmask_operators<T>::enable, T&>::type
        operator&=(T& lhs, T rhs) noexcept
    {
        lhs = lhs & rhs;
        return lhs;
    }

    template <typename T>
    constexpr bool HasFlag(T value, T flag_to_check) noexcept
    {
        // �ؼ��߼�����ö��ֵת��Ϊ�ײ����ͣ����а�λ���������������Ƿ���㡣
        using underlying = typename std::underlying_type<T>::type;
        return (static_cast<underlying>(value) & static_cast<underlying>(flag_to_check)) != 0;
    }
}

#define ENABLE_BITMASK_OPERATORS(EnumType) \
namespace sz_utils { \
    template <> \
    struct enable_bitmask_operators<EnumType> \
    { \
        static constexpr bool enable = true; \
    }; \
}

#define USING_BITMASK_OPERATORS() \
using sz_utils::operator|; \
using sz_utils::operator&; \
using sz_utils::operator~; \
using sz_utils::operator|=; \
using sz_utils::operator&=; \
using sz_utils::HasFlag;