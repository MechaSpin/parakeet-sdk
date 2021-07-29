/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#include <parakeet/ProE/internal/Parser.h>

#include <cstring>
#include <cstdint>

#include <iostream>
#include <fstream>
#include <chrono>

namespace mechaspin
{
namespace parakeet
{
namespace ProE
{
namespace internal
{
    const uint32_t FIRST_TIMESTAMP_NULL_VALUE = -1;
    const uint32_t TIMESTAMP_RESET_VALUE = 25565;

    uint16_t LIDAR_MESSAGE_HEADER = 0xFAC7;
    uint16_t LIDAR_RESPONSE_HEADER = 0x484C;
    uint16_t ALARM_MESSAGE_HEADER = 0xCECE;

    uint8_t BUFFER_POS_HEADER = 0;
    uint8_t BUFFER_POS_TOTAL_POINTS = 2;
    uint8_t BUFFER_POS_NUM_POINTS_IN_SECTOR = 4;
    uint8_t BUFFER_POS_SECTOR_DATA_OFFSET = 6;
    uint8_t BUFFER_POS_START_ANGLE = 8;
    uint8_t BUFFER_POS_END_ANGLE = 12;
    uint8_t BUFFER_POS_PROPERTY_FLAGS = 16;
    uint8_t BUFFER_POS_TIMESTAMP = 20;
    uint8_t BUFFER_POS_DEVICE_NUMBER = 24;
    uint8_t BUFFER_POS_POINT_DATA = 28;

    uint8_t SIZE_OF_DISTANCE = 2;
    uint8_t SIZE_OF_RELATIVE_START_ANGLE = 2;
    uint8_t SIZE_OF_INTENSITY = 1;
    uint8_t SIZE_OF_LIDAR_POINT = SIZE_OF_DISTANCE + SIZE_OF_RELATIVE_START_ANGLE + SIZE_OF_INTENSITY;

    Parser::Parser(std::function<void(CompleteLidarMessage*)> onCompleteLidarMessageCallback)
    {
        this->onCompleteLidarMessageCallback = onCompleteLidarMessageCallback;
        reset();
    }

    void Parser::reset()
    {
        lastGeneratedTimestamp.validTimestamp = false;
    }

    void Parser::generateTimestamp()
    {
        if (lastGeneratedTimestamp.validTimestamp)
        {
            currentLidarMessage->generatedTimestamp = lastGeneratedTimestamp;
        }

        lastGeneratedTimestamp.validTimestamp = true;
        lastGeneratedTimestamp.timestamp = std::chrono::system_clock::now();
    }

    void Parser::parseHeader()
    {
        memcpy(&header, bufferData.buffer + BUFFER_POS_HEADER, sizeof(header));
    }

    void Parser::parseNumPointsInThisPartialSector()
    {
        memcpy(&currentLidarMessage->numPoints, bufferData.buffer + BUFFER_POS_TOTAL_POINTS, sizeof(currentLidarMessage->numPoints));
    }

    void Parser::parseNumPointsInSector()
    {
        memcpy(&currentLidarMessage->numPointsInSector, bufferData.buffer + BUFFER_POS_NUM_POINTS_IN_SECTOR, sizeof(currentLidarMessage->numPointsInSector));
    }

    void Parser::parseSectorDataOffset()
    {
        memcpy(&currentLidarMessage->sectorDataOffset, bufferData.buffer + BUFFER_POS_SECTOR_DATA_OFFSET, sizeof(currentLidarMessage->sectorDataOffset));
    }

    void Parser::parseStartAngle()
    {
        memcpy(&currentLidarMessage->startAngle, bufferData.buffer + BUFFER_POS_START_ANGLE, sizeof(currentLidarMessage->startAngle));
    }

    void Parser::parseEndAngle()
    {
        memcpy(&currentLidarMessage->endAngle, bufferData.buffer + BUFFER_POS_END_ANGLE, sizeof(currentLidarMessage->endAngle));
    }

    void Parser::parseSensorProperties()
    {
        memcpy(&currentLidarMessage->sensorPropertyFlags.value, bufferData.buffer + BUFFER_POS_PROPERTY_FLAGS, sizeof(currentLidarMessage->sensorPropertyFlags.value));

        currentLidarMessage->sensorPropertyFlags.unitIsInCM = currentLidarMessage->sensorPropertyFlags.value | 0x1;
        currentLidarMessage->sensorPropertyFlags.withIntensity = currentLidarMessage->sensorPropertyFlags.value | 0x2;
        currentLidarMessage->sensorPropertyFlags.doDragPointRemoval = currentLidarMessage->sensorPropertyFlags.value | 0x4;
        currentLidarMessage->sensorPropertyFlags.doDataSmoothing = currentLidarMessage->sensorPropertyFlags.value | 0x8;
    }

    void Parser::parseTimestamp()
    {
        memcpy(&currentLidarMessage->timestamp, bufferData.buffer + BUFFER_POS_TIMESTAMP, sizeof(currentLidarMessage->timestamp));
    }

    void Parser::parseDeviceNumber()
    {
        memcpy(&currentLidarMessage->deviceNumber, bufferData.buffer + BUFFER_POS_DEVICE_NUMBER, sizeof(currentLidarMessage->deviceNumber));
    }

    void Parser::parsePoints()
    {
        uint16_t BUFFER_POS_DISTANCES = BUFFER_POS_POINT_DATA;
        uint16_t BUFFER_POS_RELATIVE_START_ANGLES = BUFFER_POS_DISTANCES + (currentLidarMessage->numPoints * SIZE_OF_DISTANCE);
        uint16_t BUFFER_POS_INTENSITY = BUFFER_POS_RELATIVE_START_ANGLES + (currentLidarMessage->numPoints * SIZE_OF_RELATIVE_START_ANGLE);

        for (uint8_t i = 0; i < currentLidarMessage->numPoints; i++)
        {
            LidarPoint lidarPoint;

            uint16_t distance;

            memcpy(&distance, bufferData.buffer + BUFFER_POS_DISTANCES + (i * SIZE_OF_DISTANCE), SIZE_OF_DISTANCE);
            lidarPoint.distance = currentLidarMessage->sensorPropertyFlags.unitIsInCM ? distance : distance / 10;

            memcpy(&lidarPoint.relativeStartAngle, bufferData.buffer + BUFFER_POS_RELATIVE_START_ANGLES + (i * SIZE_OF_RELATIVE_START_ANGLE), SIZE_OF_RELATIVE_START_ANGLE);

            if (currentLidarMessage->sensorPropertyFlags.withIntensity)
            {
                memcpy(&lidarPoint.intensity, bufferData.buffer + BUFFER_POS_INTENSITY + (i * SIZE_OF_INTENSITY), SIZE_OF_INTENSITY);
            }
            else
            {
                lidarPoint.intensity = 0;
            }

            currentLidarMessage->lidarPoints.push_back(lidarPoint);
        }
    }

    void Parser::parseChecksum()
    {
        uint16_t BUFFER_POS_CHECKSUM = BUFFER_POS_POINT_DATA + (SIZE_OF_LIDAR_POINT * currentLidarMessage->numPoints);

        memcpy(&currentLidarMessage->checksum, bufferData.buffer + BUFFER_POS_CHECKSUM, sizeof(currentLidarMessage->checksum));
    }

    void Parser::parsePartialScan()
    {
        //We are generating the timestamp first, so that we can get the most accurate time.
        generateTimestamp();

        parseNumPointsInThisPartialSector();
        parseNumPointsInSector();
        parseSectorDataOffset();
        parseStartAngle();
        parseEndAngle();
        parseSensorProperties();
        parseTimestamp();
        parseDeviceNumber();

        parsePoints();

        parseChecksum();
    }

    bool Parser::doesChecksumMatch()
    {
        uint16_t newChecksum = 0;

        newChecksum += currentLidarMessage->numPoints;
        newChecksum += currentLidarMessage->numPointsInSector;
        newChecksum += currentLidarMessage->sectorDataOffset;

        newChecksum += currentLidarMessage->startAngle >> 16;
        newChecksum += currentLidarMessage->startAngle & 0xFFFF;

        newChecksum += currentLidarMessage->endAngle >> 16;
        newChecksum += currentLidarMessage->endAngle & 0xFFFF;

        newChecksum += currentLidarMessage->sensorPropertyFlags.value >> 16;
        newChecksum += currentLidarMessage->sensorPropertyFlags.value & 0xFFFF;

        newChecksum += currentLidarMessage->timestamp >> 16;
        newChecksum += currentLidarMessage->timestamp & 0xFFFF;

        newChecksum += currentLidarMessage->deviceNumber >> 16;
        newChecksum += currentLidarMessage->deviceNumber & 0xFFFF;

        for(LidarPoint lidarPoint : currentLidarMessage->lidarPoints)
        {
            newChecksum += lidarPoint.distance;
            newChecksum += lidarPoint.relativeStartAngle;
            newChecksum += lidarPoint.intensity;
        }

        return newChecksum == currentLidarMessage->checksum;
    }

    int Parser::getTotalPointsOfAllPartialScans()
    {
        int totalPoints = 0;
        for (auto sector : partialSectorScanDataList)
        {
            totalPoints += sector->numPoints;
        }

        return totalPoints;
    }

    bool Parser::isScanComplete()
    {
        return getTotalPointsOfAllPartialScans() == currentLidarMessage->numPointsInSector;
    }

    bool Parser::isScanCorrupt()
    {
        if (getTotalPointsOfAllPartialScans() > currentLidarMessage->numPointsInSector)
        {
            return true;
        }

        if (!doesChecksumMatch())
        {
            return true;
        }

        return false;
    }

    void Parser::createAndPublishCompleteScan()
    {
        if (partialSectorScanDataList.size() <= 0)
        {
            return;
        }

        auto firstMessage = partialSectorScanDataList[0];

        //We will not ship the information from the first revolution, as it will not have a valid timestamp.
        if (!firstMessage->generatedTimestamp.validTimestamp)
        {
            partialSectorScanDataList.clear();
            return;
        }

        CompleteLidarMessage* lidarMessage = new CompleteLidarMessage();

        lidarMessage->totalPoints = firstMessage->numPointsInSector;
        lidarMessage->endAngle = firstMessage->deviceNumber;
        lidarMessage->startAngle = firstMessage->startAngle / 1000;
        lidarMessage->endAngle = firstMessage->endAngle / 1000;
        lidarMessage->timestamp = firstMessage->generatedTimestamp.timestamp;

        lidarMessage->sensorPropertyFlags = firstMessage->sensorPropertyFlags;

        for (auto sector : partialSectorScanDataList)
        {
            for (auto lidarPoint : sector->lidarPoints)
            {
                lidarMessage->lidarPoints.push_back(lidarPoint);
            }
        }

        onCompleteLidarMessageCallback(lidarMessage);

        partialSectorScanDataList.clear();
    }

    int Parser::parseLidarDataFromBuffer()
    {
        currentLidarMessage = std::make_shared<PartialLidarMessage>();

        parsePartialScan();

        partialSectorScanDataList.push_back(currentLidarMessage);
        
        if(isScanCorrupt())
        {
            partialSectorScanDataList.clear();
            partialSectorScanDataList.push_back(currentLidarMessage);
        }

        if (isScanComplete())
        {
            createAndPublishCompleteScan();
        }

        return bufferData.length;
    }

    bool Parser::isLidarMessage()
    {
        return header == LIDAR_MESSAGE_HEADER;
    }

    bool Parser::isLidarResponse()
    {
        return header == LIDAR_RESPONSE_HEADER;
    }

    bool Parser::isAlarmMessage()
    {
        return header == ALARM_MESSAGE_HEADER;
    }

    int Parser::parseSensorBuffer(int length, unsigned char* buf)
    {
        bufferData.length = length;
        bufferData.buffer = buf;

        parseHeader();

        if (isLidarMessage())
        {
            return parseLidarDataFromBuffer();
        }
        else if (isLidarResponse())
        {
            /*
            for (int i = 0; i < length; i++)
            {
                std::cout << buf[i];
            }
            std::cout << std::endl;
            */
            return length;
        }
        else if (isAlarmMessage())
        {
            throw std::runtime_error("An alarm has been triggered");
        }
        else
        {
            //std::cout << "Failure to parse data from buffer" << std::endl;
            return length;
        }
    }
}
}
}
}
