#ifndef SRCS_UTIL_UTF8UTIL_HPP_
#define SRCS_UTIL_UTF8UTIL_HPP_

#include <string>
#include <utility>
#include <iostream>

class UTF8Util {
 public:
    static int get_string_width(const std::string& str);
    static const std::string get_limited_wide_string(
                     const std::string& str, const int length);
    static const std::pair<std::string, int>
                get_one_char(const std::string& str, const int pos);
    static bool is_start_pos(const std::string& str, const int pos);

 private:
    static unsigned int bin_to_UInt_(const std::string &str);
};

#endif
