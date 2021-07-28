#ifndef VELOCITY_PUBLISHER_H_
#define VELOCITY_PUBLISHER_H_

#include "packetcallback.h"

struct VelocityPublisher : public PacketCallback
{
    void operator()(const XsDataPacket & packet, double timestamp, MOOS::MOOSAsyncCommClient *comms)
    {
        if(packet.containsVelocity())
        {
            XsVector p = packet.velocity();
            comms->Notify("XSENS_VELOCITY_X", p[0], timestamp);
            comms->Notify("XSENS_VELOCITY_Y", p[1], timestamp);
            comms->Notify("XSENS_VELOCITY_Z", p[2], timestamp);
        }
    }
};

#endif