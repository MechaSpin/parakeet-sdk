# SimpleExample Code Explanation

SimpleExample is an application which utilizes parakeet-sdk to show off how to properly use the api. This document will be going over each important section of code and providing an explanation.

### Understanding main
First things first, when interacting with our sensor, we will need to create an instance of the parakeet Driver. 
Connecting to a serial port with our instance of Driver requires knowledge of the sensors connected serial port and baudrate.

```C++
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
```

Above, we check the command line arguments for two parameters, and assign them to comPort and baudRate. 
This allows the application to be run as ./SimpleExample COMPORT BaudRate.

Next, we need to know what settings we would like the sensor to use when connected to.

```C++
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
```

Rather than asking for these parameters at runtime, we set our own values for each parameter. 
Afterwards, we try to connect to the serial port. 
If not successful, we will exit the application with an error stating it was unable to connect.

Now that the serial port is open, we need to start reading data from it.

```C++
parakeetSensorDriver.registerScanCallback(onScanComplete);

std::cout << "Starting driver." << std::endl;

parakeetSensorDriver.start();
```

First thing we do is register the callback function of the driver to onScanComplete. 
This function will be called each time a full rotation of points is received from the sensor. 
We them start the driver to allow it to start reading from the serial port.

Now that the sensor is running and the Driver is receiving data, we wanted to expose some options to prove we are getting data from the sensor, and double check our settings in realtime.

```C++
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
```

This big chunk of code is responsible for controlling command line input. 
Each command is tied to a specific character and results in different actions. 
The printOptionsMenu() function is responsible for displaying how to trigger each action.

```C++
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
```

#### Print Driver FPS action

```C++
case 'f':
	std::cout << "FPS: " << parakeetSensorDriver.getScanRate_Hz() << std::endl;
	break;
```

When the user types the 'f' character, the application will print out the scan rate of the driver.

#### Print Driver status action

```C++
case 's':
	std::cout << "Intensity data enabled: " << parakeetSensorDriver.isIntensityDataEnabled() << std::endl;
	std::cout << "Drag point enabled: " << parakeetSensorDriver.isDragPointRemovalEnabled() << std::endl;
	std::cout << "Data smoothing enabled: " << parakeetSensorDriver.isDataSmoothingEnabled() << std::endl;
	std::cout << "Scanning Frequency (Hz): " << parakeetSensorDriver.getScanningFrequency_Hz() << std::endl;
	std::cout << "Baud Rate: " << parakeetSensorDriver.getBaudRate().getValue() << std::endl;
	break;
```

When the user types the 's' character, the application will print out the state of all the parameters.

#### Print options page action

```C++
case '?':
		printOptionsMenu();
		break;
```

When the user types the '?' character, the application will print out the option menu again.

#### Quit application action

```C++
case 'q':
	std::cout << "Shutting application down." << std::endl;
	commandLoop = false;
	parakeetSensorDriver.close();
	break;
```

When the user types the 'q' character, the application will exit out of the action command loop and close the Driver.

#### Print last read min/max Polar point action

```C++
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
```

When the user types the 'z' character, the application should print out the closest point and the furthest point, and the timestamp of when it occured, in polar form.

#### Print last read min/max XY point action

```C++
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
```

When the user types the 'x' character, the application should print out the closest point and the furthest point, and the timestamp of when it occured, in cartesian form. 
This is done almost identical to the polar min/max action. 
The one key difference is we convert the PointPolar into PointXY using mechaspin::parakeet::util::transform:

```C++
mechaspin::parakeet::PointXY minPointXY = mechaspin::parakeet::util::transform(*minPoint);
mechaspin::parakeet::PointXY maxPointXY = mechaspin::parakeet::util::transform(*maxPoint);
```

### Understanding onScanComplete
onScanComplete is responsible for taking a set of scan data, and finding the minimum and maximum points to be printed later.
This function is called from the Driver when a set of data is read from the sensor, due to the fact we called driver.registerScanCallback earlier. 

```C++
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
```

This function makes a copy of the scan data and stores it into mostRecentScanData

```C++
mostRecentScanData = std::make_shared<mechaspin::parakeet::ScanDataPolar>(scanData);
```

We reset the last set of min/max points

```C++
minPoint.reset();
maxPoint.reset();

minPoint = nullptr;
maxPoint = nullptr;
```

We store the timestamp

```C++
timestamp = scanData.getTimestamp();
```

Lastly, we calculate the min and max points from the data set

```C++
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
```

With these pieces in place, the print min/max actions can print out this data.