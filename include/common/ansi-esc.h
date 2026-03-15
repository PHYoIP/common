/*
author          Oliver Blaser
date            15.03.2026
copyright       GPL-3.0 - Copyright (c) 2026 Oliver Blaser
*/

#ifndef IG_COMMON_ANSIESC_H
#define IG_COMMON_ANSIESC_H



enum ANSI_ESC_INDEX
{
    CSI_EL = 0, // erase line

    SGR_RESET,

    SGR_FG_BLACK,
    SGR_FG_RED,
    SGR_FG_GREEN,
    SGR_FG_YELLOW,
    SGR_FG_BLUE,
    SGR_FG_MAGENTA,
    SGR_FG_CYAN,
    SGR_FG_WHITE,

    SGR_FG_DEFAULT,

    SGR_BG_BLACK,
    SGR_BG_RED,
    SGR_BG_GREEN,
    SGR_BG_YELLOW,
    SGR_BG_BLUE,
    SGR_BG_MAGENTA,
    SGR_BG_CYAN,
    SGR_BG_WHITE,

    SGR_BG_DEFAULT,

    SGR_FG_BBLACK,
    SGR_FG_BRED,
    SGR_FG_BGREEN,
    SGR_FG_BYELLOW,
    SGR_FG_BBLUE,
    SGR_FG_BMAGENTA,
    SGR_FG_BCYAN,
    SGR_FG_BWHITE,

    SGR_BG_BBLACK,
    SGR_BG_BRED,
    SGR_BG_BGREEN,
    SGR_BG_BYELLOW,
    SGR_BG_BBLUE,
    SGR_BG_BMAGENTA,
    SGR_BG_BCYAN,
    SGR_BG_BWHITE,

    ANSI_ESC__end_,
};



namespace ansi {

extern const char* const * esc;

void initEscapeCodes(bool enable);

bool escEnabled();

}


#endif // IG_COMMON_ANSIESC_H
