#include <smooth/application/network/http/MIMEParser.h>


namespace smooth::application::network::http
{
    void MIMEParser::reset() noexcept
    {

    }

    bool MIMEParser::find_boundary(const std::string&& s)
    {
        bool res{false};

        std::smatch match;
        if (std::regex_match(s.begin(), s.end(), match, form_data_pattern))
        {
            auto b = match[1].str();
            boundary = {b.begin(), b.end()};

            end_boundary = boundary;
            end_boundary.insert(end_boundary.begin(), '\n');
            end_boundary.insert(end_boundary.begin(), '\n');
            end_boundary.emplace_back('-');
            end_boundary.emplace_back('-');
            res = true;
        }

        return res;
    }

    auto MIMEParser::find_boundaries() const
    {
        Boundaries b{};

        bool end_found = false;

        // Don't search until there are actually enough data
        if (data.size() > boundary.size() * 2)
        {
            auto p = std::search(data.cbegin(), data.cend(), boundary.cbegin(), boundary.cend());

            while (p != data.cend())
            {
                b.emplace_back(p);
                p = std::search(p + 1, data.cend(), boundary.cbegin(), boundary.cend());
            }

            // Is the last boundary the end-boundary?

            if (b.size() > 1)
            {
                p = std::search(b[b.size() - 1], data.cend(), end_boundary.cbegin(), end_boundary.cend());
                end_found = p != data.end();
            }
        }

        return std::make_tuple(b, end_found);
    }

    bool MIMEParser::parse(const uint8_t* p, std::size_t length, const ContentCallback& content_callback)
    {
        bool res{false};

        for (std::size_t i = 0; i < length; ++i)
        {
            data.emplace_back(*(p + i));
        }

        auto boundaries = find_boundaries();
        auto bounds = std::get<0>(boundaries);

        //auto has_end = std::get<1>(boundaries);

        auto last_consumed = data.cend();

        // Each content block is contained between two boundaries
        while (!bounds.empty() && bounds.size() % 2 == 0)
        {
            auto start = get_end_of_boundary(bounds.front());
            bounds.erase(bounds.cbegin());
            auto end = bounds.front();
            bounds.erase(bounds.cbegin());
            last_consumed = end;

            parse_content(start, end, content_callback);
        }

        // Erase already consumed data
        if (last_consumed != data.cend())
        {
            data.erase(data.begin(), last_consumed);
        }

        return res;
    }

    MIMEParser::BoundaryIterator MIMEParser::get_end_of_boundary(BoundaryIterator start)
    {
        return start + static_cast<Boundaries::difference_type>(boundary.size());
    }

    void MIMEParser::parse_content(BoundaryIterator begin, BoundaryIterator end, const ContentCallback& cb) const
    {
        std::string s{begin, end};
        s += "";
    }
}

