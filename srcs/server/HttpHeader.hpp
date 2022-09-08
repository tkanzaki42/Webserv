#ifndef HTTPHEADER_HPP
#define HTTPHEADER_HPP

#include <string>
#include <vector>
#include <sstream>

class HttpHeader {
public:
    static std::vector<std::string>& make_response200(int body_length);
    // static std::vector<std::string>& make_response404();
    // static std::vector<std::string>& make_response302(std::string path);
    // static std::vector<std::string>& make_responseUpgrade();
};

#endif
