#pragma once

#include "common.h"
#include "util.h"

#include <filesystem>

struct entitydat_t
{
    entitydat_t(std::filesystem::path path)
    {
        spdlog::info("Loading Entity DAT: {}", path.generic_string());

        auto  ffPath = util::get_ffxi_install_path();
        auto  dat    = util::load_file_as<char>((ffPath / path).generic_string());

        std::size_t index  = 0;
        std::size_t offset = 0;
        while (offset < dat.size())
        {
            auto name = std::string(dat.data() + offset, dat.data() + offset + 28); // Ignore last 4 bytes
            entityNames[index] = name;
            ++index;
            offset += 32;
        }
    }

    std::array<std::string, 1024> entityNames;
};
