#include "Options.h"
#include <algorithm>
#include <iostream>
FSmceditOptions Options::parse(int argc, char *argv[]) {
    if (argc < 3) {
        std::cout << "Usage: FSmcedit --world <world_folder> --struct <mcstructure_file>\n";
        return { .ok = false };
    }
    FSmceditOptions res;
    bool expectWorld = false;
    bool expectStruct = false;
    for (int i = 1; i < argc; i++) {
        std::string option = argv[i];

        if (expectWorld) {
            std::string worldFolder = option;
            // replace backslash with forward slash
            std::replace(worldFolder.begin(), worldFolder.end(), '\\', '/');
            // Remove trailing / \ or space
            while (worldFolder.ends_with(' '))
                worldFolder.pop_back();
            while ((worldFolder.ends_with('/'))) {
                worldFolder.pop_back();
            }
            res.worldPath = worldFolder;
            expectWorld = false;
            continue;
        } else if (expectStruct) {
            res.structPath = option;
            expectStruct = false;
            continue;
        }

        if (option == "--world") {
            expectWorld = true;
        } else if (option == "--struct") {
            expectStruct = true;
        } else {
            std::cout << "![WARN]:ignoring unknown option: \'" << option << "\'\n";
        }
    }

    return res;
}
