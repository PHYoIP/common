/*
author          Oliver Blaser
date            11.04.2026
copyright       GPL-3.0 - Copyright (c) 2026 Oliver Blaser
*/

#ifndef IG_COMMON_PACKET_H
#define IG_COMMON_PACKET_H

#include <cstddef>
#include <cstdint>



namespace packet {
namespace serialise {

    int phyoip(uint8_t* buffer, size_t size, uint8_t proto, const uint8_t* data, size_t count);

    int cmp(uint8_t* buffer, size_t size, uint8_t type, const uint8_t* data, size_t count);
    int cmpAck(uint8_t* buffer, size_t size, uint8_t status);
    int cmpPeerInfo(uint8_t* buffer, size_t size);
    int cmpReqPeerInfo(uint8_t* buffer, size_t size);

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
