// _Heavily_ based on:
// docs/dialog.hpp
// Dialog Dat Example
// (c) 2023 atom0s [atom0s@live.com]
#pragma once

#include "common.h"
#include "util.h"

#include <filesystem>
#include <vector>

#include <nlohmann/json.hpp>

class dialogdat_t
{
public:
    static dialogdat_t fromFilepath(std::filesystem::path path)
    {
        dialogdat_t ddat;

        spdlog::info("Loading Dialog DAT from file: {}", path.generic_string());

        auto ffPath = util::get_ffxi_install_path();
        ddat.data_  = util::load_file_as<uint8_t>((ffPath / path).generic_string());

        spdlog::info("Size: {}", ddat.data_.size());
        spdlog::info("First 20 bytes (encrypted): {}", spdlog::fmt_lib::join(ddat.data_.begin(), ddat.data_.begin() + 20, ", "));

        // Check if the file data is encrypted..
        if (ddat.data_.data()[3] == 0x10)
        {
            // Decrypt the file data; skipping the header..
            for (auto x = 4u; x < ddat.data_.size(); x++)
            {
                ddat.data_.data()[x] ^= 0x80;
            }
        }

        spdlog::info("First 20 bytes (decrypted): {}", spdlog::fmt_lib::join(ddat.data_.begin(), ddat.data_.begin() + 20, ", "));
        spdlog::info("First Entry: {}", ddat.get(0));

        return ddat;
    }

    static dialogdat_t fromJSON(std::string jsonStr)
    {
        dialogdat_t ddat;

        using json  = nlohmann::json;
        auto inJSON = json::parse(jsonStr);

        std::vector<std::string> data = inJSON["data"];

        // Determine size to write out to header from data
        // Reverse of:
        // (*reinterpret_cast<const uint32_t*>(this->data_.data() + 0x04) - 4) >> 2;
        uint32_t numEntries = data.size();
        uint32_t rawSize = (numEntries << 4) + 1; // TODO

        ddat.data_.resize(rawSize);

        // Write header back
        *reinterpret_cast<uint32_t*>(ddat.data_.data()) = rawSize;

        spdlog::info("Size: {}", ddat.data_.size());
        spdlog::info("First 20 bytes (encrypted): {}", spdlog::fmt_lib::join(ddat.data_.begin(), ddat.data_.begin() + 20, ", "));
        spdlog::info("First 20 bytes (decrypted): {}", spdlog::fmt_lib::join(ddat.data_.begin(), ddat.data_.begin() + 20, ", "));
        spdlog::info("Reconstructed First Entry: {}", ddat.get(0));

        return ddat;
    }

    static dialogdat_t fromDATData(std::vector<uint8_t> datVec)
    {
        dialogdat_t ddat;

        // TODO

        return ddat;
    }

    /**
     * Returns the total number of strings available within the loaded dialog.
     *
     * @return {uint32_t} The number of strings available.
     */
    auto size(void) const -> uint32_t
    {
        if (this->data_.size() == 0)
        {
            spdlog::error("Unpopulated size requested");
            return 0;
        }

        return (*reinterpret_cast<const uint32_t*>(this->data_.data() + 0x04) - 4) >> 2;
    }

    /**
     * Returns the pointer to a string within the dialog from its index.
     *
     * @param {uint16_t} idx - The index of the string to return.
     * @return {const char*} The pointer to the string if valid, nullptr otherwise.
     */
    auto get(const uint16_t idx) const -> const char*
    {
        if (this->size() < idx)
        {
            spdlog::error("Invalid index requested: {}", idx);
            return nullptr;
        }

        // Read the offset to the string..
        const auto ptr = this->data_.data();
        const auto off = *reinterpret_cast<const uint32_t*>(ptr + 0x04 * idx + 4);

        // Return the string..
        return reinterpret_cast<const char*>(ptr + 0x04 + off);
    }

    std::vector<std::string> toStringVector()
    {
        std::vector<std::string> outVec;

        auto numEntries = this->size();
        outVec.reserve(numEntries);
        for (std::size_t idx = 0; idx < numEntries; ++idx)
        {
            outVec.emplace_back(this->get(idx));
        }
        return outVec;
    }

    std::vector<uint8_t> toDAT()
    {
        // TODO:
    }

    std::string toJSON()
    {
        using json = nlohmann::json;
        json outJSON;
        for (std::size_t idx = 0; idx < this->size(); ++idx)
        {
            outJSON["data"][idx] = this->get(idx);
        }
        // Splat into utf-8 since the JSON doesn't like it
        return outJSON.dump(4, ' ', true, json::error_handler_t::replace);
    }

private:
    dialogdat_t() = default;

    std::vector<uint8_t> data_;
};
