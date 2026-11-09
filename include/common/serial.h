/*
author          Oliver Blaser
date            09.11.2026
copyright       GPL-3.0 - Copyright (c) 2026 Oliver Blaser
*/

#ifndef IG_COMMON_SERIAL_H
#define IG_COMMON_SERIAL_H

#include <cstddef>
#include <cstdint>

#include "bits/types.h"



namespace comm {

class Serial
{
public:
    using baud_type = uint32_t;

    enum class Parity
    {
        none,
        even,
        odd,
        mark,
        space,
    };

    enum class StopBits
    {
        one,          ///< 1 stop bit
        onePointFive, ///< 1.5 stop bits
        two,          ///< 2 stop bits
    };

public:
    Serial();
    virtual ~Serial();

    int open(const char* port, baud_type baud, uint8_t dataBits, Parity parity, StopBits stopBits);
    int close();

    /**
     * @brief Discards data in the input and output queues (received but not read, and written but not transmitted).
     */
    int flush();

    ssize_t read(uint8_t* buffer, size_t size);
    ssize_t read(char* buffer, size_t size) { return read((uint8_t*)buffer, size); }
    ssize_t readByte(uint8_t* byte) { return read(byte, 1); }
    ssize_t write(const uint8_t* data, size_t count);
    ssize_t write(const char* data, size_t count) { return write((const uint8_t*)data, count); }

    bool isOpen() const { return m_isOpen; }
    bool good() const { return m_good; }

private:
    bool m_isOpen;
    bool m_good;
#ifdef _WIN32
    void* m_handle;
#else
    int m_fd;
#endif

public: // make non copyable
    Serial(const Serial& other) = delete;
    Serial& operator=(const Serial& other) = delete;
};

} // namespace comm



#endif // IG_COMMON_SERIAL_H
