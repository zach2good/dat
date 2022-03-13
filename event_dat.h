#pragma once

#include "common.h"
#include "util.h"
#include <filesystem>

// https://github.com/atom0s/XiEvents/blob/main/Event%20DAT%20Structures.md

struct eventheader_t
{
    uint32_t              BlockCount; // Event block count.
    std::vector<uint32_t> BlockSizes; // Event block size table.
};

struct eventblock_t
{
    uint32_t              ActorNumber;   // Entity Server Id
    uint32_t              TagCount;      // Event count.
    std::vector<uint16_t> TagOffset;     // Event offset table.
    std::vector<uint16_t> EvectExecNum;  // Event id table.
    uint32_t              ImedCount;     // Event immediate data count.
    std::vector<uint32_t> ImidData;      // Event immediate data table.
    uint32_t              EventDataSize; // Event data size.
    std::vector<uint8_t>  EventData;     // Event data. (4 byte aligned.)
};

struct eventdat_t
{
    eventdat_t(std::filesystem::path path)
    {
        spdlog::info("Loading Event DAT: {}", path.generic_string());

        auto  dat  = load_file<uint8_t>(path.generic_string());
        auto* data = dat.data();

        uint32_t offset = 0;

        // Load Header
        Header.BlockCount = *reinterpret_cast<uint32_t*>(data + offset); //
        offset += 4;

        Header.BlockSizes = std::vector<uint32_t>(Header.BlockCount);
        for (std::size_t idx = 0; idx < Header.BlockCount; ++idx)
        {
            Header.BlockSizes[idx] = *reinterpret_cast<uint32_t*>(data + offset);
            offset += 4;
        }

        for (int i = 0; i < Header.BlockSizes.size(); i++)
        {
            auto size = Header.BlockSizes[i];

            eventblock_t eventBlock;
            eventBlock.ActorNumber = *reinterpret_cast<uint32_t*>(data + offset);
            offset += 4;

            eventBlock.TagCount = *reinterpret_cast<uint32_t*>(data + offset);
            offset += 4;

            for (std::size_t idx = 0; idx < eventBlock.TagCount; ++idx)
            {
                eventBlock.TagOffset.emplace_back(*reinterpret_cast<uint16_t*>(data + offset));
                offset += 2;
            }

            for (std::size_t idx = 0; idx < eventBlock.TagCount; ++idx)
            {
                eventBlock.EvectExecNum.emplace_back(*reinterpret_cast<uint16_t*>(data + offset));
                offset += 2;
            }

            eventBlock.ImedCount = *reinterpret_cast<uint32_t*>(data + offset);
            offset += 4;

            for (std::size_t idx = 0; idx < eventBlock.ImedCount; ++idx)
            {
                eventBlock.ImidData.emplace_back(*reinterpret_cast<uint32_t*>(data + offset));
                offset += 4;
            }

            eventBlock.EventDataSize = *reinterpret_cast<uint32_t*>(data + offset);
            offset += 4;
            if (eventBlock.EventDataSize % 4 != 0)
            {
                eventBlock.EventDataSize = eventBlock.EventDataSize + (4 - eventBlock.EventDataSize % 4);
            }

            for (std::size_t idx = 0; idx < eventBlock.EventDataSize; ++idx)
            {
                eventBlock.EventData.emplace_back(*reinterpret_cast<uint8_t*>(data + offset));
                offset += 1;
            }

            Blocks.emplace_back(eventBlock);
        }
    }

    eventheader_t             Header;
    std::vector<eventblock_t> Blocks;
};
