// Copyright 2022 tkanzaki
#include <string>
#include "srcs/util/StringConverter.hpp"

int StringConverter::stoi(const std::string &s) {
    int num;
    std::istringstream ss(s);
    ss >> num;
    return num;
}

std::string StringConverter::itos(int i) {
    std::ostringstream oss;
    oss << i;
    return (oss.str());
}
