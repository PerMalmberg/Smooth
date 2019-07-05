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
            FileInfo info{curr};
            if (!info.exists())
            {
                // https://en.cppreference.com/w/cpp/filesystem/perms
                res = mkdir(curr, S_IRWXU);
            }
        }

        return res == 0;
    }
}