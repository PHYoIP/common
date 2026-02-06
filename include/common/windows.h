/*
author          Oliver Blaser
date            01.02.2026
copyright       GPL-3.0 - Copyright (c) 2026 Oliver Blaser
*/

#ifndef IG_COMMON_WINDOWS_H
#define IG_COMMON_WINDOWS_H

#ifdef _WIN32



namespace windows {

void enableVTProcessing();

int formatMessage(int err, char* buffer, size_t size);

}



#endif // _WIN32

#endif // IG_COMMON_WINDOWS_H
