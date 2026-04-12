/*
author          Oliver Blaser
date            12.04.2026
copyright       GPL-3.0 - Copyright (c) 2026 Oliver Blaser
*/

#ifndef IG_COMMON_PACKET_H
#define IG_COMMON_PACKET_H

#include <cstddef>
#include <cstdint>
#include <ctime>
#include <vector>

#include "bits/types.h"



namespace packet {

enum class Direction
{
    ingress,
    egress,
};

class Uart
{
public:
    Uart() = delete;
    Uart(const uint8_t* data, size_t count, const Direction& direction);
    Uart(const std::vector<uint8_t>& data, const Direction& direction);
    Uart(const char* str, const Direction& direction);
    virtual ~Uart() {}

    void setDirection(const Direction& direction);

    time_t timestamp() const { return m_timestamp; }
    int32_t nsec() const { return m_nsec; }
    const uint8_t* data() const { return m_data.data(); }
    size_t size() const { return m_data.size(); }

    bool isIngress() const { return m_ingress; }

private:
    time_t m_timestamp;
    int32_t m_nsec;
    bool m_ingress;
    std::vector<uint8_t> m_data;

    void m_setTimestampNow();
};

/**
 * Functions return number of bytes written to `buffer`, or negative on error.
 */
namespace serialise {

    ssize_t phyoip(uint8_t* buffer, size_t size, uint8_t proto, const uint8_t* data, size_t count);

    ssize_t cmp(uint8_t* buffer, size_t size, uint8_t type, const uint8_t* data, size_t count);
    ssize_t cmpAck(uint8_t* buffer, size_t size, uint8_t status);
    ssize_t cmpPeerInfo(uint8_t* buffer, size_t size, const char* name, const char* description, uint8_t vMaj, uint8_t vMin, const char* versionString);
    ssize_t cmpReqPeerInfo(uint8_t* buffer, size_t size);

    ssize_t uart(uint8_t* buffer, size_t size, const Uart& packet);

} // namespace serialise

/**
 * @return `true` if the `phyoiphdr::identifier` field is valid and the version is compatible.
 */
bool isCompatible(const void* phyoiphdr);

/**
 * See `isCompatible(const void*)`.
 */
bool isCompatible(const uint8_t* data, size_t count);

/**
 * @brief Gets the total packet size.
 *
 * Assumes a compatible packet.
 *
 * @param data
 * @param count
 * @return `(size_t)(-1)` on error
 */
size_t size(const uint8_t* data, size_t count);

} // namespace packet



#endif // IG_COMMON_PACKET_H
