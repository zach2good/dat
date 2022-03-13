#pragma once

#include "common.h"

// https://github.com/atom0s/XiEvents/blob/main/Event%20DAT%20Structures.md

struct eventheader_t
{
    uint32_t  BlockCount; // Event block count.
    uint32_t* BlockSizes; // Event block size table.
};

struct eventblock_t
{
    uint32_t  Actornumber;   // Entity Server Id
    uint32_t  TagCount;      // Event count.
    uint16_t* TagOffset;     // Event offset table.
    uint16_t* EvectExecNum;  // Event id table.
    uint32_t  ImedCount;     // Event immediate data count.
    uint32_t* ImidData;      // Event immediate data table.
    uint32_t  EventDataSize; // Event data size.

    // // Event data. (4 byte aligned.)
    // if (EventDataSize % 4 == 0)
    //     uint8_t EventData[EventDataSize];
    // else
    //     uint8_t EventData[EventDataSize + (4 - EventDataSize % 4)];
};

struct eventdat_t
{
    eventheader_t Header;
    eventblock_t* Block;
};
