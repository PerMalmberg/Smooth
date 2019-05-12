#include <smooth/core/filesystem/Path.h>
#include <algorithm>

namespace smooth
{
    namespace core
    {
        namespace filesystem
        {
            const std::string Path::separator = "/";
            const std::string Path::dot_token = "..";

            void Path::append(const std::string& path)
            {
                if (p.empty())
                {
                    p = path;
                }
                else
                {
                    if (!equals_path_separator(*path.begin()))
                    {
                        p.append("/");
                    }

                    p.append(path);
                }

                remove_ending_separator();
            }

            bool Path::operator==(const Path& other) const
            {
                return p == other.p;
            }

            bool Path::operator==(const char* path) const
            {
                return *this == Path(path);
            }

            bool Path::is_parent_of(const Path& child) const
            {
                auto res = false;

                if (is_absolute())
                {
                    // Don't consider equal paths to be the parents of one another.
                    if (p != child.p)
                    {
                        // If child begins with 'this', then 'this' is a parent of 'child'.
                        // To prevent part of directory/file names to match (/a/w vs /a/what), prepend a
                        // separator before comparing.
                        auto child_path = child.p;
                        child_path += separator;

                        auto this_path = p;
                        this_path += separator;

                        res = child_path.find(this_path) == 0;
                    }
                }
                return res;
            }

            Path Path::parent() const
            {
                auto end_of_parent = std::find_end(p.begin(), p.end(), separator.begin(), separator.end());

                Path res;

                if (end_of_parent == p.end())
                {
                    res = ".";
                }
                else
                {
                    res = Path(p.cbegin(), end_of_parent + 1);
                }

                return res;
            }

            void Path::remove_ending_separator()
            {
                if(p != "/")
                {
                    if (!p.empty() && *p.rbegin() == '/')
                    {
                        p = p.substr(0, p.size() - 1);
                    }
                }
            }

            void Path::resolve()
            {
                std::vector<std::string> res{};

                auto absolute = is_absolute();

                if (p.find(dot_token) != std::string::npos || p.find('.') != std::string::npos)
                {
                    auto prev = p.begin();
                    auto pos = std::search(p.begin(), p.end(), separator.begin(), separator.end());
                    if (pos != p.end())
                    {
                        while (pos != p.end())
                        {
                            auto s = std::string(prev, pos);

                            if (!s.empty())
                            {
                                // Remove leading /
                                if (s.find('/') == 0)
                                {
                                    s = {s.begin() + 1, s.end()};
                                }

                                // Skip paths referring to the current dir
                                if (s != ".")
                                {
                                    res.emplace_back(s);
                                }
                            }
                            prev = pos;

                            pos = std::search(++pos, p.end(), separator.begin(), separator.end());
                        }
                    }

                    if (prev + 1 != p.end())
                    {
                        res.emplace_back(prev + 1, p.end());
                    }


                    std::vector<std::string> final_path{};
                    // Loop the split path, removing non-dots for each dot found
                    for (auto& part : res)
                    {
                        if (contains_dots(part))
                        {
                            if (!final_path.empty())
                            {
                                final_path.pop_back();
                            }
                        }
                        else
                        {
                            final_path.push_back(std::move(part));
                        }
                    }

                    p.clear();

                    for (auto& s : final_path)
                    {
                        append(s);
                    }

                    if (absolute && !is_absolute())
                    {
                        p.insert(0, "/");
                    }
                }

                // Treat empty paths as pointing to current directory
                if (p.empty())
                {
                    p = ".";
                }
            }

            bool Path::contains_dots(const std::string& s)
            {
                return s == dot_token;
            }
        }
    }
}