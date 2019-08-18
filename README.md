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
at the cost of some extra used bytes of RAM. However, where it is appropriate, such as with the *Queue*, things are designed so
that the result is a memory static instance, i.e. a *smooth::ipc::Queue* will _not_ behave like an *std::vector*.  

Apart from hardware/IDF-specific classes, applications written using Smooth can be compiled and run on POSIX systems (e.g. Linux)
without any special considerations. 

*) To certain limits, of course.

## Requirements

Smooth is build to work with ESP-IDF v4.x and compiled using gcc 8.

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
- Flash and SDCard initialization.

### Application level

- HTTP(s) Server
  - Simple templates 
  - Websocket support
- MQTT Client
- Sensor BME280
- 16 channel I/O expander MCP23017
- RGB LED, i.e. WS2812(B), SK6812, WS2813, (a.k.a NeoPixel). 


## Using Smooth in your project

In your ESP-IDF projects's root folder, type the following to add `smooth` as a submodule.

```Bash
git submodule add https://github.com/PerMalmberg/Smooth.git externals/smooth
```

Assuming you are following IDF's recommended way of [structuring projects](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html#example-project), make your top `CMakeLists.txt` look something like this:

```
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

```
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

### Sample/test applications

Please see the the different test projects under the test folder. When compiling these, open the
root of the repo as a CMake project. Select the project you wish to build by setting `selected_test_project` 
in the top `CMakeLists.txt`. **You will likely have to re-generate your build files after changing the selection.**  
