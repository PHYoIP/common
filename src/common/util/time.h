/*
author          Oliver Blaser
date            01.03.2025
copyright       MIT - Copyright (c) 2025 Oliver Blaser
*/

#ifndef IG_UTIL_TIME_H
#define IG_UTIL_TIME_H

#include <stdint.h>
#include <time.h>


#ifdef __cplusplus
extern "C" {
#endif


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


#ifdef __cplusplus
}
#endif

#endif // IG_UTIL_TIME_H
