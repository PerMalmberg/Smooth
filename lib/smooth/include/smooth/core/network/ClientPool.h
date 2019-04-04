#pragma once

#include <deque>
#include <memory>

namespace smooth
{
    namespace core
    {
        namespace network
        {
            template<typename Client, int Count = 5>
            class ClientPool
            {
                public:
                    explicit ClientPool(smooth::core::Task& task);

                    bool empty() const
                    {
                        return clients.empty();
                    }

                    std::shared_ptr<Client> get();

                    void return_client(std::shared_ptr<Client> client);

                private:
                    std::deque<std::shared_ptr<Client>> clients{};
            };

            template<typename Client, int Count>
            std::shared_ptr<Client> ClientPool<Client, Count>::get()
            {
                std::shared_ptr<Client> c{};

                if (!clients.empty())
                {
                    c = *clients.begin();
                    clients.pop_front();
                }

                return c;
            }

            template<typename Client, int Count>
            void ClientPool<Client, Count>::return_client(std::shared_ptr<Client> client)
            {
                client->reset();
                clients.push_back(std::move(client));
            }

            template<typename Client, int Count>
            ClientPool<Client, Count>::ClientPool(smooth::core::Task& task)
            {
                for(int i = 0; i < Count; ++i)
                {
                    clients.emplace_back(std::make_shared<Client>(task));
                }
            }
        }
    }
}