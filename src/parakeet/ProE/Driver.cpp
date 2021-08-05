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
    const int START_TIMEOUT_MS = 10000;
    const int MESSAGE_TIMEOUT_MS = 2000;
    const int STOP_TIMEOUT_MS = 1000;

    const int IP_ADDRESS_ARRAY_SIZE = 4;
    const int SUBNET_MASK_ARRAY_SIZE = 4;
    const int GATEWAY_ARRAY_SIZE = 4;
    const int IP_ADDRESS_STRING_LENGTH = 3;
    const int PORT_STRING_LENGTH = 5;

    const unsigned short UDP_MESSAGE_SIGN = 0x484C;
    const unsigned short UDP_MESSAGE_CMD = 0x0043;
    const unsigned short UDP_MESSAGE_SET_PROPERTIES_CMD = 0x0053;

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

    const char SW_SET_LIDAR_PROPERTIES_DELIMITER = ' ';

    const std::string SW_SET_SPEED(int speed)
    {
        return SW_SET_SPEED_PREFIX + std::to_string(speed) + SW_POSTFIX;
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

    std::string numberToFixedSizeString(unsigned int value, int size)
    {
        unsigned int uvalue = value;
        
        std::string result;
        while (size-- > 0)
        {
            result += ('0' + uvalue % 10);
            uvalue /= 10;
        }

        std::reverse(result.begin(), result.end());
        return result;
    }

    std::string unsignedCharArrayToString(const unsigned char* charArray, int size)
    {
        std::string result;

        for (int i = 0; i < size; i++)
        {
            result += numberToFixedSizeString(charArray[i], 3);

            if (i != size - 1)
            {
                result += '.';
            }
        }

        return result;
    }

    const std::string SW_SET_LIDAR_PROPERTIES(const unsigned char* ipAddress, const unsigned char* subnetMask, const unsigned char* gateway, const unsigned short port)
    {
        std::string result;

        result += SW_SET_LIDAR_PROPERTIES_PREFIX;

        result += unsignedCharArrayToString(ipAddress, IP_ADDRESS_ARRAY_SIZE) + SW_SET_LIDAR_PROPERTIES_DELIMITER;
        result += unsignedCharArrayToString(subnetMask, SUBNET_MASK_ARRAY_SIZE) + SW_SET_LIDAR_PROPERTIES_DELIMITER;
        result += unsignedCharArrayToString(gateway, GATEWAY_ARRAY_SIZE) + SW_SET_LIDAR_PROPERTIES_DELIMITER;
        result += numberToFixedSizeString(port, PORT_STRING_LENGTH);

        result += SW_POSTFIX;

        return result;
    }

    struct CmdHeader
    {
        unsigned short sign;
        unsigned short cmd;
        unsigned short sn;
        unsigned short len;
    }; 
    
    Driver::Driver() : parser(std::bind(&Driver::onCompleteLidarMessage, this, std::placeholders::_1))
    {
        this->registerUpdateThreadCallback(std::bind(&Driver::ethernetUpdateThreadFunction, this));

        bufferData.buffer = ethernetPortDataBuffer;
    }

    Driver::~Driver()
    {
        close();
    }

    void Driver::connect(const SensorConfiguration& sensorConfiguration)
    {
        this->sensorConfiguration = sensorConfiguration;

        open();
    }

    void Driver::open()
    {
        if (ethernetPort.open(sensorConfiguration.srcPort))
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
        bufferData.length = 0;

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
        assertIsConnected();

        sendMessageWaitForResponseOrTimeout(SW_SET_DATA_SMOOTHING(enable), MESSAGE_TIMEOUT_MS);

        sensorConfiguration.dataSmoothing = enable;
    }

    void Driver::enableRemoveDragPoint(bool enable)
    {
        assertIsConnected();

        sendMessageWaitForResponseOrTimeout(SW_SET_DRAG_POINT_REMOVAL(enable), MESSAGE_TIMEOUT_MS);

        sensorConfiguration.dragPointRemoval = enable;
    }

    void Driver::enableIntensityData(bool enable)
    {
        sensorConfiguration.intensity = enable;
    }

    void Driver::enableResampleFilter(bool enable)
    {
        assertIsConnected();

        sendMessageWaitForResponseOrTimeout(SW_SET_RESAMPLE_FILTER(enable), MESSAGE_TIMEOUT_MS);

        sensorConfiguration.resampleFilter = enable;
    }

    void Driver::setScanningFrequency_Hz(ScanningFrequency Hz)
    {
        assertIsConnected();

        sendMessageWaitForResponseOrTimeout(SW_SET_SPEED(Hz * 60), MESSAGE_TIMEOUT_MS);

        sensorConfiguration.scanningFrequency_Hz = Hz;
    }

    void Driver::setIPv4Settings(const unsigned char* ipAddress, const unsigned char* subnetMask, const unsigned char* gateway, const unsigned short port)
    {
        assertIsConnected();

        sendMessageWaitForResponseOrTimeout(SW_SET_LIDAR_PROPERTIES(ipAddress, subnetMask, gateway, port), MESSAGE_TIMEOUT_MS, UDP_MESSAGE_SET_PROPERTIES_CMD);

        sensorConfiguration.dstPort = port;
        sensorConfiguration.ipAddress = unsignedCharArrayToString(ipAddress, IP_ADDRESS_ARRAY_SIZE);

        close();
    }

    bool Driver::isDataSmoothingEnabled()
    {
        assertIsConnected();

        return sensorConfiguration.dataSmoothing;
    }

    bool Driver::isDragPointRemovalEnabled()
    {
        assertIsConnected();

        return sensorConfiguration.dragPointRemoval;
    }

    bool Driver::isIntensityDataEnabled()
    {
        return sensorConfiguration.intensity;
    }

    Driver::ScanningFrequency Driver::getScanningFrequency_Hz()
    {
        assertIsConnected();

        return sensorConfiguration.scanningFrequency_Hz;
    }

    bool Driver::isResampleFilterEnabled()
    {
        assertIsConnected();

        return sensorConfiguration.resampleFilter;
    }

    void Driver::ethernetUpdateThreadFunction()
    {
        if (!ethernetPort.isConnected())
        {
            return;
        }

        readWriteMutex.lock();

        int charsRead = ethernetPort.read(bufferData, ETHERNET_MESSAGE_DATA_BUFFER_SIZE);

        readWriteMutex.unlock();
        
        if (charsRead == 0)
        {
            return;
        }

        bufferData.length += charsRead;

        unsigned int bytesParsed = parser.parse(bufferData);

        for (unsigned int i = bytesParsed; i < bufferData.length; i++)
        {
            ethernetPortDataBuffer[i - bytesParsed] = ethernetPortDataBuffer[i];
        }
        bufferData.length -= bytesParsed;
    }

    bool Driver::isConnected()
    {
        return ethernetPort.isConnected();
    }

    void Driver::onCompleteLidarMessage(const internal::MessageParser::CompleteLidarMessage& lidarMessage)
    {
        ScanData scanData(lidarMessage.timestamp);
        scanData.count = lidarMessage.totalPoints;
        scanData.startAngle_deg = lidarMessage.startAngle;
        scanData.endAngle_deg = lidarMessage.endAngle;

        for (int i = 0; i < lidarMessage.lidarPoints.size(); i++)
        {
            scanData.dist_mm[i] = lidarMessage.lidarPoints[i].distance;
            scanData.intensity[i] = lidarMessage.lidarPoints[i].intensity;
        }

        onScanDataReceived(scanData);
    }

    bool Driver::sendMessageWaitForResponseOrTimeout(const std::string& message, int millisecondsTilTimeout, unsigned short cmd)
    {
        readWriteMutex.lock();

        bool state = sendUdpMessageWaitForResponseOrTimeout(message, "OK", std::chrono::milliseconds(millisecondsTilTimeout), cmd);

        readWriteMutex.unlock();

        return state;
    }

    bool Driver::sendMessageWaitForResponseOrTimeout(const std::string& message, int millisecondsTilTimeout)
    {
        return sendMessageWaitForResponseOrTimeout(message, millisecondsTilTimeout, UDP_MESSAGE_CMD);
    }

    bool Driver::sendUdpMessageWaitForResponseOrTimeout(const std::string& message, const std::string& response, std::chrono::milliseconds timeout, unsigned short cmd)
    {
        unsigned char buffer[2048] = { 0 };
        CmdHeader* hdr = (CmdHeader*)buffer;
        hdr->sign = UDP_MESSAGE_SIGN;
        hdr->cmd = cmd;
        hdr->sn = rand();

        hdr->len = ((static_cast<short>(message.length()) + 3) >> 2) * 4;

        memcpy(buffer + sizeof(CmdHeader), message.c_str(), message.length());

        unsigned int* pcrc = (unsigned int*)(buffer + sizeof(CmdHeader) + hdr->len);
        pcrc[0] = calculateEndOfMessageCRC((unsigned int*)(buffer), hdr->len / 4 + 2);

        return ethernetPort.sendMessageWaitForResponseOrTimeout(
            mechaspin::parakeet::internal::InetAddress(sensorConfiguration.ipAddress, sensorConfiguration.dstPort),
            mechaspin::parakeet::internal::BufferData(buffer, sizeof(CmdHeader) + sizeof(pcrc[0]) + hdr->len),
            response, timeout);
    }

    unsigned int Driver::calculateEndOfMessageCRC(unsigned int* ptr, unsigned int len)
    {
        unsigned int xbit, data;
        unsigned int crc32 = 0xFFFFFFFF;
        const unsigned int polynomial = 0x04c11db7;

        for (unsigned int i = 0; i < len; i++)
        {
            xbit = 1 << 31;
            data = ptr[i];
            for (unsigned int bits = 0; bits < 32; bits++)
            {
                if (crc32 & 0x80000000)
                {
                    crc32 <<= 1;
                    crc32 ^= polynomial;
                }
                else
                    crc32 <<= 1;

                if (data & xbit)
                    crc32 ^= polynomial;

                xbit >>= 1;
            }
        }
        return crc32;
    }
}
}
}
