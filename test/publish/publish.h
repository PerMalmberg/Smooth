//
// Created by permal on 2018-10-26.
//

#include <smooth/core/Application.h>
#include <smooth/core/timer/ElapsedTime.h>

namespace publish
{

    class ItemToPublish
    {
    public:
        ItemToPublish() = default;

        explicit ItemToPublish(int v)
                : val(v)
        {
        }

        int val = 0;
    };

    class PublisherTask
            : public smooth::core::Task
    {
    public:
        PublisherTask();

        void init() override;

        void tick() override;

    private:
        int curr = 0;
    };


    class App
            : public smooth::core::Application,
              smooth::core::ipc::IEventListener<ItemToPublish>
    {
    public:
        App();

        void init() override;

        void tick() override;

        void event(const ItemToPublish&) override;

    private:
        smooth::core::ipc::SubscribingTaskEventQueue<ItemToPublish> sub;
        PublisherTask p{};
        smooth::core::timer::ElapsedTime elapsed_time{};
    };

}