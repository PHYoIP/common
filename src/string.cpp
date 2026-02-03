/*
author          Oliver Blaser
date            03.02.2026
copyright       GPL-3.0 - Copyright (c) 2026 Oliver Blaser
*/

#include <cstdint>
#include <stdexcept>
#include <string>

#include "common/string.h"



static const char* const hexDigitsLower = "0123456789abcdef";



template <typename T> T hexstointeger(const std::string& str, const std::string& fnName)
{
    const std::string exMsg_notHex(fnName + ": not a hex string");
    const std::string hexDigits(hexDigitsLower);

    const std::string hexStr = comm::toLower_ascii(str);
    const std::string::size_type hexStrLen = hexStr.length();
    const std::string::size_type maxLen = 2 * sizeof(T);

    T r = 0;

    if ((hexStrLen <= maxLen) && (hexStrLen > 0))
    {
        for (std::string::size_type i = 0; i < hexStrLen; ++i)
        {
            const std::string::size_type digitIdx = hexDigits.find(hexStr[hexStrLen - i - 1]);

            if (digitIdx == std::string::npos) { throw std::invalid_argument(exMsg_notHex); }

            T digitValue = (T)digitIdx;
            digitValue <<= (4 * (T)i);
            r |= digitValue;
        }
    }
    else
    {
        if (!comm::isHex(hexStr)) { throw std::invalid_argument(exMsg_notHex); }
        throw std::out_of_range(fnName);
    }

    return r;
}



std::string& comm::lower_ascii(std::string& str)
{
    for (std::string::size_type i = 0; i < str.length(); ++i)
    {
        auto& c = str.at(i);

        if ((c >= 'A') && (c <= 'Z')) { c += 32; }
    }

    return str;
}

std::string& comm::upper_ascii(std::string& str)
{
    for (std::string::size_type i = 0; i < str.length(); ++i)
    {
        auto& c = str.at(i);

        if ((c >= 'a') && (c <= 'z')) c -= 32;
    }

    return str;
}

std::string comm::toLower_ascii(const std::string& str)
{
    std::string res = str;
    return comm::lower_ascii(res);
}

std::string comm::toUpper_ascii(const std::string& str)
{
    std::string res = str;
    return comm::upper_ascii(res);
}



int32_t comm::hexstoi(const std::string& str) { return hexstointeger<int32_t>(str, "hexstoi"); }



bool comm::isInteger(const std::string& str)
{
    std::string::size_type startPos = 0;

    if (str.length() > 1)
    {
        if (str[0] == '-') { startPos = 1; }
    }

    return comm::isUInteger(std::string(str, startPos));
}

bool comm::isUInteger(const std::string& str)
{
    bool r;

    if (str.length() > 0)
    {
        r = true;
        for (std::string::size_type i = 0; (i < str.length()) && r; ++i)
        {
            if ((str[i] < '0') || (str[i] > '9')) { r = false; }
        }
    }
    else r = false;

    return r;
}

bool comm::isHex(const std::string& str, std::string::size_type pos, std::string::size_type count)
{
    bool r = false;

    if ((pos < str.length()) && (count > 0))
    {
        r = true;
        for (std::string::size_type i = 0; (i < count) && ((pos + i) < str.length()); ++i)
        {
            const char& c = str[pos + i];

            if (!( // clang-format off
                ((c >= '0') && (c <= '9')) ||
                ((c >= 'A') && (c <= 'F')) ||
                ((c >= 'a') && (c <= 'f'))
                )) // clang-format on
            {
                r = false;
                break;
            }
        }
    }

    return r;
}
