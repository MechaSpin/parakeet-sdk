/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#include <iostream>

#include <parakeet/ProE/Driver.h>
#include <parakeet/Pro/Driver.h>
#include <parakeet/util.h>

const char versionNumber[] = "1.1.0";

std::shared_ptr<mechaspin::parakeet::PointPolar> minPoint;
std::shared_ptr<mechaspin::parakeet::PointPolar> maxPoint;
std::shared_ptr<mechaspin::parakeet::ScanDataPolar> mostRecentScanData;
std::chrono::system_clock::time_point timestamp;

void onScanComplete(const mechaspin::parakeet::ScanDataPolar& scanData)
{
    mostRecentScanData = std::make_shared<mechaspin::parakeet::ScanDataPolar>(scanData);
    minPoint.reset();
    maxPoint.reset();

    minPoint = nullptr;
    maxPoint = nullptr;

    timestamp = scanData.getTimestamp();

    auto pointList = mostRecentScanData->getPoints();

    for (size_t i = 0; i < pointList.size(); i++)
    {
        if (minPoint == nullptr || pointList[i].getRange_mm() < minPoint->getRange_mm())
        {
            minPoint = std::make_shared<mechaspin::parakeet::PointPolar>(pointList[i]);
        }

        if (maxPoint == nullptr || pointList[i].getRange_mm() > maxPoint->getRange_mm())
        {
            maxPoint = std::make_shared<mechaspin::parakeet::PointPolar>(pointList[i]);
        }
    }
}

void printOptionsMenu()
{
    std::cout << "**********************" << std::endl;
    std::cout << "        Options      " << std::endl;
    std::cout << "f - Print Driver FPS" << std::endl;
    std::cout << "s - Print Driver status" << std::endl;
    std::cout << "z - Print last read min/max Polar point" << std::endl;
    std::cout << "x - Print last read min/max XY point" << std::endl;
    std::cout << "? - Print options page" << std::endl;
    std::cout << "q - Quit application" << std::endl;
    std::cout << "**********************" << std::endl;
}

void startAndRunSensor(mechaspin::parakeet::Driver* parakeetSensorDriver)
{
    parakeetSensorDriver->registerScanCallback(onScanComplete);

    std::cout << "Starting driver." << std::endl;

    parakeetSensorDriver->start();

    printOptionsMenu();

    bool commandLoop = true;
    while (commandLoop)
    {
        char c = std::cin.get();

        switch (c)
        {
        case 'f':
            std::cout << "FPS: " << parakeetSensorDriver->getScanRate_Hz() << std::endl;
            break;
        case 's':
            std::cout << "Intensity data enabled: " << parakeetSensorDriver->isIntensityDataEnabled() << std::endl;
            std::cout << "Drag point enabled: " << parakeetSensorDriver->isDragPointRemovalEnabled() << std::endl;
            std::cout << "Data smoothing enabled: " << parakeetSensorDriver->isDataSmoothingEnabled() << std::endl;
            std::cout << "Scanning Frequency (Hz): " << parakeetSensorDriver->getScanningFrequency_Hz() << std::endl;
            //std::cout << "Baud Rate: " << parakeetSensorDriver->getBaudRate().getValue() << std::endl;
            break;
        case 'z':
        {
            if (minPoint == nullptr || maxPoint == nullptr)
            {
                std::cout << "No valid min/max points." << std::endl;
                continue;
            }

            std::cout << "Min point: (" << minPoint->getRange_mm() << ", " << minPoint->getAngle_deg() << ") Intensity: " << minPoint->getIntensity() << std::endl;
            std::cout << "Max point: (" << maxPoint->getRange_mm() << ", " << maxPoint->getAngle_deg() << ") Intensity: " << maxPoint->getIntensity() << std::endl;

            std::cout << "From " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - timestamp).count() << "ms ago." << std::endl;

            break;
        }
        case 'x':
        {
            if (minPoint == nullptr || maxPoint == nullptr)
            {
                std::cout << "No valid min/max points." << std::endl;
                continue;
            }

            mechaspin::parakeet::PointXY minPointXY = mechaspin::parakeet::util::transform(*minPoint);
            mechaspin::parakeet::PointXY maxPointXY = mechaspin::parakeet::util::transform(*maxPoint);

            std::cout << "Min point: (" << minPointXY.getX_mm() << ", " << minPointXY.getY_mm() << ") Intensity: " << minPointXY.getIntensity() << std::endl;
            std::cout << "Max point: (" << maxPointXY.getX_mm() << ", " << maxPointXY.getY_mm() << ") Intensity: " << maxPointXY.getIntensity() << std::endl;

            std::cout << "From " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - timestamp).count() << "ms ago." << std::endl;

            break;
        }
        case '?':
            printOptionsMenu();
            break;
        case 'q':
            std::cout << "Shutting application down." << std::endl;
            commandLoop = false;
            parakeetSensorDriver->close();
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void printMissingSensorInfo(bool parakeetPro, bool parakeetProE)
{
    std::cout << "Must start application with sensor information!" << std::endl;
    if (parakeetPro)
    {
        std::cout << "Parakeet Pro: \"Pro COMPORT BAUDRATE\" ie: \"/dev/ttyUSB0 0\"" << std::endl;
    }
    if (parakeetProE)
    {
        std::cout << "Parakeet ProE: \"ProE IPADDRESS LIDARPORT LOCALPORT\" ie: \"192.168.0.98 6543 6668\"" << std::endl;
    }
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printMissingSensorInfo(true, true);

        return -1;
    }

    mechaspin::parakeet::Driver* driver;

    if (std::string("Pro").find(argv[1]) != std::string::npos)
    {
        if (argc != 4)
        {
            printMissingSensorInfo(true, false);

            return -2;
        }

        mechaspin::parakeet::Pro::Driver::SensorConfiguration sensorConfiguration;
        sensorConfiguration.comPort = argv[2];
        sensorConfiguration.baudRate = mechaspin::parakeet::BaudRate(atoi(argv[3]));
        sensorConfiguration.dataSmoothing = false;
        sensorConfiguration.dragPointRemoval = false;
        sensorConfiguration.intensity = true;
        sensorConfiguration.scanningFrequency_Hz = mechaspin::parakeet::Driver::ScanningFrequency::Frequency_10Hz;

        mechaspin::parakeet::Pro::Driver* proDriver = new mechaspin::parakeet::Pro::Driver();

        proDriver->connect(sensorConfiguration);

        driver = proDriver;
    }
    else if (std::string("ProE").find(argv[1]) != std::string::npos)
    {
        if (argc != 5)
        {
            printMissingSensorInfo(false, true);

            return -3;
        }

        mechaspin::parakeet::ProE::Driver::SensorConfiguration sensorConfiguration;
        sensorConfiguration.ipAddress = argv[2];
        sensorConfiguration.lidarPort = atoi(argv[3]);
        sensorConfiguration.localPort = atoi(argv[4]);
        sensorConfiguration.dataSmoothing = false;
        sensorConfiguration.dragPointRemoval = false;
        sensorConfiguration.intensity = true;
        sensorConfiguration.scanningFrequency_Hz = mechaspin::parakeet::Driver::ScanningFrequency::Frequency_10Hz;
        sensorConfiguration.resampleFilter = true;

        mechaspin::parakeet::ProE::Driver* proEDriver = new mechaspin::parakeet::ProE::Driver();

        proEDriver->connect(sensorConfiguration);

        driver = proEDriver;
    }
    else
    {
        printMissingSensorInfo(true, true);

        return -4;
    }

    std::cout << "Starting Parakeet SimpleExample v" << versionNumber << std::endl;

    try
    {
        startAndRunSensor(driver);
    }
    catch (const std::runtime_error& error)
    {
        std::cout << "The following exception occured when running the parakeet driver: " << error.what() << std::endl;
    }

    delete driver;

    return 0;
}