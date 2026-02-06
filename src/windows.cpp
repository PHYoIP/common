/*
author          Oliver Blaser
date            01.02.2026
copyright       GPL-3.0 - Copyright (c) 2026 Oliver Blaser
*/

#include "common/windows.h"

#include <Windows.h>



void windows::enableVTProcessing()
{
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (handle != INVALID_HANDLE_VALUE)
    {
        DWORD mode = 0;
        if (GetConsoleMode(handle, &mode)) { SetConsoleMode(handle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING); }
    }
}

int windows::formatMessage(int err, char* buffer, size_t size)
{
    const DWORD n =
        FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, (DWORD)err,
                       MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), // using en-us because it's the fallback, so we can assume it's always available
                       buffer, (DWORD)size, NULL);

    if (n == 0) { return -1; }

    return 0;
}
