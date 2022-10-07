// Copyright 2022 tkanzaki
#ifndef SRCS_UTIL_UTIL_HPP_
#define SRCS_UTIL_UTIL_HPP_

#include <iostream>
#include <sstream>
#include <string>

class StringConverter {
 public:
     static int stoi(const std::string &s);
     static std::string itos(int i);
};

#endif  // SRCS_UTIL_UTIL_HPP_
