#pragma once

#include "common.h"
#include "util.h"

#include <filesystem>

struct entitydat_t
{
    entitydat_t(std::filesystem::path path)
    {
        spdlog::info("=== entitydat_t ===");
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

        // Debug print
        spdlog::info("numEntities: {}", index);
        for (std::size_t idx = 0; idx < 5; ++idx)
        {
            spdlog::info("{}: {}", idx, entityNames[idx]);
        }
    }

    void print()
    {
        std::size_t counter = 0;
        for (auto entry : entityNames)
        {
            if (entry != "")
            {
                spdlog::info(fmt::format("{}: {}", counter++, entry));
            }
        }
        spdlog::info(fmt::format(">>> Populated entries: {}", counter));
    }

    std::array<std::string, 1024> entityNames;
};
