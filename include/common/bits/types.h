/*
author          Oliver Blaser
date            11.04.2026
copyright       GPL-3.0 - Copyright (c) 2026 Oliver Blaser
*/

#ifndef IG_COMMON_BITS_SOCKET_H
#define IG_COMMON_BITS_SOCKET_H

#include <cstddef>
#include <cstdint>



#ifdef _WIN32

#ifdef _WIN64
#define SSIZE_MAX INT64_MAX
typedef int64_t ssize_t;
#else
#define SSIZE_MAX INT32_MAX
typedef int32_t ssize_t;
#endif

#endif // _WIN32



#endif // IG_COMMON_BITS_SOCKET_H
