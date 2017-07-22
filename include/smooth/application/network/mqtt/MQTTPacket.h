//
// Created by permal on 7/15/17.
//

#pragma once

#include <vector>
#include <bitset>
#include <smooth/core/network/IReceivablePacket.h>
#include <smooth/core/network/ISendablePacket.h>
#include <smooth/application/network/mqtt/MQTTProtocolDefinitions.h>

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace mqtt
            {
                typedef std::bitset<8> ByteSet;

                class MQTTPacket
                        : public smooth::core::network::IReceivablePacket,
                          public smooth::core::network::ISendablePacket
                {
                    public:
                        MQTTPacket();

                        // Must return the number of bytes the packet wants to fill
                        // its internal buffer, e.g. header, checksum etc. Returned
                        // value will differ depending on how much data already has been provided.
                        int get_wanted_amount() override;

                        // Used by the underlying framework to notify the packet that {length} bytes
                        // has been written to the buffer pointed to by get_write_pos().
                        // During the call to this method the packet should do whatever it needs to
                        // evaluate if it needs more data or if it is complete.
                        void data_received(int length) override;

                        // Must return the current write position of the internal buffer.
                        // Must point to a buffer than can accept the number of bytes returned by
                        // get_wanted_amount().
                        uint8_t* get_write_pos() override;

                        // Must return true when the packet has received all data it needs
                        // to fully assemble.
                        bool is_complete() override;

                        // Must return true whenever the packet is unable to correctly assemble
                        // based on received data.
                        bool is_error() override;

                        // Must return the total amount of bytes to send
                        int get_send_length() override;
                        // Must return a pointer to the data to be sent.
                        const uint8_t* get_data() override;

                        void dump() const;

                    protected:
                        PacketType mqtt_get_type() const;
                        void set_header(PacketType type, int flags);

                        void encode_remaining_length(int length);

                        void append_string(const std::string& str, std::vector<uint8_t>& target);

                        std::vector<uint8_t> packet{};
                    private:

                        int calculate_remaining_length();

                        enum ReadingHeaderSection
                        {
                            START = 1,
                            REMAINING_LENGTH,
                            DATA
                        };
                        static const int REMAINING_LENGTH_OFFSET = 1;
                        ReadingHeaderSection state = ReadingHeaderSection::START;
                        int bytes_received = 0;
                        int current_length = 0;
                        int wanted_amount = 1;
                        bool error = false;
                };
            }
        }
    }
}