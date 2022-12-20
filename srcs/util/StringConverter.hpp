// Copyright 2022 tkanzaki
#ifndef SRCS_UTIL_STRINGCONVERTER_HPP_
#define SRCS_UTIL_STRINGCONVERTER_HPP_

#include <cstdlib>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

class StringConverter {
 public:
     static int          stoi(const std::string &s);
     static unsigned int stoui(const std::string &s);
     static int          cstrtoi(const char *s);
     static std::string  itos(int i);
     static std::string  uitos(unsigned int i);
     static std::string  base64_encode(const std::string &src);
     static std::size_t  ft_strlen(const char *str);
     static std::size_t  ft_strlcpy(
            char *dst, const char *src, std::size_t dstsize);
     static char*        ft_strjoin(char const *s1, char const *s2);
 private:
     static bool         encode_base64_(const std::vector<unsigned char>& src, std::string& dst);
};

#endif  // SRCS_UTIL_STRINGCONVERTER_HPP_
