/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#ifndef PARAKEET_DRIVER_H
#define PARAKEET_DRIVER_H

#include "BaudRate.h"
#include "macros.h"
#include "ScanDataPolar.h"
#include "SerialPort.h"
#include "internal/SensorResponseParser.h"

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
            Frequency_15Hz = 15
        };

        struct SensorConfiguration
        {
            SensorConfiguration() = default;

            /// \brief Create a SensorConfiguration object with the following settings
            /// \param[in] comPort - The OS location of the serial port ie: ("COM3" | "/dev/ttyUSB0")
            /// \param[in] baudRate - The baud rate the sensor is currently set to. Using BaudRates::Auto will try to automatically determine the baud rate.
            /// \param[in] intensity - Should the sensor return intensity data
            /// \param[in] scanningFrequency_Hz - The speed which the sensor should be spinning at
            /// \param[in] dataSmoothing - Should data smoothing be enabled
            /// \param[in] dragPointRemoval - Should drag point removal be enabled
            SensorConfiguration(const std::string& comPort, BaudRate baudRate, bool intensity, ScanningFrequency scanningFrequency_Hz, bool dataSmoothing, bool dragPointRemoval)
            {
                this->comPort = comPort;
                this->baudRate = baudRate;
                this->intensity = intensity;
                this->scanningFrequency_Hz = scanningFrequency_Hz;
                this->dataSmoothing = dataSmoothing;
                this->dragPointRemoval = dragPointRemoval;
            }

            std::string comPort;
            BaudRate baudRate = BaudRates::Auto;
            ScanningFrequency scanningFrequency_Hz;
            bool intensity;
            bool dataSmoothing;
            bool dragPointRemoval;
        };

        /// \brief A deconstructor responsible for shutting down a existing serial port connection
        ~Driver();

        /// \brief Attempt connection to a Parakeet sensor through a serial port
        /// \param[in] sensorConfiguration - Sensor settings and serial port information
        void connect(const SensorConfiguration& sensorConfiguration);

        /// \brief Attempt connection to a Parakeet sensor through a serial port
        /// \param[in] comPort - The OS location of the serial port ie: ("COM3" | "/dev/ttyUSB0")
        /// \param[in] baudRate - The baud rate the sensor is currently set to. Using BaudRates::Auto will try to automatically determine the baud rate.
        /// \param[in] intensity - Should the sensor return intensity data
        /// \param[in] scanningFrequency_Hz - The speed which the sensor should be spinning at
        /// \param[in] dataSmoothing - Should data smoothing be enabled
        /// \param[in] dragPointRemoval - Should drag point removal be enabled
        /// \returns True if connection was successful, false otherwise
        PARAKEET_DEPRECATED(bool connect(const std::string& comPort, BaudRate baudRate, bool intensity, ScanningFrequency scanningFrequency_Hz, bool dataSmoothing, bool dragPointRemoval));

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
        struct ScanData;
        struct MessageData;

        void onScanDataReceived(ScanData* scanData);
        void onMessageDataReceived(MessageData* messageData);
        int parseSensorDataFromBuffer(int length, unsigned char* buf);

        void open();
        void autoFindBaudRate();
        void serialInterfaceThreadFunction();
        bool sendMessageWaitForResponseOrTimeout(internal::SensorResponse::MessageType messageType, const std::string& message, std::chrono::milliseconds timeout);

        void throwExceptionIfNotConnected();

        bool isAutoConnecting;
        SensorConfiguration sensorConfiguration;

        bool sensorReturnMessageState[internal::SensorResponse::MessageType::NA - 1];
        
        SerialPort serialPort;
        internal::SensorResponseParser sensorResponseParser;

        std::chrono::milliseconds interfaceThreadStartTime;
        int interfaceThreadFrameCount = 0;
        std::thread serialInterfaceThread;
        bool runSerialInterfaceThread;

        std::chrono::time_point<std::chrono::system_clock> timeOfFirstPoint;
        std::vector<PointPolar> pointHoldingList;
        std::function<void(const ScanDataPolar&)> scanCallbackFunction = nullptr;
};
}
}

#endif