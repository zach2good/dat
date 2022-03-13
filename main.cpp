#include <argparse/argparse.hpp>
#include <filesystem>
#include <fstream>
#include <vector>

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
    auto eventDat = eventdat_t(ffPath / "ROM/21/50.DAT");

    spdlog::info("eventDat.Header.BlockCount: {}", eventDat.Header.BlockCount);
    for (auto const& block : eventDat.Blocks)
    {
        spdlog::info("block.ActorNumber: {:08X}", block.ActorNumber);
    }

    return 0;
}
