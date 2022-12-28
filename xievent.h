#pragma once

#include "common.h"
#include "event_dat.h"
#include "opcode.h"
#include "dialog_dat.h"
#include "entity_dat.h"
#include "event_dat.h"

#include <array>
#include <functional>
#include <string>

// https://github.com/atom0s/XiEvents/blob/main/Event%20VM%20Structures.md

/**
 * PS2: REQSTACK
 * Size: 0x20
 */
struct reqstack_t
{
    uint16_t Priority;         // [Priorty]
    uint16_t StackExecPointer; // [StackExecPointer]
    float    MovePosition[4];  // [movepos]
    uint16_t MoveTime;         // [movetime]
    uint8_t  TagNum;           // [Tagnum]
    uint8_t  ReqFlag;          // [reqflag]
    float    WaitTime;         // [WaitTime]
    uint32_t WhoServerId;      // [who]
};

/**
 * PS2: N/A (This data is part of XiEvent on PS2, on PC it seems to be an extended struct.)
 * Size: 0x188
 */
struct xieventex_t
{
    uint32_t             WorkLocal[80];             // [Work_Local]
    std::array<float, 4> EventPos;                  // [Eventpos]
    std::array<float, 4> EventDir;                  // [Eventdir]
    uint8_t              ReadEventMotionResFlag[2]; // [ReadEventMotionResFlag]
    uint8_t              FadeFlag;                  // [FadeFlag]
    uint8_t              Unknown0000;               // [???]
    uint16_t             MouthIndex;                // [MouthIndex]
    uint16_t             Unknown0001;               // [???]
    float                MainSpeed;                 // [MainSpeed]
    float                MainSpeedBase;             // [MainSpeedBase]
    uint32_t             NowColor;                  // [NowColor]
    uint8_t              EndAlpha;                  // [EndAlpha]
    uint8_t              Unknown0002;               // [???]
    uint16_t             AlphaTime;                 // [AlphaTime]
    float                NowAlpha;                  // [nowAlpha]
    float                OfsAlpha;                  // [ofsAlpha]
    uint32_t             Unknown0003;               // [???]
    uint32_t             Unknown0004;               // [???]
};

/**
 * PS2: XiEvent
 * Size: 0x268
 */
class xievent_t
{
public:
    uint16_t                    EntityTargetIndex[2];   // [ActorIndex]
    uint32_t                    EntityServerId[2];      // [Actornumber]
    uint16_t                    EventCount;             // [TagCount]
    uint16_t                    Unknown0000;            // [???]
    uintptr_t                   EventOffsets;           // [TagOffset]
    uintptr_t                   EventIds;               // [EvectExecNum]
    uint16_t                    ReferenceCount;         // [ImedCount]
    uint16_t                    Unknown0001;            // [???]
    uintptr_t                   References;             // [ImidData]
    uint8_t*                    EventData;              // [EventData]
    std::array<reqstack_t, 16>  ReqStack;               // [Req]
    uint16_t                    Unknown0002;            // [???]
    uint16_t                    Unknown0003;            // [???]
    uint8_t                     Unknown0004[18];        // [???]
    uint8_t                     Unknown0005;            // [???]
    uint8_t                     Unknown0006[5];         // [???]
    uint32_t                    Unknown0007;            // [???]
    uint16_t                    JumpTableIndex = 0;     // [GosubStackPtr]
    std::array<uint16_t, 8>     JumpTable;              // [GosubRetAdrs]
    uint16_t                    ExecPointer;            // [ExecPointer]
    uint16_t                    RunPos;                 // [RunPos]
    uint8_t                     RetFlag;                // [RetFlag]
    uint8_t                     InitFlag;               // [InitFlag]
    std::array<xieventex_t*, 2> ExtData;                // [N/A]
    uint32_t                    ExtDataModifiedCounter; // [N/A]

    // TODO: Move this to be constexpr somewhere
    std::array<opcode_t, 0xD9> opcodes;

    xievent_t()
    : ExecPointer(0)
    , RetFlag(0)
    , RunPos(0)
    {
        // TODO: Move this to be constexpr somewhere

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0000.md
        opcodes[0x00] = {
            0x00,
            "RESET",
            "Ends the current ReqStack execution; resetting it back to defaults.",
            [&](xievent_t* event)
            {
                event->ReqStack[event->RunPos].StackExecPointer = 0;
                event->ReqStack[event->RunPos].Priority         = 255;
                event->ReqStack[event->RunPos].WaitTime         = -1.0;
                event->ReqStack[event->RunPos].WhoServerId      = 0;
                event->ReqStack[event->RunPos].TagNum           = 0;

                event->RetFlag = 1;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0001.md
        opcodes[0x01] = {
            0x01,
            "JMP",
            "Directly sets the ExecPointer position.",
            [&](xievent_t* event)
            {
                this->ExecPointer = FUNC_XiEvent_eventgetcode(this, 1);
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0002.md
        opcodes[0x02] = {
            0x02,
            "IF",
            "Handles multiple types of if conditional statements.",
            [&](xievent_t* event)
            {
                // TODO
                this->ExecPointer += 8;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0003.md
        opcodes[0x03] = {
            0x03,
            "LOAD",
            "Gets a value then stores it.",
            [&](xievent_t* event)
            {
                const auto val = FUNC_XiEvent_getworkofs_(this, 3);
                FUNC_XiEvent_setworkofs_(event, 1, val);

                this->ExecPointer += 5;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0004.md
        opcodes[0x04] = {
            0x04,
            "NOOP2",
            "Deprecated. This opcode appears to be deprecated, it does nothing.",
            [&](xievent_t* event)
            {
                this->ExecPointer += 3;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0005.md
        opcodes[0x05] = {
            0x05,
            "ONE",
            "Sets a value to 1.",
            [&](xievent_t* event)
            {
                FUNC_XiEvent_setworkofs_(this, 1, 1);

                this->ExecPointer += 3;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0006.md
        opcodes[0x06] = {
            0x06,
            "ZERO",
            "Sets a value to 0.",
            [&](xievent_t* event)
            {
                FUNC_XiEvent_setworkofs_(this, 1, 0);

                this->ExecPointer += 3;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x000A.md
        opcodes[0x0A] = {
            0x0A,
            "CLR_FLAG",
            "Clears a bit flag value then stores the result.",
            [&](xievent_t* event)
            {
                this->ExecPointer += 5;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x000B.md
        opcodes[0x0B] = {
            0x0B,
            "INC",
            "Increments a value then store it.",
            [&](xievent_t* event)
            {
                const auto val = FUNC_XiEvent_getworkofs_(this, 1);

                FUNC_XiEvent_setworkofs_(this, 1, val + 1);

                this->ExecPointer += 3;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x000C.md
        opcodes[0x0C] = {
            0x0C,
            "DEC",
            "Decrements a value then store it.",
            [&](xievent_t* event)
            {
                const auto val = FUNC_XiEvent_getworkofs_(this, 1);

                FUNC_XiEvent_setworkofs_(this, 1, val - 1);

                this->ExecPointer += 3;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x000D.md
        opcodes[0x0D] = {
            0x0D,
            "AND",
            "Gets the bitwise AND result of two values and stores it.",
            [&](xievent_t* event)
            {
                const auto val1 = FUNC_XiEvent_getworkofs_(this, 1);
                const auto val2 = FUNC_XiEvent_getworkofs_(this, 3);

                FUNC_XiEvent_setworkofs_(this, 1, val1 & val2);

                this->ExecPointer += 5;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x000E.md
        opcodes[0x0E] = {
            0x0E,
            "OR",
            "Gets the bitwise OR result of two values and stores it.",
            [&](xievent_t* event)
            {
                // TODO
                this->ExecPointer += 5;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x000F.md
        opcodes[0x0F] = {
            0x0F,
            "XOR",
            "Gets the bitwise XOR result of two values and stores it.",
            [&](xievent_t* event)
            {
                // TODO
                this->ExecPointer += 5;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0010.md
        opcodes[0x10] = {
            0x10,
            "LSHIFT",
            "Gets the bitwise left-shift result of two values and stores it.",
            [&](xievent_t* event)
            {
                const auto val1 = FUNC_XiEvent_getworkofs_(this, 1);
                const auto val2 = FUNC_XiEvent_getworkofs_(this, 3);

                FUNC_XiEvent_setworkofs_(this, 1, val1 << val2);

                this->ExecPointer += 5;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0011.md
        opcodes[0x11] = {
            0x11,
            "RSHIFT",
            "Gets the bitwise right-shift result of two values and stores it.",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 5;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0013.md
        opcodes[0x13] = {
            0x13,
            "RAND",
            "Generates a random number via rand(), with a given remainder, and stores it.",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 5;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0014.md
        opcodes[0x14] = {
            0x14,
            "MULT",
            "Gets the product of two values and stores it.",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 5;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0016.md
        opcodes[0x16] = {
            0x16,
            "SIN",
            "Performs a sin operation on two values and stores the result.",
            [&](xievent_t* event)
            {
                const auto val1 = FUNC_XiEvent_getworkofs_(this, 3) * 0.0015339355;
                const auto val2 = -(FUNC_XiEvent_getworkofs_(this, 5) * std::sin(val1));

                FUNC_XiEvent_setworkofs_(this, 1, val2);

                this->ExecPointer += 7;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0017.md
        opcodes[0x17] = {
            0x17,
            "COS",
            "Performs a cos operation on two values and stores the result.",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 7;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0018.md
        opcodes[0x18] = {
            0x18,
            "ATAN",
            "Performs an atan2 operation on two values and stores the result.",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 7;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0019.md
        opcodes[0x19] = {
            0x19,
            "SWAP",
            "Reads two values and stores them in flipped order. (Endian swap.)",
            [&](xievent_t* event)
            {
                const auto val1 = FUNC_XiEvent_getworkofs_(this, 1);
                const auto val2 = FUNC_XiEvent_getworkofs_(this, 3);

                FUNC_XiEvent_setworkofs_(this, 1, val2);
                FUNC_XiEvent_setworkofs_(this, 3, val1);

                this->ExecPointer += 5;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x001A.md
        opcodes[0x1A] = {
            0x1A,
            "FUNC",
            "Jumps to a new position in the event data.",
            [&](xievent_t* event)
            {
                if (this->JumpTableIndex == 8)
                {
                    this->RetFlag = 1;
                    return;
                }

                this->JumpTable[this->JumpTableIndex] = this->ExecPointer + 3;
                this->JumpTableIndex++;

                this->ExecPointer = FUNC_XiEvent_getworkofs_(this, 1);
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x001C.md
        opcodes[0x1C] = {
            0x1C,
            "WAIT",
            "Sets, or updates (decreases), the current ReqStack[RunPos].WaitTime value.",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 3;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x001D.md
        opcodes[0x1D] = {
            0x1D,
            "TALK",
            "Loads and prints an event message to chat, using EntityTargetIndex[1] as the speaker.",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 3;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x001E.md
        opcodes[0x1E] = {
            0x1E,
            "LOOK_TALK",
            "Tells an entity to look at another entity and begin 'talking'. (This puts the 'talking' entity into an animation where their mouth moves.)",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 5;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x001F.md
        opcodes[0x1F] = {
            0x1F,
            "UPDATE_EVENT_POS",
            "Updates the event position information.",
            [&](xievent_t* event)
            {
                // TODO
                if (this->EventData[this->ExecPointer + 1])
                {
                    event->ExecPointer += 2;
                }
                else
                {
                    event->ExecPointer += 8;
                }
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0020.md
        opcodes[0x20] = {
            0x20,
            "LOCK_PLAYER",
            "Sets the CliEventUcFlag flag value. (This flag is used to lock the player from controlling their character.)",
            [&](xievent_t* event)
            {
                // TODO
                if (this->EventData[this->ExecPointer + 1])
                {
                    event->ExecPointer += 2;
                }
                else
                {
                    event->ExecPointer += 8;
                }
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0021.md
        opcodes[0x21] = {
            0x21,
            "END_EVENT",
            "Sets the EventExecEnd flag value to 1.",
            [&](xievent_t* event)
            {
                // TODO
                event->RetFlag = 1;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0023.md
        opcodes[0x23] = {
            0x23,
            "WAIT_DIALOG",
            "Waits for the local player to interact with a dialog message.",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 1;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0024.md
        opcodes[0x24] = {
            0x24,
            "CREATE_DIALOG",
            "Creates a dialog window with selectable options for the player to choose from.",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 7;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0025.md
        opcodes[0x25] = {
            0x25,
            "WAIT_DIALOG",
            "Waits for a dialog select (created by opcode 0x0024) to be made by the player.",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 1;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0027.md
        opcodes[0x27] = {
            0x27,
            "CHECK_REQ",
            "Calls a helper FUNC_REQSet which in turn calls XiEvent::ReqSet after checking some conditions.",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 7;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0028.md
        opcodes[0x28] = {
            0x28,
            "CHECK_REQ_EX",
            "Similar to opcode 0x0027, but with extra checks/conditions.",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 7;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0029.md
        opcodes[0x29] = {
            0x29,
            "REQ_SET",
            "Similar to opcode 0x0028.",
            [&](xievent_t* event)
            {
                // TODO
                this->ExecPointer += 6;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x002B.md
        opcodes[0x2B] = {
            0x2B,
            "PRINT_MSG",
            "Loads and prints an event message with the given entity as the speaker.",
            [&](xievent_t* event)
            {
                // TODO
                const auto val = FUNC_XiEvent_getworkofs_(this, 1);

                this->ExecPointer += 7;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x002C.md
        opcodes[0x2C] = {
            0x2C,
            "LOAD_TASK",
            "Creates and loads a CMoSchedularTask on the desired entity. (Appears to set an entity action.)",
            [&](xievent_t* event)
            {
                // TODO
                this->ExecPointer += 13;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0032.md
        opcodes[0x32] = {
            0x32,
            "SET_SPEED",
            "Sets the ExtData[1]->MainSpeed value.",
            [&](xievent_t* event)
            {
                // TODO
                // this->ExtData[1]->MainSpeed = FUNC_XiEvent_getworkofs_(this, 1) * 0.1;

                this->ExecPointer += 3;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0035.md
        opcodes[0x35] = {
            0x35,
            "LOAD_ZONE",
            "Similar to opcode 0x0034. This appears to load an additional zone for the event, however this handler does not have a call to XiZone::Close.",
            [&](xievent_t* event)
            {
                // TODO
                this->ExecPointer += 3;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0037.md
        opcodes[0x37] = {
            0x37,
            "RESET",
            "Ends the current ReqStack execution; resetting it back to defaults.",
            [&](xievent_t* event)
            {
                // TODO
                // event->ExtData[1]->EventPos[0] = FUNC_XiEvent_getworkofs_(event, 1) * 0.001;
                // event->ExtData[1]->EventPos[2] = FUNC_XiEvent_getworkofs_(event, 3) * 0.001;
                // event->ExtData[1]->EventPos[1] = FUNC_XiEvent_getworkofs_(event, 5) * 0.001;
                // event->ExtData[1]->EventDir[1] = FUNC_XiEvent_getworkofs_(event, 7) * 6.283 * 0.00024414062;

                event->ExecPointer += 9;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0038.md
        opcodes[0x38] = {
            0x38,
            "CLR_EVENT_MODE",
            "Sets the lower-word of CliEventModeLocal to a masked value.",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 3;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0039.md
        opcodes[0x39] = {
            0x39,
            "SET_EVENT_DIR",
            "Sets the current ExtData[1]->EventDir[1] value.",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 3;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x003B.md
        opcodes[0x3B] = {
            0x3B,
            "GET_POS",
            "Gets the current position of the given entity (or uses the ExtData[1]->EventPos depending on flags) and stores it.",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 11;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x003C.md
        opcodes[0x3C] = {
            0x3C,
            "CMP",
            "Compares two values (using a shift). If condition is met, sets a bit flag and stores the result.",
            [&](xievent_t* event)
            {
                const auto val1 = FUNC_XiEvent_getworkofs_(this, 3);
                if ((val1 >> 5) < FUNC_XiEvent_getworkofs_(this, 5))
                {
                    // TODO
                    // const auto val2 = FUNC_XiEvent_getworkofs_(this, 1, val1 >> 5);
                    // FUNC_XiEvent_setworkofs_(this, 1, (1 << (val1 & 0x1F)) | val2, val1 >> 5);
                }

                this->ExecPointer += 7;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0041.md
        opcodes[0x41] = {
            0x41,
            "GET_FLAG",
            "Gets a bit flag value and stores it. One usage of this opcode is to tell the client which dialog menu options are enabled/available.",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 9;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0042.md
        opcodes[0x42] = {
            0x42,
            "CANCEL_SET_DATA",
            "Sets the CliEventCancelSetData flag to 0. If CliEventCancelSetFlag is set, then CliEventCancelFlag is also set to 0.",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 1;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0043.md
        opcodes[0x43] = {
            0x43,
            "SEND_EVENT_UPDATE",
            "Used to tell the server the server when the client has updated an event or has completed it.",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 2;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0045.md
        opcodes[0x45] = {
            0x45,
            "START_SCH_TASK",
            "Loads and starts a scheduled task with the given two entities.",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 17;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0046.md
        opcodes[0x46] = {
            0x46,
            "CAMERA_CONTROL",
            "Enables and disables the player camera control. Also disables rendering some menus to allow the game to play cutscenes without unneeded info on screen.",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 2;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0047.md
        opcodes[0x47] = {
            0x47,
            "UPDATE_LOC",
            "Updates the players location during an event. This opcode will send an 0x005C packet to the server to inform it of your position change.",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 10;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x004B.md
        opcodes[0x4B] = {
            0x4B,
            "UPDATE_YAW",
            "Updates the given entities yaw direction.",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 7;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x004E.md
        opcodes[0x4E] = {
            0x4E,
            "HIDE",
            "Sets the entities event hide flag within Render.Flags0.",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 6;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0052.md
        opcodes[0x52] = {
            0x52,
            "END_SCH_TASK",
            "Ends a CMoSchedularTask. (Load / Main)",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 15;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0053.md
        opcodes[0x53] = {
            0x53,
            "WAIT_SCH_TASK",
            "Waits for the given entities schedular to finish its current action.",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 13;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0055.md
        opcodes[0x55] = {
            0x55,
            "WAIT_SCH_TASK",
            "Waits for the Main/Load schedular to finish its current action.",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 15;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x005B.md
        opcodes[0x5B] = {
            0x5B,
            "LOAD_SCH_TASK_EXT",
            "Loads an extended schedular task.",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 15;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0060.md
        opcodes[0x60] = {
            0x60,
            "FLAG_RENDER1",
            "Handler with multiple use cases.",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 4;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0061.md
        opcodes[0x61] = {
            0x61,
            "FLAG_RENDER2",
            "Adjusts the event entities Render.Flags2 value.",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 2;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0062.md
        opcodes[0x62] = {
            0x62,
            "START_SCH_TASK2",
            "Handler that calls the same helper call as opcode 0x0045, just with a different second argument.",
            [&](xievent_t* event)
            {
                // TODO
                event->opcodes[0x45].func(event);
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0066.md
        opcodes[0x66] = {
            0x66,
            "START_SCH_TASK2",
            "Handler that calls the same helper call as opcode 0x005B, just with a different arguments.",
            [&](xievent_t* event)
            {
                // TODO
                event->opcodes[0x5B].func(event);
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0069.md
        opcodes[0x69] = {
            0x69,
            "SET_VOLUME",
            "Sets the sound volume of the desired sound type.",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 4;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x006C.md
        opcodes[0x6C] = {
            0x6C,
            "FADE",
            "Fades an enities color in and out.",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 9;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x006E.md
        opcodes[0x6E] = {
            0x6E,
            "EMOTE",
            "Sets the given entity to play an emote animation.",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 7;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x006F.md
        opcodes[0x6F] = {
            0x6F,
            "SLEEP",
            "Delays the event VM execution until ReqStack[RunPos].WaitTime has reached 0. Used as a yieldable sleep call.",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 1;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0070.md
        opcodes[0x70] = {
            0x70,
            "CHECK_RENDER_FLAG",
            "Checks the event entity for a render flag, yields if set. Otherwise, cancels the entity movement and advances.",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 1;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0072.md
        opcodes[0x72] = {
            0x72,
            "LOAD_WEATHER",
            "Appears to load event based weather information and update the weather accordingly for it.",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 10;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0075.md
        opcodes[0x75] = {
            0x75,
            "LOAD_ROOM",
            "Loads a room and updates the players sub-region with the server.",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 6;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0076.md
        opcodes[0x76] = {
            0x76,
            "FLAG_RENDER13",
            "Checks the given entities Render.Flags0 and Render.Flags3 and yields if successful.",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 5;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0079.md
        opcodes[0x79] = {
            0x79,
            "LOOK_AT",
            "Used to look at / rotate towards another entity. Also can set some unknown values in an entity.",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 10;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x007C.md
        opcodes[0x7C] = {
            0x7C,
            "FLAG_RENDER2",
            "Adjusts the given entities Render.Flags2 value.",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 6;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x007F.md
        opcodes[0x7F] = {
            0x7F,
            "WAIT_SELECT",
            "Waits for a dialog select to be made by the player.",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 1;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0080.md
        opcodes[0x80] = {
            0x80,
            "TEST",
            "Tests the given entity for several conditions. Yields or moves forward depending on the results.",
            [&](xievent_t* event)
            {
                // TODO
                // event->ExecPointer += 5;
                event->RetFlag = 1;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0081.md
        opcodes[0x81] = {
            0x81,
            "SET_WARP",
            "Sets an unknown value in the given entities warp data.",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 6;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0088.md
        opcodes[0x88] = {
            0x88,
            "SEND_WORLD_PASS",
            "Used for handling the generation of world passes. Sends 0x001B packets to handle the various world pass functionalities.",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 2;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0092.md
        opcodes[0x92] = {
            0x92,
            "FLAG_RENDER3",
            "Adjusts the given entities Render.Flags3 value.",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 6;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x0099.md
        opcodes[0x99] = {
            0x99,
            "WAIT_FOR_ANIM",
            "Yields if the given entity is playing an animation, continues otherwise.",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 5;
                // event->RetFlag = 1;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x00A4.md
        opcodes[0xA4] = {
            0xA4,
            "FLAG_RENDER3",
            "Adjusts the event entities Render.Flags3 value.",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 2;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x00A5.md
        opcodes[0xA5] = {
            0xA5,
            "FLAG_RENDER3",
            "Adjusts the event entities Render.Flags3 value.",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 2;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x00A6.md
        opcodes[0xA6] = {
            0xA6,
            "REQ_MAP_NUM",
            "Requests the event map number from the server by sending a 0x00EB packet.",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 2;
                event->RetFlag = 1;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x00B4.md
        opcodes[0xB4] = {
            0xB4,
            "REQ_MAP_NUM",
            "Handler with multiple sub-usages.",
            [&](xievent_t* event)
            {
                // TODO
                this->ExecPointer += 6;
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x00C6.md
        opcodes[0xC6] = {
            0xC6,
            "WAIT_SCH_TASK3",
            "Handler that calls the same helper call as opcode 0x0055, just with a different second argument.",
            [&](xievent_t* event)
            {
                // TODO
                event->opcodes[0x55].func(event);
            },
        };

        // https://github.com/atom0s/XiEvents/blob/main/OpCodes/0x00C8.md
        opcodes[0xC8] = {
            0xC8,
            "OPEN_MAP",
            "Opens the map window with the given parameters.",
            [&](xievent_t* event)
            {
                // TODO
                event->ExecPointer += 7;
            },
        };
    }

    ~xievent_t()
    {
    }

    uint8_t FUNC_XiEvent_eventgetcode(xievent_t* event, uint8_t index)
    {
        // TODO
        return 0; // event->EventData[this->ExecPointer + index];
    }

    void FUNC_XiEvent_setworkofs_(xievent_t* event, uint8_t index, float value)
    {
        // TODO
        // event->EventData[event->ExecPointer + index] = value;
    }

    uint8_t FUNC_XiEvent_getworkofs_(xievent_t* event, uint8_t index)
    {
        // TODO
        return 0;
    }

    void ExecProg()
    {
        /*
        for (auto const& block : dats.events.Blocks)
        {
            for (std::size_t idx = 0; idx < block.TagCount; ++idx)
            {
                // TODO: Clear state
                RetFlag = 0;
                // TODO: Don't use const_cast
                EventData = const_cast<uint8_t*>(block.EventData.data());

                uint16_t eventOffset = block.TagOffset[idx];
                uint16_t eventId     = block.EvectExecNum[idx];
                spdlog::info("Event {} (offset {})", eventId, eventOffset);

                ExecPointer = eventOffset;

                while (!RetFlag)
                {
                    try
                    {
                        if (block.EventData[ExecPointer] > 0x00D9)
                        {
                            spdlog::error("Invalid opcode: 0x{:02X}", block.EventData[ExecPointer]);
                            std::exit(-1);
                        }
                        else
                        {
                            opcode_t op = opcodes[block.EventData[ExecPointer]];
                            op.func(this);
                            spdlog::info("0x{:02X}: {}", op.code, op.name); //, op.description);
                        }
                    }
                    catch (std::exception& ex)
                    {
                        spdlog::error("Opcode: 0x{:02X}", block.EventData[ExecPointer]);
                        spdlog::error("Exception: {}", ex.what());
                        std::exit(-1);
                    }
                }
                spdlog::info("");
            }
        }
        */
    }
};
