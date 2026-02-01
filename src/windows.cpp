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
