/*
 * session.hpp
 *
 *  Created on: Apr 7, 2023
 *      Author: diamat
 */

#ifndef SRC_SESSION_HPP_
#define SRC_SESSION_HPP_

#include "decls.hpp"
#include <memory>
#include <chrono>
#include <optional>
#include "wrapper_stream.hpp"

namespace web {

class session_t {
public:
    session_t(wrapper_stream& stream, beast::flat_buffer& buffer) :
        _stream{ std::forward<decltype(stream)>(stream) },
        _buffer{ buffer } {}

    template<typename completion_t>
    auto read_header(completion_t&& token) {
        return http::async_read_header(
                _stream, _buffer, parser(), std::forward<decltype(token)>(token));
    }

    template<typename completion_t>
    auto read_body(void *buffer, size_t count, completion_t&& token) {
        request().body().data = buffer;
        request().body().size = count;
        return http::async_read(_stream,
                _buffer, parser(), std::forward<decltype(token)>(token));
    }

    template<typename response_t, typename completion_t>
    auto send_response(response_t& res, completion_t&& token) {
        return http::async_write(
                _stream, res, std::forward<decltype(token)>(token));
    }

    using parser_t = http::request_parser<http::buffer_body>;
    using request_t = typename parser_t::value_type;

    request_t& request() { return _parser.get(); }
    const request_t& request() const { return _parser.get(); }

    parser_t& parser() { return _parser; }
    const parser_t& parser() const { return _parser; }

    wrapper_stream& stream() { return _stream; };
    beast::flat_buffer& buffer() { return _buffer; };

    virtual ~session_t() = default;

    void set_keep_alive(bool value) {
        _keep_alive_override = value;
    }

    bool keep_alive() const {
        if (_keep_alive_override.has_value())
            return _keep_alive_override.value();

        return request().keep_alive();
    }

private:
    wrapper_stream& _stream;
    beast::flat_buffer& _buffer;
    parser_t _parser;
    std::optional<bool> _keep_alive_override;
};

} /* namespace web */

#endif /* SRC_SESSION_HPP_ */
