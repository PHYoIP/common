/*
author          Oliver Blaser
date            10.03.2026
copyright       GPL-3.0 - Copyright (c) 2026 Oliver Blaser
*/

#ifndef IG_COMMON_SOCKET_H
#define IG_COMMON_SOCKET_H

#include <cstddef>
#include <cstdint>



#ifdef _WIN32

#define LOG_ERR_SOCKET(msg) LOG_ERR_WSA(msg, WSAGetLastError())

#ifdef _WIN64
typedef long long sockfd_t;
#else
typedef int sockfd_t;
#endif

typedef char sockopt_optval_t;
typedef uint16_t in_port_t;

#else // *nix

#define LOG_ERR_SOCKET(msg) LOG_ERR_ERRNO(msg, errno);

typedef int sockfd_t;
typedef int sockopt_optval_t;

#endif // Windows / *nix



int sockclose(sockfd_t sockfd);



#define MAC_ADDRSTRLEN   18
#define AF_STRLEN        14
#define ETH_P_STRLEN     17
#define IPPROTO_STRLEN   17
#define ICMP_TYPE_STRLEN 15
#define SOCKADDRSTRLEN   73

char* aftos(int af, char* dst, size_t size);
char* sockaddrtos(const void* sa, char* dst, size_t size);



#endif // IG_COMMON_SOCKET_H
