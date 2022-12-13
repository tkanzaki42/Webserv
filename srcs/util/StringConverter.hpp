// Copyright 2022 tkanzaki
#ifndef SRCS_UTIL_UTIL_HPP_
#define SRCS_UTIL_UTIL_HPP_

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

class StringConverter {
 public:
     static int stoi(const std::string &s);
     static std::string itos(int i);
     static std::string base64_encode(const std::string &src);
 private:
     static bool encode_base64_(const std::vector<unsigned char>& src, std::string& dst);
};

#endif  // SRCS_UTIL_UTIL_HPP_
