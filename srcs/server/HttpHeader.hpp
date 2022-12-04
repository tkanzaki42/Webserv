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
    bool                                is_keep_alive_;


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
    void                set_header(std::string header_line);
    void                clear_contents();
    bool                get_is_keep_alive();
};

#endif  // SRCS_SERVER_HTTPHEADER_HPP_
