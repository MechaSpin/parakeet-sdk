# SimpleExample Building and Running

## Windows

#### 1. Install the parakeet-sdk following the [Building and Installing](Building%20and%20Installing.md) instructions
#### 2. Create a folder inside {PARAKEET_ROOT}/examples/SimpleExample called "build"
#### 3. Open CMake GUI
- Click browse source
- Navigate to {PARAKEET_ROOT}/examples/SimpleExample
- Click browse build
- Navigate to {PARAKEET_ROOT}/examples/SimpleExample/build
- Click configure
- Select your version of Visual Studio from the combobox
- If on a 64-bit machine, set the generator to x64
- Click Finish
- Click Configure
- Once configuration completes, click Generate

#### 4. Building with Visual Studio
- Open Visual Studio
- Open SimpleExample.sln which was created by CMake in {PARAKEET_ROOT}/examples/SimpleExample/build
- In the solution explorer, select the SimpleExample project
- Right click the project, and select build

#### 5a. Running using Visual Studio
- In the solution explorer, right click on the SimpleExample project, and select properties
- Under the debugging tab, in the Command Arguments section, insert the devices COMPORT followed by the baudrate (ie: "COM3 500000")
- Note: Using a BAUDRATE of 0 will have the application automatically detect the BAUDRATE

#### 5b. Running using Command Prompt
- Open command prompt
- Navigate to the build folder, followed by Debug or Release
- Run the app by executing 

```
{PARAKEET_ROOT}/examples/SimpleExample/build/SimpleExample.exe COMPORT BAUDRATE (ie: "COM3 500000")
```

- Note: Using a BAUDRATE of 0 will have the application automatically detect the BAUDRATE

## Linux
#### 1. Install the parakeet-sdk following the [Building and Installing](Building%20and%20Installing.md) instructions
#### 2. In a terminal, navigate to {PARAKEET_ROOT}/examples/SimpleExample/
#### 3. Building SimpleExample
```
mkdir build
cd build
cmake ..
make
```

#### 4. Running SimpleExample
- Run the app by executing 

```
{PARAKEET_ROOT}/examples/SimpleExample/build/SimpleExample.exe COMPORT BAUDRATE (ie: "/dev/ttyUSB0 500000")
```

- Note: Using a BAUDRATE of 0 will have the application automatically detect the BAUDRATE