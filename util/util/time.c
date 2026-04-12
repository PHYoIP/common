/*
author          Oliver Blaser
date            19.03.2026
copyright       MIT - Copyright (c) 2026 Oliver Blaser
*/

#include <stdint.h>
#include <time.h>

#include "bits/compiler.h"
#include "time.h"

#if ___UTIL_PLAT_WIN
#include <Windows.h>
#endif

#if ___UTIL_PLAT_ZEPHYR
#include <zephyr/kernel.h>
#endif



#define LITERAL_1e6 (1000000)
#define LITERAL_1e9 (1000000000)

// number of 100ns intervals between 1601-01-01T00:00:00Z and 1970-01-01T00:00:00Z
#define FILETIME_UNIX_EPOCH_DIFF (116444736000000000ll)

// number of 100ns intervals in one second
#define SECOND_100ns (10000000ll)



int UTIL_sleep_ms(uint32_t t_ms)
{
#if ___UTIL_PLAT_WIN

    Sleep(t_ms);
    return 0;

#elif ___UTIL_PLAT_ZEPHYR

    if (t_ms > INT32_MAX) { return -(EINVAL); }
    if (0 != k_msleep((int32_t)t_ms)) { return -(EINTR); }
    return 0;

#else

    struct timespec ts;
    UTIL_ms_to_timespec(&ts, t_ms);
    return nanosleep(&ts, NULL);

#endif
}

int UTIL_sleep_us(uint32_t t_us)
{
#if ___UTIL_PLAT_WIN

    // clamp to 1ms
    if (t_us < 1000) { t_us = 1000; }

    // round at 3/4 (normal rounding would be done at 1/2)
    t_us += 250;
    const DWORD t_ms = (DWORD)(t_us / 1000);

    Sleep(t_ms);
    return 0;

#elif ___UTIL_PLAT_ZEPHYR

    if (t_us > INT32_MAX) { return -(EINVAL); }
    if (0 != k_usleep((int32_t)t_us)) { return -(EINTR); }
    return 0;

#else

    struct timespec ts;
    UTIL_us_to_timespec(&ts, t_us);
    return nanosleep(&ts, NULL);

#endif
}



void UTIL_ms_to_timespec(struct timespec* dst, uint64_t t_ms)
{
    dst->tv_sec = (time_t)(t_ms / 1000);
    dst->tv_nsec = (int32_t)((t_ms - ((uint64_t)(dst->tv_sec) * 1000)) * LITERAL_1e6);
}

void UTIL_us_to_timespec(struct timespec* dst, uint64_t t_us)
{
    dst->tv_sec = (time_t)(t_us / LITERAL_1e6);
    dst->tv_nsec = (int32_t)((t_us - ((uint64_t)(dst->tv_sec) * LITERAL_1e6)) * 1000);
}

void UTIL_ns_to_timespec(struct timespec* dst, uint64_t t_ns)
{
    dst->tv_sec = (time_t)(t_ns / LITERAL_1e9);
    dst->tv_nsec = (int32_t)(t_ns - ((uint64_t)(dst->tv_sec) * LITERAL_1e9));
}



#if ___UTIL_PLAT_WIN && ((___UTIL_STDC >= ___UTIL_STDC_11) || (___UTIL_STDCPP >= ___UTIL_STDCPP_17))

void UTIL_filetime_to_timespec(struct timespec* tspec, const void* filetime_FILETIME)
{
    const FILETIME* filetime = (const FILETIME*)filetime_FILETIME;

    int64_t tmp = filetime->dwHighDateTime;
    tmp <<= 32;
    tmp |= filetime->dwLowDateTime;

    tmp -= FILETIME_UNIX_EPOCH_DIFF;

    tspec->tv_sec = (time_t)(tmp / SECOND_100ns);
    tspec->tv_nsec = (int32_t)((tmp % SECOND_100ns) * 100ll);
}

void UTIL_timespec_to_filetime(void* filetime_FILETIME, const struct timespec* tspec)
{
    // maybe not absolutely correct (no range checks), but ok for reasonable timestamps

    FILETIME* filetime = (FILETIME*)filetime_FILETIME;

    uint64_t tmp = (uint64_t)((tspec->tv_sec * SECOND_100ns) + FILETIME_UNIX_EPOCH_DIFF);
    tmp += (uint64_t)(tspec->tv_nsec / 100);

    filetime->dwHighDateTime = (DWORD)(tmp >> 32);
    filetime->dwLowDateTime = (DWORD)(tmp & UINT32_MAX);
}

#endif

#if ___UTIL_PLAT_UNIX || (___UTIL_STDC >= ___UTIL_STDC_11) || (___UTIL_STDCPP >= ___UTIL_STDCPP_17)

int UTIL_getTime(struct timespec* tspec)
{
#if ___UTIL_PLAT_WIN
    FILETIME filetime;
    GetSystemTimeAsFileTime(&filetime);
    UTIL_filetime_to_timespec(tspec, &filetime);
    return 0;
#else
    return clock_gettime(CLOCK_REALTIME, tspec);
#endif
}

#endif
