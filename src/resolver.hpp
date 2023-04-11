/*
 * resolver.hpp
 *
 *  Created on: Apr 8, 2023
 *      Author: diamat
 */

#ifndef SRC_RESOLVER_HPP_
#define SRC_RESOLVER_HPP_

#include "responder.hpp"
#include <memory>
#include <string>

namespace web {

class resolver {
public:
    virtual responder_ptr responder_for(const std::string_view& path) = 0;
    virtual ~resolver() = default;
};

using resolver_ptr = std::shared_ptr<resolver>;

}

#endif /* SRC_RESOLVER_HPP_ */
