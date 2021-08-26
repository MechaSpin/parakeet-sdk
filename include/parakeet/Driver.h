/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#include <parakeet/macros.h>

#include <chrono>
#include <functional>
#include <thread>

#include <parakeet/ScanDataPolar.h>

#ifndef PARAKEET_DRIVER_H
#define PARAKEET_DRIVER_H

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

        /// \brief Deconstructor to shut down any open connections
        virtual ~Driver() = default;

        /// \brief Gets the scan rate from the sensor
        /// \returns The scan rate
        double getScanRate_Hz();

        /// \brief Set the scanning frequency on the sensor
        /// \param[in] Hz - The scanning frequency to be set
        virtual void setScanningFrequency_Hz(ScanningFrequency Hz) = 0;

        /// \brief Gets the scanning frequency
        /// \returns The scanning frequency
        virtual ScanningFrequency getScanningFrequency_Hz() = 0;

        /// \brief Set the state of intensity data on the sensor
        /// \param[in] enable - The state of intensity data
        virtual void enableIntensityData(bool enable) = 0;

        /// \brief Gets the state of intensity data
        /// \returns The state of intensity data
        virtual bool isIntensityDataEnabled() = 0;

        /// \brief Set the state of data smoothing on the sensor
        /// \param[in] enable - The state of data smoothing
        virtual void enableDataSmoothing(bool enable) = 0;

        /// \brief Gets the state of data smoothing
        /// \returns The state of data smoothing
        virtual bool isDataSmoothingEnabled() = 0;

        /// \brief Set the state of drag point removal on the sensor
        /// \param[in] enable - The state of drag point removal
        virtual void enableRemoveDragPoint(bool enable) = 0;

        /// \brief Gets the state of drag point removal
        /// \returns The state of drag point removal
        virtual bool isDragPointRemovalEnabled() = 0;

        /// \brief Start the Driver's processing thread
        virtual void start();

        /// \brief Stop the Driver's processing thread
        virtual void stop();

        /// \brief Close a the current connection
        virtual void close();

        /// \brief Set a function to be called when scan data is received from the sensor
        /// \param[in] callback - The function to be called when data is received
        void registerScanCallback(std::function<void(const ScanDataPolar&)> callback);

    protected:
        static const int MAX_NUMBER_OF_POINTS_FROM_SENSOR = 1000;          // Arbitrary size
        struct ScanData
        {
            ScanData()
            {
                this->timestamp = std::chrono::system_clock::now();
            }

            ScanData(const std::chrono::system_clock::time_point& timestamp)
            {
                this->timestamp = timestamp;
            }

            double startAngle_deg;
            double endAngle_deg;
            unsigned short count;
            unsigned short reserved;
            unsigned short dist_mm[MAX_NUMBER_OF_POINTS_FROM_SENSOR];
            unsigned char intensity[MAX_NUMBER_OF_POINTS_FROM_SENSOR];

            std::chrono::system_clock::time_point timestamp;
        };

        struct DataPoint
        {
            float angle;
            float distance;
            unsigned char confidence;
        };


        void registerUpdateThreadCallback(std::function<void ()> callback);
        void assertIsConnected();

        virtual bool isConnected() = 0;

        bool isRunning();

        void onScanDataReceived(const ScanData& scanData);
    private:
        void updateThreadMainLoop();

        std::chrono::milliseconds updateThreadStartTime;
        int updateThreadFrameCount = 0;
        std::function<void ()> updateThreadCallbackFunction;
        std::thread updateThread;
        bool runUpdateThread;

        std::chrono::time_point<std::chrono::system_clock> timeOfFirstPoint;
        std::vector<PointPolar> pointHoldingList;
        std::function<void(const ScanDataPolar&)> scanCallbackFunction = nullptr;

        std::chrono::system_clock::time_point timestampOfFirstMessage;
};
}
}
#endif