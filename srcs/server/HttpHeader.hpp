#ifndef SRCS_SERVER_HTTPHEADER_HPP_
#define SRCS_SERVER_HTTPHEADER_HPP_

#include <string>
#include <vector>
#include <sstream>

class HttpHeader {
 private:
    std::vector<std::string>  content_;
    int                       body_length_;

 public:
    HttpHeader();
    ~HttpHeader();
    HttpHeader(const HttpHeader &obj);
    HttpHeader &operator=(const HttpHeader &obj);

    void                make_response(int status_code);
    const std::vector<std::string>
                        &get_content();
    void                set_body_length(int body_length);
    void                clear_contents();
    static std::string  get_reason_phrase(int status_code);
};

#endif  // SRCS_SERVER_HTTPHEADER_HPP_
