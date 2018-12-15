#pragma once

class IWiegandSignal
{
    public:
        virtual void wiegand_number(uint8_t num) = 0;
        virtual void wiegand_id(uint32_t id, uint8_t byte_count) = 0;
};
