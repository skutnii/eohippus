/*
 * static_responder.hpp
 *
 *  Created on: Apr 8, 2023
 *      Author: diamat
 */

#ifndef SRC_STATIC_RESPONDER_HPP_
#define SRC_STATIC_RESPONDER_HPP_

#include "resolver.hpp"
#include <regex>
#include <optional>
#include "file_responder.hpp"

namespace web {

class static_resolver : public resolver {
public:
    static_resolver(const std::string& root, const std::regex& prefix) :
        _doc_root{ root }, _prefix_regex{ prefix } {}

    const std::string& doc_root() const { return _doc_root; }
    const std::regex& prefix_regex() const { return _prefix_regex; }

    responder_ptr responder_for(const std::string_view& path) override;

    virtual ~static_resolver() = default;
private:
    std::string _doc_root;
    std::regex _prefix_regex;
};

} /* namespace web */

#endif /* SRC_STATIC_RESPONDER_HPP_ */
