/*
 * server.cpp
 *
 *  Created on: Apr 6, 2023
 *      Author: diamat
 */

#include "server.hpp"
#include <vector>
#include <thread>
#include <iostream>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/system/system_error.hpp>

namespace web {

const int server::default_error_handler = -1;

void server::run() {
    // Run the I/O service on the requested number of threads
    std::vector<std::thread> v;
    v.reserve(_num_threads - 1);
    for(auto i = _num_threads - 1; i > 0; --i)
        v.emplace_back(
        [this]() {
            _ioc.run();
        });

    _ioc.run();
}

void server::bind_http(const tcp::endpoint &endpoint) {
    net::co_spawn(_ioc,
            listen(endpoint,
                    [this](tcp::socket& socket) -> net::awaitable<void> {
                        return tcp(socket);
                    }),
            net::detached);
}

void server::bind_https(const tcp::endpoint &endpoint) {
    net::co_spawn(_ioc,
            listen(endpoint,
                    [this](tcp::socket& socket) -> net::awaitable<void> {
                        return secure_tcp(socket);
                    }),
            net::detached);
}

net::awaitable<void> server::tcp(tcp::socket& socket) {
    wrapper_stream stream{ std::move(socket) };
    stream.set_timeout(std::chrono::seconds(30));
    try {
        co_await http(stream);
        co_await stream.shutdown();
        std::cout << "Closed socket" << std::endl;
    } catch(std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

net::awaitable<void> server::secure_tcp(tcp::socket& socket) {
    wrapper_stream stream{ std::move(socket), _ssl_ctx };
    stream.set_timeout(std::chrono::seconds(30));
    try {
        co_await stream.handshake();
        co_await http(stream);
        co_await stream.shutdown();
        std::cout << "Closed socket" << std::endl;
    } catch(std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

net::awaitable<void> server::http(wrapper_stream& stream) {
    beast::flat_buffer buffer;

    // This lambda is used to send messages
    for(;;)
    {
        session_t session{ stream, buffer };

        beast::error_code ec;
        try {
            co_await session.read_header(net::use_awaitable);

            responder_ptr request_handler =
                    root->responder_for(session.request().target());
            if (!request_handler)
                throw error{ 404, "Not found" };

            co_await request_handler->respond(session);
        } catch(boost::system::system_error& err) {
            if (err.code() == http::error::end_of_stream)
                break;
            else {
                std::cerr << err.what() << std::endl;
                co_return;
            }
        } catch(web::error& err) {
            wrapper_stream& stream = session.stream();
            const request_t& request = session.request();

            if (error_handlers.contains(err.code())) {
                beast::write(stream,
                        error_handlers[err.code()](err, request), ec);
            } else if (error_handlers.contains(default_error_handler)) {
                beast::write(stream,
                        error_handlers[default_error_handler](err, request), ec);
            } else {
                http::write(stream,
                        internal_error(request), ec);
            }
        } catch(std::exception& err) {
            std::cerr << err.what() << std::endl;
        } catch(...) {
            std::cerr << "Unknown error" << std::endl;
        }

        if(!session.keep_alive())
        {
            // This means we should close the connection, usually because
            // the response indicated the "Connection: close" semantic.
            break;
        }
    }
}

void server::fail(beast::error_code ec, char const* what)
{
    // ssl::error::stream_truncated, also known as an SSL "short read",
    // indicates the peer closed the connection without performing the
    // required closing handshake (for example, Google does this to
    // improve performance). Generally this can be a security issue,
    // but if your communication protocol is self-terminated (as
    // it is with both HTTP and WebSocket) then you may simply
    // ignore the lack of close_notify.
    //
    // https://github.com/boostorg/beast/issues/38
    //
    // https://security.stackexchange.com/questions/91435/how-to-handle-a-malicious-ssl-tls-shutdown
    //
    // When a short read would cut off the end of an HTTP message,
    // Beast returns the error beast::http::error::partial_message.
    // Therefore, if we see a short read here, it has occurred
    // after the message has been completed, so it is safe to ignore it.

    if(ec == net::ssl::error::stream_truncated)
        return;

    std::cerr << what << ": " << ec.message() << "\n";
}

} /* namespace web */
