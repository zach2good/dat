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
    WINDURST_WOODS = 241,
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
    entitydat_t entities(entry[2]);
    //entities.print();

    // eventdat_t  events(data[241][3]);

    dialogdat_t dialogs = dialogdat_t::fromFilepath(entry[4]);
    auto startingJSON   = dialogs.toJSON();
    // spdlog::info("Original: {}", startingJSON);

    auto rebuiltDialogs = dialogdat_t::fromJSON(startingJSON);
    auto rebuiltJSON    = rebuiltDialogs.toJSON();
    // spdlog::info("Rebuilt: {}", rebuiltJSON);

    return 0;
}
