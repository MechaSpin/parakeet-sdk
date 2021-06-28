/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#include <parakeet/Driver.h>

namespace mechaspin
{
namespace parakeet
{
    const std::string Driver::CW_STOP_ROTATING = "LSTOPH";
    const std::string Driver::CW_START_NORMALLY = "LSTARH";
    const std::string Driver::CW_STOP_ROTATING_FIX_DIST = "LMEASH";
    const std::string Driver::CW_RESET_AND_RESTART = "LRESTH";
    const std::string Driver::CW_VERSION_NUMBER = "LVERSH";

    const std::string Driver::CW_ENABLE_DATA_SMOOTHING = "LSSS1H";
    const std::string Driver::CW_DISABLE_DATA_SMOOTHING = "LSSS0H";
    const std::string Driver::CW_ENABLE_DRAG_POINT_REMOVAL = "LFFF1H";
    const std::string Driver::CW_DISABLE_DRAG_POINT_REMOVAL = "LFFF0H";

    const std::string Driver::SW_START_WITH_INTENSITY = "LOCONH";
    const std::string Driver::SW_START_WITHOUT_INTENSITY = "LNCONH";

    const std::string Driver::SW_SET_SPEED_PREFIX = "LSRPM:";
    const std::string Driver::SW_SET_SPEED_POSTFIX = "H";
    const std::string Driver::SW_SET_SPEED(int speed)
    {
        return SW_SET_SPEED_PREFIX + std::to_string(speed) + SW_SET_SPEED_POSTFIX;
    }

    const std::string Driver::SW_SET_BIAS_PREFIX = "LSERR:";
    const std::string Driver::SW_SET_BIAS_POSTFIX = "H";
    const std::string Driver::SW_SET_BIAS(int bias)
    {
        return SW_SET_BIAS_PREFIX + std::to_string(bias) + SW_SET_BIAS_POSTFIX;
    }

    const std::string Driver::Driver::SW_SET_BAUD_RATE_PREFIX = "LSBPS:";
    const std::string Driver::SW_SET_BAUD_RATE_POSTFIX = "H";
    const std::string Driver::SW_SET_BAUD_RATE(int baudRate)
    {
        return SW_SET_BAUD_RATE_PREFIX + std::to_string(baudRate) + SW_SET_BAUD_RATE_POSTFIX;
    }

    Driver::~Driver()
    {
        close();
    }

    bool Driver::connect(const std::string& comPort, BaudRate baudRate, bool intensity, ScanningFrequency scanningFrequency_Hz, bool dataSmoothing, bool dragPointRemoval)
    {
        if(scanningFrequency_Hz == ScanningFrequency::NOT_INITIALIZED)
        {
            throw std::runtime_error("Scanning Frequency must be intialized on connect");
            return false;
        }

        g_withIntensity = intensity;
        g_scanningFrequency_Hz = scanningFrequency_Hz;
        g_withDataSmoothing = dataSmoothing;
        g_withDragPointRemoval = dragPointRemoval;

        if(baudRate == BaudRates::Auto)
        {
            return autoFindBaudRate(comPort, intensity, scanningFrequency_Hz, dataSmoothing, dragPointRemoval);
        }

        isAutoConnecting = false;

        if (!serialPort.open(comPort.c_str(), baudRate))
        {
            std::cout << "Could not open port " << comPort << std::endl;
            return false;
        }

        //There's no guarentee these will ever be recieved
        serialPort.write(CW_STOP_ROTATING);

        g_baudRate = baudRate;

        return true;
    }

    bool Driver::autoFindBaudRate(const std::string& comPort, bool intensity, ScanningFrequency scanningFrequency_Hz, bool dataSmoothing, bool dragPointRemoval)
    {
        isAutoConnecting = true;

        // Loop through all accepted baud rates
        for (auto baudRate : mechaspin::parakeet::BaudRates::All)
        {
            //Attempt connecting to the port
            if (!serialPort.open(comPort.c_str(), baudRate.getValue()))
            {
                std::cout << "Could not open port " << comPort << std::endl;
                return false;
            }

            //start parsing data from the sensor
            start();

            bool state = waitForMessage(LidarReturnMessage::STOP, CW_STOP_ROTATING, std::chrono::milliseconds(250));

            stop();
            close();

            if (state)
            {
                return connect(comPort, baudRate, intensity, scanningFrequency_Hz, dataSmoothing, dragPointRemoval);
            }
        }

        //failure to find the correct baud rate
        return false;
    }

    void Driver::start()
    {
        if (!serialPort.isConnected())
        {
            std::cout << "Cannot start until connected." << std::endl;
            return;
        }

        runSerialInterfaceThread = true;
        interfaceThreadStartTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
        interfaceThreadFrameCount = 0;
        serialInterfaceThread = std::thread([&] { this->serialInterfaceThreadFunction(); });

        waitForMessage(LidarReturnMessage::START, CW_START_NORMALLY, std::chrono::milliseconds(1000));

        enableIntensityData(g_withIntensity);
        enableDataSmoothing(g_withDataSmoothing);
        enableRemoveDragPoint(g_withDragPointRemoval);
        setScanningFrequency_Hz(g_scanningFrequency_Hz);
    }

    void Driver::stop()
    {
        serialPort.write(CW_STOP_ROTATING);

        runSerialInterfaceThread = false;

        if(serialInterfaceThread.joinable())
        {
            serialInterfaceThread.join();
        }
    }

    void Driver::close()
    {
        if (serialPort.isConnected() || runSerialInterfaceThread || serialInterfaceThread.joinable())
        {
           stop();
        }

        serialPort.close();
    }

    void Driver::registerScanCallback(std::function<void(const ScanDataPolar&)> callback)
    {
        scanCallbackFunction = callback;
    }

    void Driver::enableDataSmoothing(bool enable)
    {
        if (!serialPort.isConnected())
        {
            std::cout << "Cannot modify data smoothing until connected." << std::endl;
            return;
        }

        waitForMessage(LidarReturnMessage::DATASMOOTHING, enable ? CW_ENABLE_DATA_SMOOTHING : CW_DISABLE_DATA_SMOOTHING, std::chrono::milliseconds(250));

        g_withDataSmoothing = enable;
    }

    void Driver::enableRemoveDragPoint(bool enable)
    {
        if (!serialPort.isConnected())
        {
            std::cout << "Cannot modify remove drag point until connected." << std::endl;
            return;
        }

        waitForMessage(LidarReturnMessage::DRAGPOINTREMOVAL, enable ? CW_ENABLE_DRAG_POINT_REMOVAL : CW_DISABLE_DRAG_POINT_REMOVAL, std::chrono::milliseconds(250));

        g_withDragPointRemoval = enable;
    }

    void Driver::enableIntensityData(bool enable)
    {
        if (!serialPort.isConnected())
        {
            std::cout << "Cannot modify intensity data status until connected." << std::endl;
            return;
        }

        waitForMessage(LidarReturnMessage::INTENSITY, enable ? SW_START_WITH_INTENSITY : SW_START_WITHOUT_INTENSITY, std::chrono::milliseconds(250));

        g_withIntensity = enable;
    }

    void Driver::setScanningFrequency_Hz(ScanningFrequency Hz)
    {
        if (!serialPort.isConnected())
        {
            std::cout << "Cannot modify scanning frequency until connected." << std::endl;
            return;
        }

        if(Hz == ScanningFrequency::NOT_INITIALIZED)
        {
            throw std::runtime_error("Can not set scanning frequency to NOT_INITIALIZED");
        }

        waitForMessage(LidarReturnMessage::SPEED, SW_SET_SPEED(Hz * 60), std::chrono::milliseconds(250));

        g_scanningFrequency_Hz = Hz;
    }

    BaudRate Driver::getBaudRate()
    {
        return g_baudRate;
    }

    void Driver::setBaudRate(BaudRate baudRate)
    {
        if (!serialPort.isConnected())
        {
            std::cout << "Cannot set baud rate of device until connected." << std::endl;
            return;
        }

        waitForMessage(LidarReturnMessage::STOP, CW_STOP_ROTATING, std::chrono::milliseconds(200));

        waitForMessage(LidarReturnMessage::BAUDRATE, SW_SET_BAUD_RATE(baudRate.getValue()), std::chrono::milliseconds(0));

        stop();

        serialPort.changeBaudRate(baudRate);

        start();

        g_baudRate = baudRate;
    }

    bool Driver::isDataSmoothingEnabled()
    {
        if (!serialPort.isConnected())
        {
            std::cout << "Cannot get the value for data smoothing until connected." << std::endl;
            return false;
        }

        return g_withDataSmoothing;
    }

    bool Driver::isDragPointRemovalEnabled()
    {
        if (!serialPort.isConnected())
        {
            std::cout << "Cannot get the value for drag point removal until connected." << std::endl;
            return false;
        }

        return g_withDragPointRemoval;
    }

    bool Driver::isIntensityDataEnabled()
    {
        if (!serialPort.isConnected())
        {
            std::cout << "Cannot get the value for intensity data until connected." << std::endl;
            return false;
        }

        return g_withIntensity;
    }

    Driver::ScanningFrequency Driver::getScanningFrequency_Hz()
    {
        if (!serialPort.isConnected())
        {
            std::cout << "Cannot get the value for scanning frequency until connected." << std::endl;
            return NOT_INITIALIZED;
        }

        return g_scanningFrequency_Hz;
    }

    double Driver::getScanRate_Hz()
    {
        std::chrono::milliseconds currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

        auto ms = (currentTime - interfaceThreadStartTime).count();

        if(ms == 0)
        {
            return 0;
        }

        return interfaceThreadFrameCount / (ms / 1000.0);
    }

    void Driver::serialInterfaceThreadFunction()
    {
        int buf_size = 8 * 1024;
        unsigned char* line = new unsigned char[buf_size];
        int len = 0;

        while (runSerialInterfaceThread)
        {
            // modifying the baud rate of serial port can cause the connected state to be off for a brief moment
            if (!serialPort.isConnected())
            {
                continue;
            }

            int charsRead = serialPort.read(line, len, buf_size);

            if (charsRead == 0)
            {
                continue;
            }

            len += charsRead;

            int nl = Parse(len, line);

            for (int i = nl; i<len; i++)
            {
                line[i - nl] = line[i];
            }
            len -= nl;
        }
    }

    void Driver::OnMsg(CommandData* msg)
    {
        std::string stringForm(reinterpret_cast<char*>(msg->body), msg->len);

        if (stringForm.rfind("LiDAR STOP") != std::string::npos)
        {
            sensorMessages[LidarReturnMessage::STOP] = true;
        }

        if (stringForm.rfind("LiDAR START") != std::string::npos)
        {
            sensorMessages[LidarReturnMessage::START] = true;
        }

        if (stringForm.rfind("Error: OK") != std::string::npos)
        {
            sensorMessages[LidarReturnMessage::BAUDRATE] = true;
        }

        if (stringForm.rfind("LiDAR CONFID") != std::string::npos || stringForm.rfind("LiDAR NO CONFID") != std::string::npos)
        {
            sensorMessages[LidarReturnMessage::INTENSITY] = true;
        }

        if (stringForm.rfind("LiDAR set smooth ok") != std::string::npos)
        {
            sensorMessages[LidarReturnMessage::DATASMOOTHING] = true;
        }

        if (stringForm.rfind("LiDAR set filter ok") != std::string::npos)
        {
            sensorMessages[LidarReturnMessage::DRAGPOINTREMOVAL] = true;
        }

        if (stringForm.rfind("Set RPM: OK") != std::string::npos)
        {
            sensorMessages[LidarReturnMessage::SPEED] = true;
        }

        delete msg;
    }

    void Driver::OnData(ScanData* data)
    {
        double startAngle_deg = data->from / 10.0;
        double endAngle_deg = (static_cast<double>(data->from) + static_cast<double>(data->span)) / 10.0;
        double anglePerPoint_deg = (endAngle_deg - startAngle_deg) / data->count;
        double deviationFrom360_deg = 1;

        //Create PointPolar for each data point
        for(int i = 0; i < data->count; i++)
        {
            PointPolar p(data->dist[i], startAngle_deg + (anglePerPoint_deg * i), data->intensity[i]);

            pointHoldingList.push_back(p);
        }

        if(endAngle_deg + deviationFrom360_deg >= 360)
        {
            interfaceThreadFrameCount++;

            ScanDataPolar sdp(pointHoldingList);

            if (scanCallbackFunction != nullptr)
            {
                scanCallbackFunction(sdp);
            }

            pointHoldingList.clear();
        }

        delete data;
    }

    int Driver::Parse(int nl, unsigned char* buf)
    {
        int idx = 0, found = 0;
        while (idx + 8 < nl)
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
            CommandData* cmd = new CommandData;
            if (found)
            {
                cmd->len = idx;
            }
            else
            {
                cmd->len = nl;
            }
            memcpy(cmd->body, buf, cmd->len);

            OnMsg(cmd);
        }

        while (found)
        {
            unsigned short start, cnt;

            memcpy(&cnt, buf + idx + 2, 2);
            memcpy(&start, buf + idx + 4, 2);

            if (!g_withIntensity)
            {
                //2 Byte
                if (idx + 8 + cnt * 2 > nl)
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

                    if (g_withIntensity)
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
                    OnData(data);
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
                if (idx + 8 + cnt * 3 > nl)
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
                    OnData(data);
                }
                else if(!isAutoConnecting)
                {
                    printf("Checksum check failed\n");
                    delete data;
                }
            }

            if (idx + 8 > nl)
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

    bool Driver::waitForMessage(Driver::LidarReturnMessage messageType, const std::string& message, std::chrono::milliseconds timeout)
    {
        auto startTime = std::chrono::system_clock::now();

        sensorMessages[messageType] = false;

        serialPort.write(message);

        int msCount = 0;
        while (!sensorMessages[messageType]
            && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - startTime).count() < timeout.count())
        {
            if (msCount % 10)
            {
                serialPort.write(message);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            msCount++;
        }

        return sensorMessages[messageType];
    }
}
}