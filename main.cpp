#include "common.h"
#include "util.h"
#include "xievent.h"

#include <argparse/argparse.hpp>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

enum ZONES
{
    SOUTHERN_SANDORIA  = 230,
    NORTHERN_SANDORIA  = 231,
    PORT_SANDORIA      = 232,
    CHATEAU_DORAGUILLE = 233,
    BASTOK_MINES       = 234,
    BASTOK_MARKETS     = 235,
    PORT_BASTOK        = 236,
    METALWORKS         = 237,
    WINDURST_WATERS    = 238,
    WINDURST_WALLS     = 239,
    PORT_WINDURST      = 240,
    WINDURST_WOODS     = 241,
    HEAVENS_TOWER      = 242,
    RULUDE_GARDENS     = 243,
    UPPER_JEUNO        = 244,
    LOWER_JEUNO        = 245,
    PORT_JEUNO         = 246,
};

int main(int argc, char* argv[])
{
    argparse::ArgumentParser program(argv[0]);

    json data;
    {
        std::string str = util::load_file_as_string("event_dats.json");
        try
        {
            data = json::parse(str);
        }
        catch (const std::exception& e)
        {
            std::cerr << str << '\n';
            std::cerr << e.what() << '\n';
            return -1;
        }
    }

    auto entry = data[ZONES::WINDURST_WOODS];
    std::string name = entry[1];
    spdlog::info("Loading: {}", name);

    entitydat_t entities(entry[2]);
    //entities.print();

    // eventdat_t events(data[241][3]);

    dialogdat_t dialogs = dialogdat_t::fromFilepath(entry[4]);
    auto startingJSON   = dialogs.toJSON();
    // spdlog::info("Original: {}", startingJSON);

    auto rebuiltDialogs = dialogdat_t::fromJSON(startingJSON);
    // auto rebuiltJSON    = rebuiltDialogs.toJSON();
    // spdlog::info("Rebuilt: {}", rebuiltJSON);

    return 0;
}
