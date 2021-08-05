# ConfigureProEIPv4Settings Building and Running

## Windows

#### 1. Install the parakeet-sdk following the [Building and Installing](Building%20and%20Installing.md) instructions
#### 2. Create a folder inside {PARAKEET_ROOT}/utils/ConfigureProEIPv4Settings called "build"
#### 3. Open CMake GUI
- Click browse source
- Navigate to {PARAKEET_ROOT}/utils/ConfigureProEIPv4Settings
- Click browse build
- Navigate to {PARAKEET_ROOT}/utils/ConfigureProEIPv4Settings/build
- Click configure
- Select your version of Visual Studio from the combobox
- If on a 64-bit machine, set the generator to x64
- Click Finish
- Click Configure
- Once configuration completes, click Generate

#### 4. Building with Visual Studio
- Open Visual Studio
- Open ConfigureProEIPv4Settings.sln which was created by CMake in {PARAKEET_ROOT}/utils/ConfigureProEIPv4Settings/build
- In the solution explorer, select the ConfigureProEIPv4Settings project
- Right click the project, and select build

#### 5a. Running using Visual Studio
- In the solution explorer, right click on the ConfigureProEIPv4Settings project, and select properties
- Under the debugging tab, in the Command Arguments section, insert the following arguments:
`destinationIPAddress destinationPort sourcePort` ie: `192.168.158.98 6543 6668`
- Note: Using a BAUDRATE of 0 will have the application automatically detect the BAUDRATE

#### 5b. Running using Command Prompt
- Open command prompt
- Navigate to the build folder, followed by Debug or Release
- Run the app by executing 

```
{PARAKEET_ROOT}/utils/ConfigureProEIPv4Settings/build/ConfigureProEIPv4Settings.exe destinationIPAddress destinationPort sourcePort (ie: "192.168.158.98 6543 6668")
```

## Linux
#### 1. Install the parakeet-sdk following the [Building and Installing](Building%20and%20Installing.md) instructions
#### 2. In a terminal, navigate to {PARAKEET_ROOT}/utils/ConfigureProEIPv4Settings/
#### 3. Building ConfigureProEIPv4Settings
```
mkdir build
cd build
cmake ..
make
```

#### 4. Running ConfigureProEIPv4Settings
- Run the app by executing the following:

```
{PARAKEET_ROOT}/utils/ConfigureProEIPv4Settings/build/ConfigureProEIPv4Settings.exe destinationIPAddress destinationPort sourcePort (ie: "192.168.158.98 6543 6668")
```