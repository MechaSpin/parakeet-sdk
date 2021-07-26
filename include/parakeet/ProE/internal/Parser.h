/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#ifndef PARAKEET_PROE_PARSER_H
#define PARAKEET_PROE_PARSER_H

#include <functional>
#include <memory>
#include <vector>

namespace mechaspin
{
namespace parakeet
{
namespace ProE
{
namespace internal
{
class Parser
{
	public:
		struct LidarSensorProperties
		{
			bool unitIsInCM;
			bool withIntensity;
			bool doDragPointRemoval;
			bool doDataSmoothing;

			uint32_t value;
		};

		struct LidarPoint
		{
			uint16_t distance;
			uint16_t relativeStartAngle;
			uint8_t intensity;
		};

		struct CompleteLidarMessage
		{
			uint16_t totalPoints;
			uint32_t startAngle;
			uint32_t endAngle;

			LidarSensorProperties sensorPropertyFlags;

			uint32_t timestamp;
			uint32_t deviceNumber;

			std::vector<LidarPoint> lidarPoints;
		};

		Parser(std::function<void(CompleteLidarMessage*)> onCompleteLidarMessageCallback);

		int parseSensorBuffer(int length, unsigned char* buf);

	private:
		struct BufferData
		{
			int length;
			unsigned char* buffer;
		};

		struct PartialLidarMessage
		{
			uint16_t numPoints;
			uint16_t numPointsInSector;
			uint16_t sectorDataOffset;

			uint32_t startAngle;
			uint32_t endAngle;

			LidarSensorProperties sensorPropertyFlags;

			uint32_t timestamp;
			uint32_t deviceNumber;

			std::vector<LidarPoint> lidarPoints;

			uint16_t checksum;
		};

		void parseHeader();

		void parsePartialScan();

		void parseNumPointsInThisPartialSector();
		void parseNumPointsInSector();
		void parseSectorDataOffset();
		void parseStartAngle();
		void parseEndAngle();
		void parseSensorProperties();
		void parseTimestamp();
		void parseDeviceNumber();
		void parsePoints();
		void parseChecksum();

		int parseLidarDataFromBuffer();

		bool doesChecksumMatch();

		bool isLidarMessage();
		bool isLidarResponse();

		int getTotalPointsOfAllPartialScans();

		bool isScanCorrupt();
		bool isScanComplete();
		void createAndPublishCompleteScan();

		uint16_t header;
		std::shared_ptr<PartialLidarMessage> currentLidarMessage;
		std::vector<std::shared_ptr<PartialLidarMessage>> partialSectorScanDataList;
		BufferData bufferData;

		std::function<void(CompleteLidarMessage*)> onCompleteLidarMessageCallback;
};
}
}
}
}

#endif