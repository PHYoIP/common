/*
author          Oliver Blaser
date            07.02.2026
copyright       GPL-3.0 - Copyright (c) 2026 Oliver Blaser
*/

#ifndef IG_COMMON_SOCKET_H
#define IG_COMMON_SOCKET_H



#ifdef _WIN32



#define LOG_ERR_SOCKET(msg) LOG_ERR_WSA(msg, WSAGetLastError())

#ifdef _WIN64
typedef long long sockfd_t;
#else
typedef int sockfd_t;
#endif

typedef char sockopt_optval_t;



#else // *nix



#define LOG_ERR_SOCKET(msg) LOG_ERR_ERRNO(msg, errno);

typedef int sockfd_t;
typedef int sockopt_optval_t;



#endif // Windows / *nix



int sockclose(sockfd_t sockfd);



#endif // IG_COMMON_SOCKET_H
