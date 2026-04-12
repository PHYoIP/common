/*
author          Oliver Blaser
date            27.03.2026
copyright       MIT - Copyright (c) 2026 Oliver Blaser
*/

#ifndef IG_UTIL_TIME_H
#define IG_UTIL_TIME_H

#include <stdint.h>
#include <time.h>

#include "bits/compiler.h"


#ifdef __cplusplus
extern "C" {
#endif

/*! \addtogroup grp_timeLib
 * @{
 */



#define UTIL_SECOND_s (1)
#define UTIL_MINUTE_s (60 * UTIL_SECOND_s)
#define UTIL_HOUR_s   (60 * UTIL_MINUTE_s)
#define UTIL_DAY_s    (24 * UTIL_HOUR_s)

#define UTIL_SECOND_ms (UTIL_SECOND_s * 1000)
#define UTIL_MINUTE_ms (UTIL_MINUTE_s * 1000)
#define UTIL_HOUR_ms   (UTIL_HOUR_s * 1000)
#define UTIL_DAY_ms    (UTIL_DAY_s * 1000)

#define UTIL_SECOND_us (UTIL_SECOND_ms * 1000)
#define UTIL_MINUTE_us (UTIL_MINUTE_ms * 1000)
#define UTIL_HOUR_us   (UTIL_HOUR_ms * 1000ll)
#define UTIL_DAY_us    (UTIL_DAY_ms * 1000ll)

#if !CONFIG_UTIL_TIME_NO_GLOBAL_LITERALS

#define SECOND_s UTIL_SECOND_s
#define MINUTE_s UTIL_MINUTE_s
#define HOUR_s   UTIL_HOUR_s
#define DAY_s    UTIL_DAY_s

#define SECOND_ms UTIL_SECOND_ms
#define MINUTE_ms UTIL_MINUTE_ms
#define HOUR_ms   UTIL_HOUR_ms
#define DAY_ms    UTIL_DAY_ms

#define SECOND_us UTIL_SECOND_us
#define MINUTE_us UTIL_MINUTE_us
#define HOUR_us   UTIL_HOUR_us
#define DAY_us    UTIL_DAY_us

#endif // CONFIG_UTIL_TIME_NO_GLOBAL_LITERALS



/**
 * @param t_ms Time to sleep as milliseconds
 * @return `nanosleep()`, 0 on Windows
 */
int UTIL_sleep_ms(uint32_t t_ms);

/**
 * On Windows the value is clamped to a minimum of 1ms. The value is rounded to a multiple of 1ms in such a way that
 * speed is more important than accurate or binary rounding. Hundreds `[0; 750)` are rounded down and `[750; 1000)` up.
 *
 * @param t_us Time to sleep as microseconds
 * @return `nanosleep()`, 0 on Windows
 */
int UTIL_sleep_us(uint32_t t_us);



void UTIL_ms_to_timespec(struct timespec* dst, uint64_t t_ms);
void UTIL_us_to_timespec(struct timespec* dst, uint64_t t_us);
void UTIL_ns_to_timespec(struct timespec* dst, uint64_t t_ns);



#if ___UTIL_PLAT_WIN && ((___UTIL_STDC >= ___UTIL_STDC_11) || (___UTIL_STDCPP >= ___UTIL_STDCPP_17))
void UTIL_filetime_to_timespec(struct timespec* tspec, const void* filetime_FILETIME);
void UTIL_timespec_to_filetime(void* filetime_FILETIME, const struct timespec* tspec);
#endif

#if ___UTIL_PLAT_UNIX || (___UTIL_STDC >= ___UTIL_STDC_11) || (___UTIL_STDCPP >= ___UTIL_STDCPP_17)

/**
 * @brief Get current system time.
 *
 * Uses `GetSystemTimeAsFileTime()` on Windows, otherwise `clock_gettime(CLOCK_REALTIME, tspec)`.
 *
 * @param [out] tspec
 * @return `clock_gettime()`, 0 on Windows
 */
int UTIL_getTime(struct timespec* tspec);

#endif



/*! @} */

#ifdef __cplusplus
}
#endif

#endif // IG_UTIL_TIME_H
