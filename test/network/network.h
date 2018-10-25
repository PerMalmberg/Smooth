#pragma once

#include <smooth/core/Application.h>
#include <smooth/core/ipc/IEventListener.h>
#include <smooth/core/ipc/TaskEventQueue.h>
#include <smooth/application/network/mqtt/MqttClient.h>
#include <random>

#ifdef ESP_PLATFORM
#include <smooth/core/io/Output.h>
#endif

namespace network
{

    class App
            : public smooth::core::Application,
              smooth::core::ipc::IEventListener<smooth::application::network::mqtt::MQTTData>
    {
    public:

        App();

        void init() override;

        void event(const smooth::application::network::mqtt::MQTTData& event) override;

        void send_message();

        void tick() override;

    private:
        smooth::core::ipc::TaskEventQueue<smooth::application::network::mqtt::MQTTData> mqtt_data;
        smooth::application::network::mqtt::MqttClient client;
        std::random_device rand{};
        std::mt19937 gen{rand()};
        std::uniform_int_distribution<> dis{1, 3};
    };

}