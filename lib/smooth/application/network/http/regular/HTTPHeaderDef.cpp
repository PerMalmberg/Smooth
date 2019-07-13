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

namespace smooth::application::network::http::regular
{
    // Type all strings using lower-case; headers are all converted to lowercase in the response header maps.
    const char* CONTENT_LENGTH = "content-length";
    const char* CONTENT_TYPE = "content-type";
    const char* LAST_MODIFIED = "last-modified";
    const char* CONNECTION = "connection";
    const char* KEEP_ALIVE = "keep-alive";
    const char* ORIGIN = "origin";
    const char* HOST = "host";
    const char* CONTENT_DISPOSITION = "content-disposition";
    const char* UPGRADE = "upgrade";
    const char* SEC_WEBSOCKET_KEY = "sec-websocket-key";
    const char* SEC_WEBSOCKET_PROTOCOL = "sec-websocket-protocol";
    const char* SEC_WEBSOCKET_VERSION = "sec-websocket-version";
    const char* SEC_WEBSOCKET_ACCEPT = "sec-websocket-accept";
}