/*
 * router.cpp
 *
 *  Created on: Apr 8, 2023
 *      Author: diamat
 */

#include "router.hpp"
#include "utils.hpp"

namespace web {

responder_ptr router::responder_for(const std::string_view &path) {
    std::optional<std::string_view>
    prefix = utils::prefix_matching(path, _prefix_regex);

    if (!prefix)
        return nullptr;

    size_t prefix_length = prefix->ends_with('/') ? prefix->size() - 1 : prefix->size();
    std::string_view suffix = path.substr(prefix_length);
    for (auto& route : routes)
        if (responder_ptr res = route->responder_for(suffix))
            return res;

    return nullptr;
}

} /* namespace web */
