/*
Smooth - A C++ framework for embedded programming on top of Espressif's ESP-IDF
Copyright 2019 Per Malmberg (https://gitbub.com/PerMalmberg)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#pragma once

#include <memory>
#include <type_traits>

namespace smooth::core::util
{
    template<typename T, typename... Args>
    auto create_protected_shared(Args&& ... args)
    {
        class CreationWrapper
            : public T
        {
            public:
                explicit CreationWrapper(Args&& ... args)
                        : T(std::forward<Args>(args)...)
                {}
        };

        return std::make_shared<CreationWrapper>(std::forward<Args>(args)...);
    }

    template<typename T, typename... Args>
    auto create_protected_unique(Args&& ... args)
    {
        class CreationWrapper
            : public T
        {
            public:
                explicit CreationWrapper(Args&& ... args)
                        : T(std::forward<Args>(args)...)
                {}
        };

        return std::make_unique<CreationWrapper>(std::forward<Args>(args)...);
    }
}
