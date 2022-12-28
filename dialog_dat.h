#pragma once

#include "common.h"
#include "util.h"

#include <filesystem>

struct dialogdat_t
{
    dialogdat_t(std::filesystem::path path)
    {
        spdlog::info("Loading Dialog DAT: {}", path.generic_string());

        auto  ffPath = util::get_ffxi_install_path();
        auto  dat    = util::load_file_as<uint32_t>((ffPath / path).generic_string());
        auto* data   = dat.data();

        // https://github.com/Windower/POLUtils/blob/master/PlayOnline.FFXI/FileTypes/DialogTable.cs

        uint32_t size = data[0];
        spdlog::info(fmt::format("size: {}", size));
        if (size != 0x10000000 + dat.size() - 4)
        {
            spdlog::info("Invalid size");
            return;
        }

        uint32_t startPos = data[1] ^ 0x80808080;
        spdlog::info(fmt::format("startPos: {}", startPos));
        if (startPos % 4 != 0)
        {
            spdlog::info("Invalid startPos");
            return;
        }

        uint32_t entryCount = startPos / 4;
        spdlog::info(fmt::format("entryCount: {}", entryCount));

        std::vector<uint32_t> entries;

        for (uint32_t idx = 1; idx < entryCount; ++idx)
        {
            entries.emplace_back(data[idx] ^ 0x80808080);
        }
        entries.emplace_back((uint32_t)dat.size() - 4);

        std::sort(entries.begin(), entries.end());
    }
};
