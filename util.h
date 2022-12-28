#pragma once

#include "common.h"

#include <filesystem>
#include <fstream>
#include <vector>

#include <windows.h>
#include <winreg.h>

namespace util
{
    template <typename T>
    std::vector<T> load_file_as(std::string const& filepath)
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

    std::string load_file_as_string(std::string name)
    {
        std::ifstream ifs(name);
        return std::string((std::istreambuf_iterator<char>(ifs)),
                           (std::istreambuf_iterator<char>()));
    }

    // https://docs.microsoft.com/en-us/windows/win32/api/winreg/nf-winreg-reggetvaluea
    std::filesystem::path get_ffxi_install_path()
    {
        auto keys = {
            R"(SOFTWARE\PlayOnline\InstallFolder)",
            R"(SOFTWARE\PlayOnlineUS\InstallFolder)",
            R"(SOFTWARE\PlayOnlineEU\InstallFolder)"
        };

        std::string value;

        uint32_t size = 512;
        value.resize(size);

        for (const auto& key : keys)
        {
            auto res = RegGetValue(HKEY_LOCAL_MACHINE, key, "0001", RRF_RT_REG_SZ | RRF_SUBKEY_WOW6432KEY, nullptr,
                                   (void*)value.data(), (LPDWORD)&size);

            if (res == ERROR_SUCCESS)
            {
                value.resize(size - 1);
            }
        }

        return std::filesystem::path(value);
    }

    std::vector<std::string> split(std::string const& s, std::string const& delimiter)
    {
        std::size_t pos_start = 0;
        std::size_t pos_end, delim_len = delimiter.length();
        std::string token;

        std::vector<std::string> res;

        while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos)
        {
            token     = s.substr(pos_start, pos_end - pos_start);
            pos_start = pos_end + delim_len;
            res.push_back(token);
        }

        res.push_back(s.substr(pos_start));
        return res;
    }
} // namespace util
