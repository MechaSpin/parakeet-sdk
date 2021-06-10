# Parakeet Library Build Procedure

## Windows

#### 1. Install [Git](https://git-scm.com/)
#### 2. Clone parakeet-sdk, the root directory will be refered to as {PARAKEET_ROOT}

```
git clone https://github.com/MechaSpin/parakeet-sdk
```

#### 3. Install [Microsoft Visual Studio](https://visualstudio.microsoft.com/downloads/)
#### 4. Install [CMake](https://cmake.org/download/)
#### 5. Install the [UART to USB Driver](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers).
#### 6. Create a folder inside {PARAKEET_ROOT} called "build"
#### 7. Open CMake GUI
- Click browse source
- Select {PARAKEET_ROOT}
- Click browse build
- Select {PARAKEET_ROOT}/build
- Click configure
- Select your version of Visual Studio from the combobox
- If on a 64-bit machine, set the generator to x64
- Click Finish
- Click Configure
- Once configuration completes, click Generate

#### 8. Building with Visual Studio
- Once CMake generation completes, open Visual Studio as administrator (required for installing to Program Files)
- Open Parakeet.sln which was created by CMake in the previously made build folder
- In the solution explorer, select the Parakeet project
- Right click the project, and select build

#### 9. Installing with Visual Studio
- In the solution explorer, select the INSTALL project
- Right click the project, and select build
- Note: this will install the project to: 

```
Program Files/Parakeet
```

## Linux

#### 1. Install build tools
	sudo apt install build-essential
#### 2. Install CMake
	sudo apt install cmake-gui
#### 3. Clone parakeet-sdk, the root directory will be refered to as {PARAKEET_ROOT}

```
git clone https://github.com/MechaSpin/parakeet-sdk
```

#### 4. Building parakeet-sdk
- In a terminal, navigate to {PARAKEET_ROOT}
- Execute the following in the same terminal

```
mkdir build
cd build
cmake ..
make
```

### 5. Installing parakeet-sdk
- Navigate into the build folder

```
sudo make install
```

- Note: this will install the project to: 

```
/usr/local/lib/Parakeet/
/usr/local/include/Parakeet/
```