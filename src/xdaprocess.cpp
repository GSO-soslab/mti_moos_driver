#include <memory>
#include <getopt.h>
#include <cstring>
#include "xdaprocess.h"

XdaProcess::XdaProcess() = default;

void XdaProcess::Run(int argc, char* argv[])
{
    char missionFile[BUFSIZ];
    for(int c = 0; c != -1 ; c = getopt(argc, argv, "m:") )
    {
        switch (c) {
            case 'm':
                strcpy(missionFile, optarg);
                break;
            case '?':
                if (optopt == 'm') {
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                }
            default:
                break;
        }
    }
    m_device_interface = std::make_shared<XdaInterface>();
   
    // start moos
    if(strlen(missionFile) != 0) {
        MOOSTrace("Mission file: %s\n", missionFile);
        m_device_interface->Init(missionFile);
    } else {
        MOOSTrace("Args: ");
        for(int i = 0 ; i < argc - 1; i++) {
            MOOSTrace("%s, ", argv[i]);
        }   MOOSTrace("%s\n", argv[argc - 1]);

        for(int i = 0 ; i < argc; i++) {
            auto ends_with = [](std::string const &fullString, std::string const &ending) -> bool {
                if (fullString.length() >= ending.length()) {
                    return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
                } else {
                    return false;
                }
            };

            if(ends_with(std::string(argv[i]), ".moos") || ends_with(std::string(argv[i]),".moos++")) {
                strcpy(missionFile, argv[i]);
                MOOSTrace("Mission file found: %s\n", missionFile);
                m_device_interface->Init(missionFile, argc, argv);
            }
        }

    }

    if(strlen(missionFile) == 0) {
        m_device_interface->Init(argc, argv);
    }
}