#ifndef SRCS_SERVER_REQUEST_HTTPREQUEST_HPP_
#define SRCS_SERVER_REQUEST_HTTPREQUEST_HPP_

#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <string>
#include <map>
#include <utility>

#include "includes/webserv.hpp"
#include "srcs/server_request/HttpParser.hpp"

class Webserv;

class HttpRequest {
 public:
    HttpRequest(Webserv &server);
    ~HttpRequest();
    HttpRequest(const HttpRequest &obj);
    HttpRequest& operator=(const HttpRequest &obj);

   //  void                                      set_accept_fd(int accept_fd);
    int                                       receive_header();
    bool                                      analyze_request();
    void                                      print_debug();

    // getter(HttpParser)
    HttpMethod                                get_http_method() const;
    const std::string&                        get_request_path() const;
    const std::string&                        get_http_ver() const;
    const std::map<std::string, std::string>& get_header_field_map() const;
    // getter(HttpRequest)
    int                                       get_status_code() const;
    const std::string&                        get_path_to_file() const;

 private:
   //  int                                       accept_fd_;
    Webserv                                   &server_;
    HttpParser                                parser_;
    std::string                               received_line_;
    std::string                               path_to_file_;
    int                                       status_code_;

    void                                      generate_path_to_file_();
    bool                                      receive_and_store_to_file_();
};

#endif  // SRCS_SERVER_REQUEST_HTTPREQUEST_HPP_
