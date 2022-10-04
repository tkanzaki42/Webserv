#include "srcs/util/Util.hpp"

int Util::stoi(const std::string &s){
    int num;
    std::istringstream ss(s);
    ss >> num;
    return num;
}

std::string Util::itos(int i){
    std::ostringstream oss;
    oss << i;
    return (oss.str());
}
