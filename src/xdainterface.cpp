
#include "xdainterface.h"

#include <xscontroller/xsscanner.h>
#include <xscontroller/xscontrol_def.h>
#include <xscontroller/xsdevice_def.h>
#include <thread>

#include "messagepublishers/all_publishers.h"

XdaInterface::XdaInterface()
	: m_device(nullptr)
{
	MOOSTrace("Creating XsControl object...");
	m_control = XsControl::construct();
	assert(m_control != 0);
}

void XdaInterface::Init(const char * MissionFile)
{
    MOOSTrace("Creating XsControl object...");
    
    m_th = std::thread(
            [this, MissionFile] {
                Run("xsense_mti_moos_driver", MissionFile);
            }
    );
}


XdaInterface::~XdaInterface()
{
	MOOSTrace("Cleaning up ...");
	close();
	m_control->destruct();
}

void XdaInterface::spinFor(std::chrono::milliseconds timeout)
{
	MOOSXsDataPacket packet = m_xdaCallback.next(timeout);

	if (!packet.second.empty())
	{
		for (auto &cb : m_callbacks)
		{
			cb->operator()(packet.second, packet.first, &m_Comms);
		}
	}
}

void XdaInterface::registerPublishers()
{

    
    registerCallback(new ImuPublisher());
    registerCallback(new TemperaturePublisher());
    registerCallback(new MagneticFieldPublisher());
    registerCallback(new VelocityPublisher());
}

bool XdaInterface::connectDevice() {
    
    // Read baudrate parameter if set
    int baudrateParam = 115200;
    m_MissionReader.GetConfigurationParam("baudrate", baudrateParam);
    XsBaudRate baudrate = XsBaud::numericToRate(baudrateParam);
   
    // Read device ID parameter
    bool checkDeviceID = false;
    std::string deviceId;
    if(m_MissionReader.GetConfigurationParam("device_id", deviceId))
    {
        checkDeviceID = true;
    }
    
    
    // Read port parameter if set
    XsPortInfo mtPort;
    std::string portName;
    if(m_MissionReader.GetConfigurationParam("port", portName))
    {
        mtPort = XsPortInfo(portName, baudrate);
        
        if(!XsScanner::scanPort(mtPort, baudrate))
        {
            return handleError("No MTi device found. Verify port and baudrate.");
        }
        
        if(checkDeviceID && mtPort.deviceId().toString().c_str() != deviceId)
        {
            return handleError("No MTi device found with matching device ID.");
        }
    }
    else
    {
        MOOSTrace("Scanning for devices...");
        XsPortInfoArray portInfoArray = XsScanner::scanPorts(baudrate);
    
        for (auto const &portInfo : portInfoArray)
        {
            if (portInfo.deviceId().isMti() || portInfo.deviceId().isMtig())
            {
                if (checkDeviceID)
                {
                    if (portInfo.deviceId().toString().c_str() == deviceId)
                    {
                        mtPort = portInfo;
                        break;
                    }
                }
                else
                {
                    mtPort = portInfo;
                    break;
                }
            }
        }
    }
    
    
    if (mtPort.empty())
    {
        return handleError("No MTi device found.");
    }
    
   
    if (!m_control->openPort(mtPort))
    {
        return handleError("Could not open port");
    }
    
    m_device = m_control->device(mtPort.deviceId());
    assert(m_device != 0);
    
   
    m_device->addCallbackHandler(&m_xdaCallback);
    
    return true;
}


bool XdaInterface::prepare()
{
	assert(m_device != 0);

	if (!m_device->gotoConfig()) {
        return handleError("Could not go to config");
    }

	// read EMTS and device config stored in .mtb file header.
	if (!m_device->readEmtsAndDeviceConfiguration()) {
        return handleError("Could not read device configuration");
    }
	
    XsOutputConfigurationArray configArray;
	
    configArray.push_back(XsOutputConfiguration(XDI_PacketCounter, 0));
    configArray.push_back(XsOutputConfiguration(XDI_SampleTimeFine, 0));
    configArray.push_back(XsOutputConfiguration(XDI_Quaternion, 50));
    configArray.push_back(XsOutputConfiguration(XDI_Acceleration, 20));
    configArray.push_back(XsOutputConfiguration(XDI_RateOfTurn, 20));
    configArray.push_back(XsOutputConfiguration(XDI_MagneticField, 20));
    configArray.push_back(XsOutputConfiguration(XDI_Temperature, 5));
    configArray.push_back(XsOutputConfiguration(XDI_EulerAngles, 50));
    
    m_device->setOutputConfiguration(configArray);
	
    if (!m_device->gotoMeasurement()) {
        return handleError("Could not put device into measurement mode");
    }

	std::string logFile;
	if(m_MissionReader.GetConfigurationParam("log_file", logFile))
	{
		if (m_device->createLogFile(logFile) != XRV_OK) {
            return handleError("Failed to create a log file! (" + logFile + ")");
        }
		else
		{
            MOOSTrace("Created a log file: %s", logFile.c_str());
        }

		MOOSTrace("Recording to %s ...", logFile.c_str());
		if (!m_device->startRecording()) {
            return handleError("Could not start recording");
        }
	}

	return true;
}

void XdaInterface::close()
{
	if (m_device != nullptr)
	{
		m_device->stopRecording();
		m_device->closeLogFile();
		m_device->removeCallbackHandler(&m_xdaCallback);
	}
	m_control->closePort(m_port);
}

void XdaInterface::registerCallback(PacketCallback *cb)
{
	m_callbacks.push_back(cb);
}

bool XdaInterface::handleError(std::string error)
{
	// ROS_ERROR("%s", error.c_str());
	close();
	return false;
}

///////////////////// MOOS ///////////////////////////////

bool XdaInterface::OnNewMail (MOOSMSG_LIST &NewMail)
{
    MOOSMSG_LIST::iterator p;
    for( p = NewMail.begin() ; p != NewMail.end() ; p++ )
    {
        
        // todo: parser for each sensor measurements
        // todo: parser for helm behaviours
        
        CMOOSMsg & rMsg = *p;
       
        // SENSORY INFORMATION INPUT
        rMsg.GetTime();
        rMsg.GetType();
        rMsg.GetKey();
        rMsg.IsDouble();
        rMsg.IsString();
        
    }
    return true;
}

/*
   called by the base class when the application has made contact with
   the MOOSDB and a channel has been opened . Place code to specify what
   notifications you want to receive here .
*/
bool XdaInterface::OnConnectToServer()
{
    return true;
}

/*
  Called by the base class periodically. This is where you place code
  which does the work of the application
*/
bool XdaInterface::Iterate()
{
    return true;
}

/*
   called by the base class before the first :: Iterate is called . Place
   startup code here − especially code which reads configuration data from the
   mission file
*/
bool XdaInterface::OnStartUp()
{
    appTick = 10;
    commsTick = 100;
    
    if(!m_MissionReader.GetConfigurationParam("AppTick",appTick))
    {
        MOOSTrace("Warning, AppTick not set.\n");
    }
    
    if(!m_MissionReader.GetConfigurationParam("CommsTick",commsTick))
    {
        MOOSTrace("Warning, CommsTick not set.\n");
    }
    
    SetAppFreq(appTick);
    SetCommsFreq(commsTick);
    
    return true;
}

void XdaInterface::calibrate() {
    
    m_device->initializeSoftwareCalibration();
   
}