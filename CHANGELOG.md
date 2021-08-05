# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [3.0.0] - 2021-08-02
### Added
- Added a base driver class mechaspin::parakeet::Driver which holds common functionality between sensor drivers
- Added a driver class to support the Parakeet ProE (mechaspin::parakeet::ProE::Driver)
- Added an example application to set the IPv4 settings of a Parakeet-ProE sensor

### Modified
- Updated SimpleExample to support the Parakeet ProE sensor (v2.0.0)

### Removed
- [BREAKING] Removed mechaspin::parakeet::Driver, replaced by mechaspin::parakeet::Pro::Driver
- [BREAKING] Removed PointPolar.getRange(), replaced by PointPolar.getRange_mm()
- [BREAKING] Removed PointPolar.getAngleInDegrees(), replaced by PointPolar.getAngle_deg()

## [2.1.0] - 2021-07-13
### Deprecated
- Deprecated class mechaspin::parakeet::Driver, replaced by mechaspin::parakeet::Pro::Driver

## [2.0.0] - 2021-07-12
### Added
- Added sensor starting angle and spin rotation to the documentation
- Added the option to convert from ScanDataXY to ScanDataPolar
- Added PointPolar.getRange_mm()
- Added PointPolar.getAngle_deg()
- Added a deprecation macro
### Modified
- Changed Driver code to start a ScanData set at 0 degrees, rather than 36-72 degrees
- [BREAKING] Changed ScanDataPolar / ScanDataXY to require a timestamp on creation, which signifies when the first point was created
- Changed Windows and Linux if defines to be more consistent
- Modified SimpleExample CMake to find the latest version of parakeet-sdk which can be found
- Properly labeled units of measure on variables
- Updated copyright action to call the action from inside the parakeet-devtools repo
### Deprecated
- Deprecated method PointPolar.getRange(), use PointPolar.getRange_mm()
- Deprecated method PointPolar.getAngleInDegrees(), use PointPolar.getAngle_deg()
- Deprecated method Driver.connect requiring all sensor settings. Replaced with Driver.connect which takes in a SensorConfiguration object which holds all sensor settings

## [1.0.1] - 2021-06-09
### Modified
- Changed documentation to more clearly state the need for the UART to USB Driver

## [1.0.0] - 2021-06-07
### Added
- Driver for communicating with a Parakeet Pro through a serial port
- Support for modifying sensor settings on the fly
