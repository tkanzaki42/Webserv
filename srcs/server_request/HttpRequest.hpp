#ifndef SRCS_SERVER_REQUEST_HTTPREQUEST_HPP_
#define SRCS_SERVER_REQUEST_HTTPREQUEST_HPP_

#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <string>
#include <map>
#include <utility>

#include "includes/webserv.hpp"
#include "srcs/server_request/HttpParser.hpp"
#include "srcs/util/PathUtil.hpp"
#include "srcs/util_network/FDManager.hpp"

# define TMP_POST_DATA_DIR  "./file/"
# define TMP_POST_DATA_FILE "./file/upload_file"
# define REQUEST_ENTITY_MAX 1000000

class HttpRequest {
 public:
    explicit HttpRequest(FDManager *fd_manager);
    ~HttpRequest();
    HttpRequest(const HttpRequest &obj);
    HttpRequest& operator=(const HttpRequest &obj);

    int                 receive_header();
    void                analyze_request();
    void                print_debug();

    // getter(HttpParser)
    HttpMethod          get_http_method() const;
    const std::string&  get_request_target() const;
    const std::string&  get_query_string() const;
    const std::string&  get_path_info() const;
    const std::string&  get_path_to_file() const;
    const std::string&  get_http_ver() const;
    const std::string&  get_header_field(const std::string& key);
    const std::map<std::string, std::string>&
                        get_header_field_map() const;
    // getter(HttpRequest)
    FileType            get_file_type();
    int                 get_status_code() const;
    int                 get_virtual_host_index() const;
    bool                get_is_autoindex() const;
    struct sockaddr_in  get_client_addr();
    // setter
    void                set_file_type(FileType file_type);

 private:
    FDManager           *fd_manager_;
    HttpParser          parser_;
    std::string         received_line_;
    FileType            file_type_;
    int                 status_code_;
    int                 virtual_host_index_;
    bool                is_autoindex_;

    void                check_redirect_();
    int                 receive_and_store_to_file_();
    int                 receive_normal_data_(std::ofstream &ofs_outfile);
    int                 delete_file_();
};

#endif  // SRCS_SERVER_REQUEST_HTTPREQUEST_HPP_
