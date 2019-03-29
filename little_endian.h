#pragma once
#include<type_traits>
#include<cinttypes>

#ifdef _MSC_VER
#   define NOMINMAX
#   include <WinSock2.h>
#elif __GNUC__
#   include <arpa/inet.h>
#   include <byteswap.h>
#else

#endif

std::uint16_t ToNetwork2(std::uint16_t x)
{
    return htons(x);
}

std::uint32_t ToNetwork4(std::uint32_t x)
{
    return htonl(x);
}

std::uint64_t ToNetwork8(std::uint64_t x)
{
#ifdef __GNUC__
    return bswap_64(x);
#else
    return htonll(x);
#endif // __GNUC__
}

template<class Ty>
Ty ConvertToLittleEndianForSure(const Ty& x);

template<>
std::uint8_t ConvertToLittleEndianForSure<std::uint8_t>(const std::uint8_t& x)
{
    return x;
}

template<>
std::uint16_t ConvertToLittleEndianForSure<std::uint16_t>(const std::uint16_t& x)
{
    auto l = ToNetwork2(x);
    return  ((l >> 8) & ((std::uint16_t)0x00FF)) |
            ((l << 8) & ((std::uint16_t)0xFF00));
}

template<>
std::uint32_t ConvertToLittleEndianForSure<std::uint32_t>(const std::uint32_t& x)
{
    auto l = ToNetwork4(x);
    return  ((l >> 24) & ((std::uint32_t)0x000000FFL)) |
            ((l >>  8) & ((std::uint32_t)0x0000FF00L)) |
            ((l <<  8) & ((std::uint32_t)0x00FF0000L)) |
            ((l << 24) & ((std::uint32_t)0xFF000000L));
}

template<>
std::uint64_t ConvertToLittleEndianForSure<std::uint64_t>(const std::uint64_t& x)
{
    auto l = ToNetwork8(x);
    return  ((l >> 56) & ((std::uint64_t)0x00000000000000FFLL)) |
            ((l >> 40) & ((std::uint64_t)0x000000000000FF00LL)) |
            ((l >> 24) & ((std::uint64_t)0x0000000000FF0000LL)) |
            ((l >>  8) & ((std::uint64_t)0x00000000FF000000LL)) |
            ((l <<  8) & ((std::uint64_t)0x000000FF00000000LL)) |
            ((l << 24) & ((std::uint64_t)0x0000FF0000000000LL)) |
            ((l << 40) & ((std::uint64_t)0x00FF000000000000LL)) |
            ((l << 56) & ((std::uint64_t)0xFF00000000000000LL));
}

template<class Ty>
Ty ConvertToLittleEndian(Ty x)
{
    static const union {
        std::uint32_t int_value;
        unsigned char char_value[4];
    } Endianness = { 0xAABBCCDD };

    if (Endianness.char_value[0] != 0xDD ||
        Endianness.char_value[1] != 0xCC ||
        Endianness.char_value[2] != 0xBB ||
        Endianness.char_value[3] != 0xAA)
    {
        return ConvertToLittleEndianForSure(x);
    }else // hopefully a proper little endian machine
        return x;
}
