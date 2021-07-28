#ifndef XSENS_MTI_DRIVER_XDAPROCESS_H
#define XSENS_MTI_DRIVER_XDAPROCESS_H

#include "memory"
#include "MOOS/libMOOS/MOOSLib.h"
#include "xdainterface.h"

class XdaProcess {
private:
    std::shared_ptr<XdaInterface> m_device_interface;
    
    bool running = true;
public:
    XdaProcess();
    void Run(int argc, char* argv[]);
    
    void Stop();
};

#endif //XSENS_MTI_DRIVER_XDAPROCESS_H
