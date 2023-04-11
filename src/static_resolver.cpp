/*
 * static_responder.cpp
 *
 *  Created on: Apr 8, 2023
 *      Author: diamat
 */

#include "static_resolver.hpp"
#include "error.hpp"
#include "utils.hpp"
#include "file_responder.hpp"
#include <filesystem>
#include <iostream>

namespace web {

responder_ptr
static_resolver::responder_for(const std::string_view& url_path) {
    std::optional<std::string_view>
    match = utils::prefix_matching(url_path, _prefix_regex);

    if (!match)
        return nullptr;

    std::string_view suffix = url_path.substr(match->size());
    std::filesystem::path file_path{ _doc_root };
    file_path /= suffix;
    if (!file_path.has_filename())
        file_path /= "index.html";

    std::shared_ptr<file_responder> res_ptr = std::make_shared<file_responder>();
    res_ptr->path = file_path.string();

    return res_ptr;
}

} /* namespace web */
