#pragma once

#include <fstream>
#include <vector>

#include "common.h"

// https://stackoverflow.com/questions/51352863/what-is-the-idiomatic-c17-standard-approach-to-reading-binary-files
std::vector<uint8_t> load_file(std::string const& filepath)
{
    spdlog::info("load_file: {}", filepath);

    std::ifstream ifs(filepath, std::ios::binary | std::ios::ate);

    if (!ifs)
    {
        throw std::runtime_error(filepath + ": " + std::strerror(errno));
    }

    auto end = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    auto size = std::size_t(end - ifs.tellg());

    if (size == 0)
    {
        return {};
    }

    std::vector<uint8_t> buffer(size);

    if (!ifs.read((char*)buffer.data(), buffer.size()))
    {
        throw std::runtime_error(filepath + ": " + std::strerror(errno));
    }

    spdlog::info("size: {}", buffer.size());

    return buffer;
}
