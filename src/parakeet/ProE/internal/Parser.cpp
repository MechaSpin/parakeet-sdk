/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#include <parakeet/ProE/internal/Parser.h>

#include <cstring>
#include <cstdint>

#include <iostream>

namespace mechaspin
{
namespace parakeet
{
namespace ProE
{
namespace internal
{
    uint16_t LIDAR_MESSAGE_HEADER = 0xFAC7;
    uint16_t LIDAR_RESPONSE_HEADER = 0x484C;

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
        uint32_t startAngle;

        memcpy(&startAngle, bufferData.buffer + BUFFER_POS_START_ANGLE, sizeof(currentLidarMessage->startAngle));

        currentLidarMessage->startAngle = startAngle / 1000;
    }

    void Parser::parseEndAngle()
    {
        uint32_t endAngle;

        memcpy(&endAngle, bufferData.buffer + BUFFER_POS_END_ANGLE, sizeof(currentLidarMessage->endAngle));

        currentLidarMessage->endAngle = endAngle / 1000;
    }

    void Parser::parseSensorProperties()
    {
        uint32_t sensorPropertyFlags;

        memcpy(&sensorPropertyFlags, bufferData.buffer + BUFFER_POS_PROPERTY_FLAGS, sizeof(sensorPropertyFlags));

        currentLidarMessage->sensorPropertyFlags.unitIsInCM = sensorPropertyFlags | 0x1;
        currentLidarMessage->sensorPropertyFlags.withIntensity = sensorPropertyFlags | 0x2;
        currentLidarMessage->sensorPropertyFlags.doDragPointRemoval = sensorPropertyFlags | 0x4;
        currentLidarMessage->sensorPropertyFlags.doDataSmoothing = sensorPropertyFlags | 0x8;
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
        uint8_t BUFFER_POS_DISTANCES = BUFFER_POS_POINT_DATA;
        uint8_t BUFFER_POS_RELATIVE_START_ANGLES = BUFFER_POS_DISTANCES + (currentLidarMessage->numPointsInSector * SIZE_OF_DISTANCE);
        uint8_t BUFFER_POS_INTENSITY = BUFFER_POS_RELATIVE_START_ANGLES + (currentLidarMessage->numPointsInSector * SIZE_OF_RELATIVE_START_ANGLE);

        for (uint8_t i = 0; i < currentLidarMessage->numPointsInSector; i++)
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
        uint16_t BUFFER_POS_CHECKSUM = BUFFER_POS_POINT_DATA + (SIZE_OF_LIDAR_POINT * currentLidarMessage->numPointsInSector);

        memcpy(&currentLidarMessage->checksum, bufferData.buffer + BUFFER_POS_CHECKSUM, sizeof(currentLidarMessage->checksum));
    }

    void Parser::parsePartialScan()
    {
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
        return getTotalPointsOfAllPartialScans() > currentLidarMessage->numPointsInSector;
    }

    void Parser::createAndPublishCompleteScan()
    {
        auto firstMessage = partialSectorScanDataList[0];

        CompleteLidarMessage* lidarMessage = new CompleteLidarMessage();

        lidarMessage->totalPoints = firstMessage->numPoints;
        lidarMessage->endAngle = firstMessage->deviceNumber;
        lidarMessage->startAngle = firstMessage->startAngle;
        lidarMessage->endAngle = firstMessage->endAngle;
        lidarMessage->timestamp = firstMessage->timestamp;

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
            for (int i = 0; i < length; i++)
            {
                std::cout << buf[i];
            }
            std::cout << std::endl;
            return length;
        }
        else
        {
            std::cout << "Failure to parse data from buffer" << std::endl;
            return length;
        }
    }
}
}
}
}
