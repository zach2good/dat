// Dialog Dat Example
// (c) 2023 atom0s [atom0s@live.com]

class dialog final
{
    std::vector<uint8_t> data_;

public:
    dialog(void)
    {}
    ~dialog(void)
    {}

    /**
     * Loads a dialog DAT file into the local buffer for later usage.
     *
     * @param {std::string&} file - The dialog DAT file to load.
     * @return {bool} True on success, false otherwise.
     */
    auto load(const std::string& file) -> bool
    {
        // Open the file for binary reading..
        FILE* f = nullptr;
        if (::fopen_s(&f, file.c_str(), "rb") != ERROR_SUCCESS)
            return false;

        // Obtain the file size..
        ::fseek(f, 0, SEEK_END);
        const auto size = ::ftell(f);
        ::fseek(f, 0, SEEK_SET);

        // Validate the file size; must be at least 4 bytes to be valid..
        if (size < 4)
        {
            ::fclose(f);
            return false;
        }

        // Read the file data..
        this->data_ = std::vector<uint8_t>(size, '\0');
        ::fread(this->data_.data(), 1, this->data_.size(), f);
        ::fclose(f);

        // Check if the file data is encrypted..
        if (this->data_[3] == 0x10)
        {
            // Decrypt the file data; skipping the header..
            for (auto x = 4u; x < this->data_.size(); x++)
                this->data_[x] ^= 0x80;
        }

        return true;
    }

    /**
     * Returns the total number of strings available within the loaded dialog.
     *
     * @return {uint32_t} The number of strings available.
     */
    auto size(void) const -> uint32_t
    {
        if (this->data_.size() == 0)
            return 0;

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
            return nullptr;

        // Read the offset to the string..
        const auto ptr = this->data_.data();
        const auto off = *reinterpret_cast<const uint32_t*>(ptr + 0x04 * idx + 4);

        // Return the string..
        return reinterpret_cast<const char*>(ptr + 0x04 + off);
    }
};