/*
 * file_responder.hpp
 *
 *  Created on: Apr 8, 2023
 *      Author: diamat
 */

#ifndef SRC_FILE_RESPONDER_HPP_
#define SRC_FILE_RESPONDER_HPP_

#include "responder.hpp"
#include <string>

namespace web {

struct file_responder : public responder {
    std::string path;
    net::awaitable<void> respond(session_t& session) override;
    virtual ~file_responder() = default;
};

} /* namespace web */

#endif /* SRC_FILE_RESPONDER_HPP_ */
