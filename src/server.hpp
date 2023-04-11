/*
 * server.hpp
 *
 *  Created on: Apr 6, 2023
 *      Author: diamat
 */

#ifndef SRC_SERVER_HPP_
#define SRC_SERVER_HPP_

#include <string>
#include "decls.hpp"
#include "cert.hpp"
#include "utils.hpp"
#include <iostream>
#include <functional>
#include <unordered_map>
#include <functional>

#include "error.hpp"
#include "session.hpp"
#include "resolver.hpp"

namespace web {

class server {
public:
    server(int num_threads) :
        _num_threads{ num_threads},
        _ioc{ 1 },
        _ssl_ctx{ ssl::context::tlsv12 } {
            load_server_certificate(_ssl_ctx);
        }

    resolver_ptr root;

    using request_t = http::request<http::buffer_body>;

    using error_handler_t = std::function<
            http::message_generator (const error&, const request_t&)>;

    std::unordered_map<int, error_handler_t> error_handlers;
    static const int default_error_handler;

    net::awaitable<void> http(wrapper_stream& stream);
    void run();

    void bind_http(const tcp::endpoint& endpoint);
    void bind_https(const tcp::endpoint& endpoint);

    void fail(beast::error_code ec, char const* what);

    template<typename handler_t>
    net::awaitable<void> listen(const tcp::endpoint& endpoint, handler_t handler) {
        beast::error_code ec;

        // Open the acceptor
        tcp::acceptor acceptor(_ioc);
        acceptor.open(endpoint.protocol(), ec);
        if(ec)
            co_return fail(ec, "open");

        // Allow address reuse
        acceptor.set_option(net::socket_base::reuse_address(true), ec);
        if(ec)
            co_return fail(ec, "set_option");

        // Bind to the server address
        acceptor.bind(endpoint, ec);
        if(ec)
            co_return fail(ec, "bind");

        // Start listening for connections
        acceptor.listen(net::socket_base::max_listen_connections, ec);
        if(ec)
            co_return fail(ec, "listen");
        for (;;)
        {
            tcp::socket socket = co_await acceptor.async_accept(
                                                    net::make_strand(_ioc), net::use_awaitable);
            co_spawn(_ioc, handler(socket), net::detached);
        }
    }

    net::awaitable<void> tcp(tcp::socket& socket);

    net::awaitable<void> secure_tcp(tcp::socket& socket);

    http::response<http::string_body>
    internal_error(const request_t& request) {
        http::response<http::string_body>
        res{ http::status::internal_server_error, request.version() };

        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(request.keep_alive());
        res.body() = "Internal server error";
        res.prepare_payload();
        return res;
    }

private:
    net::io_context _ioc;
    ssl::context _ssl_ctx;
    int _num_threads;
};

} /* namespace web */

#endif /* SRC_SERVER_HPP_ */
