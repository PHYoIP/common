/*
author          Oliver Blaser
date            09.11.2026
copyright       GPL-3.0 - Copyright (c) 2026 Oliver Blaser
*/

#ifndef IG_COMMON_STRING_H
#define IG_COMMON_STRING_H

#include <cstdint>
#include <string>
#include <vector>



namespace comm {



std::string& lower_ascii(std::string& str);
std::string& upper_ascii(std::string& str);
std::string toLower_ascii(const std::string& str);
std::string toUpper_ascii(const std::string& str);



constexpr char toHexStr_defaultDelimiter = 0x20;

std::string toHexStr(int8_t value);
std::string toHexStr(uint8_t value);
std::string toHexStr(int16_t value);
std::string toHexStr(uint16_t value);
std::string toHexStr(int32_t value);
std::string toHexStr(uint32_t value);
std::string toHexStr(int64_t value);
std::string toHexStr(uint64_t value);
std::string toHexStr(int16_t value, char delimiter);
std::string toHexStr(uint16_t value, char delimiter);
std::string toHexStr(int32_t value, char delimiter);
std::string toHexStr(uint32_t value, char delimiter);
std::string toHexStr(int64_t value, char delimiter);
std::string toHexStr(uint64_t value, char delimiter);
std::string toHexStr(const std::vector<char>& data, char delimiter = toHexStr_defaultDelimiter);
std::string toHexStr(const std::vector<uint8_t>& data, char delimiter = toHexStr_defaultDelimiter);
std::string toHexStr(const char* data, size_t count, char delimiter = toHexStr_defaultDelimiter);
std::string toHexStr(const uint8_t* data, size_t count, char delimiter = toHexStr_defaultDelimiter);

int32_t hexstoi(const std::string& str);
int64_t hexstoi64(const std::string& str);
uint32_t hexstoui(const std::string& str);
uint64_t hexstoui64(const std::string& str);
std::vector<uint8_t> hexstovector(const std::string& str, char delimiter = toHexStr_defaultDelimiter);



constexpr bool isBlank(char ch) { return ((ch == 0x09) || (ch == 0x20)); }
constexpr bool isCntrl(char ch) { return (((ch >= 0x00) && (ch <= 0x1F)) || (ch == 0x7F)); }
constexpr bool isDigit(char ch) { return ((ch >= 0x30) && (ch <= 0x39)); }
constexpr bool isGraph(char ch) { return ((ch >= 0x21) && (ch <= 0x7E)); }
constexpr bool isHex(char ch) { return (((ch >= 0x30) && (ch <= 0x39)) || ((ch >= 0x41) && (ch <= 0x46)) || ((ch >= 0x61) && (ch <= 0x66))); }
constexpr bool isLower(char ch) { return ((ch >= 0x61) && (ch <= 0x7A)); }
constexpr bool isNull(char ch) { return (ch == 0x00); }
constexpr bool isPrint(char ch) { return ((ch >= 0x20) && (ch <= 0x7E)); }
constexpr bool isPunct(char ch)
{
    return (((ch >= 0x21) && (ch <= 0x2F)) || ((ch >= 0x3A) && (ch <= 0x40)) || ((ch >= 0x5B) && (ch <= 0x60)) || ((ch >= 0x7B) && (ch <= 0x7E)));
}
constexpr bool isSpace(char ch) { return (((ch >= 0x09) && (ch <= 0x0D)) || (ch == 0x20)); }
constexpr bool isUpper(char ch) { return ((ch >= 0x41) && (ch <= 0x5A)); }
constexpr bool isAlpha(char ch) { return (isLower(ch) || isUpper(ch)); }
constexpr bool isAlnum(char ch) { return (isAlpha(ch) || isDigit(ch)); }



/**
 * See `omw::isUInteger()`.
 */
bool isInteger(const std::string& str);

/**
 * Ignores leading zeros.
 *
 * An empty string returns `false`.
 */
bool isUInteger(const std::string& str);

/**
 * An empty string returns `false`.
 */
bool isHex(const std::string& str, std::string::size_type pos = 0, std::string::size_type count = std::string::npos);



} // namespace comm



#endif // IG_COMMON_STRING_H
