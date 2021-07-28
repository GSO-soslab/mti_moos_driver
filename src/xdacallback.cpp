
#include "xdacallback.h"

#include <xscontroller/xsdevice_def.h>
#include <xstypes/xsdatapacket.h>

XdaCallback::XdaCallback(size_t maxBufferSize)
	: m_maxBufferSize(maxBufferSize)
{
}

XdaCallback::~XdaCallback() throw()
{
}

MOOSXsDataPacket XdaCallback::next(const std::chrono::milliseconds &timeout)
{
    MOOSXsDataPacket packet;
    
    std::unique_lock<std::mutex> lock(m_mutex);
    
    if (m_condition.wait_for(lock, timeout, [&] { return !m_buffer.empty(); }))
    {
        assert(!m_buffer.empty());
        
        packet = m_buffer.front();
        m_buffer.pop_front();
    }
    
    return packet;
}

void XdaCallback::onLiveDataAvailable(XsDevice *, const XsDataPacket *packet)
{
	std::unique_lock<std::mutex> lock(m_mutex);
    // std::chrono::duration<double, std::ratio<1>> now = std::chrono::high_resolution_clock::now().time_since_epoch();
    auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
    
	assert(packet != 0);

	// Discard oldest packet if buffer full
	if (m_buffer.size() == m_maxBufferSize)
	{
		m_buffer.pop_front();
	}

	// Push new packet
	m_buffer.push_back(MOOSXsDataPacket(now.count(), *packet));

	// Manual unlocking is done before notifying, to avoid waking up
	// the waiting thread only to block again
	lock.unlock();
	m_condition.notify_one();
}
