

#ifndef XDACALLBACK_H
#define XDACALLBACK_H

#include <xscontroller/xscallback.h>
#include <mutex>
#include <condition_variable>
#include <list>

#include "MOOS/libMOOS/App/MOOSApp.h"

struct XsDataPacket;
struct XsDevice;

// typedef std::pair<ros::Time, XsDataPacket> RosXsDataPacket;
typedef std::pair<double, XsDataPacket> MOOSXsDataPacket;

class XdaCallback : public XsCallback
{
public:
	XdaCallback(size_t maxBufferSize = 5);
	virtual ~XdaCallback() throw();

	
	MOOSXsDataPacket next(const std::chrono::milliseconds &timeout);
    // RosXsDataPacket next(const std::chrono::milliseconds &timeout);

protected:
	void onLiveDataAvailable(XsDevice *, const XsDataPacket *packet) override;

private:
	std::mutex m_mutex;
	std::condition_variable m_condition;
    // std::list<RosXsDataPacket> m_buffer;
    std::list<MOOSXsDataPacket> m_buffer;
	size_t m_maxBufferSize;
};

#endif
