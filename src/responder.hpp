/*
 * resource.hpp
 *
 *  Created on: Apr 7, 2023
 *      Author: diamat
 */

#ifndef SRC_RESOURCE_HPP_
#define SRC_RESOURCE_HPP_

#include "session.hpp"
#include <memory>

namespace web {

class responder {
public:
    virtual net::awaitable<void> respond(session_t& session) = 0;
    virtual ~responder() = default;
};

using responder_ptr = std::shared_ptr<responder>;

}



#endif /* SRC_RESOURCE_HPP_ */
