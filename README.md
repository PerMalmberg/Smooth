# Smooth
C++ framework for writing applications based on Espressif's ESP-IDF.

## Forking?

Unless you're seriously intending to contribute to the project, please do not fork. Press that :star:-button instead - it shows your
support for the project while keeping the number of (potentially) dead forks to a minimum.

## Overview

Smooth provides a set of classes that makes life as a developer easier compared to working directly with ESP-IDF & FreeRTOS APIs.
An application built with Smooth is entirely event driven and thread-safe*. Smooth utilizes the power of FreeRTOS, but hides all the complexities from the application programmer.

Traditionally, embedded systems require a fully static memory footprint after start-up. Smooth takes
a somewhat more pragmatic view on this; it utilizes the standard library (which is not memory static) to provide cleaner code,
at the cost of some extra used bytes of RAM. However, where it is appropriate, such as with the queues, things are designed so
that the result is a memory static instance, i.e. a `smooth::ipc::Queue` will have a memory static footprint once initialized.  

[mock-idf](mock-idf/README.md) provides the ability to compile even applications that uses ESP-32 hardware for Linux
with the only consideration that the mocks do not actually simulate the hardware. 

*) To certain limits, of course.

## Requirements

* ESP-IDF v4.x
* GCC 8

Smooth is developed on a Linux machine so how well it compiles using the Windows toolset povided by Espressif is unknown. 

### Provided functionality

#### Core

- Application initialization
- Wifi configuration / control
- Tasks
- Queues with support for proper C++ objects, not just plain data structures
- Timer Events
- Event-driven TCP Sockets, including TLS support and server sockets.
- System events

#### Hardware level

- Output
- Input
- Input with interrupt to event translation
- I2C Master Device class
- SPI Master Device class
- Flash and SDCard initialization.

### Application level

- HTTP(s) Server
  - Simple templates 
  - Websocket support
- MQTT Client
- Device support
  - SPI
    - Sensors
      - BME280
    - Displays
      - ILI9341
      - ST7735
      - SH1107
  - I2C
    - BME280
    - MCP23017
    - DHT12
    - AxpPMU
    - PCF8563
  - RGB LED, i.e. WS2812(B), SK6812, WS2813, (a.k.a NeoPixel). 
- Filesystem helpers


## Using Smooth in your project (compiling for ESP)

In your projects's root folder, type the following to add `smooth` as a submodule.

```shell script
git submodule add https://github.com/PerMalmberg/Smooth.git externals/smooth
```

Then, to retrieve Smooth and its submodules, run:

```shell script
git submodule update --init --checkout --recursive
``` 

Assuming you are following IDF's recommended way of [structuring projects](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html#example-project), make your top `CMakeLists.txt` look something like this:

```cmake
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

if(${ESP_PLATFORM})
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Include Smooth as a component
set(EXTRA_COMPONENT_DIRS
         externals/smooth/smooth_component)

project(name_of_your_project)
else()
    # Empty project when not building for ESP (i.e. when loading the project into an IDE with already configured tool chains for native Linux)
endif()
```

Next, your `main/CMakeLists.txt` should look something like this:

```cmake
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

set(SOURCES main.cpp
        main.cpp
        main.h
        )

idf_component_register(SRCS ${SOURCES}
        INCLUDE_DIRS
            ${CMAKE_CURRENT_LIST_DIR}
            $ENV{IDF_PATH}/components
        REQUIRES
            smooth_component
        )
```

Now build your project using the following commands, or via a properly setup IDE.

```shell script
cd your_project_root
mkdir build && cd build
cmake .. -G "Ninja" -DESP_PLATFORM=1 -DCMAKE_TOOLCHAIN_FILE=$IDF_PATH/tools/cmake/toolchain-esp32.cmake && ninja
```

or, if you're using old-fashioned `make`

```shell script
cd your_project_root
mkdir build && cd build
cmake .. -DESP_PLATFORM=1 -DCMAKE_TOOLCHAIN_FILE=$IDF_PATH/tools/cmake/toolchain-esp32.cmake && ninja
```

Next, flash your project to the target device.

`idf.py -C .. --baud 921600 -p /dev/ttyUSB1 app-flash monitor`

If it's the first time, use this command instead to also flash the partition table:

`idf.py -C .. --baud 921600 -p /dev/ttyUSB1 flash monitor`

### Menuconfig / sdkconfig

Don't forget to configure your target properly by running `ninja menuconfig` to update your file `sdkconfig` before building.
There is an `sdkconfig` file included with Smooth and used in the test projects. While you can use it for as a base by copying
it to your project root, you are encouraged to adjust it to your specific needs and use case.

## Sample/test applications

Please see the the different test projects under the test folder. When compiling these, open the
root of the repo as a CMake project. Select the project you wish to build by setting `selected_test_project` 
in the top `CMakeLists.txt`. **You will likely have to re-generate your build files after changing the selection.**  

## Using Smooth in your project (compiling for Linux or MacOS)

### System Libraries
Some libraries provided in the ESP distribution need to be installed as system libraries on the host. On Debian / Ubuntu:

```shell
apt-get install libsodium-dev libmbedtls-dev
```

With Homebrew on MacOS:

```shell
brew install libsodium mbedtls
```


### CMake Config
To build your application on the host platform you must maintain a parallel build configuration as follows:

Top `CMakeList.txt`

```cmake
if(${ESP_PLATFORM})
    include($ENV{IDF_PATH}/tools/cmake/project.cmake)

    # Include Smooth as a component
    set(EXTRA_COMPONENT_DIRS
             externals/smooth/smooth_component)
    project(your_project_name)
else()
    if(${APPLE})
        include_directories(SYSTEM /usr/local/include)
        link_directories(/usr/local/lib)
    endif()


    add_subdirectory(main)
    add_subdirectory(externals/smooth/lib)
    add_subdirectory(externals/smooth/mock-idf)
endif()
``` 

Your `main/CMakeList.txt`:

```cmake
cmake_minimum_required(VERSION 3.10)

set(SOURCES} app.cpp app.h)

if(${ESP_PLATFORM})
    idf_component_register(SRCS ${SOURCES}
                            INCLUDE_DIRS
                                ${CMAKE_CURRENT_LIST_DIR}
                                $ENV{IDF_PATH}/components
                            REQUIRES
                                smooth_component)
else()
    project(your_project_name.elf)
    add_executable(${PROJECT_NAME} ${SOURCES})
    target_link_libraries(${PROJECT_NAME} smooth pthread)
    target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR})
endif()
```

### Building on MacOS
Currently only GCC is supported. You can install GCC via homebrew and then pass flags to CMake to use that compiler:

```shell
brew install gcc
mkdir cmake-macos
cd cmake-macos
cmake -DCMAKE_C_COMPILER=gcc-9 -DCMAKE_CXX_COMPILER=g++-9 ..
make
```


Here's an example of how your `main.cpp` could look like if you want to compile for both ESP and Linux. The example
 assumes you have named your main class `App`  and it is derived from `smooth::core::Application`, which most
  applications based on Smooth do. Doing so is not mandatory, it saves you some setup; see
   [Application.cpp](https://github.com/PerMalmberg/Smooth/blob/master/lib/smooth/core/Application.cpp) for 
   details on what it does for you.  

```c++

extern "C"
{
#ifdef ESP_PLATFORM
void app_main()
{
    App app{};
    app.start();
}
#else
int main(int /*argc*/, char** /*argv*/)
{
    App app{};
    app.start();
    return 0;
}
#endif

}
```
