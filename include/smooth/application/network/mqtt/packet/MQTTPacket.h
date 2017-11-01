//
// Created by permal on 7/15/17.
//

#pragma once

#include <vector>
#include <smooth/core/util/ByteSet.h>
#include <smooth/core/network/IPacketAssembly.h>
#include <smooth/core/network/IPacketDisassembly.h>
#include <smooth/application/network/mqtt/MQTTProtocolDefinitions.h>
#include <smooth/application/network/mqtt/packet/PacketIdentifierFactory.h>
#include "IPacketReceiver.h"

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace mqtt
            {
                namespace packet
                {
                    class MQTTPacket
                            : public smooth::core::network::IPacketAssembly,
                              public smooth::core::network::IPacketDisassembly
                    {
                        public:
                            MQTTPacket() = default;
                            MQTTPacket& operator=(const MQTTPacket&) = default;
                            MQTTPacket(const MQTTPacket& other) = default;

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

                            bool is_too_big() const;

                            void dump(const char* header) const;

                            PacketType get_mqtt_type() const;
                            const char* get_mqtt_type_as_string() const;

                            virtual void visit(IPacketReceiver& receiver);

                            QoS get_qos() const;

                            virtual uint16_t get_packet_identifier() const
                            {
                                return 0;
                            }

                            void set_dup_flag();

                            bool validate_packet() const;

                            virtual std::vector<uint8_t>::const_iterator get_payload_cbegin() const
                            {
                                return packet.cend();
                            }

                        protected:

                            virtual bool has_packet_identifier() const
                            {
                                return false;
                            }

                            virtual bool has_payload() const
                            {
                                return false;
                            }

                            virtual int get_variable_header_length() const
                            {
                                return 0;
                            }

                            long get_payload_length() const;

                            uint16_t read_packet_identifier(std::vector<uint8_t>::const_iterator pos) const
                            {
                                return *pos << 8 | *(pos + 1);
                            }

                            void set_header(PacketType type, QoS qos, bool dup, bool retain);
                            void set_header(PacketType type, uint8_t flags);

                            void encode_remaining_length(int length);

                            void append_string(const std::string& str, std::vector<uint8_t>& target);
                            void append_msb_lsb(uint16_t value, std::vector<uint8_t>& target);
                            void append_data(const uint8_t* data, int length, std::vector<uint8_t>& target);
                            void apply_constructed_data(const std::vector<uint8_t>& variable);

                            std::vector<uint8_t> packet{};
                            int calculate_remaining_length_and_variable_header_offset() const;
                            std::string get_string(std::vector<uint8_t>::const_iterator offset) const;

                            std::vector<uint8_t>::const_iterator get_variable_header_start() const
                            {
                                return packet.cbegin() + variable_header_start_ix;
                            }

                        private:

                            enum ReadingHeaderSection
                            {
                                START = 1,
                                REMAINING_LENGTH,
                                DATA
                            };
                            mutable long variable_header_start_ix = 0;
                            ReadingHeaderSection state = ReadingHeaderSection::START;
                            int bytes_received = 0;
                            int remaining_bytes_to_read = 1;
                            int received_header_length = 0;
                            mutable bool error = false;
                            bool too_big = false;
                    };
                }
            }
        }
    }
}