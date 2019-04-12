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
                    std::vector<std::shared_ptr<Client>> in_use{};
            };

            template<typename Client, int Count>
            std::shared_ptr<Client> ClientPool<Client, Count>::get()
            {
                std::shared_ptr<Client> c{};

                if (!clients.empty())
                {
                    c = *clients.begin();
                    in_use.push_back(c);
                    clients.pop_front();
                }

                return c;
            }

            template<typename Client, int Count>
            void ClientPool<Client, Count>::return_client(std::shared_ptr<Client> client)
            {
                client->reset();
                auto found = std::find(in_use.begin(), in_use.end(), client);
                if(found != in_use.end())
                {
                    in_use.erase(found);
                }

                clients.push_back(std::move(client));
            }

            template<typename Client, int Count>
            ClientPool<Client, Count>::ClientPool(smooth::core::Task& task)
            {
                for(int i = 0; i < Count; ++i)
                {
                    clients.emplace_back(std::make_shared<Client>(task, *this));
                }
            }
        }
    }
}