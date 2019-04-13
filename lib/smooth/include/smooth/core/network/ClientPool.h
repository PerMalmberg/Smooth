#pragma once

#include <deque>
#include <memory>

namespace smooth
{
    namespace core
    {
        namespace network
        {
            /// ClientPool holds a number of client instances which are requested by the
            /// owning ServerSocket. When a client is done, i.e. connection closed, it
            /// is returned to the pool for reuse at a later time.
            /// \tparam Client The client type held by the pool.
            template<typename Client>
            class ClientPool
            {
                public:
                    ClientPool(smooth::core::Task& task, int count);

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

            template<typename Client>
            std::shared_ptr<Client> ClientPool<Client>::get()
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

            template<typename Client>
            void ClientPool<Client>::return_client(std::shared_ptr<Client> client)
            {
                client->reset();
                auto found = std::find(in_use.begin(), in_use.end(), client);
                if(found != in_use.end())
                {
                    in_use.erase(found);
                }

                clients.push_back(std::move(client));
            }

            template<typename Client>
            ClientPool<Client>::ClientPool(smooth::core::Task& task, int count)
            {
                for(int i = 0; i < count; ++i)
                {
                    clients.emplace_back(std::make_shared<Client>(task, *this));
                }
            }
        }
    }
}