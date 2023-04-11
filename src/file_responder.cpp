/*
 * file_responder.cpp
 *
 *  Created on: Apr 8, 2023
 *      Author: diamat
 */

#include "file_responder.hpp"
#include "error.hpp"
#include "utils.hpp"

namespace web {

net::awaitable<void>
file_responder::respond(session_t &session) {
    session_t::request_t& req = session.request();

    // Make sure we can handle the method
    if( req.method() != http::verb::get &&
            req.method() != http::verb::head)
        throw error{ 400, "Unknown HTTP-method" };

    // Request path must be absolute and not contain "..".
    if( req.target().empty() ||
            req.target()[0] != '/' ||
            req.target().find("..") != beast::string_view::npos)
        throw error{ 400, "Illegal request-target" };

    // Attempt to open the file
    beast::error_code ec;
    http::file_body::value_type body;
    body.open(path.c_str(), beast::file_mode::scan, ec);

    // Handle the case where the file doesn't exist
    if(ec == beast::errc::no_such_file_or_directory)
        throw error{ 404, req.target() };

    // Handle an unknown error
    if(ec)
        throw error{ 500, ec.message() };

    // Cache the size since we need it after the move
    auto const size = body.size();

    // Respond to HEAD request
    if(req.method() == http::verb::head) {
        http::response<http::empty_body>
        res{ http::status::ok, req.version() };
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, utils::mime_type(path));
        res.content_length(size);
        res.keep_alive(req.keep_alive());
        co_await session.send_response(res, net::use_awaitable);
    } else {
        // Respond to GET request
        http::response<http::file_body> res{
            std::piecewise_construct,
            std::make_tuple(std::move(body)),
            std::make_tuple(http::status::ok, req.version())};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, utils::mime_type(path));
        res.content_length(size);
        res.keep_alive(req.keep_alive());
        co_await session.send_response(res, net::use_awaitable);
    }
}

} /* namespace web */
