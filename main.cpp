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

    // Windurst Woods
    std::string name = data[241][1];
    entitydat_t entities(data[241][2]);
    // eventdat_t  events(data[241][3]);
    // dialogdat_t dialogs(data[241][4]);

    return 0;
}
