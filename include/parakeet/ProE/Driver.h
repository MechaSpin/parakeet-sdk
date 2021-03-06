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
        virtual ~Driver();

        /// \brief Attempt connection to a Parakeet sensor through a ethernet port
        /// \param[in] sensorConfiguration - Sensor settings and ethernet port information
        void connect(const SensorConfiguration& sensorConfiguration);

        /// \brief Start the Driver's processing thread
        void start() override;

        /// \brief Stop the Driver's processing thread
        void stop() override;

        /// \brief Close the ethernet connection
        void close() override;

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

        /// \brief Set the IP address and port for the sensor. Messages to the sensor will be sent to this address.
        /// \param[in] ipAdress - The IP Address the sensor will live on, as an array of four bytes
        /// \param[in] subnetMask - The subnet mask the sensor will live on, as an array of four bytes
        /// \param[in] gateway - The gateway the sensor will live on, as an array of four bytes
        /// \param[in] port - The port that the sensor will be listening on
        void setSensorIPv4Settings(const std::uint8_t ipAddress[], const std::uint8_t subnetMask[], const std::uint8_t gateway[], const unsigned short port);
        
        /// \brief Set the IP address and port for the sensor to publish data to. Messages from the sensor will be received by this address.
        /// \param[in] ipAdress - The IP Address the sensor will send messages to, as an array of four bytes
        /// \param[in] port - The port that the sensor will be sending messages to
        void setSensorDestinationIPv4Settings(const std::uint8_t ipAddress[], const unsigned short port);
        
    private:
        static const int ETHERNET_MESSAGE_DATA_BUFFER_SIZE = 8192;// Arbitrary size

        void open();
        void ethernetUpdateThreadFunction();
        bool isConnected();

        void onCompleteLidarMessage(const internal::MessageParser::CompleteLidarMessage& lidarMessage);

        unsigned int calculateEndOfMessageCRC(unsigned int* ptr, unsigned int len);

        bool sendMessageWaitForResponseOrTimeout(const std::string& message, int millisecondsTilTimeout);
        bool sendMessageWaitForResponseOrTimeout(const std::string& message, int millisecondsTilTimeout, unsigned short cmd);
        bool sendUdpMessageWaitForResponseOrTimeout(const std::string& message, const std::string& response, std::chrono::milliseconds timeout, unsigned short cmd);

        unsigned char ethernetPortDataBuffer[ETHERNET_MESSAGE_DATA_BUFFER_SIZE];
        mechaspin::parakeet::internal::BufferData bufferData;

        SensorConfiguration sensorConfiguration;
        UdpSocket ethernetPort;
        internal::MessageParser parser;
        std::mutex readWriteMutex;
};
}
}
}

#endif