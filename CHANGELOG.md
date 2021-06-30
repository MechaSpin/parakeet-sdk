# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [2.0.0] - In-Development
### Added
- Added sensor starting angle and spin rotation to the documentation
- Added the option to convert from ScanDataXY to ScanDataPolar
- Added a deprecation macro
### Modified
- Changed Driver code to start a ScanData set at 0 degrees, rather than 36-72 degrees
- Changed ScanDataPolar / ScanDataXY to require a timestamp on creation, which signifys when the first point was created
- Changed Windows and Linux if defines to be more consistent
- Modified SimpleExample CMake to find the latest version of parakeet-sdk which can be found
- Properly labeled units of measure on variables

## [1.0.1] - 2021-06-09
### Modified
- Changed documentation to more clearly state the need for the UART to USB Driver.

## [1.0.0] - 2021-06-07
### Added
- Driver for communicating with a Parakeet Pro through a serial port
- Support for modifying sensor settings on the fly
