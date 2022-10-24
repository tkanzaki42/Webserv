#ifndef SRCS_SERVER_ERRORPAGE_HPP_
#define SRCS_SERVER_ERRORPAGE_HPP_

#include <iostream>
#include <string>

class ErrorPage {
 public:
    static std::string get(int status_code);
};

#endif  // SRCS_SERVER_ERRORPAGE_HPP_
