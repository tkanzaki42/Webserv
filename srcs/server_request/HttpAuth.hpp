#ifndef SRCS_SERVER_REQUEST_HTTPAUTH_HPP_
#define SRCS_SERVER_REQUEST_HTTPAUTH_HPP_

#include <iostream>

#include "includes/webserv.hpp"
#include "srcs/server_request/HttpAuthType.hpp"

# define BASIC_USER_PASS    "tkanzaki:42tokyo"

class HttpAuth {
 public:
    explicit HttpAuth();
    ~HttpAuth();
    HttpAuth(const HttpAuth &obj);
    HttpAuth& operator=(const HttpAuth &obj);
    
    void         set_client_info(const std::string &client_info);
    HttpAuthType check_auth_type();
    HttpAuthType get_auth_type() const;
    bool         do_basic();
 private:
    std::string  client_info_;
    HttpAuthType auth_type_;
};

#endif  // SRCS_SERVER_REQUEST_HTTPAUTH_HPP_
