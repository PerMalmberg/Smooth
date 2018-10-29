#pragma once

#include <string>
#include <sstream>
#include <iomanip>
#include <vector>

namespace smooth
{
    namespace core
    {
        namespace logging
        {
            /// Base class for all log arguments
            class BaseArg
            {
                public:
                    virtual ~BaseArg() = default;
                    virtual operator const char*() const = 0;
                    virtual const std::string str() const = 0;

                    BaseArg(const BaseArg&) = delete;

                protected:
                    BaseArg() = default;
            };

            /// Base class for log arguments that holds a string to be logged.
            class BaseArgWithData
                    : public BaseArg
            {
                public:
                    explicit BaseArgWithData(std::string value)
                            : data(std::move(value))
                    {
                    }

                    operator const char*() const override
                    {
                        return data.c_str();
                    }

                    const std::string str() const override
                    {
                        return data;
                    }

                protected:
                    BaseArgWithData() = default;
                    std::string data;
            };

            /// Class to log a std::string& or const char*
            class Str
                    : public BaseArg
            {
                public:
                    explicit Str(const char* value)
                            : data(value)
                    {
                    }

                    explicit Str(const std::string& value)
                            : data(value.c_str())
                    {
                    }

                    operator const char*() const override
                    {
                        return data;
                    }

                    const std::string str() const override
                    {
                        return std::string(data);
                    }

                private:
                    const char* data;
            };

            /// Class to log an int32_t
            class Int32
                    : public BaseArgWithData
            {
                public:

                    explicit Int32(int32_t value)
                            : BaseArgWithData(std::to_string(value))
                    {
                    }
            };

            /// Class to log a unt32_t
            class UInt32
                    : public BaseArgWithData
            {
                public:
                    explicit UInt32(uint32_t value)
                            : BaseArgWithData(std::to_string(value))
                    {
                    }
            };

            /// Class to log an int64_t
            class Int64
                    : public BaseArgWithData
            {
                public:
                    explicit Int64(int64_t value)
                            : BaseArgWithData(std::to_string(value))
                    {
                    }
            };

            /// Class to log a uint64_t
            class UInt64
                    : public BaseArgWithData
            {
                public:
                    explicit UInt64(uint64_t value)
                            : BaseArgWithData(std::to_string(value))
                    {
                    }
            };

            /// Class to log a bool
            class Bool
                    : public BaseArgWithData
            {
                public:
                    explicit Bool(bool value)
                            : BaseArgWithData(value ? "true" : "false")
                    {
                    }
            };

            /// Template class for decimal values
            /// \tparam T type of value
            template<typename T>
            class Decimal
                    : public BaseArgWithData
            {
                protected:
                    explicit Decimal(T value)
                            : BaseArgWithData(std::to_string(value))
                    {
                    }

                    explicit Decimal(T value, int decimals)
                    {
                        std::stringstream ss;
                        ss << std::fixed << std::setprecision(decimals) << value;
                        data = ss.str();
                    }
            };

            /// Class to log a float
            class Float
                    : public Decimal<float>
            {
                public:
                    /// Logs a float value
                    /// \tparam value Value to log
                    explicit Float(float value)
                            : Decimal(value)
                    {
                    }

                    /// Logs a float value with the specified number of decimals
                    /// \tparam value Value to log
                    /// \tparam decimals Number of decimals
                    Float(float value, int decimals)
                            : Decimal(value, decimals)
                    {
                    }
            };

            /// Class to log a double
            class Double
                    : public Decimal<double>
            {
                public:
                    /// Logs a double value
                    /// \tparam value Value to log
                    explicit Double(double value)
                            : Decimal(value)
                    {
                    }

                    /// Logs a double value with the specified number of decimals
                    /// \tparam value Value to log
                    /// \tparam decimals Number of decimals
                    Double(double value, int decimals)
                            : Decimal(value, decimals)
                    {
                    }
            };


            /// Class to log a value in hex format
            template<typename T>
            class Hex
                    : public BaseArgWithData
            {
                public:
                    explicit Hex(const T& value, bool show_base = false)
                    {
                        std::stringstream ss;
                        // We cast to an uint64_t to get the formatting we want
                        ss << std::hex << (show_base ? std::showbase : std::noshowbase) << static_cast<uint64_t>(value);
                        data = ss.str();
                    }
            };

            /// Class to log a pointer address
            class Pointer
                    : public BaseArgWithData
            {
                public:
                    explicit Pointer(const void* value, bool show_base = false)
                    {
                        std::stringstream ss;
                        ss << std::hex << (show_base ? std::showbase : std::noshowbase) << value;
                        data = ss.str();
                    }
            };

            // Class to log a array of (possibly) binary data
            template<typename T>
            class Vector
                    : public BaseArgWithData
            {
                public:
                    /// Log the array
                    /// \param src The source
                    /// \param treat_as_readable_text If true, treat the data as readable text.
                    Vector(const std::vector<T>& src, bool treat_as_readable_text)
                    {
                        std::stringstream ss;
                        for (const auto& b : src)
                        {
                            if (treat_as_readable_text)
                            {
                                ss << static_cast<char>(b);
                            }
                            else
                            {
                                ss << "0x" << std::hex <<  static_cast<uint64_t>(b);
                            }
                        }
                        data = ss.str();
                    }
            };

            /// Class to format the log messages.
            /// Use "{1}...{n} as place holders in the format string for the respective BaseArg.
            class Format
            {
                public:
                    /// Logs the single argument as-is, without additional formatting.
                    Format(const char* arg)
                    {
                        data = arg;
                    }

                    /// Logs the single argument as-is, without additional formatting.
                    explicit Format(const BaseArg& arg)
                    {
                        data = arg;
                    }

                    explicit Format(const std::string& arg)
                    {
                        data = arg;
                    }

                    /// Replaces the all {1} in the format string with arg1 and logs the resulting string.
                    Format(const std::string& format, const BaseArg& arg1)
                    {
                        data = format;
                        replace(1, arg1);
                    }

                    /// Replaces the all {1}...{n} in the format string with arg1...argN and logs the resulting string.
                    Format(const std::string& format, const BaseArg& arg1, const BaseArg& arg2)
                    {
                        data = format;
                        replace(1, arg1);
                        replace(2, arg2);
                    }

                    /// Replaces the all {1}...{n} in the format string with arg1...argN and logs the resulting string.
                    Format(const std::string& format, const BaseArg& arg1, const BaseArg& arg2, const BaseArg& arg3)
                    {
                        data = format;
                        replace(1, arg1);
                        replace(2, arg2);
                        replace(3, arg3);
                    }

                    /// Replaces the all {1}...{n} in the format string with arg1...argN and logs the resulting string.
                    Format(const std::string& format,
                           const BaseArg& arg1,
                           const BaseArg& arg2,
                           const BaseArg& arg3,
                           const BaseArg& arg4)
                    {
                        data = format;
                        replace(1, arg1);
                        replace(2, arg2);
                        replace(3, arg3);
                        replace(4, arg4);
                    }

                    /// Replaces the all {1}...{n} in the format string with arg1...argN and logs the resulting string.
                    Format(const std::string& format,
                           const BaseArg& arg1,
                           const BaseArg& arg2,
                           const BaseArg& arg3,
                           const BaseArg& arg4,
                           const BaseArg& arg5)
                    {
                        data = format;
                        replace(1, arg1);
                        replace(2, arg2);
                        replace(3, arg3);
                        replace(4, arg4);
                        replace(5, arg5);
                    }

                    /// Replaces the all {1}...{n} in the format string with arg1...argN and logs the resulting string.
                    Format(const std::string& format,
                           const BaseArg& arg1,
                           const BaseArg& arg2,
                           const BaseArg& arg3,
                           const BaseArg& arg4,
                           const BaseArg& arg5,
                           const BaseArg& arg6)
                    {
                        data = format;
                        replace(1, arg1);
                        replace(2, arg2);
                        replace(3, arg3);
                        replace(4, arg4);
                        replace(5, arg5);
                        replace(6, arg6);
                    }

                    /// Replaces the all {1}...{n} in the format string with arg1...argN and logs the resulting string.
                    Format(const std::string& format,
                           const BaseArg& arg1,
                           const BaseArg& arg2,
                           const BaseArg& arg3,
                           const BaseArg& arg4,
                           const BaseArg& arg5,
                           const BaseArg& arg6,
                           const BaseArg& arg7)
                    {
                        data = format;
                        replace(1, arg1);
                        replace(2, arg2);
                        replace(3, arg3);
                        replace(4, arg4);
                        replace(5, arg5);
                        replace(6, arg6);
                        replace(7, arg7);
                    }

                    /// Replaces the all {1}...{n} in the format string with arg1...argN and logs the resulting string.
                    Format(const std::string& format,
                           const BaseArg& arg1,
                           const BaseArg& arg2,
                           const BaseArg& arg3,
                           const BaseArg& arg4,
                           const BaseArg& arg5,
                           const BaseArg& arg6,
                           const BaseArg& arg7,
                           const BaseArg& arg8)
                    {
                        data = format;
                        replace(1, arg1);
                        replace(2, arg2);
                        replace(3, arg3);
                        replace(4, arg4);
                        replace(5, arg5);
                        replace(6, arg6);
                        replace(7, arg7);
                        replace(7, arg8);
                    }

                    /// Replaces the all {1}...{n} in the format string with arg1...argN and logs the resulting string.
                    Format(const std::string& format,
                           const BaseArg& arg1,
                           const BaseArg& arg2,
                           const BaseArg& arg3,
                           const BaseArg& arg4,
                           const BaseArg& arg5,
                           const BaseArg& arg6,
                           const BaseArg& arg7,
                           const BaseArg& arg8,
                           const BaseArg& arg9)
                    {
                        data = format;
                        replace(1, arg1);
                        replace(2, arg2);
                        replace(3, arg3);
                        replace(4, arg4);
                        replace(5, arg5);
                        replace(6, arg6);
                        replace(7, arg7);
                        replace(7, arg8);
                        replace(7, arg9);
                    }

                    operator const char*() const
                    {
                        return data.c_str();
                    }

                    const char* get() const
                    {
                        return *this;
                    }

                private:
                    void replace(int i, const BaseArg& arg)
                    {
                        std::stringstream tmp;
                        tmp << "{" << i << "}";

                        // Replace all occurrences
                        auto pos = data.find(tmp.str());
                        while (pos != std::string::npos)
                        {
                            data.replace(pos, tmp.str().size(), arg);
                            pos = data.find(tmp.str());
                        }
                    }

                    std::string data;
            };

            class Log
            {
                public:
                    /// Logs the provided message as an error, with the tag as a suffix.
                    static void error(const std::string& tag, const Format& fmt);
                    /// Logs the provided message as a warning, with the tag as a suffix.
                    static void warning(const std::string& tag, const Format& fmt);
                    /// Logs the provided message as info, with the tag as a suffix.
                    static void info(const std::string& tag, const Format& fmt);
                    /// Logs the provided message as debug, with the tag as a suffix.
                    static void debug(const std::string& tag, const Format& fmt);
                    /// Logs the provided message as verbose, with the tag as a suffix.
                    static void verbose(const std::string& tag, const Format& fmt);
            };
        }
    }
}
