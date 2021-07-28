#ifndef PACKETCALLBACK_H
#define PACKETCALLBACK_H

#include "xstypes/xsdatapacket.h"
#include "MOOS/libMOOS/MOOSLib.h"

struct PacketCallback
{
    public:
        virtual void operator()(const XsDataPacket & packet, double timestamp, MOOS::MOOSAsyncCommClient *comms) = 0;
};

#endif
