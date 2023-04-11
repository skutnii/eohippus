/*
 * utils.hpp
 *
 *  Created on: Apr 6, 2023
 *      Author: diamat
 */

#ifndef SRC_UTILS_HPP_
#define SRC_UTILS_HPP_

#include "decls.hpp"
#include <string>
#include <regex>
#include <optional>

namespace web
{

namespace utils
{

// Return a reasonable mime type based on the extension of a file.
beast::string_view
mime_type(beast::string_view path);

// Append an HTTP rel-path to a local filesystem path.
// The returned path is normalized for the platform.
std::string
path_cat(beast::string_view base, beast::string_view path);

std::optional<std::string_view>
prefix_matching(const std::string_view& str, const std::regex& regex);

}

}


#endif /* SRC_UTILS_HPP_ */
