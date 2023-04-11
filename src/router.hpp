/*
 * router.h
 *
 *  Created on: Apr 8, 2023
 *      Author: diamat
 */

#ifndef SRC_ROUTER_H_
#define SRC_ROUTER_H_

#include "resolver.hpp"
#include <regex>
#include <vector>

namespace web {

class router : public resolver {
public:
    router(const std::regex& prefix_matcher) :
        _prefix_regex{ prefix_matcher } {}

    std::vector<resolver_ptr> routes;

    responder_ptr responder_for(const std::string_view& path) override;
private:
    std::regex _prefix_regex;
};

} /* namespace web */

#endif /* SRC_ROUTER_H_ */
