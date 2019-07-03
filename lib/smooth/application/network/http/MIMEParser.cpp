#include <smooth/application/network/http/MIMEParser.h>
#include <smooth/core/util/string_util.h>
#include <smooth/application/network/http/HTTPHeaderDef.h>

using namespace smooth::core::util;

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
            // https://www.w3.org/Protocols/rfc1341/7_2_Multipart.html
            // Note that the encapsulation boundary must occur at the beginning of a line, i.e., following a CRLF,
            // and that that initial CRLF is considered to be part of the encapsulation boundary rather than part
            // of the preceding part. The boundary must be followed immediately either by another CRLF and the header
            // fields for the next part, or by two CRLFs, in which case there are no header fields for the next part
            // (and it is therefore assumed to be of Content-Type text/plain).

            // Note that we don't include the leading CRLF in the boundary pattern - that is handled separately
            // so that we also match the very first boundary after the HTTP-headers.
            boundary = {b.begin(), b.end()};
            boundary.insert(boundary.begin(), '-');
            boundary.insert(boundary.begin(), '-');

            end_boundary = boundary;

            boundary.emplace_back('\r');
            boundary.emplace_back('\n');

            // The ending boundary is the same as the normal one, but suffixed by "--\r\n" instead of just \r\n
            end_boundary.emplace_back('-');
            end_boundary.emplace_back('-');
            end_boundary.emplace_back('\r');
            end_boundary.emplace_back('\n');
            res = true;
        }

        return res;
    }

    auto MIMEParser::find_boundaries() const
    {
        Boundaries b{};

        // Don't search until there are actually enough data
        if (data.size() > boundary.size() * 2)
        {
            auto p = std::search(data.cbegin(), data.cend(), boundary.cbegin(), boundary.cend());

            while (p != data.cend())
            {
                b.emplace_back(p);
                p = std::search(p + 1, data.cend(), boundary.cbegin(), boundary.cend());
            }

            // Also find the end boundary
            p = std::search(data.cbegin(), data.cend(), end_boundary.cbegin(), end_boundary.cend());
            if(p != data.end())
            {
                b.emplace_back(p);
            }

            // Adjust for possible preceding CRLF, as the CRLF is considered part of the boundary.
            adjust_boundary_begining_for_crlf(data.begin(), b);

        }

        return b;
    }

    void MIMEParser::parse(const uint8_t* p, std::size_t length, const ContentCallback& content_callback)
    {
        for (std::size_t i = 0; i < length; ++i)
        {
            data.emplace_back(*(p + i));
        }

        auto bounds = find_boundaries();

        auto last_consumed = data.cend();

        // Each content block is contained between two boundaries
        while (!bounds.empty() && bounds.size() >= 2)
        {
            auto start_of_content = get_end_of_boundary(bounds.front());
            bounds.erase(bounds.cbegin());
            auto end_of_content = bounds.front();
            last_consumed = end_of_content;

            parse_content(start_of_content, end_of_content, content_callback);
        }

        // Erase already consumed data
        if (last_consumed != data.cend())
        {
            data.erase(data.begin(), last_consumed);
        }
    }

    MIMEParser::BoundaryIterator MIMEParser::get_end_of_boundary(BoundaryIterator begin)
    {
        // Adjust for CRLF at beginning of boundary pattern
        auto offset = is_crlf(begin) ? 2 : 0;
        return begin + static_cast<Boundaries::difference_type>(boundary.size()) + offset;
    }

    void MIMEParser::parse_content(BoundaryIterator start_of_content, BoundaryIterator end_of_content,
                                   const ContentCallback& cb) const
    {
        // If the first data isn't a CRLF, then there are one or more Content-headers for this data.
        if (is_crlf(start_of_content))
        {
            // All content is considered text/plain
            cb("", start_of_content, end_of_content);
        }
        else
        {
            auto header_data = consume_headers(start_of_content, end_of_content);
            start_of_content = std::get<0>(header_data);
            auto& headers = std::get<1>(header_data);
            auto& content_dispositon = std::get<2>(header_data);

            if (start_of_content != end_of_content)
            {
                cb(content_dispositon["name"], start_of_content, end_of_content);
            }

            (void) headers;
        }
    }

    void MIMEParser::adjust_boundary_begining_for_crlf(MIMEParser::BoundaryIterator start_of_data,
                                                       MIMEParser::Boundaries& found_boundaries) const
    {
        for (auto& current : found_boundaries)
        {
            auto distance = static_cast<long>(std::distance(start_of_data, current));
            auto offset = static_cast<long>(crlf.size());

            if (distance > offset)
            {
                // Check if the boundary has a preceding CRLF
                if (is_crlf(start_of_data + distance - offset))
                {
                    current = start_of_data + distance - offset;
                }
            }
        }
    }

    bool MIMEParser::is_crlf(MIMEParser::BoundaryIterator start) const
    {
        return *start == '\r' && *(start + 1) == '\n';
    }

    std::tuple<MIMEParser::BoundaryIterator,
            std::unordered_map<std::string, std::string>,
            std::unordered_map<std::string, std::string>>
    MIMEParser::consume_headers(MIMEParser::BoundaryIterator begin, MIMEParser::BoundaryIterator end) const
    {
        std::unordered_map<std::string, std::string> headers{};
        std::unordered_map<std::string, std::string> content_disp{};
        auto start_of_actual_content = end;

        auto end_of_headers = std::search(begin, end, crlf_double.begin(), crlf_double.end());
        if (end_of_headers != end)
        {
            start_of_actual_content = end_of_headers + static_cast<long>(crlf_double.size());

            std::stringstream ss;

            std::for_each(begin, end_of_headers, [&ss](auto& c) {
                if (c != '\n')
                {
                    ss << static_cast<char>(c);
                }
            });

            std::string s;
            while (std::getline(ss, s, '\r'))
            {
                auto colon = std::find(s.begin(), s.end(), ':');
                if (colon != s.end() && !s.empty())
                {
                    if (std::distance(colon, s.end()) > 2)
                    {
                        headers[to_lower_copy({s.begin(), colon})] = {colon + 2, s.end()};
                    }
                }

                parse_content_disposition(headers, content_disp);
            }
        }

        return std::make_tuple(start_of_actual_content, headers, content_disp);
    }

    void MIMEParser::parse_content_disposition(const std::unordered_map<std::string, std::string>& headers,
                                               std::unordered_map<std::string, std::string>& content_disposition) const
    {
        try
        {
            const auto& content_dis = headers.at(CONTENT_DISPOSITION);

            auto part = split(content_dis, ";", true);

            for (const auto& p : part)
            {
                auto key_value = split(p, "=", true);
                if (key_value.size() > 1)
                {
                    // Remove leading and ending quotation mark
                    auto filter = [](char c){ return c != '"'; };
                    content_disposition[key_value[0]] = trim(key_value[1], filter);
                }
                else
                {
                    // Single-value data
                    content_disposition[key_value[0]] = key_value[0];
                }
            }
        }
        catch (...)
        {}
    }
}

