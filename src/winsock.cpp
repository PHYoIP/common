/*
author          Oliver Blaser
date            06.02.2026
copyright       GPL-3.0 - Copyright (c) 2026 Oliver Blaser
*/

#include <cstddef>
#include <cstdint>

#include "common/winsock.h"

#include <WinSock2.h>

#include <Windows.h>


#define LOG_MODULE_LEVEL LOG_LEVEL_DBG
#define LOG_MODULE_NAME  WINSOCK
#include "common/log.h"



#ifdef _MSC_VER
#pragma comment(lib, "ws2_32.lib")
#endif // _MSC_VER



int winsock::init()
{
    const WORD reqVersion = 0x0202; // 2.2

    WSADATA wsaData;
    const int err = WSAStartup(reqVersion, &wsaData);
    if (err)
    {
        LOG_ERR_WSA("WSAStartup() failed", err);
        winsock::cleanup();
        return -(__LINE__);
    }

    if (wsaData.wVersion != reqVersion)
    {
        LOG_WRN("WinSock version: %i.%i, requested: %i.%i", (int)(wsaData.wVersion >> 8), (int)(wsaData.wVersion & 0x00FF), (int)(reqVersion >> 8),
                (int)(reqVersion & 0x00FF));
    }

    return 0;
}

int winsock::cleanup()
{
    int err;

    err = WSACleanup();
    if (err)
    {
        err = WSAGetLastError();

        if (err == WSAEINPROGRESS)
        {
            static int depth = 0;

            if ((depth % 5) == 0) { LOG_WRN("WSACleanup() failed, in progress ..."); }

            ++depth;

            if (depth < 240)
            {
                Sleep(500);
                return winsock::cleanup();
            }
        }

        return err;
    }

    return 0;
}
