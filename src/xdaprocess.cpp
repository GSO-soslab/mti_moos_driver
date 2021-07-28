#include <memory>
#include <getopt.h>
#include "xdaprocess.h"

XdaProcess::XdaProcess() = default;

void XdaProcess::Run(int argc, char* argv[])
{
    char *missionFile;
    for(int c = 0; c != -1 ; c = getopt(argc, argv, "m:") )
    {
        switch (c) {
            case 'm':
                missionFile = optarg;
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
    m_device_interface->Init(missionFile);
   
    // register publishers
    m_device_interface->registerPublishers();
   
    // connect to devices
    if (!m_device_interface->connectDevice())
    {
        throw CMOOSException("can not connect to device");
    }
   
    // prepare the device for reading
    if (!m_device_interface->prepare()) {
        throw CMOOSException("can not prepare device");
    }
  
    m_device_interface->calibrate();
    
    // run it
    while (running)
    {
        auto now = std::chrono::high_resolution_clock::now();
        m_device_interface->spinFor(std::chrono::milliseconds(100));
        
        std::this_thread::sleep_until(now + std::chrono::milliseconds(20));
    }

    m_device_interface.reset();
}

void XdaProcess::Stop() {
    running = false;
}