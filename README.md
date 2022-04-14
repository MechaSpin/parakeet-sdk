# Parakeet-SDK

Driver library for the Parakeet-Pro and Parakeet-ProE LiDAR sensors.

Documentation for building and installing the library can be found [here](docs/Building%20and%20Installing.md).

## Additional information

The Parakeet-Pro sensor comes with a CP210x USB to UART Bridge connector which needs a specific driver installed to be used on Windows. You can find the latest driver [here](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers).

Both the Parakeet-Pro and Parakeet-ProE LiDAR sensors have their starting angle of 0 in the center of the device, on the opposite side of their connection port. Both devices spin clockwise.

## Test Apps

### SimpleExample

SimpleExample is a test app which shows the basics of interacting with the Parakeet-SDK. Documentation describing [build steps](docs/Building%20and%20Running%20SimpleExample.md) and [key sections](docs/SimpleExample%20Code%20Explanation.md) has been created to provide clarity.
