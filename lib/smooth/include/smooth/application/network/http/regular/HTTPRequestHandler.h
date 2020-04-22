/*
Smooth - A C++ framework for embedded programming on top of Espressif's ESP-IDF
Copyright 2020 Per Malmberg (https://gitbub.com/PerMalmberg)

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

#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include "smooth/application/network/http/IServerResponse.h"
#include "smooth/application/network/http/IConnectionTimeoutModifier.h"
#include "smooth/application/network/http/regular/MIMEParser.h"

namespace smooth::application::network::http::regular
{
    /// Base class interface for all HTTP request handlers
    /// This class is tightly coupled with HTTPServer::handle() and the
    /// call order in that method. When inheriting from this class,
    /// The HTTPServer ensures that your inheriting class is served all
    /// the data it needs before calling request().
    class HTTPRequestHandler : public IFormData, public IURLEncodedData
    {
        public:
            virtual ~HTTPRequestHandler() = default;

            /// Called for each part of the incoming data in the current request
            // See protected methods for reading request-specific data/information.
            virtual void request(IConnectionTimeoutModifier& timeout_modifier,
                                 const std::string& url,
                                 const std::vector<uint8_t>& content) = 0;

            // Convenience methods for setting up the handler.
            virtual void start_of_request() {} // Called before the first call to request().

            virtual void end_of_request() {} // Called after the last call to request().

            /// Called multiple times while the MIMEParser is decoding form data
            virtual void form_data(const std::string& /*field_name*/,
                                   const std::string& /*actual_file_name*/,
                                   const BoundaryIterator& /*begin*/,
                                   const BoundaryIterator& /*end*/) override {}

            /// Called multiple times while the the MIMEParser is decoding URL encoded data
            virtual void url_encoded(std::unordered_map<std::string, std::string>& /*data*/) override {}

            void prepare_mime();

            void update_call_params(bool first_part,
                                    bool last_part,
                                    IServerResponse& /*response*/,
                                    const std::unordered_map<std::string, std::string>& headers,
                                    const std::unordered_map<std::string, std::string>& request_parameters);

        protected:
            MIMEParser mime{};

            /// Returns true during the first call to request()
            bool is_first() const { return request_params.first_part; }

            /// Returns true during the last call to request()
            bool is_last() const { return request_params.last_part; }

            /// Returns the response used to communicate to the client.
            IServerResponse& response() const { return *request_params.response; }

            /// Returns the headers for the current request
            const std::unordered_map<std::string, std::string>& headers() const { return *request_params.headers; }

            /// Returns the request parameters (page?a=1&b=2 etc.) for the currect request
            const std::unordered_map<std::string, std::string>& request_parameters() const
            {
                return *request_params.request_parameters;
            }

        private:
            /// This structure holds parameters for the current request,
            /// to be accessed via above methods.
            struct RequestParams
            {
                bool first_part;
                bool last_part;
                IServerResponse* response{};
                const std::unordered_map<std::string, std::string>* headers{ nullptr };
                const std::unordered_map<std::string, std::string>* request_parameters{ nullptr };
            };

            RequestParams request_params{};
    };
}
