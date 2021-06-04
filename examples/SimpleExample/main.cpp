/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#include <iostream>

#include <parakeet/Driver.h>
#include <parakeet/util.h>

const char versionNumber[] = "1.0.0";

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
        if (minPoint == nullptr || pointList[i].getRange() < minPoint->getRange())
        {
            minPoint = std::make_shared<mechaspin::parakeet::PointPolar>(pointList[i]);
        }

        if (maxPoint == nullptr || pointList[i].getRange() > maxPoint->getRange())
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

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cout
            << "Start application with parameters: {COM_PORT BAUDRATE}. ie: {COM4 500000}" << std::endl 
            << "Specify a Baud Rate of 0 to automatically detect the baud rate" << std::endl;
        return -1;
    }

    std::cout << "Starting Parakeet SimpleExample v" << versionNumber << std::endl;

    mechaspin::parakeet::Driver parakeetSensorDriver;

    std::string comPort = argv[1];
    mechaspin::parakeet::BaudRate baudRate(atoi(argv[2]));
    bool useDataSmoothing = false;
    bool useDragPointRemoval = false;
    bool intensity = true;
    mechaspin::parakeet::Driver::ScanningFrequency startingScanFrequency_Hz = mechaspin::parakeet::Driver::ScanningFrequency::Frequency_10Hz;

    std::cout << "Attempting connection to sensor." << std::endl;
    if(!parakeetSensorDriver.connect(comPort, baudRate, intensity, startingScanFrequency_Hz, useDataSmoothing, useDragPointRemoval))
    {
        std::cout << "Unable to connect to " << comPort << ", check to ensure that is the proper COM Port and that the unit is plugged in." << std::endl;
        return -2;
    }

    parakeetSensorDriver.registerScanCallback(onScanComplete);

    std::cout << "Starting driver." << std::endl;

    parakeetSensorDriver.start();

    printOptionsMenu();

    bool commandLoop = true;
    while (commandLoop)
    {
        char c = std::cin.get();

        switch (c)
        {
        case 'f':
            std::cout << "FPS: " << parakeetSensorDriver.getScanRate_Hz() << std::endl;
            break;
        case 's':
            std::cout << "Intensity data enabled: " << parakeetSensorDriver.isIntensityDataEnabled() << std::endl;
            std::cout << "Drag point enabled: " << parakeetSensorDriver.isDragPointRemovalEnabled() << std::endl;
            std::cout << "Data smoothing enabled: " << parakeetSensorDriver.isDataSmoothingEnabled() << std::endl;
            std::cout << "Scanning Frequency (Hz): " << parakeetSensorDriver.getScanningFrequency_Hz() << std::endl;
            std::cout << "Baud Rate: " << parakeetSensorDriver.getBaudRate().getValue() << std::endl;
            break;
        case 'z':
        {
            if (minPoint == nullptr || maxPoint == nullptr)
            {
                std::cout << "No valid min/max points." << std::endl;
                continue;
            }

            std::cout << "Min point: (" << minPoint->getRange() << ", " << minPoint->getAngleInDegrees() << ") Intensity: " << minPoint->getIntensity() << std::endl;
            std::cout << "Max point: (" << maxPoint->getRange() << ", " << maxPoint->getAngleInDegrees() << ") Intensity: " << maxPoint->getIntensity() << std::endl;

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

            std::cout << "Min point: (" << minPointXY.getX() << ", " << minPointXY.getY() << ") Intensity: " << minPointXY.getIntensity() << std::endl;
            std::cout << "Max point: (" << maxPointXY.getX() << ", " << maxPointXY.getY() << ") Intensity: " << maxPointXY.getIntensity() << std::endl;

            std::cout << "From " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - timestamp).count() << "ms ago." << std::endl;

            break;
        }
        case '?':
            printOptionsMenu();
            break;
        case 'q':
            std::cout << "Shutting application down." << std::endl;
            commandLoop = false;
            parakeetSensorDriver.close();
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    return 0;
}