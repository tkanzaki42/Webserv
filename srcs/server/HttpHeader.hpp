#ifndef SRCS_SERVER_HTTPHEADER_HPP_
#define SRCS_SERVER_HTTPHEADER_HPP_

#include <string>
#include <vector>
#include <sstream>

class HttpHeader {
 private :
    std::vector<std::string> content_;
    int                      body_length_;
 public:
    HttpHeader();
    ~HttpHeader();
    HttpHeader(const HttpHeader &obj);
    HttpHeader &operator=(const HttpHeader &obj);

    void make_response200();
    const std::vector<std::string> &get_content();
    void set_body_length(int body_length);
    void clear_contents();
    // std::vector<std::string>& make_response404();
    // std::vector<std::string>& make_response302(std::string path);
    // std::vector<std::string>& make_responseUpgrade();
};

#endif  // SRCS_SERVER_HTTPHEADER_HPP_
