/*
author          Oliver Blaser
date            01.02.2026
copyright       GPL-3.0 - Copyright (c) 2026 Oliver Blaser
*/

#ifndef IG_COMMON_LOG_H
#define IG_COMMON_LOG_H

#include <stdio.h>

#include "common/ansi-esc.h"



#define ___LOG_OUTSTREAM stderr



#define LOG_LEVEL_OFF (0)
#define LOG_LEVEL_ERR (1)
#define LOG_LEVEL_WRN (2)
#define LOG_LEVEL_INF (3)
#define LOG_LEVEL_DBG (4)



// stringify
#define ___LOG_STR_HELPER(x) #x
#define ___LOG_STR(x)        ___LOG_STR_HELPER(x)

// optional args
#define ___LOG_OPT_VA_ARGS(...) , ##__VA_ARGS__



#ifndef CONFIG_LOG_MAX_LEVEL
#define CONFIG_LOG_MAX_LEVEL LOG_LEVEL_INF
#endif

#ifndef COMMON_LOG_DEFINE_FUNCTIONS
#ifndef LOG_MODULE_LEVEL
#error "define LOG_MODULE_LEVEL before including log.h"
#endif
#ifndef LOG_MODULE_NAME
#error "define LOG_MODULE_NAME before including log.h"
#endif
#endif // COMMON_LOG_DEFINE_FUNCTIONS

#ifndef LOG_TIMESTAMP_STR

#ifdef __cplusplus
#define LOG_TIMESTAMP_STR LOG_tNow_local_iso8601().c_str()
#else
#define LOG_TIMESTAMP_STR LOG_tNow_local_iso8601()
#endif

#endif // LOG_TIMESTAMP_STR



#define LOG_COL_DEFAULT SGR_FG_DEFAULT
#define LOG_COL_ERR     SGR_FG_BRED
#define LOG_COL_WRN     SGR_FG_BYELLOW
#define LOG_COL_INF     LOG_COL_DEFAULT
#define LOG_COL_DBG     SGR_FG_BBLACK


// clang-format off
#define LOG_ERR(msg, ...) fprintf(___LOG_OUTSTREAM, "%s" "[%s] " "%s" ___LOG_STR(LOG_MODULE_NAME) " <ERR> " msg "%s" "\n", ansi::esc[CSI_EL], LOG_TIMESTAMP_STR, ansi::esc[LOG_COL_ERR] ___LOG_OPT_VA_ARGS(__VA_ARGS__), ansi::esc[SGR_RESET])
#define LOG_WRN(msg, ...) fprintf(___LOG_OUTSTREAM, "%s" "[%s] " "%s" ___LOG_STR(LOG_MODULE_NAME) " <WRN> " msg "%s" "\n", ansi::esc[CSI_EL], LOG_TIMESTAMP_STR, ansi::esc[LOG_COL_WRN] ___LOG_OPT_VA_ARGS(__VA_ARGS__), ansi::esc[SGR_RESET])
#define LOG_INF(msg, ...) fprintf(___LOG_OUTSTREAM, "%s" "[%s] " "%s" ___LOG_STR(LOG_MODULE_NAME) " <INF> " msg "%s" "\n", ansi::esc[CSI_EL], LOG_TIMESTAMP_STR, ansi::esc[LOG_COL_INF] ___LOG_OPT_VA_ARGS(__VA_ARGS__), ansi::esc[SGR_RESET])
#define LOG_DBG(msg, ...) fprintf(___LOG_OUTSTREAM, "%s" "[%s] " "%s" ___LOG_STR(LOG_MODULE_NAME) " <DBG> " "%s" "%s():%i" "%s" " " msg "%s" "\n", ansi::esc[CSI_EL], LOG_TIMESTAMP_STR, ansi::esc[LOG_COL_DEFAULT], ansi::esc[LOG_COL_DBG], __func__, (int)(__LINE__), ansi::esc[LOG_COL_DEFAULT] ___LOG_OPT_VA_ARGS(__VA_ARGS__), ansi::esc[SGR_RESET])
// clang-format on



#define LOG_LEVEL_IS_ENABLED(_level) ((LOG_MODULE_LEVEL >= (_level)) && (CONFIG_LOG_MAX_LEVEL >= (_level)))



#if !LOG_LEVEL_IS_ENABLED(LOG_LEVEL_DBG)
#undef LOG_DBG
#define LOG_DBG(...) (void)0
#endif

#if !LOG_LEVEL_IS_ENABLED(LOG_LEVEL_INF)
#undef LOG_INF
#define LOG_INF(...) (void)0
#endif

#if !LOG_LEVEL_IS_ENABLED(LOG_LEVEL_WRN)
#undef LOG_WRN
#define LOG_WRN(...) (void)0
#endif

#if !LOG_LEVEL_IS_ENABLED(LOG_LEVEL_ERR)
#undef LOG_ERR
#define LOG_ERR(...) (void)0
#endif



#define LOG_DBG_HD(data, count, msg) \
    {                                \
        LOG_DBG(msg);                \
        ___LOG_hexDump(data, count); \
    }

#define LOG_INF_HD(data, count, msg) \
    {                                \
        LOG_INF(msg);                \
        ___LOG_hexDump(data, count); \
    }

#define LOG_WRN_HD(data, count, msg) \
    {                                \
        LOG_WRN(msg);                \
        ___LOG_hexDump(data, count); \
    }

#define LOG_ERR_HD(data, count, msg) \
    {                                \
        LOG_ERR(msg);                \
        ___LOG_hexDump(data, count); \
    }

#if !LOG_LEVEL_IS_ENABLED(LOG_LEVEL_DBG)
#undef LOG_DBG_HD
#define LOG_DBG_HD(...) (void)0
#endif

#if !LOG_LEVEL_IS_ENABLED(LOG_LEVEL_INF)
#undef LOG_INF_HD
#define LOG_INF_HD(...) (void)0
#endif

#if !LOG_LEVEL_IS_ENABLED(LOG_LEVEL_WRN)
#undef LOG_WRN_HD
#define LOG_WRN_HD(...) (void)0
#endif

#if !LOG_LEVEL_IS_ENABLED(LOG_LEVEL_ERR)
#undef LOG_ERR_HD
#define LOG_ERR_HD(...) (void)0
#endif



#include <string.h>

#define LOG_ERR_ERRNO(msg, eno) LOG_ERR(msg " (%i %s)", eno, strerror(eno))

#if !LOG_LEVEL_IS_ENABLED(LOG_LEVEL_ERR)
#undef LOG_ERR_ERRNO
#define LOG_ERR_ERRNO(...) (void)0
#endif



#ifdef _WIN32

#include "common/windows.h"

#define LOG_ERR_WSA(msg, err)                                                                    \
    {                                                                                            \
        char buffer[1024];                                                                       \
        buffer[0] = 0x20;                                                                        \
        if (0 != windows::formatMessage(err, buffer + 1, sizeof(buffer) - 1)) { buffer[0] = 0; } \
        LOG_ERR(msg " (%i%s)", err, buffer);                                                     \
    }


#if !LOG_LEVEL_IS_ENABLED(LOG_LEVEL_ERR)
#undef LOG_ERR_WSA
#define LOG_ERR_WSA(...) (void)0
#endif

#endif // _WIN32



#ifdef __cplusplus
#include <string>
std::string LOG_tNow_local_iso8601();
#else  // __cplusplus
const char* LOG_tNow_local_iso8601();
#endif // __cplusplus

void ___LOG_hexDump(const uint8_t* data, size_t count);



#endif // IG_COMMON_LOG_H



// #####################################################################################################################
// #####################################################################################################################
// #####################################################################################################################



#ifdef COMMON_LOG_DEFINE_FUNCTIONS
#undef COMMON_LOG_DEFINE_FUNCTIONS

#include <ctype.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#ifdef __cplusplus
#include <string>
#endif


static inline void ___LOG_t_local_iso8601(char* buffer, size_t size, time_t t)
{
#ifdef _MSC_VER
#pragma warning(suppress :4996)
    const struct tm* tm = localtime(&t);
#else
    const struct tm* tm = localtime(&t);
#endif

    if (!(tm && (strftime(buffer, size, "%FT%T", tm) > 0)))
    {
#ifdef _MSC_VER
#pragma warning(suppress :4996)
        sprintf(buffer, "%" PRIi64, (int64_t)t);
#else
        sprintf(buffer, "%" PRIi64, (int64_t)t);
#endif
    }
}


#ifdef __cplusplus

std::string LOG_tNow_local_iso8601()
{
    char buffer[25];
    ___LOG_t_local_iso8601(buffer, sizeof(buffer), time(NULL));
    return buffer;
}

#else // __cplusplus

const char* LOG_tNow_local_iso8601()
{
#warning "LOG_tNow_local_iso8601() is not thread safe"
    static char buffer[25];
    ___LOG_t_local_iso8601(buffer, sizeof(buffer), time(NULL));
    return buffer;
}

#endif // __cplusplus



/**
 * @brief Converts the data buffer bytes to printable characters.
 *
 * Writes 16 characters and a terminating null to `buffer`.
 *
 * @param buffer Destination string buffer
 * @param p First element to parse, points into the source data buffer
 * @param end First element after the source data buffer
 */
static void hexDump_dataToString(char* buffer, const uint8_t* p, const uint8_t* end)
{
    size_t i = 0;

    while ((i < 16) && (p < end))
    {
        if (isprint(*p)) { buffer[i] = (char)(*p); }
        else { buffer[i] = '.'; }

        ++p;
        ++i;
    }

    while (i < 16)
    {
        buffer[i] = ' ';
        ++i;
    }

    buffer[i] = 0;
}

void ___LOG_hexDump(const uint8_t* data, size_t count)
{
    if (!data) { count = 0; }

    const uint8_t* const end = (data + count);

    for (size_t i = 0; i < count; ++i)
    {
        const int byte = *(data + i);
        const size_t row = (i / 16);
        const size_t col = (i % 16);

        if (col == 0)
        {
            if (i == 0) { fprintf(___LOG_OUTSTREAM, "%05zx ", i); }
            else
            {
                char str[17];
                hexDump_dataToString(str, data + 16 * (row - 1), end);
                fprintf(___LOG_OUTSTREAM, "  | %s\n%05zx ", str, i);
            }
        }
        else if (col == 8) { fprintf(___LOG_OUTSTREAM, " "); }

        fprintf(___LOG_OUTSTREAM, " %02x", byte);
    }

    if (count == 0) { fprintf(___LOG_OUTSTREAM, "%05x ", 0); }



    size_t lastRowSize = (count % 16);
    if ((lastRowSize == 0) && (count != 0)) { lastRowSize = 16; }
    const size_t remaining = (16 - lastRowSize);

    if (remaining >= 8) { fprintf(___LOG_OUTSTREAM, " "); }
    for (size_t i = 0; i < remaining; ++i) { fprintf(___LOG_OUTSTREAM, "   "); }

    char str[17];
    hexDump_dataToString(str, end - lastRowSize, end);
    fprintf(___LOG_OUTSTREAM, "  | %s", str);

    fprintf(___LOG_OUTSTREAM, "\n");
}



#endif // COMMON_LOG_DEFINE_FUNCTIONS
