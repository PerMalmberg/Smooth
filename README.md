# Smooth
C++ framework for writing applications based on Espressif's ESP-IDF.

## Overview

Smooth provides a set of classes that makes life as a developer easier compared to working directly with ESP-IDF & FreeRTOS APIs.
An application built with Smooth is entirely event driven and thread-safe. Smooth utilizes the power of FreeRTOS, but hides all the complexities from the application programmer.

### Provided functionality

- Application initialization
- Wifi configuration / control
- Tasks
- Queues
- Mutex / RecursiveMutex
- (Software) Timer Events
- Event-driven TCP Sockets (including SSL/TLS)
- System events

## Installation

In your ESP-IDF projects's root folder, type the following to add `smooth` as a submodule.

```Bash
git submodule add https://github.com/PerMalmberg/Smooth.git components/smooth
```

### Sample application

```C++
extern "C" void app_main()
{
    Application app("Main app", 4096, 6);
    app.set_system_log_level(ESP_LOG_ERROR);
    app.start();

    Wifi wifi;
    wifi.connect_to_ap("YourHostName", "Your SSID", "YourPassword", true);

    SocketDispatcher::instance();

    ESP_LOGV("Main", "Free heap: %u", esp_get_free_heap_size());

    smooth::core::Task::never_return();
}
```