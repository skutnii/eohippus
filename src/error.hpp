/*
 * exception.hpp
 *
 *  Created on: Apr 7, 2023
 *      Author: diamat
 */

#ifndef SRC_ERROR_HPP_
#define SRC_ERROR_HPP_

#include <stdexcept>
#include <string>

namespace web {

class error : public std::runtime_error {
public:
    error(int code, const char *what) :
        _code{ code },
        std::runtime_error{ what }
   {}

   error(int code, const std::string& what) :
       error{ code, what.c_str() }
  {}

  int code() { return _code; };
private:
    int _code;
};

} /* namespace web */

#endif /* SRC_ERROR_HPP_ */
