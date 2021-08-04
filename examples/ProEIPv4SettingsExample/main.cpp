/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#include <iostream>
#include <string>
#include <vector>

#include <parakeet/ProE/Driver.h>

const char versionNumber[] = "1.0.0";

void readIPv4FormattedString(unsigned char* outputResult)
{
    std::string lineIn;
    std::getline(std::cin, lineIn);

    std::vector<std::string> numbers;
    std::string currentNumber;
    char dotCount = 0;
    for(char c : lineIn)
    {
        if(c == '.')
        {
            dotCount++;
            numbers.push_back(currentNumber);
            currentNumber = "";
        }
        else
        {
            currentNumber += c;
        }
    }
    numbers.push_back(currentNumber);

    if(dotCount != 3)
    {
        return;
    }

    for(int i = 0; i < 4; i++)
    {
        outputResult[i] = atoi(numbers[i].c_str());
    }
}

void readPort(unsigned short* outputResult)
{
    std::string lineIn;
    std::getline(std::cin, lineIn);

    *outputResult = atoi(lineIn.c_str());
}

void setIPv4InformationFromUser(mechaspin::parakeet::ProE::Driver& driver)
{
    unsigned char ipAddress[4];
    unsigned char subnetMask[4];
    unsigned char defaultGateway[4];
    unsigned short port;

    std::cout << "Please enter the IP Address the sensor should be listed under." << std::endl;

    readIPv4FormattedString(ipAddress);

    std::cout << "Please enter the subnet mask the sensor should be listed under." << std::endl;

    readIPv4FormattedString(subnetMask);

    std::cout << "Please enter the default gateway the sensor should be listed under." << std::endl;

    readIPv4FormattedString(defaultGateway);

    std::cout << "Please enter the port which the sensor should listen on." << std::endl;
    readPort(&port);

    driver.setSensorSettings(ipAddress, subnetMask, defaultGateway, port);
}

int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        std::cout << "Run this app via:" << std::endl
                  << "./ProEIPv4SettingsExample DSTIPADDRESS DSTPORT SRCPORT" << std::endl
                  << "ie:" << std::endl
                  << "./ProEIPv4SettingsExample 192.168.158.98 6543 6668" << std::endl;

        return -1;
    }

    std::cout << "Starting Parakeet ProEIPv4SettingsExample v" << versionNumber << std::endl;

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
        setIPv4InformationFromUser(proEDriver);

        std::cout << "IPv4 settings set!" << std::endl;
    }
    catch (const std::runtime_error& error)
    {
        std::cout << "The following exception occured when attempting to modify IPv4 data: " << error.what() << std::endl;
    }

    std::cout << "Shutting down" << std::endl;

    return 0;
}