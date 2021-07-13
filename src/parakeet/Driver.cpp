/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#include <parakeet/Driver.h>
#include <parakeet/exceptions/NotConnectedToSensorException.h>

namespace mechaspin
{
namespace parakeet
{

    Driver::~Driver()
    {
        close();
    }

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
        throwExceptionIfNotConnected();

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

	void Driver::throwExceptionIfNotConnected()
	{
        if(!isConnected())
        {
            throw exceptions::NotConnectedToSensorException();
        }
	}

    bool Driver::isUpdateThreadRunning()
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
}
}