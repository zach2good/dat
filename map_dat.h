#pragma once

#include "common.h"

/*
    uint32_t offset = 0;
    DATHEAD* dathead = nullptr;
    do
    {
        dathead = reinterpret_cast<DATHEAD*>(dat.data() + offset);
        uint32_t length = (dathead->next & 0x7ffff) * 16;
        spdlog::info("[{}] {}", offset, dathead->to_string());
        offset += length;
    }
    while (dathead->next != 0);
*/

// For models, maps etc.
// https://git.ashitaxi.com/PrivateServerTools/MapViewer/src/branch/master/FFXIMapViewer/MapFile.h
#pragma pack(push, 1)
struct DATHEAD
{
    uint32_t id; // DWORD
    uint32_t type : 7;
    uint32_t next : 19;
    uint32_t is_shadow : 1;
    uint32_t is_extracted : 1;
    uint32_t ver_num : 3;
    uint32_t is_virtual : 1;

    std::string to_string()
    {
        // TODO: Why do I need to re-cast these to uint32_t?
        return fmt::format("id: {}, type: {}, next: {}, is_shadow: {}, is_extracted: {}, ver_num: {}, is_virtual: {}",
                           id, (uint32_t)type, (uint32_t)next, (uint32_t)is_shadow, (uint32_t)is_extracted, (uint32_t)ver_num, (uint32_t)is_virtual);
    }
};
#pragma pack(pop)
