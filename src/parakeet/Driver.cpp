/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#include <parakeet/Driver.h>
#include <parakeet/exceptions/NotConnectedToSensorException.h>

#include <iostream>

namespace mechaspin
{
namespace parakeet
{
	void Driver::stop()
	{
        runUpdateThread = false;

        if(updateThread.joinable())
        {
            updateThread.join();
        }
	}

	void Driver::close()
	{
        if (isConnected() || runUpdateThread || updateThread.joinable())
        {
           stop();
        }
	}

    void Driver::start()
    {
        assertIsConnected();

        runUpdateThread = true;
        updateThreadStartTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
        updateThreadFrameCount = 0;
        updateThread = std::thread([&] { this->updateThreadMainLoop(); });
    }

	void Driver::updateThreadMainLoop()
	{
        while (runUpdateThread)
        {
			if(updateThreadCallbackFunction)
			{
				updateThreadCallbackFunction();
			}
		}
	}

	void Driver::assertIsConnected()
	{
        if(!isConnected())
        {
            throw exceptions::NotConnectedToSensorException();
        }
	}

    bool Driver::isRunning()
	{
		return runUpdateThread;
	}
	

    double Driver::getScanRate_Hz()
    {
        std::chrono::milliseconds currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

        auto ms = (currentTime - updateThreadStartTime).count();

        if(ms == 0)
        {
            return 0;
        }

        return updateThreadFrameCount / (ms / 1000.0);
    }

    void Driver::registerScanCallback(std::function<void(const ScanDataPolar&)> callback)
    {
        scanCallbackFunction = callback;
    }

    void Driver::registerUpdateThreadCallback(std::function<void()> callback)
    {
        updateThreadCallbackFunction = callback;
    }
    
    void Driver::onScanDataReceived(const ScanData& scanData)
    {
        double anglePerPoint_deg = (scanData.endAngle_deg - scanData.startAngle_deg) / scanData.count;
        double deviationFrom360_deg = 1;

        //Create PointPolar for each data point
        for(int i = 0; i < scanData.count; i++)
        {
            PointPolar pointPolar(scanData.dist_mm[i], scanData.startAngle_deg + (anglePerPoint_deg * i), scanData.intensity[i]);

            pointHoldingList.push_back(pointPolar);
        }

        if(scanData.endAngle_deg + deviationFrom360_deg >= 360)
        {
            updateThreadFrameCount++;

            ScanDataPolar scanDataPolar(pointHoldingList, scanData.timestamp);

            if (scanCallbackFunction != nullptr)
            {
                scanCallbackFunction(scanDataPolar);
            }

            pointHoldingList.clear();
        }
    }
}
}