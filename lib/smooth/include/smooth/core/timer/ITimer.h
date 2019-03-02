//
// Created by permal on 7/16/17.
//

#pragma once

#include <string>
#include <chrono>

namespace smooth
{
    namespace core
    {
        namespace timer
        {
            /// Interface for timers
            class ITimer
            {
                public:
                    virtual ~ITimer() = default;

                    /// Starts the timer with the already set interval.
                    virtual void start() = 0;
                    /// Starts the timer with the specified interval.
                    /// \param interval The new interval
                    virtual void start(std::chrono::milliseconds interval) = 0;
                    /// Stops the timer
                    virtual void stop() = 0;
                    /// Resets the timer, i.e. starts a new interval.
                    virtual void reset() = 0;
                    /// Gets the ID of the timer.
                    /// \return The id.
                    virtual int16_t get_id() const = 0;
                    /// Gets the name of the timer
                    /// \return The name.
                    virtual const std::string& get_name() = 0;
            };
        }
    }
}