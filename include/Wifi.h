//
// Created by permal on 6/24/17.
//

#pragma once

#include <string>

class Wifi
{
public:
    Wifi(const std::string &ssid, const std::string &password);
    Wifi(const Wifi&) = delete;
    Wifi(const Wifi&&) = delete;

    virtual ~Wifi();

    bool is_connected() const;

private:

};

