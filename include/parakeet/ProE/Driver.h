/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#ifndef PARAKEET_PROE_DRIVER_H
#define PARAKEET_PROE_DRIVER_H

#include <parakeet/Driver.h>
#include <parakeet/UdpSocket.h>
#include <parakeet/ProE/internal/Parser.h>

#include <thread>
#include <iostream>
#include <string>

#include <stdio.h>
#include <functional>

#include <mutex>

namespace mechaspin
{
namespace parakeet
{
namespace ProE
{
class Driver : public mechaspin::parakeet::Driver
{
    public:
        struct SensorConfiguration
        {
            SensorConfiguration() = default;

            /// \brief Create a SensorConfiguration object with the following settings
            /// \param[in] intensity - Should the sensor return intensity data
            /// \param[in] scanningFrequency_Hz - The speed which the sensor should be spinning at
            /// \param[in] dataSmoothing - Should data smoothing be enabled
            /// \param[in] dragPointRemoval - Should drag point removal be enabled
            SensorConfiguration(const std::string& ipAddress, int dstPort, int srcPort, bool intensity, ScanningFrequency scanningFrequency_Hz, bool dataSmoothing, bool dragPointRemoval, bool resampleFilter)
            {
                this->ipAddress = ipAddress;
                this->dstPort = dstPort;
                this->srcPort = srcPort;
                this->intensity = intensity;
                this->scanningFrequency_Hz = scanningFrequency_Hz;
                this->dataSmoothing = dataSmoothing;
                this->dragPointRemoval = dragPointRemoval;
                this->resampleFilter = resampleFilter;
            }

            std::string ipAddress;
            int dstPort;
            int srcPort;
            bool intensity;
            ScanningFrequency scanningFrequency_Hz;
            bool dataSmoothing;
            bool dragPointRemoval;
            bool resampleFilter;
        };

        /// \brief A constructor responsible for intializing default variable states
        Driver();

        /// \brief A deconstructor responsible for shutting down any existing connections
        ~Driver();

        /// \brief Attempt connection to a Parakeet sensor through a ethernet port
        /// \param[in] sensorConfiguration - Sensor settings and ethernet port information
        void connect(const SensorConfiguration& sensorConfiguration);

        /// \brief Start the Driver's processing thread
        void start();

        /// \brief Stop the Driver's processing thread
        void stop();

        /// \brief Close the ethernet connection
        void close();

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

        /// \brief Set the state of the resample filter on the sensor
        /// \param[in] enable - The state of the resample filter 
        void enableResampleFilter(bool enable);

        /// \brief Gets the state of the resample filter
        /// \returns The state of the resample filter
        bool isResampleFilterEnabled();

        /// \brief Sets the sensor boot-up settings
        /// \param[in] ipAdress - The IP Address the sensor should live on
        /// \param[in] subnetMask - The subnetMask the sensor should live on
        /// \param[in] gateway - The gateway the sensor should live on
        /// \param[in] port - The port that the sensor should be publishing point data from
        void setSensorSettings(const unsigned char* ipAddress, const unsigned char* subnetMask, const unsigned char* gateway, const unsigned short port);
    private:
        void open();
        void ethernetUpdateThreadFunction();
        bool isConnected();

        void onCompleteLidarMessage(internal::Parser::CompleteLidarMessage* lidarMessage);

        unsigned int calculateEndOfMessageCRC(unsigned int* ptr, unsigned int len);

        bool sendMessageWaitForResponseOrTimeout(const std::string& message, int millisecondsTilTimeout);
        bool sendMessageWaitForResponseOrTimeout(const std::string& message, int millisecondsTilTimeout, unsigned short cmd);
        bool sendUdpMessageWaitForResponseOrTimeout(const std::string& message, const std::string& response, std::chrono::milliseconds timeout, unsigned short cmd);

        unsigned char* ethernetPortDataBuffer;
        unsigned int ethernetPortDataBufferLength;

        SensorConfiguration sensorConfiguration;
        UdpSocket ethernetPort;
        internal::Parser parser;
        std::mutex readWriteMutex;
};
}
}
}

#endif