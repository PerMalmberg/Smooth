// Smooth - C++ framework for writing applications based on Espressif's ESP-IDF.

// Copyright (C) 2017 Per Malmberg (https://github.com/PerMalmberg)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include <smooth/core/filesystem/filesystem.h>
#include <stack>
#include <smooth/core/filesystem/Fileinfo.h>

namespace smooth::core::filesystem
{
    bool create_directory(Path&& path)
    {
        std::stack<Path> stack{};

        do
        {
            stack.push(path);
            path = path.parent();
        }
        while (path.has_parent());

        int res = 0;

        while (!stack.empty() && res == 0)
        {
            auto curr = stack.top();
            stack.pop();
            FileInfo info{ curr };

            if (!info.exists())
            {
                // https://en.cppreference.com/w/cpp/filesystem/perms
                res = mkdir(curr, S_IRWXU);
            }
        }

        return res == 0;
    }

    bool exists(const Path& path)
    {
        FileInfo fi{ path };

        return fi.exists();
    }

    bool exists(const Path&& path)
    {
        FileInfo fi{ path };

        return fi.exists();
    }

    bool is_directory(const Path& path)
    {
        FileInfo fi{ path };

        return fi.exists() && fi.is_directory();
    }
}
