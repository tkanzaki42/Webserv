// Copyright 2022 tkanzaki
#ifndef SRCS_UTIL_STRINGCONVERTER_HPP_
#define SRCS_UTIL_STRINGCONVERTER_HPP_

#include <cstdlib>

#include <iostream>
#include <sstream>
#include <string>

class StringConverter {
 public:
     static int stoi(const std::string &s);
     static int cstrtoi(const char *s);
     static std::string itos(int i);
     static std::size_t ft_strlen(const char *str);
     static std::size_t ft_strlcpy(
            char *dst, const char *src, std::size_t dstsize);
     static char* ft_strjoin(char const *s1, char const *s2);
};

#endif  // SRCS_UTIL_STRINGCONVERTER_HPP_
