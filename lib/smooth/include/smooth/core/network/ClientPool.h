#pragma once

#include <deque>
#include <memory>
#include <vector>
#include <algorithm>

namespace smooth::core
{
    class Task;
}

namespace smooth::core::network
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

            template<typename ...Args>
            void create_clients(Args ...args);

        private:
            smooth::core::Task& task;
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
        if (found != in_use.end())
        {
            in_use.erase(found);
        }

        clients.push_back(std::move(client));
    }

    template<typename Client>
    ClientPool<Client>::ClientPool(smooth::core::Task& task, int count)
        :task(task)
    {
        in_use.reserve(static_cast<typename decltype(in_use)::size_type>(count));
    }

    template<typename Client>
    template<typename... ProtocolArguments>
    void ClientPool<Client>::create_clients(ProtocolArguments... args)
    {
        for (std::size_t i = 0; i < in_use.capacity(); ++i)
        {
            clients.emplace_back(std::make_shared<Client>(task, *this, args...));
        }
    }
}
