//
// Copyright (c) 2023 Sergii Kutnii (mnkutster at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

//------------------------------------------------------------------------------
//
// Example: HTTP SSL server, coroutine
//
//------------------------------------------------------------------------------

#include "cert.hpp"
#include "server.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include "decls.hpp"
#include "static_resolver.hpp"

int main(int argc, char* argv[])
{
    using namespace web;

    // Check command line arguments.
    if (argc != 3)
    {
        std::cerr <<
            "Usage: beast-server <doc_root> <threads>\n" <<
            "Example:\n" <<
            "    beast-server /path/to/htdocs 4\n";
        return EXIT_FAILURE;
    }

    server srv{ std::atoi(argv[2]) };

    auto const address = net::ip::make_address("0.0.0.0");
    tcp::endpoint http_endpoint{ address, 80 };
    srv.bind_http(http_endpoint);

    tcp::endpoint https_endpoint{ address, 443 };
    srv.bind_https(https_endpoint);

    srv.root = std::make_shared<static_resolver>( argv[1], std::regex{ "\\/" } );

    //Default http error handler
    srv.error_handlers[server::default_error_handler] =
            server::error_handler_t{
                [](const error& err,
                        const server::request_t& request) -> http::message_generator {
                    http::response<http::string_body>
                    res{ http::status::bad_request, request.version() };
                    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                    res.set(http::field::content_type, "text/html");
                    res.keep_alive(request.keep_alive());
                    res.body() = err.what();
                    res.prepare_payload();
                    return res;
                }
            };

    srv.error_handlers[404] =
            server::error_handler_t{
                [](const error& err,
                        const server::request_t& request) -> http::message_generator {
                    http::response<http::string_body>
                    res{ http::status::not_found, request.version() };
                    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                    res.set(http::field::content_type, "text/html");
                    res.keep_alive(request.keep_alive());
                    res.body() = std::string{ "Not found: " } + err.what();
                    res.prepare_payload();
                    return res;
                }
            };

    srv.run();

    return EXIT_SUCCESS;
}
