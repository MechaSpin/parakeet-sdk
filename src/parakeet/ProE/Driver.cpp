/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#include <parakeet/ProE/Driver.h>

#include <parakeet/exceptions/NoResponseFromSensorException.h>
#include <parakeet/exceptions/UnableToOpenPortException.h>

namespace mechaspin
{
namespace parakeet
{
namespace ProE
{
    const int ETHERNET_MESSAGE_DATA_BUFFER_SIZE = 8192;          // Arbitrary size
    const int START_TIMEOUT_MS = 10000;
    const int MESSAGE_TIMEOUT_MS = 2000;
    const int STOP_TIMEOUT_MS = 1000;

    const std::string CW_STOP_ROTATING = "LSTOPH";
    const std::string CW_START_NORMALLY = "LSTARH";
    const std::string CW_STOP_ROTATING_FIX_DIST = "LMEASH";
    const std::string CW_RESET_AND_RESTART = "LRESTH";
    const std::string CW_VERSION_NUMBER = "LVERSH";

    const std::string CW_REQUEST_OUTPUT_TO_SCAN_THE_CLOUD_POINT = "LGCPSH";
    const std::string CW_DISABLE_OUTPUT_SCAN_CLOUD_POINTS = "LTCPSH";
    const std::string CW_SAVE_FIRMWARE_TO_FLASH = "LSFAPH";
    const std::string CW_SAVE_STRATEGY_0_TO_FLASH = "LSPZ0H";
    const std::string CW_SAVE_STRATEGY_1_TO_FLASH = "LSPZ1H";

    const std::string CW_DATA_UNIT_ACQUISITION = "LSMMDH";

    const std::string SW_SET_SPEED_PREFIX = "LSRPM:";
    const std::string SW_SET_BIAS_PREFIX = "LSERR:";
    const std::string SW_SET_LIDAR_PROPERTIES_PREFIX = "LSUDP:";
    const std::string SW_SET_OUTPUT_UNIT_OF_MEASURE_PREFIX = "LSMMU:";

    const std::string SW_SET_DATA_SMOOTHING_PREFIX = "LSSS";
    const std::string SW_SET_DRAG_POINT_REMOVAL_PREFIX = "LFFF";
    const std::string SW_SET_SMOOTH_PREFIX = "LFFF";
    const std::string SW_SET_RESAMPLE_FILTER_PREFIX = "LSRES:00";

    const std::string SW_POSTFIX = "H";

    const std::string SW_SET_SPEED(int speed)
    {
        return SW_SET_SPEED_PREFIX + std::to_string(speed) + SW_POSTFIX;
    }
    
    const std::string SW_SET_LIDAR_PROPERTIES(const std::string& ipAddress, const std::string& subnetMask, const std::string& gateway, int port)
    {
        std::string delimiter = " ";
        return SW_SET_LIDAR_PROPERTIES_PREFIX + 
            ipAddress + delimiter +
            subnetMask + delimiter +
            gateway + delimiter +
            std::to_string(port) + 
            SW_POSTFIX;
    }

    const std::string SW_SET_BIAS(int bias)
    {
        return SW_SET_BIAS_PREFIX + std::to_string(bias) + SW_POSTFIX;
    }

    const std::string SW_SET_DATA_SMOOTHING(bool enable)
    {
        return SW_SET_DATA_SMOOTHING_PREFIX + std::to_string((int)enable) + SW_POSTFIX;
    }

    const std::string SW_SET_DRAG_POINT_REMOVAL(bool enable)
    {
        return SW_SET_DRAG_POINT_REMOVAL_PREFIX + std::to_string((int)enable) + SW_POSTFIX;
    }

    const std::string SW_SET_OUTPUT_UNIT_OF_MEASURE(bool mm)
    {
        return SW_SET_OUTPUT_UNIT_OF_MEASURE_PREFIX + std::to_string((int)mm) + SW_POSTFIX;
    }

    const std::string SW_SET_SMOOTH(bool enable)
    {
        return SW_SET_SMOOTH_PREFIX + std::to_string((int)enable) + SW_POSTFIX;
    }

    const std::string SW_SET_RESAMPLE_FILTER(bool enable)
    {
        return SW_SET_RESAMPLE_FILTER_PREFIX + std::to_string((int)enable) + SW_POSTFIX;
    }
    
    Driver::Driver() : parser(std::bind(&Driver::onCompleteLidarMessage, this, std::placeholders::_1))
    {
        this->registerUpdateThreadCallback(std::bind(&Driver::ethernetUpdateThreadFunction, this));

        ethernetPortDataBuffer = new unsigned char[ETHERNET_MESSAGE_DATA_BUFFER_SIZE];
    }

    Driver::~Driver()
    {
        close();

        delete ethernetPortDataBuffer;
    }

    void Driver::connect(const SensorConfiguration& sensorConfiguration)
    {
        this->sensorConfiguration = sensorConfiguration;

        open();
    }

    void Driver::open()
    {
        if (ethernetPort.open(sensorConfiguration.ipAddress.c_str(), sensorConfiguration.lidarPort, sensorConfiguration.localPort))
        {
            sendMessageWaitForResponseOrTimeout(CW_STOP_ROTATING, STOP_TIMEOUT_MS);
        }
        else
        {
            throw exceptions::UnableToOpenPortException();
        }
    }

    void Driver::close()
    {
        mechaspin::parakeet::Driver::close();

        ethernetPort.close();
    }

    void Driver::start()
    {
        ethernetPortDataBufferLength = 0;

        parser.reset();

        enableIntensityData(sensorConfiguration.intensity);
        enableDataSmoothing(sensorConfiguration.dataSmoothing);
        enableRemoveDragPoint(sensorConfiguration.dragPointRemoval);
        setScanningFrequency_Hz(sensorConfiguration.scanningFrequency_Hz);
        enableResampleFilter(sensorConfiguration.resampleFilter);

        if (!sendMessageWaitForResponseOrTimeout(CW_START_NORMALLY, START_TIMEOUT_MS))
        {
            throw exceptions::NoResponseFromSensorException();
        }

        mechaspin::parakeet::Driver::start();
    }

    void Driver::stop()
    {
        sendMessageWaitForResponseOrTimeout(CW_STOP_ROTATING, STOP_TIMEOUT_MS);

        mechaspin::parakeet::Driver::stop();
    }

    void Driver::enableDataSmoothing(bool enable)
    {
        throwExceptionIfNotConnected();

        sendMessageWaitForResponseOrTimeout(SW_SET_DATA_SMOOTHING(enable), MESSAGE_TIMEOUT_MS);

        sensorConfiguration.dataSmoothing = enable;
    }

    void Driver::enableRemoveDragPoint(bool enable)
    {
        throwExceptionIfNotConnected();

        sendMessageWaitForResponseOrTimeout(SW_SET_DRAG_POINT_REMOVAL(enable), MESSAGE_TIMEOUT_MS);

        sensorConfiguration.dragPointRemoval = enable;
    }

    void Driver::enableIntensityData(bool enable)
    {
        sensorConfiguration.intensity = enable;
    }

    void Driver::enableResampleFilter(bool enable)
    {
        throwExceptionIfNotConnected();

        sendMessageWaitForResponseOrTimeout(SW_SET_RESAMPLE_FILTER(enable), MESSAGE_TIMEOUT_MS);

        sensorConfiguration.resampleFilter = enable;
    }

    void Driver::setScanningFrequency_Hz(ScanningFrequency Hz)
    {
        throwExceptionIfNotConnected();

        sendMessageWaitForResponseOrTimeout(SW_SET_SPEED(Hz * 60), MESSAGE_TIMEOUT_MS);

        sensorConfiguration.scanningFrequency_Hz = Hz;
    }

    bool Driver::isDataSmoothingEnabled()
    {
        throwExceptionIfNotConnected();

        return sensorConfiguration.dataSmoothing;
    }

    bool Driver::isDragPointRemovalEnabled()
    {
        throwExceptionIfNotConnected();

        return sensorConfiguration.dragPointRemoval;
    }

    bool Driver::isIntensityDataEnabled()
    {
        return sensorConfiguration.intensity;
    }

    Driver::ScanningFrequency Driver::getScanningFrequency_Hz()
    {
        throwExceptionIfNotConnected();

        return sensorConfiguration.scanningFrequency_Hz;
    }

    bool Driver::isResampleFilterEnabled()
    {
        throwExceptionIfNotConnected();

        return sensorConfiguration.resampleFilter;
    }

    void Driver::ethernetUpdateThreadFunction()
    {
        if (!ethernetPort.isConnected())
        {
            return;
        }

        readWriteMutex.lock();

        int charsRead = ethernetPort.read(ethernetPortDataBuffer, ethernetPortDataBufferLength, ETHERNET_MESSAGE_DATA_BUFFER_SIZE);

        readWriteMutex.unlock();
        
        if (charsRead == 0)
        {
            return;
        }

        ethernetPortDataBufferLength += charsRead;

        unsigned int nl = parser.parseSensorBuffer(ethernetPortDataBufferLength, ethernetPortDataBuffer);

        for (unsigned int i = nl; i < ethernetPortDataBufferLength; i++)
        {
            ethernetPortDataBuffer[i - nl] = ethernetPortDataBuffer[i];
        }
        ethernetPortDataBufferLength -= nl;
    }

    bool Driver::isConnected()
    {
        return ethernetPort.isConnected();
    }

    void Driver::onCompleteLidarMessage(internal::Parser::CompleteLidarMessage* lidarMessage)
    {
        ScanData* scanData = new ScanData(lidarMessage->timestamp);
        scanData->count = lidarMessage->totalPoints;
        scanData->startAngle_deg = lidarMessage->startAngle;
        scanData->endAngle_deg = lidarMessage->endAngle;

        for (int i = 0; i < lidarMessage->lidarPoints.size(); i++)
        {
            scanData->dist_mm[i] = lidarMessage->lidarPoints[i].distance;
            scanData->intensity[i] = lidarMessage->lidarPoints[i].intensity;
        }

        onScanDataReceived(scanData);

        delete lidarMessage;
    }

    bool Driver::sendMessageWaitForResponseOrTimeout(const std::string& message, int millisecondsTilTimeout)
    {
        readWriteMutex.lock();

        bool state = ethernetPort.sendMessageWaitForResponseOrTimeout(message, "OK", std::chrono::milliseconds(millisecondsTilTimeout));

        readWriteMutex.unlock();

        return state;
    }
}
}
}
