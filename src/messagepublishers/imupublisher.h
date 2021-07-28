#ifndef IMUPUBLISHER_H
#define IMUPUBLISHER_H

#include "packetcallback.h"

struct ImuPublisher : public PacketCallback
{
    void operator()(const XsDataPacket &packet, double timestamp, MOOS::MOOSAsyncCommClient *comms)
    {
    
        bool quaternion_available = packet.containsOrientation();
        bool gyro_available = packet.containsCalibratedGyroscopeData();
        bool accel_available = packet.containsCalibratedAcceleration();

        if (quaternion_available)
        {
            XsQuaternion q = packet.orientationQuaternion();
    
            comms->Notify("XSENS_QUATERNION_W", q.w(), timestamp);
            comms->Notify("XSENS_QUATERNION_X", q.x(), timestamp);
            comms->Notify("XSENS_QUATERNION_Y", q.y(), timestamp);
            comms->Notify("XSENS_QUATERNION_Z", q.z(), timestamp);
    
            XsEuler e = packet.orientationEuler();
    
            comms->Notify("XSENS_ROLL", e.roll(), timestamp);
            comms->Notify("XSENS_PITCH", e.pitch(), timestamp);
            comms->Notify("XSENS_YAW", e.yaw(), timestamp);
        }

        if (gyro_available)
        {
            XsVector g = packet.calibratedGyroscopeData();
            comms->Notify("XSENS_GYRO_X", g[0], timestamp);
            comms->Notify("XSENS_GYRO_Y", g[1], timestamp);
            comms->Notify("XSENS_GYRO_Z", g[2], timestamp);
        }

        if (accel_available)
        {
            XsVector a = packet.calibratedAcceleration();
            comms->Notify("XSENS_ACCEL_X", a[0], timestamp);
            comms->Notify("XSENS_ACCEL_Y", a[1], timestamp);
            comms->Notify("XSENS_ACCEL_Z", a[2], timestamp);
        }
    }
};

#endif
