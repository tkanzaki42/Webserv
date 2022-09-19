#ifndef SRCS_SERVER_HTTPHEADER_HPP_
#define SRCS_SERVER_HTTPHEADER_HPP_

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

#endif  // SRCS_SERVER_HTTPHEADER_HPP_
