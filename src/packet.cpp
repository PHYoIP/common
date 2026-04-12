/*
author          Oliver Blaser
date            12.04.2026
copyright       GPL-3.0 - Copyright (c) 2026 Oliver Blaser
*/

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <string>
#include <vector>

#include "common/packet.h"
#include "common/socket.h"
#include "util/macros.h"
#include "util/time.h"

#include <phyoip/protocol/cmp.h>
#include <phyoip/protocol/discop.h>
#include <phyoip/protocol/phyoip.h>
#include <phyoip/protocol/uart.h>

#ifdef _WIN32
#include <WinSock2.h>
#else // *nix
#include <arpa/inet.h>
#endif // Windows / *nix


#define LOG_MODULE_LEVEL LOG_LEVEL_INF
#define LOG_MODULE_NAME  COMMON_PACKET
#include "common/log.h"



#ifdef _MSC_VER
#pragma warning(disable :4996)
#endif



constexpr uint8_t phyoiphdrIdentifier[] = PHYOIP_HDR_IDENTIFIER_INITIALIZER_LIST;



packet::Uart::Uart(const uint8_t* data, size_t count, const Direction& direction)
    : m_timestamp(0), m_nsec(0), m_ingress(), m_data(data, data + count)
{
    m_setTimestampNow();
    setDirection(direction);
}

packet::Uart::Uart(const std::vector<uint8_t>& data, const Direction& direction)
    : m_timestamp(0), m_nsec(0), m_ingress(), m_data(data)
{
    m_setTimestampNow();
    setDirection(direction);
}

packet::Uart::Uart(const char* str, const Direction& direction)
    : m_timestamp(0), m_nsec(0), m_ingress(), m_data((const uint8_t*)str, (const uint8_t*)str + strlen(str))
{
    m_setTimestampNow();
    setDirection(direction);
}

void packet::Uart::setDirection(const Direction& direction)
{
    switch (direction)
    {
    case Direction::ingress:
        m_ingress = true;
        break;

    case Direction::egress:
        m_ingress = false;
        break;
    }
}

void packet::Uart::m_setTimestampNow()
{
    struct timespec tspec;
    if (0 != UTIL_getTime(&tspec)) // this needs at least C11/C++17
    {
        tspec.tv_sec = 0;
        tspec.tv_nsec = 0;
    }

    m_timestamp = tspec.tv_sec;
    m_nsec = (int32_t)tspec.tv_nsec;
}



ssize_t packet::serialise::phyoip(uint8_t* buffer, size_t size, uint8_t proto, const uint8_t* data, size_t count)
{
    constexpr uint8_t dataoffs = sizeof(struct phyoiphdr);

    if (count > (size - dataoffs))
    {
        LOG_ERR("%s can't serialise %zu bytes of data", UTIL__FUNCSIG__, count);
        return -(__LINE__);
    }

    struct phyoiphdr* const hdr = (struct phyoiphdr*)(buffer);
    for (size_t i = 0; i < PHYOIP_HDR_IDENTIFIER_SIZE; ++i) { hdr->identifier[i] = (char)phyoiphdrIdentifier[i]; }
    hdr->vermaj = PHYOIP_PROTOCOL_VERSION_MAJ;
    hdr->vermin = PHYOIP_PROTOCOL_VERSION_MIN;
    hdr->hsize = dataoffs;
    hdr->proto = proto;

    if (!data) { count = 0; }
    for (size_t i = 0; i < count; ++i) { buffer[dataoffs + i] = *(data + i); }

    return (ssize_t)dataoffs + (ssize_t)count; // type overflow is nearly impossible, no check is done
}

ssize_t packet::serialise::cmp(uint8_t* buffer, size_t size, uint8_t type, const uint8_t* data, size_t count)
{
    constexpr uint8_t dataoffs = sizeof(struct phyoip_cmphdr);

    if (count > UINT16_MAX)
    {
        LOG_ERR("%s can't serialise %zu bytes of data", UTIL__FUNCSIG__, count);
        return -(__LINE__);
    }

    if (size < (dataoffs + count))
    {
        LOG_ERR("%s insufficient buffer size: %zu", UTIL__FUNCSIG__, size);
        return -(__LINE__);
    }

    std::vector<uint8_t> cmpBuffer(dataoffs + count, 0);

    struct phyoip_cmphdr* const hdr = (struct phyoip_cmphdr*)cmpBuffer.data();
    hdr->hsize = dataoffs;
    hdr->dsize = htons((uint16_t)count);
    hdr->type = type;

    if (!data) { count = 0; }
    for (size_t i = 0; i < count; ++i) { cmpBuffer[dataoffs + i] = *(data + i); }

    return packet::serialise::phyoip(buffer, size, PHYOIP_PROTO_CMP, cmpBuffer.data(), cmpBuffer.size());
}

ssize_t packet::serialise::cmpAck(uint8_t* buffer, size_t size, uint8_t status)
{
    std::vector<uint8_t> data(sizeof(struct phyoip_cmpack), 0);

    struct phyoip_cmpack* const ack = (struct phyoip_cmpack*)data.data();
    ack->status = status;

    return packet::serialise::cmp(buffer, size, PHYOIP_CMP_ACK, data.data(), data.size());
}

ssize_t packet::serialise::cmpPeerInfo(uint8_t* buffer, size_t size, const char* name, const char* description, uint8_t vMaj, uint8_t vMin,
                                       const char* versionString)
{
    if (!name)
    {
        LOG_ERR("%s name is NULL", UTIL__FUNCSIG__);
        return -(__LINE__);
    }

    const std::string nameStr = name;
    const std::string descriptionStr = (description ? description : "");
    const std::string versionStr = (versionString ? versionString : "");

    const size_t nameoffs = sizeof(struct phyoip_cmppi);
    const size_t descoffs = (description ? (sizeof(struct phyoip_cmppi) + nameStr.length() + 1) : 0);
    const size_t veroffs = (versionString ? (sizeof(struct phyoip_cmppi) + nameStr.length() + 1 + descriptionStr.length() + 1) : 0);

    if ((nameoffs > UINT16_MAX) || (descoffs > UINT16_MAX) || (veroffs > UINT16_MAX))
    {
        LOG_ERR("%s string pointer/offset overflow", UTIL__FUNCSIG__);
        return -(__LINE__);
    }

    std::vector<uint8_t> data((sizeof(struct phyoip_cmppi) + nameStr.length() + 1 + descriptionStr.length() + 1 + versionStr.length() + 1), 0);

    struct phyoip_cmppi* const info = (struct phyoip_cmppi*)data.data();
    info->proto = PHYOIP_PROTO_UART;                // these should also be passed by arbument
    info->supplier = htons(PHYOIP_SUPPLIER_PHYOIP); // TODO create a class, see `packet::serialise::uart`
    info->nameoffs = htons((uint16_t)nameoffs);
    info->version.v.maj = vMaj;
    info->version.v.min = vMin;
    info->veroffs = htons((uint16_t)veroffs);
    info->descoffs = htons((uint16_t)descoffs);

    strcpy((char*)data.data() + nameoffs, nameStr.c_str());
    if (description) { strcpy((char*)data.data() + descoffs, descriptionStr.c_str()); }
    if (versionString) { strcpy((char*)data.data() + veroffs, versionStr.c_str()); }

    return packet::serialise::cmp(buffer, size, PHYOIP_CMP_PEERINFO, data.data(), data.size());
}

ssize_t packet::serialise::cmpReqPeerInfo(uint8_t* buffer, size_t size) { return packet::serialise::cmp(buffer, size, PHYOIP_CMP_REQPEERINFO, nullptr, 0); }

ssize_t packet::serialise::uart(uint8_t* buffer, size_t size, const Uart& packet)
{
    constexpr uint8_t dataoffs = sizeof(struct phyoip_uarthdr);

    if (packet.size() > UINT16_MAX)
    {
        LOG_ERR("%s can't serialise %zu bytes of data", UTIL__FUNCSIG__, packet.size());
        return -(__LINE__);
    }

    if (size < (dataoffs + packet.size()))
    {
        LOG_ERR("%s insufficient buffer size: %zu", UTIL__FUNCSIG__, size);
        return -(__LINE__);
    }

    std::vector<uint8_t> uartBuffer(dataoffs + packet.size(), 0);

    struct phyoip_uarthdr* const hdr = (struct phyoip_uarthdr*)uartBuffer.data();
    hdr->hsize = dataoffs;
    hdr->dsize = htons((uint16_t)packet.size());
    hdr->timestamp = htonll((int64_t)packet.timestamp());
    hdr->nsec = htonl(packet.nsec());
    if (packet.isIngress()) { hdr->ingress = 1; }
    else { hdr->ingress = 0; }

    for (size_t i = 0; i < packet.size(); ++i) { uartBuffer[dataoffs + i] = *(packet.data() + i); }

    return packet::serialise::phyoip(buffer, size, PHYOIP_PROTO_UART, uartBuffer.data(), uartBuffer.size());
}



bool packet::isCompatible(const void* phyoiphdr)
{
    const struct phyoiphdr* const hdr = (const struct phyoiphdr*)phyoiphdr;

    for (size_t i = 0; i < PHYOIP_HDR_IDENTIFIER_SIZE; ++i)
    {
        if ((uint8_t)(hdr->identifier[i]) != phyoiphdrIdentifier[i]) { return false; }
    }

    if (hdr->vermaj == 0) // development phase
    {
        // maj and min changes can't be differentiated
        if (hdr->vermin != PHYOIP_PROTOCOL_VERSION_MIN)
        {
            LOG_ERR("incompatible protocol version %i.%i, expected %i.%i", (int)(hdr->vermaj), (int)(hdr->vermin), (int)(PHYOIP_PROTOCOL_VERSION_MAJ),
                    (int)(PHYOIP_PROTOCOL_VERSION_MIN));

            return false;
        }
    }
    else // as soon as the development phase is over, only this is needed
    {
        if (hdr->vermaj != PHYOIP_PROTOCOL_VERSION_MAJ)
        {
            LOG_ERR("incompatible protocol version %i.%i, expected %i.%i", (int)(hdr->vermaj), (int)(hdr->vermin), (int)(PHYOIP_PROTOCOL_VERSION_MAJ),
                    (int)(PHYOIP_PROTOCOL_VERSION_MIN));

            return false;
        }
    }

    return true;
}

bool packet::isCompatible(const uint8_t* data, size_t count)
{
    if (count < sizeof(struct phyoiphdr)) { return false; }

    return isCompatible(data);
}

size_t packet::size(const uint8_t* data, size_t count)
{
    if (count < sizeof(struct phyoiphdr)) { return (size_t)(-1); }

    const struct phyoiphdr* const hdr = (const struct phyoiphdr*)data;
    const size_t hsize = hdr->hsize;
    const uint8_t proto = hdr->proto;
    static_assert(sizeof(hdr->hsize) == 1);

    size_t r = (size_t)(-1);

    switch (proto)
    {
    case PHYOIP_PROTO_CMP:
        if (count >= (hsize + sizeof(struct phyoip_cmphdr)))
        {
            const struct phyoip_cmphdr* const cmphdr = (const struct phyoip_cmphdr*)(data + hsize);
            r = (hsize + cmphdr->hsize + ntohs(cmphdr->dsize));
            static_assert((sizeof(cmphdr->hsize) == 1) && (sizeof(cmphdr->dsize) == 2));
        }
        break;

    case PHYOIP_PROTO_DISCOP:
        if (count >= (hsize + sizeof(struct phyoip_discophdr)))
        {
            const struct phyoip_discophdr* const discophdr = (const struct phyoip_discophdr*)(data + hsize);
            r = (hsize + discophdr->hsize + ntohs(discophdr->dsize));
            static_assert((sizeof(discophdr->hsize) == 1) && (sizeof(discophdr->dsize) == 2));
        }
        break;

    case PHYOIP_PROTO_UART:
        if (count >= (hsize + sizeof(struct phyoip_uarthdr)))
        {
            const struct phyoip_uarthdr* const uarthdr = (const struct phyoip_uarthdr*)(data + hsize);
            r = (hsize + uarthdr->hsize + ntohs(uarthdr->dsize));
            static_assert((sizeof(uarthdr->hsize) == 1) && (sizeof(uarthdr->dsize) == 2));
        }
        break;

    default:
        LOG_ERR("invalid protocol: 0x%02x", (int)proto);
        r = (size_t)(-1);
        break;
    }

    return r;
}
