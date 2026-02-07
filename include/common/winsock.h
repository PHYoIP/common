/*
author          Oliver Blaser
date            06.02.2026
copyright       GPL-3.0 - Copyright (c) 2026 Oliver Blaser
*/

#ifndef IG_COMMON_WINSOCK_H
#define IG_COMMON_WINSOCK_H

#ifdef _WIN32

#include <cstddef>
#include <cstdint>



namespace winsock {

int init();
int cleanup();

}



#endif // _WIN32

#endif // IG_COMMON_WINSOCK_H
