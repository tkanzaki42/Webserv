// Copyright 2022 tkanzaki
#ifndef PROJ_WEBSERV_SRCS_UTIL_UTIL_HPP_
#define PROJ_WEBSERV_SRCS_UTIL_UTIL_HPP_

#include <iostream>
#include <sstream>
#include <string>

class Util {
 public:
     static int stoi(const std::string &s);
     static std::string itos(int i);
};

#endif  // PROJ_WEBSERV_SRCS_UTIL_UTIL_HPP_
