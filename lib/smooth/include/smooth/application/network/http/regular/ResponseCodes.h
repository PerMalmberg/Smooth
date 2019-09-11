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

#pragma once

#include <unordered_map>

namespace smooth::application::network::http::regular
{
    enum class ResponseCode
    {
        // Informational = 1××
        Continue = 100,
        SwitchingProtocols = 101,
        Processing = 102,

        //_Success = 2××
        OK = 200,
        Created = 201,
        Accepted = 202,
        Non_authoritativeInformation = 203,
        No_Content = 204,
        Reset_Content = 205,
        Partial_Content = 206,
        Multi_Status = 207,
        Already_Reported = 208,
        IM_Used = 226,

        //_Redirection = 3××
        Multiple_Choices = 300,
        Moved_Permanently = 301,
        Found = 302,
        See_Other = 303,
        Not_Modified = 304,
        Use_Proxy = 305,
        Temporary_Redirect = 307,
        Permanent_Redirect = 308,

        //_Client_Error = 4××
        Bad_Request = 400,
        Unauthorized = 401,
        Payment_Required = 402,
        Forbidden = 403,
        Not_Found = 404,
        Method_Not_Allowed = 405,
        Not_Acceptable = 406,
        Proxy_Authentication_Required = 407,
        Request_Timeout = 408,
        Conflict = 409,
        Gone = 410,
        Length_Required = 411,
        Precondition_Failed = 412,
        Payload_Too_Large = 413,
        Request_URI_Too_Long = 414,
        Unsupported_Media_Type = 415,
        Requested_Range_Not_Satisfiable = 416,
        Expectation_Failed = 417,
        Im_a_teapot = 418,
        Misdirected_Request = 421,
        Unprocessable_Entity = 422,
        Locked = 423,
        Failed_Dependency = 424,
        Upgrade_Required = 426,
        Precondition_Required = 428,
        Too_Many_Requests = 429,
        Request_Header_Fields_Too_Large = 431,
        Connection_Closed_Without_Response = 444,
        Unavailable_For_Legal_Reasons = 451,
        Client_Closed_Request = 499,

        //_Server_Error = 5××
        Internal_Server_Error = 500,
        Not_Implemented = 501,
        Bad_Gateway = 502,
        Service_Unavailable = 503,
        Gateway_Timeout = 504,
        HTTP_Version_Not_Supported = 505,
        Variant_Also_Negotiates = 506,
        Insufficient_Storage = 507,
        Loop_Detected = 508,
        Not_Extended = 510,
        Network_Authentication_Required = 511,
        Network_Connect_Timeout_Error = 599
    };

    const std::unordered_map<ResponseCode, const char*> response_code_to_text = {
        { ResponseCode::Continue, "Continue" },
        { ResponseCode::SwitchingProtocols, "Switching Protocols" },
        { ResponseCode::Processing, "Processing" },
        { ResponseCode::OK, "OK" },
        { ResponseCode::Created, "Created" },
        { ResponseCode::Accepted, "Accepted" },
        { ResponseCode::Non_authoritativeInformation, "Non-Authoritative Information" },
        { ResponseCode::No_Content, "No Content" },
        { ResponseCode::Reset_Content, "Reset Content" },
        { ResponseCode::Partial_Content, "Partial Content" },
        { ResponseCode::Multi_Status, "Multi Status" },
        { ResponseCode::Already_Reported, "Already Reported" },
        { ResponseCode::IM_Used, "IM Used" },
        { ResponseCode::Multiple_Choices, "Multiple Choices" },
        { ResponseCode::Moved_Permanently, "Moved Permanently" },
        { ResponseCode::Found, "Found" },
        { ResponseCode::See_Other, "See Other" },
        { ResponseCode::Not_Modified, "Not Modified" },
        { ResponseCode::Use_Proxy, "Use Proxy" },
        { ResponseCode::Temporary_Redirect, "Temporary Redirect" },
        { ResponseCode::Permanent_Redirect, "Permanent Redirect" },
        { ResponseCode::Bad_Request, "Bad Request" },
        { ResponseCode::Unauthorized, "Unauthorized" },
        { ResponseCode::Payment_Required, "Payment Required" },
        { ResponseCode::Forbidden, "Forbidden" },
        { ResponseCode::Not_Found, "Not Found" },
        { ResponseCode::Method_Not_Allowed, "Method Not Allowed" },
        { ResponseCode::Not_Acceptable, "Not Acceptable" },
        { ResponseCode::Proxy_Authentication_Required, "Proxy Authentication Required" },
        { ResponseCode::Request_Timeout, "Request Timeout" },
        { ResponseCode::Conflict, "Conflict" },
        { ResponseCode::Gone, "Gone" },
        { ResponseCode::Length_Required, "Length Required" },
        { ResponseCode::Precondition_Failed, "Precondition Failed" },
        { ResponseCode::Payload_Too_Large, "Payload Too Large" },
        { ResponseCode::Request_URI_Too_Long, "Request URI Too Long" },
        { ResponseCode::Unsupported_Media_Type, "Unsupported Media Type" },
        { ResponseCode::Requested_Range_Not_Satisfiable, "Requested Range Not Satisfiable" },
        { ResponseCode::Expectation_Failed, "Expectation Failed" },
        { ResponseCode::Im_a_teapot, "I'm a teapot" },
        { ResponseCode::Misdirected_Request, "Misdirected Request" },
        { ResponseCode::Unprocessable_Entity, "Unprocessable Entity" },
        { ResponseCode::Locked, "Locked" },
        { ResponseCode::Failed_Dependency, "Failed Dependency" },
        { ResponseCode::Upgrade_Required, "Upgrade Required" },
        { ResponseCode::Precondition_Required, "Precondition Required" },
        { ResponseCode::Too_Many_Requests, "Too Many Requests" },
        { ResponseCode::Request_Header_Fields_Too_Large, "Request Header Fields Too Large" },
        { ResponseCode::Connection_Closed_Without_Response, "Connection Closed Without Response" },
        { ResponseCode::Unavailable_For_Legal_Reasons, "Unavailable For Legal Reasons" },
        { ResponseCode::Client_Closed_Request, "Client Closed Request" },
        { ResponseCode::Internal_Server_Error, "Internal Server Error" },
        { ResponseCode::Not_Implemented, "Not Implemented" },
        { ResponseCode::Bad_Gateway, "Bad Gateway" },
        { ResponseCode::Service_Unavailable, "Service Unavailable" },
        { ResponseCode::Gateway_Timeout, "Gateway Timeout" },
        { ResponseCode::HTTP_Version_Not_Supported, "HTTP Version Not Supported" },
        { ResponseCode::Variant_Also_Negotiates, "Variant Also Negotiates" },
        { ResponseCode::Insufficient_Storage, "Insufficient Storage" },
        { ResponseCode::Loop_Detected, "Loop Detected" },
        { ResponseCode::Not_Extended, "Not Extended" },
        { ResponseCode::Network_Authentication_Required, "Network Authentication Required" },
        { ResponseCode::Network_Connect_Timeout_Error, "Network Connect Timeout Error" }
    };
}
