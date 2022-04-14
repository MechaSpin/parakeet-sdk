/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#include <iostream>
#include <string>
#include <vector>

#include <parakeet/ProE/Driver.h>
#include <parakeet/util.h>

const char versionNumber[] = "1.0.0";

std::vector<uint8_t> readIPv4FormattedString()
{
    std::vector<uint8_t> outputResult;

    while(outputResult.size() != 4)
    {
        std::string lineIn;
        std::getline(std::cin, lineIn);

        outputResult = mechaspin::parakeet::util::addressToByteArray(lineIn);
    }

    return outputResult;
}

unsigned short readPort()
{
    std::string lineIn;
    std::getline(std::cin, lineIn);

    return atoi(lineIn.c_str());
}

void setSourceIPv4InformationFromUser(mechaspin::parakeet::ProE::Driver& driver)
{
    std::cout << "Please enter the IP Address the sensor should be listed under." << std::endl;
    std::vector<uint8_t> srcIPAddress = readIPv4FormattedString();

    std::cout << "Please enter the subnet mask the sensor should be listed under." << std::endl;
    std::vector<uint8_t> subnetMask = readIPv4FormattedString();

    std::cout << "Please enter the default gateway the sensor should be listed under." << std::endl;
    std::vector<uint8_t> defaultGateway = readIPv4FormattedString();

    std::cout << "Please enter the port which the sensor should listen on." << std::endl;
    unsigned short srcPort = readPort();

    driver.setSensorIPv4Settings(srcIPAddress.data(), subnetMask.data(), defaultGateway.data(), srcPort);

    std::cout << "IPv4 source settings set!" << std::endl 
            << "Please power cycle the device, and reconfigure your runtime parameters." << std::endl;
}

void setDestinationIPv4InformationFromUser(mechaspin::parakeet::ProE::Driver& driver)
{
    std::cout << "Please enter the IP Address the sensor should send messages to." << std::endl;
    std::vector<uint8_t> destIPAddress = readIPv4FormattedString();

    std::cout << "Please enter the port which the sensor should send messages to." << std::endl;
    unsigned short destPort = readPort();

    driver.setSensorDestinationIPv4Settings(destIPAddress.data(), destPort);

    std::cout << "IPv4 destination settings set!" << std::endl 
            << "Please power cycle the device, and reconfigure your network adapter." << std::endl;
}

int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        std::cout << "Run this app via:" << std::endl
                  << "./ConfigureProEIPv4Settings DSTIPADDRESS DSTPORT SRCPORT" << std::endl
                  << "ie:" << std::endl
                  << "./ConfigureProEIPv4Settings 192.168.158.98 6668 6543" << std::endl;

        return -1;
    }

    std::cout << "Starting Parakeet ConfigureProEIPv4Settings v" << versionNumber << std::endl;

    mechaspin::parakeet::ProE::Driver::SensorConfiguration sensorConfiguration;
    sensorConfiguration.ipAddress = argv[1];
    sensorConfiguration.dstPort = atoi(argv[2]);
    sensorConfiguration.srcPort = atoi(argv[3]);
    sensorConfiguration.dataSmoothing = false;
    sensorConfiguration.dragPointRemoval = false;
    sensorConfiguration.intensity = true;
    sensorConfiguration.scanningFrequency_Hz = mechaspin::parakeet::Driver::ScanningFrequency::Frequency_15Hz;
    sensorConfiguration.resampleFilter = true;

    mechaspin::parakeet::ProE::Driver proEDriver;

    std::cout << "Connecting to sensor..." << std::endl;

    proEDriver.connect(sensorConfiguration);
    
    std::cout << "Connected" << std::endl;

    try
    {
        bool invalidInput;

        do
        {
            invalidInput = false;

            std::cout << std::endl << "Please enter 1 to modify the sensor source settings, or 2 to modify the sensor destination settings" << std::endl;

            std::string lineIn;
            std::getline(std::cin, lineIn);


            if(lineIn == "1")
            {
                setSourceIPv4InformationFromUser(proEDriver);
            }
            else if(lineIn == "2")
            {
                setDestinationIPv4InformationFromUser(proEDriver);
            }
            else
            {
                invalidInput = true;
            }
        } while (invalidInput);
    }
    catch (const std::runtime_error& error)
    {
        std::cout << "The following exception occured when attempting to modify IPv4 data: " << error.what() << std::endl;
    }

    std::cout << "Shutting down" << std::endl;

    return 0;
}