#ifndef SRCS_SERVER_CGI_HPP_
#define SRCS_SERVER_CGI_HPP_

#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <algorithm>

#include "includes/webserv.hpp"
#include "srcs/server_request/HttpRequest.hpp"
#include "srcs/config/Config.hpp"

class CGI {
 public:
    explicit CGI(HttpRequest& request);
    ~CGI();
    CGI(const CGI &obj);
    CGI& operator=(const CGI &obj);

    int                              exec_cgi(FileType file_type);
    std::size_t                      get_content_length();
    const std::vector<std::string>&  get_header_content();
    const std::vector<std::string>&  get_body_content();

 private:
    HttpRequest&              request_;
    char                      **path_;
    char                      **exec_envs_;
    FileType                  file_type_;
    std::vector<std::string>  header_content_;
    std::vector<std::string>  body_content_;

    void         run_child_process_();
    int          close_pipe_(int pipe_no);
    int          connect_pipe_(int pipe_no_old, int pipe_no_new);
    void         generate_exec_paths_();
    void         generate_env_vars_();
    char*        duplicate_string_(const std::string &str);
    std::string  read_shebang_();
    void         separate_to_header_and_body_(const std::string &read_buffer);
    void         cleanup_();

};

#endif  // SRCS_SERVER_CGI_HPP_
