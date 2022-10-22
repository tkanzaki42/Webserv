#ifndef SRCS_SERVER_HTTPHEADER_HPP_
#define SRCS_SERVER_HTTPHEADER_HPP_

#include <string>
#include <vector>
#include <sstream>
#include <map>

class HttpHeader {
 private:
    std::string                         status_line_;
    std::map<std::string, std::string>  header_field_;
    int                                 body_length_;

    void                store_header_(std::string header_line);
 public:
    HttpHeader();
    ~HttpHeader();
    HttpHeader(const HttpHeader &obj);
    HttpHeader &operator=(const HttpHeader &obj);

    void                make_response(int status_code);
    std::string         get_status_line();
    const std::map<std::string, std::string>
                        &get_content();
    void                set_body_length(int body_length);
    void                clear_contents();
    static std::string  get_reason_phrase(int status_code);
};

#endif  // SRCS_SERVER_HTTPHEADER_HPP_
