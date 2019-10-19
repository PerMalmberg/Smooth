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
    /// \brief The purpose of this template function is to allow forcing instantiation of classes to be held
    /// only by a std::shared_ptr, by making T's constructor protected. This us useful when you want
    /// to use std::shared_from_this and prevent an instance being directly created with new() which
    /// would result usage of in std::shared_from_this in an exception.
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

    /// \brief The purpose of this template function is to allow forcing instantiation of classes to be held
    /// only by a std::unique_ptr, by making T's constructor protected. As there's no std::unique_from_this,
    /// this method has limited usage compared to create_protected_shared.
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
