/*
 * defs.hpp
 *
 *  Created on: Apr 6, 2023
 *      Author: diamat
 */

#ifndef SRC_DEFS_HPP_
#define SRC_DEFS_HPP_

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <boost/asio/detached.hpp>
#include <boost/config.hpp>
#include <memory>

namespace web
{

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

}

#endif /* SRC_DEFS_HPP_ */
