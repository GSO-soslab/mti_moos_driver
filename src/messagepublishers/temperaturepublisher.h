#ifndef TEMPERATUREPUBLISHER_H_
#define TEMPERATUREPUBLISHER_H_

#include "packetcallback.h"

struct TemperaturePublisher : public PacketCallback
{
    void operator()(const XsDataPacket &packet, double timestamp, MOOS::MOOSAsyncCommClient *comms)
    {
        if (packet.containsTemperature())
        {
            comms->Notify("XSENS_TEMPERATURE", packet.temperature(), timestamp);
        }
    }
};

#endif
