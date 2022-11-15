#include "srcs/util/split.hpp"

std::vector<std::string> split(const std::string &s, char c) {
    std::vector<std::string> v;
    std::stringstream ss(s);
    std::string str;
    while (std::getline(ss, str, c)) {
        if (!str.empty()) {
            v.push_back(str);
        }
    }
    return v;
}
