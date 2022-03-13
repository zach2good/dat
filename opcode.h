#pragma once

#include "common.h"

#include <array>
#include <functional>
#include <string>

class xievent_t;

struct opcode_t
{
    using func_t = std::function<void(xievent_t* event)>;

    uint8_t     code;
    std::string name;
    std::string description;
    func_t      func;
};
