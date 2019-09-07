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
