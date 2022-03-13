#include <argparse/argparse.hpp>
#include <filesystem>
#include <fstream>

#include "common.h"
#include "event_dat.h"
#include "util.h"

int main(int argc, char* argv[])
{
    argparse::ArgumentParser program(argv[0]);

    // TODO: Read from registry
    auto ffPath = std::filesystem::path("C:/Program Files (x86)/PlayOnline/SquareEnix/FINAL FANTASY XI");

    // https://github.com/atom0s/XiEvents/blob/main/Event%20DAT%20Files.md
    // 241 Windurst Woods Zone Events
    auto wwPath = std::filesystem::path(ffPath / "ROM/21/50.DAT");
    auto dat    = load_file(wwPath.generic_string());

    uint32_t blockCount = *reinterpret_cast<uint32_t*>(dat.data());
    spdlog::info("blockCount: {}", blockCount);

    auto blockSizes = std::vector<uint32_t>(blockCount);
    for (std::size_t idx = 0; idx < blockCount; ++idx)
    {
        blockSizes[idx] = *reinterpret_cast<uint32_t*>(dat.data() + 1 + idx);
    }

    spdlog::info("First ActorNumber: {}", *reinterpret_cast<uint32_t*>(dat.data() + 1 + blockCount));

    return 0;
}
