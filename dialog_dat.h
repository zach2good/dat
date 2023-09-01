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
// Constructor factories
public:
    static dialogdat_t fromFilepath(std::filesystem::path path)
    {
        spdlog::info("=== dialogdat_t::fromFilepath ===");
        dialogdat_t ddat;

        spdlog::info("Loading Dialog DAT from file: {}", path.generic_string());

        auto ffPath = util::get_ffxi_install_path();
        ddat.data_  = util::load_file_as<uint8_t>((ffPath / path).generic_string());

        spdlog::info("Total file bytes: {}", ddat.data_.size());
        spdlog::info("First 40 bytes (encrypted): {}", spdlog::fmt_lib::join(ddat.data_.begin(), ddat.data_.begin() + 40, ", "));

        // Check if the file data is encrypted..
        if (ddat.data_.data()[3] == 0x10)
        {
            // Decrypt the file data; skipping the header..
            for (auto x = 4u; x < ddat.data_.size(); x++)
            {
                ddat.data_.data()[x] ^= 0x80;
            }
        }

        spdlog::info("First 40 bytes (decrypted): {}", spdlog::fmt_lib::join(ddat.data_.begin(), ddat.data_.begin() + 40, ", "));
        spdlog::info("ddat.size(): {}", ddat.getSize());
        spdlog::info("First Entry: {}", ddat.getString(0));

        return ddat;
    }

    static dialogdat_t fromJSON(std::string jsonStr)
    {
        spdlog::info("=== dialogdat_t::fromJSON ===");
        dialogdat_t ddat;

        using json  = nlohmann::json;
        auto inJSON = json::parse(jsonStr);

        std::unordered_map<uint32_t, std::string> stringData = inJSON["data"];

        // Determine size to write out to header from data
        // TODO: Write out header bytes

        // Write out size information
        uint32_t rawSize = stringData.size(); // 60704: correct
        spdlog::info("rawSize: {}", rawSize);
        ddat.data_.resize(rawSize);
        ddat.setSize(rawSize);

        // Write back encrypt flag
        ddat.data_.data()[3] |= inJSON["encrypt"] ? 0x10 : 0x00;

        spdlog::info("numEntries: {}, encrypt: {}", stringData.size(), ddat.data_.data()[3] == 0x10);

        // Write strings back
        ddat.setString(0, stringData[0]);

        // std::size_t counter = 0;
        // for (auto const& str : data)
        // {
        //     ddat.set(counter++, str);
        // }

        // No need to decrypt, this has come from unencrypted JSON

        spdlog::info("First 40 bytes: {}", spdlog::fmt_lib::join(ddat.data_.begin(), ddat.data_.begin() + 40, ", "));
        spdlog::info("Reconstructed First Entry: {}", std::string(ddat.getString(0)));

        return ddat;
    }

    static dialogdat_t fromDATData(std::vector<uint8_t> datVec)
    {
        dialogdat_t ddat;

        // TODO

        return ddat;
    }

// Methods
public:
    /**
     * Returns the total number of strings available within the loaded dialog.
     *
     * @return {uint32_t} The number of strings available.
     */
    auto getSize() const -> uint32_t
    {
        if (this->data_.size() == 0)
        {
            spdlog::error("Invalid size");
            return 0;
        }

        return ((*reinterpret_cast<const uint32_t*>(this->data_.data() + 0x04)) - 4) >> 2;
    }

    void setSize(uint32_t newSize)
    {
        if (this->data_.size() == 0)
        {
            spdlog::error("Invalid size");
            return;
        }

        *reinterpret_cast<uint32_t*>(this->data_.data() + 0x04) = (newSize << 2) + 4;
    }

    /**
     * Returns the pointer to a string within the dialog from its index.
     *
     * @param {uint16_t} idx - The index of the string to return.
     * @return {const char*} The pointer to the string if valid, nullptr otherwise.
     */
    auto getString(const uint16_t idx) const -> const char*
    {
        if (this->getSize() < idx)
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

    void setString(const uint16_t idx, std::string const& str)
    {
        // Read the offset to the string..
        const auto ptr = this->data_.data();
        const auto off = idx + 4;
        std::memcpy(ptr + 0x04 + off, str.data(), str.size());
    }

    std::vector<std::string> toStringVector()
    {
        std::vector<std::string> outVec;

        auto numEntries = this->getSize();
        outVec.reserve(numEntries);
        for (std::size_t idx = 0; idx < numEntries; ++idx)
        {
            outVec.emplace_back(this->getString(idx));
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
        spdlog::info("=== dialogdat_t::toJSON ===");
        spdlog::info("numEntries: {}, encrypt: {}", this->getSize(), this->data_.data()[3] == 0x10);

        // Get and write all the strings into JSON
        std::unordered_map<uint32_t, std::string> outMap;
        for (std::size_t idx = 0; idx < this->getSize(); ++idx)
        {
            outMap[idx] = this->getString(idx);
        }

        json outJSON;
        outJSON["data"] = outMap;

        // This data is not trackable with the data strings, so write as its own member
        outJSON["encrypt"] = this->data_.data()[3] == 0x10;

        // Splat into utf-8 since the JSON doesn't like it
        // TODO: This seems lossy, find a better way to write the extended chars into JSON
        return outJSON.dump(4, ' ', true, json::error_handler_t::replace);
    }

private:
    dialogdat_t() = default; // Disallow construction from outside the factory constructors

    std::vector<uint8_t> data_;
};
