/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#pragma once

#include "BaudRate.h"
#include "ScanDataPolar.h"
#include "SerialPort.h"

#include <thread>
#include <iostream>
#include <string>

#include <stdio.h>
#include <functional>

namespace mechaspin
{
namespace parakeet
{
class Driver
{
    public:
        /// \brief All currently supported Scanning Frequencies
        enum ScanningFrequency
        {
            Frequency_7Hz = 7,
            Frequency_10Hz = 10,
            Frequency_15Hz = 15,
            NOT_INITIALIZED = -1
        };

        /// \brief A deconstructor responsible for shutting down a existing serial port connection
        ~Driver();

        /// \brief Start the Driver's processing thread
        /// \param[in] comPort - The OS location of the serial port ie: ("COM3" | "/dev/ttyUSB0")
        /// \param[in] baudRate - The baud rate the sensor is currently set to. Using BaudRates::Auto will try to automatically determine the baud rate.
        /// \param[in] intensity - Should the sensor return intensity data
        /// \param[in] scanningFrequency_Hz - The speed which the sensor should be spinning at
        /// \param[in] dataSmoothing - Should data smoothing be enabled
        /// \param[in] dragPointRemoval - Should drag point removal be enabled
        /// \returns True if connection was successful, false otherwise
        bool connect(const std::string& comPort, BaudRate baudRate, bool intensity, ScanningFrequency scanningFrequency_Hz, bool dataSmoothing, bool dragPointRemoval);

        /// \brief Start the Driver's processing thread
        void start();

        /// \brief Stop the Driver's processing thread
        void stop();

        /// \brief Close a serial port connection
        void close();

        /// \brief Set a function to be called when scan data is received from the sensor
        /// \param[in] callback - The function to be called when data is received
        void registerScanCallback(std::function<void(const ScanDataPolar&)> callback);

        /// \brief Gets the scan rate from the sensor
        /// \returns The scan rate
        double getScanRate_Hz();

        /// \brief Set the scanning frequency on the sensor
        /// \param[in] Hz - The scanning frequency to be set
        void setScanningFrequency_Hz(ScanningFrequency Hz);

        /// \brief Gets the scanning frequency
        /// \returns The scanning frequency
        ScanningFrequency getScanningFrequency_Hz();

        /// \brief Set the state of intensity data on the sensor
        /// \param[in] enable - The state of intensity data
        void enableIntensityData(bool enable);

        /// \brief Gets the state of intensity data
        /// \returns The state of intensity data
        bool isIntensityDataEnabled();

        /// \brief Set the state of data smoothing on the sensor
        /// \param[in] enable - The state of data smoothing
        void enableDataSmoothing(bool enable);

        /// \brief Gets the state of data smoothing
        /// \returns The state of data smoothing
        bool isDataSmoothingEnabled();

        /// \brief Set the state of drag point removal on the sensor
        /// \param[in] enable - The state of drag point removal
        void enableRemoveDragPoint(bool enable);

        /// \brief Gets the state of drag point removal
        /// \returns The state of drag point removal
        bool isDragPointRemovalEnabled();

        /// \brief Set the baud rate of the sensor
        /// \param[in] baudRate - The baud rate to be set
        void setBaudRate(BaudRate baudRate);

        /// \brief Gets the current baud rate
        /// \returns The current baud rate
        BaudRate getBaudRate();

    private:
        struct ScanData
        {
            unsigned short from;
            unsigned short span;
            unsigned short count;
            unsigned short reserved;
            unsigned short dist[1000];
            unsigned char intensity[1000];
        };

        struct CommandData
        {
            int len;
            unsigned char body[8 * 1024];
        };

        void OnData(ScanData* data);
        void OnMsg(CommandData* msg);
        int Parse(int nl, unsigned char* buf);

        bool isAutoConnecting;

        bool g_withIntensity;
        bool g_withDataSmoothing;
        bool g_withDragPointRemoval;
        ScanningFrequency g_scanningFrequency_Hz;
        BaudRate g_baudRate = BaudRates::Auto;

        bool autoFindBaudRate(const std::string& comPort, bool intensity, ScanningFrequency scanningFrequency_Hz, bool dataSmoothing, bool dragPointRemoval);

        std::thread serialInterfaceThread;
        bool runSerialInterfaceThread;
        void serialInterfaceThreadFunction();

        SerialPort serialPort;

        std::chrono::milliseconds interfaceThreadStartTime;
        int interfaceThreadFrameCount = 0;

        std::vector<PointPolar> pointHoldingList;
        std::function<void(const ScanDataPolar&)> scanCallbackFunction = nullptr;

        static const std::string CW_STOP_ROTATING;
        static const std::string CW_START_NORMALLY;
        static const std::string CW_STOP_ROTATING_FIX_DIST;
        static const std::string CW_RESET_AND_RESTART;
        static const std::string CW_VERSION_NUMBER;

        static const std::string CW_ENABLE_DATA_SMOOTHING;
        static const std::string CW_DISABLE_DATA_SMOOTHING;
        static const std::string CW_ENABLE_DRAG_POINT_REMOVAL;
        static const std::string CW_DISABLE_DRAG_POINT_REMOVAL;

        static const std::string SW_START_WITH_INTENSITY;
        static const std::string SW_START_WITHOUT_INTENSITY;

        static const std::string SW_SET_SPEED_PREFIX;
        static const std::string SW_SET_SPEED_POSTFIX;
        static const std::string SW_SET_SPEED(int speed);

        static const std::string SW_SET_BIAS_PREFIX;
        static const std::string SW_SET_BIAS_POSTFIX;
        static const std::string SW_SET_BIAS(int bias);

        static const std::string SW_SET_BAUD_RATE_PREFIX;
        static const std::string SW_SET_BAUD_RATE_POSTFIX;
        static const std::string SW_SET_BAUD_RATE(int baudRate);

        enum LidarReturnMessage
        {
            STOP,
            START,
            INTENSITY,
            SPEED,
            BAUDRATE,
            DATASMOOTHING,
            DRAGPOINTREMOVAL,
            MAX
        };

        bool sensorMessages[LidarReturnMessage::MAX - 1];

        bool waitForMessage(Driver::LidarReturnMessage messageType, const std::string& message, std::chrono::milliseconds timeout);
};
}
}