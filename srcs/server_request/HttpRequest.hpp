#ifndef SRCS_SERVER_REQUEST_HTTPREQUEST_HPP_
#define SRCS_SERVER_REQUEST_HTTPREQUEST_HPP_

#include <string.h>
#include <iostream>
#include <string>
#include <map>
#include <utility>

#include "includes/webserv.hpp"
#include "srcs/server_request/HttpParser.hpp"

class HttpRequest {
 public:
    HttpRequest();
    ~HttpRequest();
    HttpRequest(const HttpRequest &obj);
    HttpRequest& operator=(const HttpRequest &obj);

    void                                      set_accept_fd(int accept_fd);
    int                                       recv_until_double_newline();
    void                                      analyze_request();
    void                                      print_debug();

    // getter(HttpParser)
    HttpMethod                                get_http_method() const;
    const std::string&                        get_request_path() const;
    const std::string&                        get_http_ver() const;
    const std::map<std::string, std::string>& get_header_field() const;
    // getter(HttpRequest)
    int                                       get_status_code() const;
    const std::string&                        get_path_to_file() const;

 private:
    int                                       accept_fd_;
    HttpParser                                parser_;
    std::string                               received_line_;
    int                                       status_code_;
    std::string                               path_to_file_;

    void                                      generate_path_to_file_();
};

#endif  // SRCS_SERVER_REQUEST_HTTPREQUEST_HPP_