/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#include <parakeet/Pro/Driver.h>

#include <parakeet/exceptions/UnableToDetermineBaudRateException.h>
#include <parakeet/exceptions/UnableToOpenPortException.h>

namespace mechaspin
{
namespace parakeet
{
namespace Pro
{
    const int SERIAL_MESSAGE_DATA_BUFFER_SIZE = 8192;          // Arbitrary size

    struct Driver::MessageData
    {
        int len;
        unsigned char body[SERIAL_MESSAGE_DATA_BUFFER_SIZE];
    };

    const std::string CW_STOP_ROTATING = "LSTOPH";
    const std::string CW_START_NORMALLY = "LSTARH";
    const std::string CW_STOP_ROTATING_FIX_DIST = "LMEASH";
    const std::string CW_RESET_AND_RESTART = "LRESTH";
    const std::string CW_VERSION_NUMBER = "LVERSH";

    const std::string CW_ENABLE_DATA_SMOOTHING = "LSSS1H";
    const std::string CW_DISABLE_DATA_SMOOTHING = "LSSS0H";
    const std::string CW_ENABLE_DRAG_POINT_REMOVAL = "LFFF1H";
    const std::string CW_DISABLE_DRAG_POINT_REMOVAL = "LFFF0H";

    const std::string SW_START_WITH_INTENSITY = "LOCONH";
    const std::string SW_START_WITHOUT_INTENSITY = "LNCONH";

    const std::string SW_SET_SPEED_PREFIX = "LSRPM:";
    const std::string SW_SET_SPEED_POSTFIX = "H";

    const std::string SW_SET_BIAS_PREFIX = "LSERR:";
    const std::string SW_SET_BIAS_POSTFIX = "H";

    const std::string SW_SET_BAUD_RATE_PREFIX = "LSBPS:";
    const std::string SW_SET_BAUD_RATE_POSTFIX = "H";
    
    const std::string SW_SET_SPEED(int speed)
    {
        return SW_SET_SPEED_PREFIX + std::to_string(speed) + SW_SET_SPEED_POSTFIX;
    }

    const std::string SW_SET_BAUD_RATE(int baudRate)
    {
        return SW_SET_BAUD_RATE_PREFIX + std::to_string(baudRate) + SW_SET_BAUD_RATE_POSTFIX;
    }
    
    const std::string SW_SET_BIAS(int bias)
    {
        return SW_SET_BIAS_PREFIX + std::to_string(bias) + SW_SET_BIAS_POSTFIX;
    }
    
    Driver::Driver()
    {
        this->registerUpdateThreadCallback(std::bind(&Driver::serialUpdateThreadFunction, this));

        serialPortDataBuffer = new unsigned char[SERIAL_MESSAGE_DATA_BUFFER_SIZE];
    }

    void Driver::connect(const SensorConfiguration& sensorConfiguration)
    {
        this->sensorConfiguration = sensorConfiguration;

        if(sensorConfiguration.baudRate == BaudRates::Auto)
        {
            autoFindBaudRate();
        }

        open();
    }

    bool Driver::connect(const std::string& comPort, BaudRate baudRate, bool intensity, ScanningFrequency scanningFrequency_Hz, bool dataSmoothing, bool dragPointRemoval)
    {
        SensorConfiguration paramConfiguration(comPort, baudRate, intensity, scanningFrequency_Hz, dataSmoothing, dragPointRemoval);
        
        try
        {
            connect(paramConfiguration);
        }
        catch (const std::runtime_error&)
        {
            return false;
        }

        return true;
    }

    void Driver::open()
    {
        if (serialPort.open(sensorConfiguration.comPort.c_str(), sensorConfiguration.baudRate))
        {
            serialPort.write(CW_STOP_ROTATING);
        }
        else
        {
            throw exceptions::UnableToOpenPortException();
        }
    }

    void Driver::autoFindBaudRate()
    {
        // Loop through all accepted baud rates
        for (BaudRate baudRate : BaudRates::All)
        {
            //Attempt connecting to the port
            this->sensorConfiguration.baudRate = baudRate;
            
            open();

            //start parsing data from the sensor
            isAutoConnecting = true;
            start();

            bool state = sendMessageWaitForResponseOrTimeout(internal::SensorResponse::STOP, CW_STOP_ROTATING, std::chrono::milliseconds(500));

            stop();
            close();
            isAutoConnecting = false;

            if(state)
            {
                return;
            }
        }

        throw exceptions::UnableToDetermineBaudRateException();
    }

    void Driver::start()
    {
        serialPortDataBufferLength = 0;

        mechaspin::parakeet::Driver::start();

        sendMessageWaitForResponseOrTimeout(internal::SensorResponse::START, CW_START_NORMALLY, std::chrono::milliseconds(1000));

        enableIntensityData(sensorConfiguration.intensity);
        enableDataSmoothing(sensorConfiguration.dataSmoothing);
        enableRemoveDragPoint(sensorConfiguration.dragPointRemoval);
        setScanningFrequency_Hz(sensorConfiguration.scanningFrequency_Hz);
    }

    void Driver::stop()
    {
        serialPort.write(CW_STOP_ROTATING);

        mechaspin::parakeet::Driver::stop();
    }

    void Driver::close()
    {
        mechaspin::parakeet::Driver::close();

        serialPort.close();
    }

    void Driver::enableDataSmoothing(bool enable)
    {
        if (!serialPort.isConnected())
        {
            std::cout << "Cannot modify data smoothing until connected." << std::endl;
            return;
        }

        sendMessageWaitForResponseOrTimeout(internal::SensorResponse::DATASMOOTHING, enable ? CW_ENABLE_DATA_SMOOTHING : CW_DISABLE_DATA_SMOOTHING, std::chrono::milliseconds(250));

        sensorConfiguration.dataSmoothing = enable;
    }

    void Driver::enableRemoveDragPoint(bool enable)
    {
        throwExceptionIfNotConnected();

        sendMessageWaitForResponseOrTimeout(internal::SensorResponse::DRAGPOINTREMOVAL, enable ? CW_ENABLE_DRAG_POINT_REMOVAL : CW_DISABLE_DRAG_POINT_REMOVAL, std::chrono::milliseconds(250));

        sensorConfiguration.dragPointRemoval = enable;
    }

    void Driver::enableIntensityData(bool enable)
    {
        throwExceptionIfNotConnected();

        sendMessageWaitForResponseOrTimeout(internal::SensorResponse::INTENSITY, enable ? SW_START_WITH_INTENSITY : SW_START_WITHOUT_INTENSITY, std::chrono::milliseconds(250));

        sensorConfiguration.intensity = enable;
    }

    void Driver::setScanningFrequency_Hz(ScanningFrequency Hz)
    {
        throwExceptionIfNotConnected();

        sendMessageWaitForResponseOrTimeout(internal::SensorResponse::SPEED, SW_SET_SPEED(Hz * 60), std::chrono::milliseconds(250));

        sensorConfiguration.scanningFrequency_Hz = Hz;
    }

    BaudRate Driver::getBaudRate()
    {
        return sensorConfiguration.baudRate;
    }

    void Driver::setBaudRate(BaudRate baudRate)
    {
        throwExceptionIfNotConnected();

        sendMessageWaitForResponseOrTimeout(internal::SensorResponse::STOP, CW_STOP_ROTATING, std::chrono::milliseconds(200));

        sendMessageWaitForResponseOrTimeout(internal::SensorResponse::BAUDRATE, SW_SET_BAUD_RATE(baudRate.getValue()), std::chrono::milliseconds(0));

        if(isUpdateThreadRunning())
        {
            stop();

            serialPort.changeBaudRate(baudRate);

            start();
        }
        else
        {
            serialPort.changeBaudRate(baudRate);
        }

        sensorConfiguration.baudRate = baudRate;
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
        throwExceptionIfNotConnected();

        return sensorConfiguration.intensity;
    }

    Driver::ScanningFrequency Driver::getScanningFrequency_Hz()
    {
        throwExceptionIfNotConnected();

        return sensorConfiguration.scanningFrequency_Hz;
    }

    void Driver::serialUpdateThreadFunction()
    {
        // modifying the baud rate of serial port can cause the connected state to be off for a brief moment
        if (!serialPort.isConnected())
        {
            return;
        }

        int charsRead = serialPort.read(serialPortDataBuffer, serialPortDataBufferLength, SERIAL_MESSAGE_DATA_BUFFER_SIZE);

        if (charsRead == 0)
        {
            return;
        }

        serialPortDataBufferLength += charsRead;

        unsigned int nl = parseSensorDataFromBuffer(serialPortDataBufferLength, serialPortDataBuffer);

        for (unsigned int i = nl; i < serialPortDataBufferLength; i++)
        {
            serialPortDataBuffer[i - nl] = serialPortDataBuffer[i];
        }
        serialPortDataBufferLength -= nl;
    }

    void Driver::onMessageDataReceived(MessageData* message)
    {
        std::string messageAsString(reinterpret_cast<char*>(message->body), message->len);

        internal::SensorResponse sensorResponse = sensorResponseParser.getSensorResponseFromMessage(messageAsString);

        if(sensorResponse.getMessageType() != internal::SensorResponse::NA)
        {
            sensorReturnMessageState[sensorResponse.getMessageType()] = true;
        }

        delete message;
    }

    int Driver::parseSensorDataFromBuffer(int length, unsigned char* buf)
    {
        int idx = 0, found = 0;
        while (idx + 8 < length)
        {
            if (buf[idx] == 'S' && buf[idx + 1] == 'T' && buf[idx + 6] == 'E' && buf[idx + 7] == 'D')
            {
                idx += 8;
            }
            if (buf[idx] == 0xce && buf[idx + 1] == 0xfa)
            {
                found = 1;
                break;
            }
            idx++;
        }

        if (idx > 0)// && found == 0)
        {
            MessageData* cmd = new MessageData;
            if (found)
            {
                cmd->len = idx;
            }
            else
            {
                cmd->len = length;
            }
            memcpy(cmd->body, buf, cmd->len);

            onMessageDataReceived(cmd);
        }

        while (found)
        {
            unsigned short start, cnt;

            memcpy(&cnt, buf + idx + 2, 2);
            memcpy(&start, buf + idx + 4, 2);

            if (!sensorConfiguration.intensity)
            {
                //2 Byte
                if (idx + 8 + cnt * 2 > length)
                {
                    break;
                }

                ScanData* data = new ScanData;
                data->from = start;
                data->span = 360;
                data->count = cnt;

                unsigned short sum = start + cnt;

                unsigned char* pdata = buf + idx + 6;
                for (int i = 0; i < cnt; i++)
                {
                    unsigned short lo_byte = pdata[i * 2];
                    unsigned short hi_byte = pdata[i * 2 + 1];

                    unsigned short val = (hi_byte << 8) + lo_byte;

                    sum += val;

                    if (sensorConfiguration.intensity)
                    {
                        data->dist[i] = val & 0x1FFF;
                        data->intensity[i] = val >> 13;
                    }
                    else
                    {
                        data->dist[i] = val;
                        data->intensity[i] = 0;
                    }
                }

                unsigned short lo = pdata[cnt * 2];
                unsigned short hi = pdata[cnt * 2+1];
                unsigned short chk = lo | (hi << 8);
                if (chk == sum)
                {
                    onScanDataReceived(data);
                }
                else if(!isAutoConnecting)
                {
                    printf("Checksum check failed\n");
                    delete data;
                }

                idx += 8 + cnt * 2;
            }
            else
            {
                //3 Byte
                if (idx + 8 + cnt * 3 > length)
                {
                    break;
                }

                ScanData* data = new ScanData;
                data->from = start;
                data->span = 360;
                data->count = cnt;
                unsigned short sum = start + cnt;

                unsigned char* pdata = buf + idx + 6;
                for (int i = 0; i < cnt; i++)
                {
                    data->intensity[i] = pdata[i * 3];

                    sum += pdata[i * 3];

                    unsigned short lo_byte = pdata[i * 3 + 1];
                    unsigned short hi_byte = pdata[i * 3 + 2];

                    unsigned short val = (hi_byte << 8) + lo_byte;

                    sum += val;

                    data->dist[i] = val;
                }

                idx += 8 + cnt * 3;

                unsigned short lo = pdata[cnt * 3];
                unsigned short hi = pdata[cnt * 3+1];
                if (((hi<<8) | lo) == sum)
                {
                    onScanDataReceived(data);
                }
                else if(!isAutoConnecting)
                {
                    printf("Checksum check failed\n");
                    delete data;
                }
            }

            if (idx + 8 > length)
            {
                break;
            }

            if (buf[idx] == 0xce && buf[idx + 1] == 0xfa)
            {
                found = 1;
            }
            else
            {
                found = 0;
            }
        }

        return idx;
    }

    bool Driver::sendMessageWaitForResponseOrTimeout(internal::SensorResponse::MessageType messageType, const std::string& message, std::chrono::milliseconds timeout)
    {
        auto startTime = std::chrono::system_clock::now();

        sensorReturnMessageState[messageType] = false;

        serialPort.write(message);

        int msCount = 0;
        while (!sensorReturnMessageState[messageType]
            && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - startTime).count() < timeout.count())
        {
            if (msCount % 10)
            {
                serialPort.write(message);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            msCount++;
        }

        return sensorReturnMessageState[messageType];
    }

    bool Driver::isConnected()
    {
        return serialPort.isConnected();
    }
}
}
}