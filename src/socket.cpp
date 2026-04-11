/*
author          Oliver Blaser
date            10.03.2026
copyright       GPL-3.0 - Copyright (c) 2026 Oliver Blaser
*/

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>


#include "common/socket.h"

#ifdef _WIN32

#include <sys/types.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#include <Windows.h>

#else // _WIN32

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#endif // _WIN32



#ifdef _MSC_VER
#pragma warning(disable :4996)
#endif



#define SWITCH_CASE_STRCPY_DEFINE(_dst, _define, _off)    \
    case _define:                                         \
        strcpy((_dst), (const char*)(#_define) + (_off)); \
        break



#ifdef _WIN32
static_assert((sockfd_t)INVALID_SOCKET < 0, "the implementation assumes this");
static_assert((int)SOCKET_ERROR < 0, "the implementation assumes this");
#endif



int sockclose(sockfd_t sockfd)
{
#ifdef _WIN32
    return closesocket(sockfd);
#else // *nix

    return close(sockfd);

#endif // Windows / *nix
}



char* aftos(int af, char* dst, size_t size)
{
    char* r = NULL;

    if (dst && (size >= AF_STRLEN))
    {
        r = dst;

        switch (af)
        {
            SWITCH_CASE_STRCPY_DEFINE(dst, AF_UNSPEC, 3);
            SWITCH_CASE_STRCPY_DEFINE(dst, AF_UNIX, 3);
            SWITCH_CASE_STRCPY_DEFINE(dst, AF_INET, 3);
            SWITCH_CASE_STRCPY_DEFINE(dst, AF_IPX, 3);
            SWITCH_CASE_STRCPY_DEFINE(dst, AF_INET6, 3);
#ifndef _WIN32
#if (AF_UNIX != AF_LOCAL)
            SWITCH_CASE_STRCPY_DEFINE(dst, AF_LOCAL, 3);
#endif
            SWITCH_CASE_STRCPY_DEFINE(dst, AF_PACKET, 3);
            SWITCH_CASE_STRCPY_DEFINE(dst, AF_X25, 3);
            SWITCH_CASE_STRCPY_DEFINE(dst, AF_AX25, 3);
#endif // _WIN32

        default:
        {
            static_assert(AF_STRLEN >= 7, "increase AF_STRLEN");
            static_assert(AF_MAX < 100, "increase the compare value in the AF_STRLEN assertion");

            const size_t res = (size_t)snprintf(dst, size, "AF_#%i", af);
            if (res >= size) { r = NULL; }
        }
        break;
        }
    }

    return r;
}

static char* sockaddrtos_inet(const struct sockaddr_in* sa, char* dst, size_t size)
{
    const in_port_t port = sa->sin_port;

    char* r = (char*)inet_ntop(AF_INET, &(sa->sin_addr), dst, size);

    if (r && port)
    {
        static_assert(SOCKADDRSTRLEN >= (INET_ADDRSTRLEN + 1 + 5), "increase SOCKADDRSTRLEN");

        char portStr[7];
        const size_t res = (size_t)snprintf(portStr, sizeof(portStr), ":%i", (int)port);
        if (res >= size) { r = NULL; }
        else { strcat(r, portStr); }
    }

    return r;
}

static char* sockaddrtos_inet6(const struct sockaddr_in6* sa, char* dst, size_t size)
{
    char* r = NULL;
    const in_port_t port = sa->sin6_port;

    char addrStrBuffer[INET6_ADDRSTRLEN];
    const char* addrStr = inet_ntop(AF_INET6, &(sa->sin6_addr), addrStrBuffer, sizeof(addrStrBuffer));

    if (addrStr)
    {
        r = dst;

        static_assert(SOCKADDRSTRLEN >= (INET6_ADDRSTRLEN + 3 + 5), "increase SOCKADDRSTRLEN");

        if (port == 0) { strcpy(dst, addrStr); }
        else
        {
            const size_t res = (size_t)snprintf(dst, size, "[%s]:%i", addrStr, (int)port);
            if (res >= size) { r = NULL; }
        }
    }

    return r;
}

char* sockaddrtos(const void* sa, char* dst, size_t size)
{
    char* r = NULL;

    if (sa && dst && (size >= SOCKADDRSTRLEN))
    {
        const int af = ((const struct sockaddr*)sa)->sa_family;

        switch (af)
        {
        case AF_INET:
            r = sockaddrtos_inet((const struct sockaddr_in*)sa, dst, size);
            break;

        case AF_INET6:
            r = sockaddrtos_inet6((const struct sockaddr_in6*)sa, dst, size);
            break;

        default:
        {
            static_assert(SOCKADDRSTRLEN >= (AF_STRLEN + 11), "increase SOCKADDRSTRLEN");

            char afStrBuffer[AF_STRLEN];
            const char* const afStr = aftos(af, afStrBuffer, sizeof(afStrBuffer));

            strcpy(dst, "(sockaddr ");
            strcat(dst, afStr);
            strcat(dst, ")");
            r = dst;
        }
        break;
        }
    }

    return r;
}
