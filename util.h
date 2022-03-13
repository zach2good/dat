#pragma once

#include "common.h"

#include <fstream>
#include <vector>

// https://stackoverflow.com/questions/51352863/what-is-the-idiomatic-c17-standard-approach-to-reading-binary-files
template <typename T>
std::vector<T> load_file(std::string const& filepath)
{
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

    std::vector<T> buffer(size);

    if (!ifs.read((char*)buffer.data(), buffer.size()))
    {
        throw std::runtime_error(filepath + ": " + std::strerror(errno));
    }

    return buffer;
}
