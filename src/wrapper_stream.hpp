/*
 * wrapper_stream.hpp
 *
 *  Created on: Apr 7, 2023
 *      Author: diamat
 */

#ifndef SRC_ASYNC_WRAPPER_STREAM_HPP_
#define SRC_ASYNC_WRAPPER_STREAM_HPP_

#include "decls.hpp"
#include <stdexcept>
#include <iostream>
#include <variant>

namespace web {

class wrapper_stream {
private:
    using tcp_stream = beast::tcp_stream;
    using ssl_stream = beast::ssl_stream<tcp_stream>;

public:
    using executor_type = typename beast::tcp_stream::executor_type;

    bool is_ssl() const {
        return (_ssl != nullptr);
    }

    bool is_tcp() const {
        return (_tcp != nullptr);
    }

    wrapper_stream(const wrapper_stream& other) = delete;

    wrapper_stream(wrapper_stream&& other)  = default;

    wrapper_stream(tcp::socket&& socket) :
        _tcp{ new tcp_stream{ std::forward<decltype(socket)>(socket) } }
    {}

    wrapper_stream(tcp::socket&& socket, ssl::context& ctx) :
        _ssl{ new ssl_stream{ std::forward<decltype(socket)>(socket), ctx } }
    {}

    executor_type get_executor() {
        if (is_ssl())
            return _ssl->get_executor();
        else if (is_tcp())
            return _tcp->get_executor();
        else
            return executor_type{};
    }

    template<class MutableBufferSequence>
    std::size_t read_some(MutableBufferSequence const& buf) {
        if (is_ssl())
            return _ssl->read_some(buf);
        else if (is_tcp())
            return _tcp->read_some(buf);
        else
            return 0;
    }

    template<class MutableBufferSequence>
    std::size_t read_some(MutableBufferSequence const& buf,
            boost::system::error_code& e) {
        if (is_ssl())
            return _ssl->read_some(buf, e);
        else if (is_tcp())
            return _tcp->read_some(buf, e);
        else
            return 0;
    }

    template<class ConstBufferSequence>
    std::size_t write_some(ConstBufferSequence const& buf) {
        if (is_ssl())
            return _ssl->write_some(buf);
        else if (is_tcp())
            return _tcp->write_some(buf);
        else
            return 0;
    }

    template<class ConstBufferSequence>
    std::size_t write_some(ConstBufferSequence const& buf,
            boost::system::error_code& e) {
        if (is_ssl())
            return _ssl->write_some(buf, e);
        else if (is_tcp())
            return _tcp->write_some(buf, e);
        else
            return 0;
    }

    template<
        class MutableBufferSequence,
        BOOST_BEAST_ASYNC_TPARAM2 ReadHandler =
            net::default_completion_token_t<executor_type>
    >
    auto async_read_some(
        MutableBufferSequence const& buffers,
        ReadHandler&& handler =
            net::default_completion_token_t<executor_type>{});

    template<
        class ConstBufferSequence,
        BOOST_BEAST_ASYNC_TPARAM2 WriteHandler =
            net::default_completion_token_t<executor_type>
    >
    auto async_write_some(
        ConstBufferSequence const& buffers,
        WriteHandler&& handler =
            net::default_completion_token_t<executor_type>{});

    void set_timeout(const std::chrono::seconds& t) {
        _timeout = t;
    }

    net::awaitable<void> shutdown() {
        if (is_ssl()) {
            if (_timeout)
                beast::get_lowest_layer(*_ssl).expires_after(*_timeout);

            co_await _ssl->async_shutdown(net::use_awaitable);
        } else if (is_tcp()) {
            beast::error_code ec;
            _tcp->socket().shutdown(tcp::socket::shutdown_send, ec);
        }
    }

    net::awaitable<void> handshake() {
        if (is_ssl()) {
            if (_timeout)
                beast::get_lowest_layer(*_ssl).expires_after(*_timeout);

            co_await _ssl->async_handshake(
                    ssl::stream_base::server, net::use_awaitable);
        }
    }

private:
    wrapper_stream() = default;

    std::shared_ptr<tcp_stream> _tcp = nullptr;
    std::shared_ptr<ssl_stream> _ssl = nullptr;
    std::optional<std::chrono::seconds> _timeout;
};

template<
     class MutableBufferSequence,
     BOOST_BEAST_ASYNC_TPARAM2 ReadHandler>
 auto wrapper_stream::async_read_some(
     MutableBufferSequence const& buffers,
     ReadHandler&& token) {
    auto initiation = [this](
          net::completion_handler_for<void(boost::system::error_code, std::size_t)>
            auto&& completion_handler,
          MutableBufferSequence const& buf) {
        if (is_ssl()) {
            if (_timeout)
                beast::get_lowest_layer(*_ssl).expires_after(*_timeout);

            return _ssl->async_read_some(buf,
                    std::forward<decltype(completion_handler)>(completion_handler));
        } else if (is_tcp()) {
            if (_timeout)
                beast::get_lowest_layer(*_tcp).expires_after(*_timeout);

            return _tcp->async_read_some(buf,
                    std::forward<decltype(completion_handler)>(completion_handler));
        }
    };

    return net::async_initiate<ReadHandler,
            void(boost::system::error_code, std::size_t)>(initiation, token, buffers);
}

 template<
     class ConstBufferSequence,
     BOOST_BEAST_ASYNC_TPARAM2 WriteHandler>
 auto wrapper_stream::async_write_some(
     ConstBufferSequence const& buffers,
     WriteHandler&& token) {
     auto initiation = [this](
            net::completion_handler_for<void(boost::system::error_code, std::size_t)>
             auto&& completion_handler,
            ConstBufferSequence const& buf) {
         if (is_ssl()) {
             if (_timeout)
                 beast::get_lowest_layer(*_ssl).expires_after(*_timeout);

             return _ssl->async_write_some(buf,
                     std::forward<decltype(completion_handler)>(completion_handler));
         } else if (is_tcp()) {
             if (_timeout)
                 beast::get_lowest_layer(*_tcp).expires_after(*_timeout);

             return _tcp->async_write_some(buf,
                     std::forward<decltype(completion_handler)>(completion_handler));
         }
     };

     return net::async_initiate<WriteHandler,
             void(boost::system::error_code, std::size_t)>(initiation, token, buffers);
}

}

#endif /* SRC_ASYNC_WRAPPER_STREAM_HPP_ */
