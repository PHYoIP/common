/*
author          Oliver Blaser
date            01.02.2026
copyright       GPL-3.0 - Copyright (c) 2026 Oliver Blaser
*/

#include <stddef.h>
#include <stdint.h>

#include "common/ansi-esc.h"
#include "util/macros.h"



static const char* const escCodes[] = {
    "\033[2K",
    //
    "\033[0m",
    //
    "\033[30m",
    "\033[31m",
    "\033[32m",
    "\033[33m",
    "\033[34m",
    "\033[35m",
    "\033[36m",
    "\033[37m",
    //
    "\033[39m",
    //
    "\033[40m",
    "\033[41m",
    "\033[42m",
    "\033[43m",
    "\033[44m",
    "\033[45m",
    "\033[46m",
    "\033[47m",
    //
    "\033[49m",
    //
    "\033[90m",
    "\033[91m",
    "\033[92m",
    "\033[93m",
    "\033[94m",
    "\033[95m",
    "\033[96m",
    "\033[97m",
    //
    "\033[100m",
    "\033[101m",
    "\033[102m",
    "\033[103m",
    "\033[104m",
    "\033[105m",
    "\033[106m",
    "\033[107m",
};
static_assert(SIZEOF_ARRAY(escCodes) == ANSI_ESC__end_);

static const char* escNull[SIZEOF_ARRAY(escCodes)];
static_assert(SIZEOF_ARRAY(escNull) == ANSI_ESC__end_);



static void initEscNull();



const char* const * ansi::esc;

void ansi::initEscapeCodes(bool enable)
{
    if (enable) { ansi::esc = escCodes; }
    else
    {
        initEscNull();
        ansi::esc = escNull;
    }
}



void initEscNull()
{
    for (size_t i = 0; i < SIZEOF_ARRAY(escNull); ++i) { escNull[i] = ""; }
}
