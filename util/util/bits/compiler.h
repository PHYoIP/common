/*
author          Oliver Blaser
date            12.04.2026
copyright       MIT - Copyright (c) 2026 Oliver Blaser
*/

#ifndef IG_UTIL_BITS_COMPILER_H
#define IG_UTIL_BITS_COMPILER_H

// #include


#ifdef __cplusplus
extern "C" {
#endif



#ifdef _WIN32
#define ___UTIL_PLAT_WIN (1)
#endif

#if (defined(__unix__) || defined(__unix))
#define ___UTIL_PLAT_UNIX (1)
#endif

#ifdef __linux__
#define ___UTIL_PLAT_LINUX (1)
#endif

#ifdef __ZEPHYR__
#define ___UTIL_PLAT_ZEPHYR (1)
#endif



#define ___UTIL_STDC __STDC_VERSION__

#ifdef _MSC_VER
#define ___UTIL_STDCPP (_MSVC_LANG)
#else
#define ___UTIL_STDCPP (__cplusplus)
#endif

// https://en.cppreference.com/w/c/preprocessor/replace.html
// https://en.cppreference.com/w/cpp/preprocessor/replace.html
// https://learn.microsoft.com/en-us/cpp/preprocessor/predefined-macros
#define ___UTIL_STDC_11   201112L
#define ___UTIL_STDC_17   201710L
#define ___UTIL_STDCPP_98 199711L
#define ___UTIL_STDCPP_11 201103L
#define ___UTIL_STDCPP_14 201402L
#define ___UTIL_STDCPP_17 201703L
#define ___UTIL_STDCPP_20 202002L
#define ___UTIL_STDCPP_23 202302L



#ifdef __cplusplus
}
#endif

#endif // IG_UTIL_BITS_COMPILER_H
