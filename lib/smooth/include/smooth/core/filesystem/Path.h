#pragma once

#include <string>
#include <vector>

namespace smooth
{
    namespace core
    {
        namespace filesystem
        {
            class Path
            {
                public:
                    Path() = default;

                    Path(const Path&) = default;

                    Path(Path&&) = default;

                    Path& operator=(const Path&) = default;

                    Path(const char* path)
                            : p(path)
                    {
                        remove_ending_separator();
                        resolve();
                    }

                    Path(std::string::const_iterator beg, std::string::const_iterator end)
                    {
                        std::copy(beg, end, std::back_inserter(p));
                        remove_ending_separator();
                        resolve();
                    }

                    Path& operator/(const char* path)
                    {
                        append(path);
                        return *this;
                    }

                    Path& operator/(const Path& path)
                    {
                        append(path.p);
                        return *this;
                    }

                    explicit operator std::string() const
                    {
                        return p;
                    }

                    explicit operator const char*() const
                    {
                        return p.c_str();
                    }

                    bool operator==(const Path& p) const;

                    bool operator==(const char*) const;

                    bool is_parent_of(const Path& child) const;

                    Path parent() const;

                    bool is_relative() const
                    {
                        return !is_absolute();
                    }

                    bool is_absolute() const
                    {
                        return !p.empty() && p[0] == '/';
                    }

                    bool empty() const
                    {
                        return p.empty();
                    }

                private:
                    void append(const std::string& path);

                    void remove_ending_separator();

                    void resolve();

                    bool static equals_path_separator(char c)
                    {
                        return c == '/' || c == '\\';
                    }

                    bool contains_dots(const std::string& s);

                    static const std::string separator;
                    static const std::string dot_token;
                    std::string p;
            };
        }
    }
}
