#ifndef SRCS_SERVER_STATUSDESCRIPTION_HPP_
#define SRCS_SERVER_STATUSDESCRIPTION_HPP_

#include <iostream>
#include <string>

class StatusDescription {
 public:
    static std::string get_message(int status_code);
    static std::string get_reason(int status_code);
};

#endif  // SRCS_SERVER_STATUSDESCRIPTION_HPP_
