/*
author          Oliver Blaser
date            07.02.2026
copyright       GPL-3.0 - Copyright (c) 2026 Oliver Blaser
*/

#include "common/socket.h"

#ifdef _WIN32

#include <WinSock2.h>

#include <Windows.h>

#else // _WIN32

#include <sys/socket.h>
#include <unistd.h>

#endif // _WIN32



#ifdef _WIN32

// the implementation assumes this:
static_assert((sockfd_t)INVALID_SOCKET < 0);
static_assert((int)SOCKET_ERROR < 0);

int sockclose(sockfd_t sockfd) { return closesocket(sockfd); }

#else // *nix

int sockclose(sockfd_t sockfd) { return close(sockfd); }

#endif // Windows / *nix
