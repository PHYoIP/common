/*
author          Oliver Blaser
date            14.03.2026
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



#include <byteswap.h>
#include <endian.h>

#if ((defined(__BYTE_ORDER) && (__BYTE_ORDER == __BIG_ENDIAN)) || (defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __BIG_ENDIAN__)) || defined(__ARMEB__) || \
     defined(__AARCH64EB__) || defined(__THUMBEB__) || defined(_MIPSEB) || defined(__MIPSEB) || defined(__MIPSEB__))
static inline uint64_t htonll(uint64_t hostlonglong) { return hostlonglong; }
static inline uint64_t ntohll(uint64_t netlonglong) { return netlonglong; }
#endif

#if ((defined(__BYTE_ORDER) && (__BYTE_ORDER == __LITTLE_ENDIAN)) || (defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __LITTLE_ENDIAN__)) || \
     defined(__ARMEL__) || defined(__AARCH64EL__) || defined(__THUMBEL__) || defined(_MIPSEL) || defined(__MIPSEL) || defined(__MIPSEL__))
static inline uint64_t htonll(uint64_t hostlonglong) { return bswap_64(hostlonglong); }
static inline uint64_t ntohll(uint64_t netlonglong) { return bswap_64(netlonglong); }
#endif



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
