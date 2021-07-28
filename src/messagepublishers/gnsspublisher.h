#ifndef GNSSPUBLISHER_H
#define GNSSPUBLISHER_H

#include "packetcallback.h"

#define FIX_TYPE_2D_FIX (2)
#define FIX_TYPE_3D_FIX (3)
#define FIX_TYPE_GNSS_AND_DEAD_RECKONING (4)

struct GnssPublisher : public PacketCallback
{
    
    void operator()(const XsDataPacket &packet, double timestamp, MOOS::MOOSAsyncCommClient *comms)
    {
        if (packet.containsRawGnssPvtData())
        {
            XsRawGnssPvtData gnss = packet.rawGnssPvtData();
            
   
            comms->Notify("XSENS_LATITUDE", (double)gnss.m_lat * 1e-7, timestamp);
            comms->Notify("XSENS_LONGITUDE", (double)gnss.m_lon * 1e-7, timestamp);
            comms->Notify("XSENS_ALTITUDE", (double)gnss.m_height * 1e-7, timestamp);
    
            // Position covariance [m^2], ENU
            /* // we don't need those values for our application but i'm leaving them here for documentation purposes
            double sh = ((double)gnss.m_hAcc * 1e-3);
            double sv = ((double)gnss.m_vAcc * 1e-3);
            auto position_covariance = {sh * sh, 0, 0, 0, sh * sh, 0, 0, 0, sv * sv}
            */
    
            switch (gnss.m_fixType)
            {
                case FIX_TYPE_2D_FIX: // fall through
                case FIX_TYPE_3D_FIX: // fall through
                case FIX_TYPE_GNSS_AND_DEAD_RECKONING:
                    comms->Notify("XSENS_GNSS_FIX", (double)0, timestamp);
                    break;
                default:
                    comms->Notify("XSENS_GNSS_FIX", -1, timestamp);
            }
        }
    }
};

#endif
