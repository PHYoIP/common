/*
author          Oliver Blaser
date            09.11.2026
copyright       GPL-3.0 - Copyright (c) 2026 Oliver Blaser
*/

#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>

#include "bits/types.h"
#include "common/serial.h"
#include "util/macros.h"

#ifdef _WIN32
#include <Windows.h>
#else
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#endif



#ifndef _WIN32
static int baudToSpeed(comm::Serial::baud_type baud, speed_t* speed);
static inline int ATTR_ALWAYS_INLINE alias_open(const char* path, int flags) { return open(path, flags); }
static inline int ATTR_ALWAYS_INLINE alias_close(int fd) { return close(fd); }
static inline ssize_t ATTR_ALWAYS_INLINE alias_read(int fd, void* buf, size_t count) { return read(fd, buf, count); }
static inline ssize_t ATTR_ALWAYS_INLINE alias_write(int fd, const void* buf, size_t count) { return write(fd, buf, count); }
#endif



comm::Serial::Serial()
    : m_isOpen(false),
      m_good(true),
#ifdef _WIN32
      m_handle(INVALID_HANDLE_VALUE)
#else
      m_fd(-1)
#endif
{}

comm::Serial::~Serial()
{
    if (m_isOpen) { this->close(); }
}

int comm::Serial::open(const char* port, comm::Serial::baud_type baud, uint8_t dataBits, Parity parity, StopBits stopBits)
{
    if (m_isOpen) { return -(__LINE__); }

    int r = 0;

#ifdef _WIN32

    DCB dcb;
    memset(&dcb, 0, sizeof(DCB));

    dcb.DCBlength = sizeof(DCB);
    dcb.BaudRate = baud;
    dcb.fBinary = TRUE;
    dcb.fParity = (parity == Parity::none ? FALSE : TRUE);
    dcb.fOutxCtsFlow = FALSE;
    dcb.fOutxDsrFlow = FALSE;
    dcb.fDtrControl = DTR_CONTROL_ENABLE;
    dcb.fDsrSensitivity = FALSE;
    dcb.fTXContinueOnXoff = TRUE;
    dcb.fOutX = FALSE;
    dcb.fInX = FALSE;
    dcb.fErrorChar = FALSE;
    dcb.fNull = FALSE;
    dcb.fRtsControl = RTS_CONTROL_ENABLE;
    dcb.fAbortOnError = FALSE;
    dcb.XonLim = 0;
    dcb.XoffLim = 1;
    dcb.ByteSize = dataBits;

    // clang-format off
    switch (parity)
    {
    case Parity::none:  dcb.Parity = NOPARITY;    break;
    case Parity::even:  dcb.Parity = EVENPARITY;  break;
    case Parity::odd:   dcb.Parity = ODDPARITY;   break;
    case Parity::mark:  dcb.Parity = MARKPARITY;  break;
    case Parity::space: dcb.Parity = SPACEPARITY; break;
    default:            r = -(__LINE__);          break;
    }

    switch (stopBits)
    {
    case StopBits::one:          dcb.StopBits = ONESTOPBIT;   break;
    case StopBits::onePointFive: dcb.StopBits = ONE5STOPBITS; break;
    case StopBits::two:          dcb.StopBits = TWOSTOPBITS;  break;
    default:                     r = -(__LINE__);             break;
    }
    // clang-format on

    if (r == 0)
    {
        m_handle = CreateFileA((std::string("\\\\.\\") + port).c_str(), (GENERIC_READ | GENERIC_WRITE), 0, NULL, OPEN_EXISTING, 0, NULL);

        if (m_handle == INVALID_HANDLE_VALUE) { r = -(__LINE__); }
        else
        {
            COMMTIMEOUTS tmo;
            tmo.ReadIntervalTimeout = MAXDWORD;
            tmo.ReadTotalTimeoutMultiplier = 0;
            tmo.ReadTotalTimeoutConstant = 0;
            tmo.WriteTotalTimeoutMultiplier = 0;
            tmo.WriteTotalTimeoutConstant = 0;

            if (!SetCommState(m_handle, &dcb)) { r = -(__LINE__); }
            else
            {
                if (!SetCommTimeouts(m_handle, &tmo)) { r = -(__LINE__); }
            }

            if (r == 0) { m_isOpen = true; }
            else
            {
                if (!CloseHandle(m_handle)) { r += -(__LINE__ * 10000); }
                m_handle = INVALID_HANDLE_VALUE;
            }
        }
    }

#else // platform

    m_fd = alias_open(port, O_RDWR | O_NOCTTY | O_NONBLOCK);

    if (!(m_fd >= 0)) { r = -(__LINE__); }
    else
    {
        struct termios tty;
        if (tcgetattr(m_fd, &tty) == 0)
        {
#if !defined(CMSPAR) && defined(PAREXT)
#define CMSPAR_PAREXT_ALIAS
#define CMSPAR PAREXT
#endif

            tty.c_cflag &= ~(PARENB | PARODD);
#ifdef CMSPAR
            tty.c_cflag &= ~CMSPAR;
#endif
            // clang-format off
            switch (parity)
            {
            case Parity::none:  /* nop */                                   break;
            case Parity::even:  tty.c_cflag |= PARENB;                      break;
            case Parity::odd:   tty.c_cflag |= PARENB | PARODD;             break;
#ifdef CMSPAR
            case Parity::mark:  tty.c_cflag |= PARENB | CMSPAR | PARODD;    break;
            case Parity::space: tty.c_cflag |= PARENB | CMSPAR;             break;
#endif
            default:            r = -(__LINE__);                            break;
            }
            // clang-format on

#ifdef CMSPAR_PAREXT_ALIAS
#undef CMSPAR_PAREXT_ALIAS
#undef CMSPAR
#endif

            // CSTOPB means 2 stop bits for 6..8 data bits, but 1.5 for 5 data bits
            tty.c_cflag &= ~CSTOPB;
            switch (stopBits)
            {
            case StopBits::one: // nop
                break;

            case StopBits::onePointFive:
                if (dataBits == 5) { tty.c_cflag |= CSTOPB; }
                else { r = -(__LINE__); }
                break;

            case StopBits::two:
                if (dataBits != 5) { tty.c_cflag |= CSTOPB; }
                else { r = -(__LINE__); } // CS5|CSTOPB would give 1.5, not 2
                break;

            default:
                r = -(__LINE__);
                break;
            }

            tty.c_cflag &= ~CSIZE;
            // clang-format off
            switch (dataBits)
            {
            case 5: tty.c_cflag |= CS5; break;
            case 6: tty.c_cflag |= CS6; break;
            case 7: tty.c_cflag |= CS7; break;
            case 8: tty.c_cflag |= CS8; break;

            default:
                tty.c_cflag |= CS8;
                r = -(__LINE__);
                break;
            }
            // clang-format on

            tty.c_cflag &= ~CRTSCTS;       // disable RTS/CTS hardware flow control
            tty.c_cflag |= CREAD | CLOCAL; // turn on READ & ignore ctrl lines (CLOCAL = 1)



            tty.c_lflag &= ~ICANON; // non-canonical mode
            tty.c_lflag &= ~ECHO;   // disable echo
            tty.c_lflag &= ~ECHOE;  // disable erasure
            tty.c_lflag &= ~ECHONL; // disable new-line echo
            tty.c_lflag &= ~ISIG;   // disable interpretation of INTR, QUIT and SUSP
            tty.c_lflag &= ~IEXTEN; // disable implementation-defined input processing



            tty.c_iflag &= ~(IXON | IXOFF | IXANY);                                      // turn off s/w flow ctrl
            tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL); // disable any special handling of received bytes
            if (parity == Parity::none) { tty.c_iflag &= ~INPCK; }
            else { tty.c_iflag |= INPCK; }
            tty.c_iflag |= IGNPAR; // drop bytes with invalid parity



            tty.c_oflag &= ~OPOST; // prevent special interpretation of output bytes (e.g. newline chars)
            tty.c_oflag &= ~ONLCR; // prevent conversion of newline to carriage return/line feed



            // non blocking read
            tty.c_cc[VTIME] = 0;
            tty.c_cc[VMIN] = 0;



            if (r == 0)
            {
                speed_t speed;
                if (baudToSpeed(baud, &speed) == 0)
                {
                    if (cfsetispeed(&tty, speed) == 0)
                    {
                        if (cfsetospeed(&tty, speed) == 0)
                        {
                            if (tcsetattr(m_fd, TCSANOW, &tty) != 0) { r = -(__LINE__); }
                        }
                        else { r = -(__LINE__); }
                    }
                    else { r = -(__LINE__); }
                }
                else { r = -(__LINE__); }
            }
        }
        else { r = -(__LINE__); }

        if (r == 0) { m_isOpen = true; }
        else
        {
            if (alias_close(m_fd) != 0) { r += -(__LINE__ * 10000); }
            m_fd = -1;
        }
    }

#endif // platform

    m_good = (r == 0);
    return r;
}

int comm::Serial::close()
{
    if (!m_isOpen) { return -(__LINE__); }

    int r = 0;

#ifdef _WIN32
    if (!CloseHandle(m_handle)) { r = -(__LINE__); }
    m_handle = INVALID_HANDLE_VALUE;
#else
    if (alias_close(m_fd) != 0) { r = -(__LINE__); }
    m_fd = -1;
#endif

    m_isOpen = false;
    m_good = (r == 0);
    return r;
}

int comm::Serial::flush()
{
    if (!m_isOpen) { return -(__LINE__); }

    int r = 0;

#ifdef _WIN32
    if (!PurgeComm(m_handle, PURGE_RXCLEAR | PURGE_TXCLEAR)) { r = -(__LINE__); }
#else
    if (tcflush(m_fd, TCIOFLUSH) != 0) { r = -(__LINE__); }
#endif

    m_good = (r == 0);
    return r;
}

ssize_t comm::Serial::read(uint8_t* buffer, size_t size)
{
    if (!m_isOpen) { return -(__LINE__); }

    ssize_t r = 0;

#ifdef _WIN32

    const DWORD dwBufSize = (size > MAXDWORD ? MAXDWORD : (DWORD)size);
    DWORD dwNBytesRead = 0;

    if (ReadFile(m_handle, buffer, dwBufSize, &dwNBytesRead, NULL)) { r = (ssize_t)dwNBytesRead; }
    else { r = -(__LINE__); }

#else // platform

    r = alias_read(m_fd, buffer, size);
    if ((r < 0) && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)) { r = 0; }

#endif // platform

    m_good = (r >= 0);
    return r;
}

ssize_t comm::Serial::write(const uint8_t* data, size_t count)
{
    if (!m_isOpen) { return -(__LINE__); }

    ssize_t r = 0;

#ifdef _WIN32

    const DWORD dwCount = (count > MAXDWORD ? MAXDWORD : (DWORD)count);
    DWORD dwNBytesWritten = 0;

    if (WriteFile(m_handle, data, dwCount, &dwNBytesWritten, NULL)) { r = (ssize_t)dwNBytesWritten; }
    else { r = -(__LINE__); }

#else // platform

    r = alias_write(m_fd, data, count);
    if ((r < 0) && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)) { r = 0; }

#endif // platform

    m_good = (r >= 0);
    return r;
}



#ifndef _WIN32

int baudToSpeed(comm::Serial::baud_type baud, speed_t* speed)
{
    int err = 0;

    // clang-format off
    switch (baud)
    {
    case 0:         *speed = B0;        break;
    case 50:        *speed = B50;       break;
    case 75:        *speed = B75;       break;
    case 110:       *speed = B110;      break;
    case 134:       *speed = B134;      break;
    case 150:       *speed = B150;      break;
    case 200:       *speed = B200;      break;
    case 300:       *speed = B300;      break;
    case 600:       *speed = B600;      break;
    case 1200:      *speed = B1200;     break;
    case 1800:      *speed = B1800;     break;
    case 2400:      *speed = B2400;     break;
    case 4800:      *speed = B4800;     break;
    case 9600:      *speed = B9600;     break;
    case 19200:     *speed = B19200;    break;
    case 38400:     *speed = B38400;    break;
    case 57600:     *speed = B57600;    break;
    case 115200:    *speed = B115200;   break;
    case 230400:    *speed = B230400;   break;
    case 460800:    *speed = B460800;   break;
#if defined(__linux__)
    case 500000:    *speed = B500000;   break;
    case 576000:    *speed = B576000;   break;
    case 921600:    *speed = B921600;   break;
    case 1000000:   *speed = B1000000;  break;
    case 1152000:   *speed = B1152000;  break;
    case 1500000:   *speed = B1500000;  break;
    case 2000000:   *speed = B2000000;  break;
    case 2500000:   *speed = B2500000;  break;
    case 3000000:   *speed = B3000000;  break;
    case 3500000:   *speed = B3500000;  break;
    case 4000000:   *speed = B4000000;  break;
#endif
    default:        err = -1;           break;
    };
    // clang-format on

    return err;
}

#endif // platform
